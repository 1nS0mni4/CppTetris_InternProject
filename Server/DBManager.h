#pragma once
#include "pch.h"
#include "Defines.h"

#include <sql.h>
#include <sqlext.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <sal.h>

class DBManager {
	SINGLETON(DBManager);

public:
	void Init();
	RETCODE Execute(SQLWCHAR* query);
	void Close();

private:
	SQLHENV     hEnv = NULL;
	SQLHDBC     hDbc = NULL;
	SQLHSTMT    hStmt = NULL;
};

