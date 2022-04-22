---------------------------------
--! @file
--! @author Josh Righetti
--! @date 2022
---------------------------------

zipf=os.getenv("TEST_ZIP")
zip=rkunzip.open(zipf)

l=rkunzip.list(zip)
assert(l ~= nil)
assert(#l == 3)
print("zip list:", table.concat(l,", "))

a=rkunzip.extract(zip,"testa")
assert(a ~= nil)
print('extracted testa:')
print(a)

b=rkunzip.extract(zip,"testb")
assert(b ~= nil)
print('extracted testb:')
print(b)

c=rkunzip.extract(zip,"testc")
assert(c ~= nil)
assert(c == "testc")
print('extracted testc:')
print(c)

rkunzip.close(zip)

zipf=os.getenv("TEST_ZIP2")
zip=rkunzip.open(zipf)

l=rkunzip.list(zip)
--assert(l ~= nil)
--print("zip2 list:", table.concat(l,", "))
--rkunzip.close(zip)
