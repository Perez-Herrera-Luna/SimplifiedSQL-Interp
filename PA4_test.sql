-- CS 457/657 PA4

-- On P1:
CREATE DATABASE CS457_PA4;
USE CS457_PA4;
create table Flights (seat int, status int);
insert into Flights values(22, 0);
insert into Flights values(23, 1);
begin transaction;
update flights set status = 1 where seat = 22;
commit;
select * from Flights;
.exit

---------------------
-- Expected output --
---------------------

-- On P1:
-- Database CS457_PA4 created.
-- Using database CS457_PA4.
-- Table Flights created.
-- 1 new record inserted.
-- 1 new record inserted.
-- Transaction starts.
-- 1 record modified.
-- Transaction committed.
-- seat int|status int
-- 22|1
-- 23|1
-- All done.
