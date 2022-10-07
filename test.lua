local a = 999

local function aa()
    print(a)
end

local function bb()
    aa()
end

local function cc()
    local test = 2
    bb()
    print(test)
end

cc()
print(a)