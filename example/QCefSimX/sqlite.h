#pragma once
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtCore/QMutex>
#include <memory>
#include <mutex>

class SQLite
{
	static std::unique_ptr<SQLite> _instance;
	static std::mutex _mutex;

public:
	static SQLite& Instance() {
		static std::once_flag s_flag;
		std::call_once(s_flag, [&]() {
			_instance.reset(new SQLite);
		});

		return *_instance;
	}
	const QSqlDatabase& database() const { return m_db; }
	~SQLite();

private:
	SQLite();

	bool createConnection();      // 创建数据库连接

public:
	void close();                 // 关闭数据库

	// 查询方法
	QSqlQuery query(const QString& sql);

	// 增删改查方法
	bool execSQL(const QString& sql);                          // 执行SQL语句（增删改）
	QSqlQuery select(const QString& sql);                      // 查询
	bool insert(const QString& tableName, const QMap<QString, QVariant>& data);  // 插入
	bool update(const QString& tableName, const QMap<QString, QVariant>& data, const QString& where);  // 更新
	bool remove(const QString& tableName, const QString& where);  // 删除

	// 事务处理
	bool beginTransaction();      // 开始事务
	bool commitTransaction();     // 提交事务
	bool rollbackTransaction();   // 回滚事务

private:
	void requestAccessToken(void);                           // 加锁
	void releaseAccessToken(void);                           // 解锁

private:
	QSqlDatabase m_db;
	QMutex mutex;                 // 多线程资源访问控制
};
