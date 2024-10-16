#pragma once
#include "pch.h"
#include "Defines.h"

enum class QueryType : USHORT {
	UserLogin,
	UpdateScore,
};

class DBManager {
	SINGLETON(DBManager);

public:


public:
	void Init();
	RETCODE Execute(wstring query);
	void Close();

public:
	static wstring GetQuery(QueryType type);

protected:
	void RegisterQuery();

private:
	SQLHENV     hEnv = NULL;
	SQLHDBC     hDbc = NULL;
	SQLHSTMT    hStmt = NULL;
	SQLWCHAR* outConnString;
	SQLWCHAR connString[155] = L"DRIVER=SQL Server; UID=dgwe; WSID=DESKTOP-1QNQQDF; APP=Microsoft�� Windows�� Operating System; Trusted_Connection=Yes; SERVER=DESKTOP-1QNQQDF;";
	SQLWCHAR connStringHome[155] = L"DRIVER=ODBC Driver 17 for SQL Server;TrustServerCertificate=Yes; WSID=D-PERA;APP=Microsoft�� Windows�� Operating System;Trusted_Connection=Yes;SERVER=D-pera";
	map<QueryType, wstring> _queries;
};

