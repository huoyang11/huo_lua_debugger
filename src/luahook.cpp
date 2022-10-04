#include "luahook.h"
#include "luacommon.h"
#include "LuaTable.h"
#include "LuaValue.h"
#include <iostream>
#include <assert.h>

#define noLuaClosure(f)		((f) == NULL || (f)->c.tt == LUA_VCCL)

using namespace HiFun;

namespace huo_lua
{
    static const char* getobjname(const Proto* p, int lastpc, int reg,
        const char** name);
    static void funcinfo(lua_Debug* ar, Closure* cl) {
        if (noLuaClosure(cl)) {
            ar->source = "=[C]";
            ar->srclen = LL("=[C]");
            ar->linedefined = -1;
            ar->lastlinedefined = -1;
            ar->what = "C";
        }
        else {
            const Proto* p = cl->l.p;
            if (p->source) {
                ar->source = getstr(p->source);
                ar->srclen = tsslen(p->source);
            }
            else {
                ar->source = "=?";
                ar->srclen = LL("=?");
            }
            ar->linedefined = p->linedefined;
            ar->lastlinedefined = p->lastlinedefined;
            ar->what = (ar->linedefined == 0) ? "main" : "Lua";
        }
        luaO_chunkid(ar->short_src, ar->source, ar->srclen);
    }

    static int filterpc(int pc, int jmptarget) {
        if (pc < jmptarget)  /* is code conditional (inside a jump)? */
            return -1;  /* cannot know who sets that register */
        else return pc;  /* current position sets that register */
    }

    static int findsetreg(const Proto* p, int lastpc, int reg) {
        int pc;
        int setreg = -1;  /* keep last instruction that changed 'reg' */
        int jmptarget = 0;  /* any code before this address is conditional */
        if (testMMMode(GET_OPCODE(p->code[lastpc])))
            lastpc--;  /* previous instruction was not actually executed */
        for (pc = 0; pc < lastpc; pc++) {
            Instruction i = p->code[pc];
            OpCode op = GET_OPCODE(i);
            int a = GETARG_A(i);
            int change;  /* true if current instruction changed 'reg' */
            switch (op) {
            case OP_LOADNIL: {  /* set registers from 'a' to 'a+b' */
                int b = GETARG_B(i);
                change = (a <= reg && reg <= a + b);
                break;
            }
            case OP_TFORCALL: {  /* affect all regs above its base */
                change = (reg >= a + 2);
                break;
            }
            case OP_CALL:
            case OP_TAILCALL: {  /* affect all registers above base */
                change = (reg >= a);
                break;
            }
            case OP_JMP: {  /* doesn't change registers, but changes 'jmptarget' */
                int b = GETARG_sJ(i);
                int dest = pc + 1 + b;
                /* jump does not skip 'lastpc' and is larger than current one? */
                if (dest <= lastpc && dest > jmptarget)
                    jmptarget = dest;  /* update 'jmptarget' */
                change = 0;
                break;
            }
            default:  /* any instruction that sets A */
                change = (testAMode(op) && reg == a);
                break;
            }
            if (change)
                setreg = filterpc(pc, jmptarget);
        }
        return setreg;
    }

    static void kname(const Proto* p, int c, const char** name) {
        TValue* kvalue = &p->k[c];
        *name = (ttisstring(kvalue)) ? svalue(kvalue) : "?";
    }

    static const char* upvalname(const Proto* p, int uv) {
        TString* s = check_exp(uv < p->sizeupvalues, p->upvalues[uv].name);
        if (s == NULL) return "?";
        else return getstr(s);
    }

    static void rname(const Proto* p, int pc, int c, const char** name) {
        const char* what = getobjname(p, pc, c, name); /* search for 'c' */
        if (!(what && *what == 'c'))  /* did not find a constant name? */
            *name = "?";
    }

