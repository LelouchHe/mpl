
local mu = {}
mu.__index = mu

function mu.__call(_, p)
    return mu.new(p)
end

function mu.new(p)
    p = p or {}
    local t = {}
    for v in mu.iter(p) do
        t[#t + 1] = v
    end
    setmetatable(t, mu)
    return t
end

function mu.map(p, f)
    local t = {}
    for v in mu.iter(p) do
        t[#t + 1] = f(v)
    end
    return mu.new(t)
end

function mu.each(p, f)
    for v in mu.iter(p) do
        f(v)
    end
    return p
end

function mu.select(p, f)
    local t = {}
    for v in mu.iter(p) do
        if f(v) then
            t[#t + 1] =v
        end
    end
    return mu.new(t)
end

function mu.reject(p, f)
    local t = {}
    for v in mu.iter(p) do
        if not f(v) then
            t[#t + 1] = v
        end
    end
    return mu.new(t)
end

function mu.invoke(p, n, ...)
    for v in mu.iter(p) do
        v[n](v, ...)
    end
end

function mu.pluck(p, n)
    local t = {}
    for v in mu.iter(p) do
        t[#t + 1] = v[n]
    end
    return mu.new(t)
end

function mu.reduce(p, s, f)
    local x = s
    for v in mu.iter(p) do
        x = f(x, v)
    end
    return x
end

local function pick_max(p, f, cmp)
    f = f or mu.identity
    local x
    local max
    for v in mu.iter(p) do
        if not x then
            x = f(v)
            max = v
        elseif cmp(x, f(v)) then
            x = f(v)
            max = v
        end
    end
    return max
end

function mu.max(p, f)
    return pick_max(p, f, function(a, b) return a < b end)
end

function mu.min(p, f)
    return pick_max(p, f, function(a, b) return a > b end)
end

function mu.include(p, f)
    local is_ok = f
    if type(f) ~= 'function' then
        is_ok = function(a) return a == f end
    end
    for v in mu.iter(p) do
        if is_ok(v) then
            return true
        end
    end
    return false
end

function mu.detect(p, f)
    for v in mu.iter(p) do
        if f(v) then
            return v
        end
    end
    return nil
end

function mu.all(p, f)
    f = f or mu.identity
    for v in mu.iter(p) do
        if not f(v) then
            return false
        end
    end
    return true
end

function mu.any(p, f)
    f = f or mu.identity
    for v in mu.iter(p) do
        if f(v) then
            return true
        end
    end
    return false
end

function mu.to_array(p)
    local t = {}
    for v in mu.iter(p) do
        t[#t + 1] = v
    end
    return t
end

function mu.sort(p, f)
    f = f or function(a, b) return a < b end
    local t = mu.value(p);
    table.sort(t, f)
    return mu.new(t)
end

function mu.reverse(p)
    local t = {}
    for v in mu.iter(p) do
        table.insert(t, 1, v)
    end
    return mu.new(t)
end

function mu.flatten(p)
    local t = {}
    for i = 1, #p do
        if type(p[i]) == 'table' then
            local tt = mu.flatten(p[i])
            for j = 1, #tt do
                t[#t + 1] = tt[j]
            end
        else
            t[#t + 1] = p[i]
        end
    end
    return t
end

function mu.first(p, n)
    if not n then
        return p[1]
    end

    if n > #p then
        n = #p
    end
    local t = {}
    for i = 1, n do
        t[#t + 1] = p[i]
    end
    return t
end

function mu.rest(p, n)
    n = n or 1
    local t = {}
    for i = n, #p do
        t[#t + 1] = p[i]
    end
    return t
end

function mu.slice(p, s, l)
    if not s or s < 1 then
        s = 1
    end
    if not l or l > #p then
        l = #p
    end
    local t = {}
    for i = s, l do
        t[#t + 1] = p[i]
    end
    return t
end

function mu.push(p, x)
    table.insert(p, x)
    return p
end

function mu.pop(p)
    table.remove(p)
    return table.remove(p)
end

function mu.shift(p)
    return table.remove(p, 1)
end

function mu.unshift(p, x)
    table.insert(p, 1, x)
    return p
end

function mu.join(p, s)
    return table.concat(p, s)
end

function mu.extend(d, s)
    for k, v in pairs(s) do
        d[k] = v
    end
    return d
end

function mu.keys(p)
    local t = {}
    for k in pairs(p) do
        t[#t + 1] = k
    end
    return t
end

function mu.values(p)
    local t = {}
    for k, v in pairs(p) do
        t[#t + 1] = v
    end
    return t
end

function mu.is_empty(p)
    for k, v in pairs(p) do
        return false
    end
    return true
end

local mc = {}

function mc.__call(p, ...)
    local t = {...}
    for i, v in ipairs(t) do
        p._args[#p._args + 1] = v
    end

    if #p._args >= p._argc then
        return p._func(table.unpack(p._args))
    end

    return p 
end

function mu.curry(f, n)
    local t = {}
    t._func = f
    t._argc = n
    t._args = {}
    setmetatable(t, mc)
    return t
end

function mu.curry(f, n, ...)
    local args = {...}
    local function cu(...)
        for i, v in ipairs{...} do
            args[#args + 1] = v
        end
        if #args >= n then
            return f(table.unpack(args))
        end

        return cu
    end
    return cu
end

function mu.wrap(f, wr)
    return function(...)
        return wr(f, ...)
    end
end

function mu.compose(...)
    local fs = {...}
    return function(...)
        local args = {...}
        for i = #fs, 1, -1 do
            args = {fs[i](table.unpack(args))}
        end
        return table.unpack(args)
    end
end

function mu.identity(p)
    return p
end

function mu.iter(p)
    if type(p) == 'function' then
        return p
    end

    return coroutine.wrap(function()
        for i = 1, #p do
            coroutine.yield(p[i])
        end
    end)
end

function mu.range(s, l, step)
    step = step or 1
    if not l then
        l = s
        s = 1
        step = 1
    end

    local t = {}
    for i = s, l, step do
        t[#t + 1] = i
    end
    return t
end

function mu.is_equal(a, b)
    if a == b then
        -- 原始比较
        return true
    elseif type(a) ~= type(b) then
        return false
    elseif type(a) ~= 'table' then
        return false
    end

    local t = {}
    for k, v in pairs(b) do
        t[k] = v
    end
    for k, v in pairs(a) do
        if not mu.is_equal(v, t[k]) then
            return false
        end
        t[k] = nil
    end
    for k, v in pairs(t) do
        return false
    end

    return true
end

function mu.value(p)
    local t = {}
    for v in mu.iter(p) do
        t[#t + 1] = v
    end
    return t
end

local function pt(t)
    if type(t) ~= 'table' then
        return
    end
    for k, v in pairs(t) do
        print(string.format('[%s] = %s', k, v))
    end
end

function mu.debug(p)
    pt(p)
    local mt = getmetatable(p)
    if not mt then
        return
    end
    print(' ---- meta ----')
    pt(getmetatable(p))
end

return mu.new();
