#include "dbmanager.h"

DbManager::DbManager(const QString& path) : db_path(path) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(db_path);

}

DbManager::~DbManager() {
    close();
}

bool DbManager::validate_db() const {
     return m_db.tables().contains(QLatin1String("keys")) &&
            m_db.tables().contains(QLatin1String("creds"));
}

bool DbManager::connect() {
    return m_db.open();
}

void DbManager::close() {
    m_db.close();
}

bool DbManager::create_tables() {
    if(!m_db.isOpen()) {
        return false;
    }

    if(validate_db()) {
        return true;
    }

    QSqlQuery query;
    QString query_string = "CREATE TABLE keys"
                           "(id INTEGER PRIMARY KEY, "
                           "key BLOB)";

    query.prepare(query_string);
    if(!query.exec()) {
        return false;
    }

    query_string = "CREATE TABLE creds"
                           "(id INTEGER PRIMARY KEY, "
                           "website BLOB, "
                           "username BLOB, "
                           "password BLOB, "
                           "note BLOB)";

    query.prepare(query_string);
    if(!query.exec()) {
        return false;
    }

   return true;
}

bool DbManager::insert_key(QByteArray encrypted_password) {
    QSqlQuery query;
    QString query_string = "INSERT INTO keys (key) VALUES (:key)";

    query.prepare(query_string);
    query.bindValue(":key", encrypted_password);
    if(!query.exec()) {
        return false;
    }

    return true;
}

QByteArray DbManager::read_key() {
    QSqlQuery query;
    QString query_string = "SELECT key FROM keys WHERE id = 1";

    query.prepare(query_string);
    if(!query.exec()) {
        return QByteArray();
    }

    while(query.next()) {
        return QByteArray(query.value("key").toByteArray());
    }

    return QByteArray();
}

bool DbManager::insert_creds(QByteArray website, QByteArray username, QByteArray password, QByteArray note) {
    QSqlQuery query;
    QString query_string = "INSERT INTO creds (website, username, password, note) VALUES "
                           "(:website, :username, :password, :note)";

    query.prepare(query_string);
    query.bindValue(":website", website);
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":note", note);

    if(!query.exec()) {
        return false;
    }

    return true;
}

QVector<QVector<QByteArray>> DbManager::read_creds() {
    QVector<QVector<QByteArray>> creds{};

    QSqlQuery query;
    QString query_string = "SELECT website, username, password, note FROM creds";

    query.prepare(query_string);
    if(!query.exec()) {
        return creds;
    }

    while(query.next()) {
        creds.push_back(QVector<QByteArray>{query.value("website").toByteArray(), query.value("username").toByteArray(),
                        query.value("password").toByteArray(), query.value("note").toByteArray()});
    }

    return creds;
}

bool DbManager::clear() {
    QSqlQuery query;
    QString query_string = "DELETE FROM creds";

    query.prepare(query_string);
    if(!query.exec()) {
        return false;
    }

    return true;
}
