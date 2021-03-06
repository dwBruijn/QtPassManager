#ifndef PASSMANAGER_H
#define PASSMANAGER_H

#include <iostream>
#include <vector>
#include <QString>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QFile>
#include <QStandardPaths>
#include <QRandomGenerator>
#include <QStringList>
#include "qaesencryption.h"
#include "dbmanager.h"

#define AES256_BLOCK_SIZE 16

class PassManager {
public:
    PassManager();

    // DB encryption key = "";
    QString enc_key;

    // Function to create DB tables and store encrypted DB password
    bool create_tables();
    // Function to write the DB the file pointed to by db_path
    void save() const;
    // Function to load the DB from the file pointed to by dp_path
    bool load();
    // Function to encrypt DB entries
    void encrypt();
    // Function to decrypt encrypted DB entries
    void decrypt();
    // Function to backup the DB to a location on the disk
    bool backup() const;
    // Function to add a new entry to the DB
    void add_entry(QString, QString, QString, QString);
    // Function to remove a remove an entry from the DB
    void remove_entry(int);
    // Function to update DB entry
    void update_entry(int, QString, QString, QString, QString);
    // Function to clear the DB
    void clear_db();
    // Function to get a copy of an DB entry
    QVector<QString> get_entry_copy(int) const;
    // Function to get a copy of the DB
    QVector<QVector<QString>> get_db_copy() const;
    // Function to generate a password based on the specified length
    QString generate_password(int) const;
    // Function to check if the DB exists on disk
    bool db_exists() const;

protected:
    // DB path
    static QString db_path;
    // QAESEncryption instance
    QAESEncryption* encryption;
    // DbManager instance
    DbManager* db_manager;
    // Encrypted DB content
    QVector<QVector<QByteArray>> encrypted_entries;
    // Decrypted DB entries
    QVector<QVector<QString>> decrypted_entries;
    // String of characters to be used for password generation
    static QString characters;
    // List of IVs used for AES encryption
    QStringList ivs = {
            "jW2jT]%0k2#-2R1.d(7'6V0Z|4=-HX2G9@F;561.07@21,NHq42)*896M(18R+9w080*Hs^,45G?;]5R7}'*0Z67?Y7|%SFI**0g",
            ")OR2*711+M)*a,5D/qB}/#]|fN*30oA<#;>]B80>,4J9@<<;J5;#wL*]p$G9D0i1860;Y<I*.)0diN/yi*08;^hJ596]2NY9&+#l",
            "J>}b;|h)]6h6.O(n(*R)j87WrfZJ%>DM635u68i/s7@<*=/[V562&46E}P)/+8pZ$0c/3)690A/t.8eJ+4gJxf1&q?99%AP{)c71",
            "6J00^0cZ-X16a=Z+K<_{8J34#1@9*Tb@3B4_^5'k0+YY[^--IZ71y0^62T&w>9}o738zAj7p1.%NA<Y[I08'z02H06;DK88p(I0z",
            ",<4t/*8[$;6clb/yo1f3*$iq]1{6L5@861(&s*7(Y44<85r8<9R._&4})0k|=41)dh*Xf)aL-/&8T&72704<!=v[h5i21^]Jj1i6",
            "z@9;[k-lkl^bQ6?*30R5kU]O9.](]M}'9N'6|;;2Mmq!OV18|BS]f06c_F=>)+3^246X|F61I3<+I.8Z3tE4,HE_56o{[*7;*rf^",
            "@F7L-7$;'h*|?0io3Zo}063)193|3'o/6-E+877Tr[vW87sNc8q<6,69-IS72cz6|2I/p{I8D6W520/$&6*Y3&%v,*0/85(ikK!J",
            "72|/3;10O@!(|t)]8}02Gf126N)+6*w6e2P57/dWd*JW5+aR?h31e/=4o5<H@Uzu2<;71Qd63B{k9-PQ_41[5yA'e^?2A27$[8-7",
            "H(6>8!$l9!4?6_,7L%4}z3Lu8;sb^q}9%lVy6I57L]8<,-ho?310Dd_h|y1#iz%3]rN'zr5T1Bc2uQ5cb!K39386)50c0+%.w.X'",
            "2-n$rL3v4T/*/22F%2tN}.yDX78#50z3Z9-B10X5*4]97+R-OK'2^F%7$>95c8jLu531C==1|V7Cd=o;5L6/B17jF2C9<1]R4'DY",
        };
};

#endif // PASSMANAGER_H