    static const char* gxf(const Proto* p, int pc, Instruction i, int isup) {
        int t = GETARG_B(i);  /* table index */
        const char* name;  /* name of indexed variable */
        if (isup)  /* is an upvalue? */
            name = upvalname(p, t);
        else
            getobjname(p, pc, t, &name);
        return (name && strcmp(name, LUA_ENV) == 0) ? "global" : "field";
    }

    static void rkname(const Proto* p, int pc, Instruction i, const char** name) {
        int c = GETARG_C(i);  /* key index */
        if (GETARG_k(i))  /* is 'c' a constant? */
            kname(p, c, name);
        else  /* 'c' is a register */
            rname(p, pc, c, name);
    }

    static const char* getobjname(const Proto* p, int lastpc, int reg,
        const char** name) {
        int pc;
        *name = luaF_getlocalname(p, reg + 1, lastpc);
        if (*name)  /* is a local? */
            return "local";
        /* else try symbolic execution */
        pc = findsetreg(p, lastpc, reg);
        if (pc != -1) {  /* could find instruction? */
            Instruction i = p->code[pc];
            OpCode op = GET_OPCODE(i);
            switch (op) {
            case OP_MOVE: {
                int b = GETARG_B(i);  /* move from 'b' to 'a' */
                if (b < GETARG_A(i))
                    return getobjname(p, pc, b, name);  /* get name for 'b' */
                break;
            }
            case OP_GETTABUP: {
                int k = GETARG_C(i);  /* key index */
                kname(p, k, name);
                return gxf(p, pc, i, 1);
            }
            case OP_GETTABLE: {
                int k = GETARG_C(i);  /* key index */
                rname(p, pc, k, name);
                return gxf(p, pc, i, 0);
            }
            case OP_GETI: {
                *name = "integer index";
                return "field";
            }
            case OP_GETFIELD: {
                int k = GETARG_C(i);  /* key index */
                kname(p, k, name);
                return gxf(p, pc, i, 0);
            }
            case OP_GETUPVAL: {
                *name = upvalname(p, GETARG_B(i));
                return "upvalue";
            }
            case OP_LOADK:
            case OP_LOADKX: {
                int b = (op == OP_LOADK) ? GETARG_Bx(i)
                    : GETARG_Ax(p->code[pc + 1]);
                if (ttisstring(&p->k[b])) {
                    *name = svalue(&p->k[b]);
                    return "constant";
                }
                break;
            }
            case OP_SELF: {
                rkname(p, pc, i, name);
                return "method";
            }
            default: break;  /* go through to return NULL */
            }
        }
        return NULL;  /* could not find reasonable name */
    }

    static const char* funcnamefromcode(lua_State* L, const Proto* p,
        int pc, const char** name) {
        TMS tm = (TMS)0;  /* (initial value avoids warnings) */
        Instruction i = p->code[pc];  /* calling instruction */
        switch (GET_OPCODE(i)) {
        case OP_CALL:
        case OP_TAILCALL:
            return getobjname(p, pc, GETARG_A(i), name);  /* get function name */
        case OP_TFORCALL: {  /* for iterator */
            *name = "for iterator";
            return "for iterator";
        }
                        /* other instructions can do calls through metamethods */
        case OP_SELF: case OP_GETTABUP: case OP_GETTABLE:
        case OP_GETI: case OP_GETFIELD:
            tm = TM_INDEX;
            break;
        case OP_SETTABUP: case OP_SETTABLE: case OP_SETI: case OP_SETFIELD:
            tm = TM_NEWINDEX;
            break;
        case OP_MMBIN: case OP_MMBINI: case OP_MMBINK: {
            tm = cast(TMS, GETARG_C(i));
            break;
        }
        case OP_UNM: tm = TM_UNM; break;
        case OP_BNOT: tm = TM_BNOT; break;
        case OP_LEN: tm = TM_LEN; break;
        case OP_CONCAT: tm = TM_CONCAT; break;
        case OP_EQ: tm = TM_EQ; break;
            /* no cases for OP_EQI and OP_EQK, as they don't call metamethods */
        case OP_LT: case OP_LTI: case OP_GTI: tm = TM_LT; break;
        case OP_LE: case OP_LEI: case OP_GEI: tm = TM_LE; break;
        case OP_CLOSE: case OP_RETURN: tm = TM_CLOSE; break;
        default:
            return NULL;  /* cannot find a reasonable name */
        }
        *name = getstr(G(L)->tmname[tm]) + 2;
        return "metamethod";
    }

