#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>

class DbManager
{
public:
    DbManager(const QString&);
    ~DbManager();

    // Function to validate the DB
    bool validate_db() const;
    // Function to open a connection to the DB
    bool connect();
    // Function to close the DB connection
    void close();
    // Function to create tables
    bool create_tables();

    // Function to insert DB encryption key (password)
    bool insert_key(QByteArray);
    // Function to read DB encryption key
    QByteArray read_key();

    // Function to insert creds into the DB
    bool insert_creds(QByteArray, QByteArray, QByteArray, QByteArray);
    // Function to read saved creds from the DB
    QVector<QVector<QByteArray>> read_creds();

    // Function to delete saved creds
    bool clear();


private:
    QString db_path;
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
