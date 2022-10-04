local a = 999

local function aa()
    print(a)
end

local function bb()
    aa()
end

local function cc()
    bb()
end

cc()