    static int currentpc(CallInfo* ci) {
        lua_assert(isLua(ci));
        return pcRel(ci->u.l.savedpc, ci_func(ci)->p);
    }

    static const char* funcnamefromcall(lua_State* L, CallInfo* ci,
        const char** name) {
        if (ci->callstatus & CIST_HOOKED) {  /* was it called inside a hook? */
            *name = "?";
            return "hook";
        }
        else if (ci->callstatus & CIST_FIN) {  /* was it called as a finalizer? */
            *name = "__gc";
            return "metamethod";  /* report it as such */
        }
        else if (isLua(ci))
            return funcnamefromcode(L, ci_func(ci)->p, currentpc(ci), name);
        else
            return NULL;
    }

    static const char* getfuncname(lua_State* L, CallInfo* ci, const char** name) {
        /* calling function is a known function? */
        if (ci != NULL && !(ci->callstatus & CIST_TAIL))
            return funcnamefromcall(L, ci->previous, name);
        else return NULL;  /* no way to find a name */
    }

    static int findfield(lua_State* L, int objidx, int level) {
        if (level == 0 || !lua_istable(L, -1))
            return 0;  /* not found */
        lua_pushnil(L);  /* start 'next' loop */
        while (lua_next(L, -2)) {  /* for each pair in table */
            if (lua_type(L, -2) == LUA_TSTRING) {  /* ignore non-string keys */
                if (lua_rawequal(L, objidx, -1)) {  /* found object? */
                    lua_pop(L, 1);  /* remove value (but keep name) */
                    return 1;
                }
                else if (findfield(L, objidx, level - 1)) {  /* try recursively */
                    /* stack: lib_name, lib_table, field_name (top) */
                    lua_pushliteral(L, ".");  /* place '.' between the two names */
                    lua_replace(L, -3);  /* (in the slot occupied by table) */
                    lua_concat(L, 3);  /* lib_name.field_name */
                    return 1;
                }
            }
            lua_pop(L, 1);  /* remove value */
        }
        return 0;  /* not found */
    }

    static int pushglobalfuncname(lua_State* L, lua_Debug* ar) {
        int top = lua_gettop(L);
        lua_getinfo(L, "f", ar);  /* push function */
        lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
        if (findfield(L, top + 1, 2)) {
            const char* name = lua_tostring(L, -1);
            if (strncmp(name, LUA_GNAME ".", 3) == 0) {  /* name start with '_G.'? */
                lua_pushstring(L, name + 3);  /* push name without prefix */
                lua_remove(L, -2);  /* remove original name */
            }
            lua_copy(L, -1, top + 1);  /* copy name to proper place */
            lua_settop(L, top + 1);  /* remove table "loaded" and name copy */
            return 1;
        }
        else {
            lua_settop(L, top);  /* remove function and global table */
            return 0;
        }
    }

