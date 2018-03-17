-- Basic Lua tests --


-- To run tests simply do:
-- $ lxi run basic-tests.lua


--[[

Why Lua?

Lua provides a set of unique features that makes it distinct from other
languages. These include:

 * Extensible
 * Simple
 * Efficient
 * Portable
 * Free and open

Official Lua documentation:
http://www.lua.org/docs.html

Lua quick guide:
https://www.tutorialspoint.com/lua/lua_quick_guide.htm

Lua tutorial:
https://www.tutorialspoint.com/lua/index.htm

--]]


-- Print --
a = 42
print("Hello lxi-tools")
print("Value of a is " .. a)
print("Running " .. _VERSION)
print("Is " .. a .. " the answer?")


-- Conditional --
a = true
if (a)
then
   print("a is true")
else
   print("a is false")
end


-- Loop --
for i=10, 1, -1 -- for init, max/min value, increment
do 
   print("loop count " .. i) 
end


-- Conditional loops
i = 0
j = 5
while (i < j)
do
    print("loop count " .. i)
    i = i + 1
end
repeat
    print("loop count " .. i)
    i = i - 1
until (i == 0)


-- Function --
function add(a, b)
    return (a + b)
end
print("10 + 10 = " .. add(10,10))


-- Array --
array = {"duck", 42, "dog"}
print(array[1])
print(array[2])
print(array[3])


-- File I/O --
file = io.open("test.txt", "a")
io.output(file)
io.write("Hello lxi-tools\n")
io.close(file)


-- String to number conversion
a_string = "+5.111E+02"
a_number = tonumber(a_string)
print("a_number = " .. a_number)
print(type(a_string))
print(type(a_number))


-- Math
print("2*2 = " .. 2 * 2)
print("2^4 = " .. math.pow(2,4))
print("sin(2) = " .. math.sin(2))
print("sqrt(16) = " .. math.sqrt(16))
print("pi = " .. math.pi)
print("a_number * 2 = " .. a_number * 2)


-- OS functions
os.execute ("echo 'Hello lxi-tools'")
print(os.date("The time is %X"))
print("TERM = " .. os.getenv("TERM"))

