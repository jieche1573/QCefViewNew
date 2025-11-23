#include "sqlite.h"
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>

std::unique_ptr<SQLite> SQLite::_instance;
std::mutex SQLite::_mutex;

SQLite::SQLite()
{
	createConnection();
}

SQLite::~SQLite()
{
	close();
}

bool SQLite::createConnection()
{
	// 创建数据库连接，使用 QSQLITE 驱动
	m_db = QSqlDatabase::addDatabase("QSQLITE", "sqlite");

	// 设置数据库文件路径，默认为应用程序目录下的 data.db
	QString dbPath = QCoreApplication::applicationDirPath() + "/simulation.db";

	// 也可以从配置文件读取路径
	// QString dbPath = Utils::Instance().SQLite_DatabasePath;

	m_db.setDatabaseName(dbPath);

	if (m_db.open())
	{
		qDebug() << "SQLite database opened successfully:" << dbPath;
		return true;
	}
	else
	{
		qDebug() << "SQLite database open error:" << m_db.lastError().text();
		return false;
	}
}

void SQLite::close()
{
	if (m_db.isOpen())
	{
		m_db.close();
		qDebug() << "SQLite database closed";
	}
}

void SQLite::requestAccessToken(void)
{
	mutex.lock();  // 如果已经 lock 且还未 unlock，一直等待，直到 unlock，然后 lock
}

void SQLite::releaseAccessToken(void)
{
	mutex.unlock();
}

QSqlQuery SQLite::query(const QString& sql)
{
	requestAccessToken();
	QSqlQuery q(m_db);
	q.exec(sql);
	releaseAccessToken();
	return q;
}

bool SQLite::execSQL(const QString& sql)
{
	requestAccessToken();
	QSqlQuery q(m_db);
	bool result = q.exec(sql);

	if (!result)
	{
		qDebug() << "SQL execution failed:" << q.lastError().text();
		qDebug() << "SQL:" << sql;
	}
	else
	{
		qDebug() << "SQL executed successfully:" << sql;
	}

	releaseAccessToken();
	return result;
}

QSqlQuery SQLite::select(const QString& sql)
{
	requestAccessToken();
	QSqlQuery q(m_db);
	q.exec(sql);

	if (q.lastError().isValid())
	{
		qDebug() << "Select query failed:" << q.lastError().text();
		qDebug() << "SQL:" << sql;
	}

	releaseAccessToken();
	return q;
}

bool SQLite::insert(const QString& tableName, const QMap<QString, QVariant>& data)
{
	if (data.isEmpty())
	{
		qDebug() << "Insert failed: data is empty";
		return false;
	}

	requestAccessToken();

	// 构建 INSERT SQL 语句
	QString fields;
	QString placeholders;
	QList<QVariant> values;

	QMapIterator<QString, QVariant> iter(data);
	while (iter.hasNext())
	{
		iter.next();
		if (!fields.isEmpty())
		{
			fields += ", ";
			placeholders += ", ";
		}
		fields += iter.key();
		placeholders += "?";
		values.append(iter.value());
	}

	QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
		.arg(tableName)
		.arg(fields)
		.arg(placeholders);

	QSqlQuery q(m_db);
	q.prepare(sql);

	// 绑定参数
	for (const QVariant& value : values)
	{
		q.addBindValue(value);
	}

	bool result = q.exec();

	if (!result)
	{
		qDebug() << "Insert failed:" << q.lastError().text();
		qDebug() << "SQL:" << sql;
	}
	else
	{
		qDebug() << "Insert successful into table:" << tableName;
	}

	releaseAccessToken();
	return result;
}

bool SQLite::update(const QString& tableName, const QMap<QString, QVariant>& data, const QString& where)
{
	if (data.isEmpty())
	{
		qDebug() << "Update failed: data is empty";
		return false;
	}

	requestAccessToken();

	// 构建 UPDATE SQL 语句
	QString setClause;
	QList<QVariant> values;

	QMapIterator<QString, QVariant> iter(data);
	while (iter.hasNext())
	{
		iter.next();
		if (!setClause.isEmpty())
		{
			setClause += ", ";
		}
		setClause += QString("%1 = ?").arg(iter.key());
		values.append(iter.value());
	}

	QString sql = QString("UPDATE %1 SET %2").arg(tableName).arg(setClause);

	if (!where.isEmpty())
	{
		sql += " WHERE " + where;
	}

	QSqlQuery q(m_db);
	q.prepare(sql);

	// 绑定参数
	for (const QVariant& value : values)
	{
		q.addBindValue(value);
	}

	bool result = q.exec();

	if (!result)
	{
		qDebug() << "Update failed:" << q.lastError().text();
		qDebug() << "SQL:" << sql;
	}
	else
	{
		qDebug() << "Update successful, rows affected:" << q.numRowsAffected();
	}

	releaseAccessToken();
	return result;
}

bool SQLite::remove(const QString& tableName, const QString& where)
{
	requestAccessToken();

	QString sql = QString("DELETE FROM %1").arg(tableName);

	if (!where.isEmpty())
	{
		sql += " WHERE " + where;
	}

	QSqlQuery q(m_db);
	bool result = q.exec(sql);

	if (!result)
	{
		qDebug() << "Delete failed:" << q.lastError().text();
		qDebug() << "SQL:" << sql;
	}
	else
	{
		qDebug() << "Delete successful, rows affected:" << q.numRowsAffected();
	}

	releaseAccessToken();
	return result;
}

bool SQLite::beginTransaction()
{
	requestAccessToken();
	bool result = m_db.transaction();
	releaseAccessToken();

	if (result)
	{
		qDebug() << "Transaction started";
	}
	else
	{
		qDebug() << "Failed to start transaction:" << m_db.lastError().text();
	}

	return result;
}

bool SQLite::commitTransaction()
{
	requestAccessToken();
	bool result = m_db.commit();
	releaseAccessToken();

	if (result)
	{
		qDebug() << "Transaction committed";
	}
	else
	{
		qDebug() << "Failed to commit transaction:" << m_db.lastError().text();
	}

	return result;
}

bool SQLite::rollbackTransaction()
{
	requestAccessToken();
	bool result = m_db.rollback();
	releaseAccessToken();

	if (result)
	{
		qDebug() << "Transaction rolled back";
	}
	else
	{
		qDebug() << "Failed to rollback transaction:" << m_db.lastError().text();
	}

	return result;
}