    static void pushfuncname(lua_State* L, lua_Debug* ar) {
        if (pushglobalfuncname(L, ar)) {  /* try first a global name */
            lua_pushfstring(L, "function '%s'", lua_tostring(L, -1));
            lua_remove(L, -2);  /* remove name */
        }
        else if (*ar->namewhat != '\0')  /* is there a name from code? */
            lua_pushfstring(L, "%s '%s'", ar->namewhat, ar->name);  /* use it */
        else if (*ar->what == 'm')  /* main? */
            lua_pushliteral(L, "main chunk");
        else if (*ar->what != 'C')  /* for Lua functions, use <file:line> */
            lua_pushfstring(L, "function <%s:%d>", ar->short_src, ar->linedefined);
        else  /* nothing left... */
            lua_pushliteral(L, "?");
    }

#define ispseudo(i)		((i) <= LUA_REGISTRYINDEX)
    static TValue* index2value(lua_State* L, int idx) {
        CallInfo* ci = L->ci;
        if (idx > 0) {
            StkId o = ci->func + idx;
            api_check(L, idx <= L->ci->top - (ci->func + 1), "unacceptable index");
            if (o >= L->top) return &G(L)->nilvalue;
            else return s2v(o);
        }
        else if (!ispseudo(idx)) {  /* negative index */
            api_check(L, idx != 0 && -idx <= L->top - (ci->func + 1), "invalid index");
            return s2v(L->top + idx);
        }
        else if (idx == LUA_REGISTRYINDEX)
            return &G(L)->l_registry;
        else {  /* upvalues */
            idx = LUA_REGISTRYINDEX - idx;
            api_check(L, idx <= MAXUPVAL + 1, "upvalue index too large");
            if (ttisCclosure(s2v(ci->func))) {  /* C closure? */
                CClosure* func = clCvalue(s2v(ci->func));
                return (idx <= func->nupvalues) ? &func->upvalue[idx - 1]
                    : &G(L)->nilvalue;
            }
            else {  /* light C function or Lua function (through a hook)?) */
                api_check(L, ttislcf(s2v(ci->func)), "caller not a C function");
                return &G(L)->nilvalue;  /* no upvalues */
            }
        }
    }

    static std::string lua_valpointer(lua_State* L, int index, const char* name)
    {
        std::string ret = name;
        char buf[] = "0xffffffffffffffff";

        const void* p = lua_topointer(L, index);
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "0x%p", p);

        ret += " ";
        ret += buf;

