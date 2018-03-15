-- Basic Lua tests



-- Print
a = 42
print("Hello world")
print("Value of a is " .. a)
print("Lua version " .. _VERSION)



-- Conditional
a = true

if (a)
then
   print("a is true")
else
   print("a is false")
end



-- Loop
a = 0
b = 5

while (a < b)
do
    print("loop count " .. a)
    a = a + 1
end



-- File I/O

file = io.open("test.txt", "a")
io.output(file)
io.write("This is the end\n")
io.close(file)



-- String/number conversion

a_string = "+5.111E+02"
a_float = tonumber(a_string)
print("a_float = " .. a_float)
print("a_float * 2 = " .. a_float * 2)



-- Math

print("2*2 = " .. 2 * 2)
print("2^4 = " .. math.pow(2,4))
print("sin(2) = " .. math.sin(2))
print("sqrt(16) = " .. math.sqrt(16))
print("pi = " .. math.pi)



-- OS functions

os.execute ("echo 'Hello world'")
print(os.date("The time is %X"))
print("TERM = " .. os.getenv("TERM"))
