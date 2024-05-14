SELECT TOP (1000) [num]
  FROM [TetrisDB].[dbo].[test]

create table TetrisDB.dbo.userTbl(
name nvarchar(21) primary key not null);


create table dataTbl(
name nvarchar(21) not null foreign key references userTbl(name),
grade nvarchar(20) default('Newbie'),
score int default(0)
);

GO

drop procedure UserLogin;
create procedure UserLogin(@name nvarchar(21) = NULL) as begin
	DECLARE @count INT;
	DECLARE @score INT;

	select @count = COUNT(*) from TetrisDB.dbo.userTbl where name = @name;

	if @count = 0
	begin
		insert into TetrisDB.dbo.userTbl(name) values(@name);
		insert into TetrisDB.dbo.dataTbl(name, grade, score) values(@name, 'Newbie', 0);
	end

	select @score = score from TetrisDB.dbo.dataTbl where name = @name;

	return @score;
end

GO

select score from TetrisDB.dbo.dataTbl where name = 'test1';


create procedure UpdateScore(@name nvarchar(21) = NULL, @score INT = 0) as begin
	UPDATE TetrisDB.dbo.dataTbl SET score += @score WHERE name = @name;

	DECLARE @result INT;
	DECLARE @grade nvarchar(50);

	select @grade = 
	case 
	when score >= 50000 then 'MASTER'
	when score >= 40000 then 'DIAMOND'
	when score >= 30000 then 'GOLD'
	when score >= 20000 then 'SILVER'
	ELSE 'BRONZE'
	end
	from dataTbl 
	where name = @name;

	UPDATE dataTbl SET grade = @grade where name = @name;
end
GO

drop procedure UpdateScore;

drop table userTbl;
drop table dataTbl;
select* from userTbl;
select* from dataTbl;


exec UserLogin 'test1';

exec UpdateScore 'test1', 10000;
