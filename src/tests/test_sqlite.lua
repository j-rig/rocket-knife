---------------------------------
--! @file
--! @author Josh Righetti
--! @date 2022
---------------------------------

sqlitef=os.getenv("TEST_SQLITE")
sqlite=rksqlite.open(sqlitef)

r=rksqlite.exec(sqlite, "drop table if exists employee")
assert(r ~= nil)
r=rksqlite.exec(sqlite, "create table employee(empid integer,name varchar(20),title varchar(10))")
assert(r ~= nil)

emp = {
  "insert into employee values(101,'John Smith','CEO')",
  "insert into employee values(102,'Raj Reddy','Sysadmin')",
  "insert into employee values(103,'Jason Bourne','Developer')",
  "insert into employee values(104,'Jane Smith','Sale Manager')",
  "insert into employee values(105,'Rita Patel','DBA')"
}

for i,v in pairs(emp) do
  r=rksqlite.exec(sqlite, v)
  assert(r ~= nil)
end

r=rksqlite.exec(sqlite, "select * from employee")
assert(r ~= nil)
assert(#r == 5)
for i in pairs(r) do
  print("row", i)
  for k,v in pairs(r[i]) do
    print("\t",k,"=",v)
  end
end

rksqlite.close(sqlite)
