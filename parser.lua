
-- 0.1
-- 支持变量,NUMBER类型,四则运算+赋值,函数即可
-- 唯一的输出 print

a = 1
b = a
c = a + b
d = - c
e = a and not c
f = true
g = nil

function fib(n)
    if n <= 1 then
        return n
    else
        return fib(n - 1) + fib(n - 2)
    end
end

e = fib(5)