        return ret;
    }

    static std::string tostring(lua_State* L,int current_stack)
    {
        std::string ret = "";
        std::string val = "";
        char buf[] = "0xffffffffffffffff";

        switch (lua_type(L, current_stack))
        {
            case LUA_TNIL: {
                ret += "nil";
                break;
            }
            case LUA_TBOOLEAN: {
                val = "true";
                if (lua_toboolean(L, current_stack) == 0) val = "false";
                ret += val;
                break;
            }
            case LUA_TLIGHTUSERDATA: {
                val = "lightuserdata ";
                void* p = lua_touserdata(L, current_stack);
                memset(buf, 0, sizeof(buf));
                snprintf(buf, sizeof(buf), "0x%p", p);
                ret += val + buf;
                break;
            }
            case LUA_TNUMBER: {
                if (!lua_isinteger(L, current_stack)) {
                    memset(buf, 0, sizeof(buf));
                    snprintf(buf, sizeof(buf), "%g", lua_tonumber(L, current_stack));
                    ret += buf;
                }
                else {
                    lua_Integer inum = lua_tointeger(L, current_stack);
                    ret += std::to_string(inum);
                }
                break;
            }
            case LUA_TSTRING: {
                const char* str = lua_tostring(L, current_stack);
                ret += "\"";
                ret += str;
                ret += "\"";
                break;
            }
            case LUA_TTABLE: {
                LuaTable tab = LuaValue(index2value(L, current_stack));
                //TODO 
                LuaValue k,v;
                while (tab.TableNext(k, v))
                {

                }

                break;
            }
            case LUA_TFUNCTION: {
                ret += lua_valpointer(L, current_stack, "function");
                break;
            }
            case LUA_TUSERDATA: {
                ret += lua_valpointer(L, current_stack, "userdata");
                break;
            }
            case LUA_TTHREAD: {
                ret += lua_valpointer(L, current_stack, "thread");
                break;
            }
            default: {
                ret += "(val err)";
                break;
            }
        }

        return ret;
    }

    static std::string tostring(lua_State* L, TValue* val)
    {
        setobj2s(L, L->top, val);
        api_incr_top(L);
        std::string ret = tostring(L,-1);
        lua_pop(L,1);
        return ret;
    }

    static struct lua_frame_info get_frame_info(lua_State* L, CallInfo* ci)
    {
        auto oldtop = L->top;
        struct lua_Debug ar = {0};
        ar.i_ci = ci;
        struct lua_frame_info frame_info = {0};

        Instruction i = *(ci->u.l.savedpc);
        frame_info.code_arg.opcode = GET_OPCODE(i);
        frame_info.code_arg.a = GETARG_A(i);
        frame_info.code_arg.b = GETARG_B(i);
        frame_info.code_arg.c = GETARG_C(i);
        frame_info.code_arg.ax = GETARG_Ax(i);
        frame_info.code_arg.bx = GETARG_Bx(i);
        frame_info.code_arg.sb = GETARG_sB(i);
        frame_info.code_arg.sc = GETARG_sC(i);
        frame_info.code_arg.sbx = GETARG_sBx(i);
        frame_info.code_arg.isk = GETARG_k(i);

        frame_info.op_name = opnames[GET_OPCODE(i)];

        lua_getinfo(L, "Slnt", &ar);
        frame_info.line = ar.currentline;
        frame_info.filepath = ar.short_src;

        pushfuncname(L, &ar);
        frame_info.funname = getstr((struct TString *)s2v(L->top - 1)->value_.gc);
        lua_pop(L,1);

        int index = 1;
        while (1)
        {
            const char *name = lua_getlocal(L, &ar, index);
            if (!name) break;
            if ((name[0] == '('))
            {
                lua_pop(L, 1);
                break;
            }

            frame_info.locals.push_back({ name , tostring(L, -1) });
            index++;
            lua_pop(L,1);
        }

        TValue* fi = s2v(ci->func);
        if (ttypetag(fi) == LUA_VCCL)
        {
            CClosure* f = clCvalue(fi);
            for (int i = 0; i < f->nupvalues; i++)
            {
                frame_info.upvalues.push_back({ std::to_string(i) , tostring(L,&f->upvalue[i])});
            }
        }
        else if (ttypetag(fi) == LUA_VLCL)
        {
            LClosure* f = clLvalue(fi);
            Proto* p = f->p;
            for (int i = 0; i < p->sizeupvalues; i++)
            {
                frame_info.upvalues.push_back({ getstr(p->upvalues[i].name) , tostring(L,f->upvals[i]->v) });
            }
        }

        assert(L->top == oldtop);

        return frame_info;
    };

    void lua_Hook_call(lua_State* L, lua_Debug* ar)
    {
        auto oldtop = L->top;

        std::vector<lua_frame_info> frames;
        struct lua_frame_info frame_info = {0};

        CallInfo* ci = ar->i_ci;

        ci->u.l.savedpc--;
        frames.push_back(get_frame_info(L, ci));
        ci->u.l.savedpc++;

        for (CallInfo* it = ci->previous; it != &L->base_ci; it = it->previous)
        {
            frames.push_back(get_frame_info(L, it));
        }

        for (auto it : frames)
        {
            std::cout << "filepath: " << it.filepath << " "
                << "funname: " << it.funname << " "
                << "line: " << it.line << " "<< std::endl;
        }
        std::cout << std::endl;

        assert(L->top == oldtop);

        if (Hook_call) Hook_call(frames);
    }

    std::function<void(const std::vector<lua_frame_info>& frames)> Hook_call;
    void set_lua_Hook_call(std::function<void(const std::vector<lua_frame_info>& frames)> fun)
    {
        Hook_call = fun;
    }
}