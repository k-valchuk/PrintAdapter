#pragma once

class DBTransaction
{
public:
	DBTransaction(QSqlDatabase &_db)
	:db(_db), isRollback(true)
	{
		db.transaction();
	};

	~DBTransaction()
	{ 
		if (isRollback) 
			db.rollback();
	};

	bool commit() { isRollback = false;  return db.commit(); };

private:
	QSqlDatabase &db;
	bool isRollback;
};
