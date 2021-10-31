#include "passmanager.h"

QString PassManager::db_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/PassManager.sqlite";
QString PassManager::characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ:;.,/=-+*<>{}()[]_%#$@!?^&";

PassManager::PassManager() : encryption(new QAESEncryption(QAESEncryption::AES_256, QAESEncryption::CBC)),
    db_manager(new DbManager(PassManager::db_path))
{}

bool PassManager::create_tables() {
    if(!db_manager->connect() || !db_manager->create_tables()) {
        return false;
    }

    QByteArray encrypted_password = enc_key.toLocal8Bit();

    for (int i = 0; i < ivs.size(); i++) {
        encrypted_password = encryption->encode(encrypted_password,
                                  QCryptographicHash::hash(enc_key.toLocal8Bit(), QCryptographicHash::Sha256),
                                  QCryptographicHash::hash((ivs[i] + enc_key).toLocal8Bit(), QCryptographicHash::Md5));
    }

    return db_manager->insert_key(encrypted_password);
}

void PassManager::save() const {
    if(!db_manager->clear()) {
        return;
    }

    for(QVector<QByteArray> entry : encrypted_entries) {
        db_manager->insert_creds(entry[0], entry[1], entry[2], entry[3]);
    }
}

bool PassManager::load() {
    QByteArray encrypted_key;
    QString decrypted_key;

    if(!db_exists()) {
        return false;
    }

    if(!db_manager->connect()) {
        return false;
    }

    encrypted_key = db_manager->read_key();
    if(encrypted_key.size() > 0) {
        for (int i = ivs.size() - 1; i >= 0; i--)
        {
            encrypted_key = encryption->decode(encrypted_key,
                                      QCryptographicHash::hash(enc_key.toLocal8Bit(), QCryptographicHash::Sha256),
                                      QCryptographicHash::hash((ivs[i] + enc_key).toLocal8Bit(), QCryptographicHash::Md5));
        }

        decrypted_key = QString(encrypted_key);
        decrypted_key.truncate(decrypted_key.length() % AES256_BLOCK_SIZE == 0 ?
                                   decrypted_key.length() : decrypted_key.indexOf(QChar::Null)-1);

        if(decrypted_key == enc_key) {
            encrypted_entries = db_manager->read_creds();

            return true;
        }
    }

    return false;
}

void PassManager::encrypt()
{
    encrypted_entries.clear();

    for(QVector<QString> entry : decrypted_entries) {
        QByteArray website = entry[0].toLocal8Bit();
        QByteArray username = entry[1].toLocal8Bit();
        QByteArray password = entry[2].toLocal8Bit();
        QByteArray note = entry[3].toLocal8Bit();

        for (int i = 0; i < ivs.size(); i++)
        {
            website = encryption->encode(website,
                                      QCryptographicHash::hash(enc_key.toLocal8Bit(), QCryptographicHash::Sha256),
                                      QCryptographicHash::hash((ivs[i] + enc_key).toLocal8Bit(), QCryptographicHash::Md5));
            username = encryption->encode(username,
                                      QCryptographicHash::hash(enc_key.toLocal8Bit(), QCryptographicHash::Sha256),
                                      QCryptographicHash::hash((ivs[i] + enc_key).toLocal8Bit(), QCryptographicHash::Md5));
            password = encryption->encode(password,
                                      QCryptographicHash::hash(enc_key.toLocal8Bit(), QCryptographicHash::Sha256),
                                      QCryptographicHash::hash((ivs[i] + enc_key).toLocal8Bit(), QCryptographicHash::Md5));
            note = encryption->encode(note,
                                      QCryptographicHash::hash(enc_key.toLocal8Bit(), QCryptographicHash::Sha256),
                                      QCryptographicHash::hash((ivs[i] + enc_key).toLocal8Bit(), QCryptographicHash::Md5));
        }

        encrypted_entries.push_back(QVector<QByteArray>{website, username, password, note});
    }
}

void PassManager::decrypt()
{
    decrypted_entries.clear();

    for(QVector<QByteArray> entry : encrypted_entries) {
        QByteArray website = entry[0], username = entry[1], password = entry[2], note = entry[3];

        for (int i = ivs.size() - 1; i >= 0; i--)
        {
            website = encryption->decode(website,
                                      QCryptographicHash::hash(enc_key.toLocal8Bit(), QCryptographicHash::Sha256),
                                      QCryptographicHash::hash((ivs[i] + enc_key).toLocal8Bit(), QCryptographicHash::Md5));
            username = encryption->decode(username,
                                      QCryptographicHash::hash(enc_key.toLocal8Bit(), QCryptographicHash::Sha256),
                                      QCryptographicHash::hash((ivs[i] + enc_key).toLocal8Bit(), QCryptographicHash::Md5));
            password = encryption->decode(password,
                                      QCryptographicHash::hash(enc_key.toLocal8Bit(), QCryptographicHash::Sha256),
                                      QCryptographicHash::hash((ivs[i] + enc_key).toLocal8Bit(), QCryptographicHash::Md5));
            note = encryption->decode(note,
                                      QCryptographicHash::hash(enc_key.toLocal8Bit(), QCryptographicHash::Sha256),
                                      QCryptographicHash::hash((ivs[i] + enc_key).toLocal8Bit(), QCryptographicHash::Md5));
        }

        QString web = QString(website);
        web.truncate(web.length() % AES256_BLOCK_SIZE == 0 ? web.length() : web.indexOf(QChar::Null)-1);
        QString user = QString(username);
        user.truncate(user.length() % AES256_BLOCK_SIZE == 0 ? user.length() : user.indexOf(QChar::Null)-1);
        QString pass = QString(password);
        pass.truncate(pass.length() % AES256_BLOCK_SIZE == 0 ? pass.length() : pass.indexOf(QChar::Null)-1);
        QString no = QString(note);
        no.truncate(no.length() % AES256_BLOCK_SIZE == 0 ? no.length() : no.indexOf(QChar::Null)-1);

        decrypted_entries.push_back(QVector<QString>{web, user, pass, no});
    }
}

bool PassManager::backup() const {
    if(!db_exists()) {
        return false;
    }

    QFile in_file(PassManager::db_path);
    in_file.open(QIODevice::ReadOnly);
    QByteArray data = in_file.readAll();
    in_file.close();

    QFile out_file(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/PassManagerBkp.sqlite");
    out_file.open(QIODevice::WriteOnly);
    out_file.write(data);
    out_file.close();

    return true;
}

void PassManager::add_entry(QString website, QString username, QString password, QString note) {
    QVector<QString> row{website, username, password, note};
    decrypted_entries.emplace_back(row);
}

void PassManager::remove_entry(int index) {
    decrypted_entries.erase(decrypted_entries.begin() + index);
}

void PassManager::update_entry(int index, QString website, QString username, QString password, QString note) {
    QVector<QString>& row = decrypted_entries[index];
    row[0] = website;
    row[1] = username;
    row[2] = password;
    row[3] = note;
}


void PassManager::clear_db() {
    decrypted_entries.clear();
}

QVector<QString> PassManager::get_entry_copy(int index) const {
    return decrypted_entries[index];
}

QVector<QVector<QString>> PassManager::get_db_copy() const {
    return decrypted_entries;
}

QString PassManager::generate_password(int pass_len) const {
    QRandomGenerator rnd = QRandomGenerator::securelySeeded();

    QString out = "";
    for(int i = 0; i < pass_len; i++) {
        out += PassManager::characters[rnd.bounded(0, PassManager::characters.length())];
    }

    return out;
}

bool PassManager::db_exists() const {
    return QFile::exists(PassManager::db_path);
}











