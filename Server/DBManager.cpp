#include "pch.h"
#include "DBManager.h"

#define TRYODBC(h, ht, x)   {   RETCODE rc = x;\
                                if (rc != SQL_SUCCESS) \
                                { \
                                    /*HandleDiagnosticRecord (h, ht, rc);*/ \
                                } \
                                if (rc == SQL_ERROR) \
                                { \
                                    /*fwprintf(stderr, L"Error in " L#x L"\n");*/ \
                                }  \
                            }

DBManager::DBManager() {
	outConnString = new SQLWCHAR[1024];
}

DBManager::~DBManager() {
	delete outConnString;
}

void DBManager::Init() {
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_ERROR) {
		fwprintf(stderr, L"Unable to allocate an environment handle\n");
		exit(-1);
	}

	// Register this as an application that expects 3.x behavior,
	// you must register something if you use AllocHandle
	{
		RETCODE rc = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
		if (rc != SQL_SUCCESS) {
			/*HandleDiagnosticRecord(hEnv, SQL_HANDLE_ENV, rc);*/

		} if (rc == SQL_ERROR) {
			fwprintf((__acrt_iob_func(2)), L"Error in " L"SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0)" L"\n");
		}
	}

	// Allocate a connection
	{
		RETCODE rc = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
		if (rc != SQL_SUCCESS) {
			//HandleDiagnosticRecord(hEnv, SQL_HANDLE_ENV, rc);
		} if (rc == SQL_ERROR) {
			fwprintf((__acrt_iob_func(2)), L"Error in " L"SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc)" L"\n");
		}
	}

	// Connect to the driver.  Use the connection string if supplied
	// on the input, otherwise let the driver manager prompt for input.
	{
		SQLSMALLINT outConnStrSz = 1024;
		RETCODE rc = SQLDriverConnect(hDbc, GetDesktopWindow(), connString, _countof(connString), outConnString, 1024, &outConnStrSz, SQL_DRIVER_COMPLETE);
		if (rc != SQL_SUCCESS) {
			//HandleDiagnosticRecord(hDbc, SQL_HANDLE_DBC, rc);
		} if (rc == SQL_ERROR) {
			fwprintf((__acrt_iob_func(2)), L"Error in " L"SQLDriverConnect(hDbc, GetDesktopWindow(), pwszConnStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE)" L"\n");
		}
	}

	{
		RETCODE rc = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
		if (rc != SQL_SUCCESS) {
			//HandleDiagnosticRecord(hDbc, SQL_HANDLE_DBC, rc);
		} if (rc == SQL_ERROR) {
			fwprintf((__acrt_iob_func(2)), L"Error in " L"SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)" L"\n");
		}
	}

	RegisterQuery();
}

RETCODE DBManager::Execute(wstring query) {
	SQLSMALLINT sNumResults;

	RETCODE retCode = SQLExecDirect(hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

	TRYODBC(hStmt,
			SQL_HANDLE_STMT,
			SQLFreeStmt(hStmt, SQL_CLOSE));

	cout << "Result : " << retCode << "\n\n";
	return retCode;
}

void DBManager::Close() {
	// Free ODBC handles and exit

	if (hStmt) {
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	}

	if (hDbc) {
		SQLDisconnect(hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	}

	if (hEnv) {
		SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	}
}

wstring DBManager::GetQuery(QueryType type) {

	if (GetInstance()._queries.size() <= 0)
		return L"";

	return GetInstance()._queries[type];
}

void DBManager::RegisterQuery() {
	_queries[QueryType::UserLogin] = L"exec TetrisDB.dbo.UserLogin";
	_queries[QueryType::UpdateScore] = L"exec TetrisDB.dbo.UpdateScore";

}
