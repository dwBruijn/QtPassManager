#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("QtPassManager");

    connect(ui->pushButtonUnlock, SIGNAL(clicked()), this, SLOT(decrypt_db()));
    connect(ui->pushButtonSave, SIGNAL(clicked()), this, SLOT(save_db()));
    connect(ui->pushButtonBackup, SIGNAL(clicked()), this, SLOT(backup_db()));
    connect(ui->pushButtonAddEntry, SIGNAL(clicked()), this, SLOT(add_entry()));
    connect(ui->pushButtonRemoveEntry, SIGNAL(clicked()), this, SLOT(remove_entry()));
    connect(ui->pushButtonGeneratePassword, SIGNAL(clicked()), this, SLOT(generate_pass()));
    connect(ui->lineEditSearch, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));

    ui->lineEditEncryptionKey->setFocus();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::decrypt_db() {
    pass_manager.enc_key = ui->lineEditEncryptionKey->text();

    if(!pass_manager.load()) {
        ui->labelDatabaseInfo->setText("Database loaded");
        ui->labelEncryptionInfo->setText("Failed to decrypt database");
        return;
    }

    pass_manager.decrypt();

    ui->labelDatabaseInfo->setText("Database loaded");
    ui->labelEncryptionInfo->setText("Database decrypted");

    QVector<QVector<QString>> db = pass_manager.get_db_copy();
    ui->tableWidgetCredentials->clear();
    ui->tableWidgetCredentials->setRowCount(db.size());
    ui->tableWidgetCredentials->setColumnCount(4);

    for(unsigned int i = 0; i < db.size(); i++) {
        for(unsigned int j = 0; j < db[i].size(); j++) {
            ui->tableWidgetCredentials->setItem(i, j, new QTableWidgetItem(db[i][j]));
        }
    }

    db_changes_saved = true;

    ui->pushButtonUnlock->setEnabled(false);
    ui->lineEditEncryptionKey->setEnabled(false);

    ui->pushButtonBackup->setEnabled(true);
    ui->pushButtonSave->setEnabled(true);
    ui->pushButtonAddEntry->setEnabled(true);
    ui->pushButtonRemoveEntry->setEnabled(true);
    ui->pushButtonGeneratePassword->setEnabled(true);
    ui->spinBoxPasswordLength->setEnabled(true);
    ui->lineEditSearch->setEnabled(true);
    ui->tableWidgetCredentials->setEnabled(true);

    ui->tableWidgetCredentials->setHorizontalHeaderLabels({"Website", "Username", "Password", "Note"});
    ui->tableWidgetCredentials->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->lineEditSearch->setFocus();
}

void MainWindow::backup_db() {
    if(pass_manager.backup()) {
        ui->labelDatabaseInfo->setText("Database backup complete");
    } else {
        ui->labelDatabaseInfo->setText("Failed to backup database");
    }
}

void MainWindow::save_db() {
    pass_manager.clear_db();

    QVector<QString> row(4);
    for(int i = 0; i < ui->tableWidgetCredentials->rowCount(); i++) {
        for(int j = 0; j < 4; j++) {
            row[j] = ui->tableWidgetCredentials->item(i, j)->text();
        }
        pass_manager.add_entry(row[0], row[1], row[2], row[3]);
    }
    pass_manager.encrypt();
    pass_manager.save();

    db_changes_saved = true;
}

void MainWindow::add_entry() {
    int row_count = ui->tableWidgetCredentials->rowCount();

    ui->tableWidgetCredentials->insertRow(row_count);
    ui->tableWidgetCredentials->scrollToItem(ui->tableWidgetCredentials->takeItem(row_count, 0));
    ui->tableWidgetCredentials->setItem(row_count, 0, new QTableWidgetItem(""));
    ui->tableWidgetCredentials->setItem(row_count, 1, new QTableWidgetItem(""));
    ui->tableWidgetCredentials->setItem(row_count, 2, new QTableWidgetItem(pass_manager.generate_password(pass_len)));
    ui->tableWidgetCredentials->setItem(row_count, 3, new QTableWidgetItem(""));

    db_changes_saved = false;
}

void MainWindow::remove_entry() {
    QList<QTableWidgetItem*> selected = ui->tableWidgetCredentials->selectedItems();

    if(selected.size() > 0) {
        db_changes_saved = false;
    }

    int last_row = -1;
    for(int i = 0; i < selected.size(); i++) {
        if(last_row == selected[i]->row()) {
            selected.removeAt(i--);
            continue;
        }
        last_row = selected[i]->row();
    }

    for(QTableWidgetItem* item : selected) {
        ui->tableWidgetCredentials->removeRow(item->row());
    }
}

void MainWindow::generate_pass() {
    QList<QTableWidgetItem*> selected = ui->tableWidgetCredentials->selectedItems();

    if(selected.size() > 0) {
        db_changes_saved = false;
    }

    int last_row = -1;
    for(int i = 0; i < selected.size(); i++) {
        if(last_row == selected[i]->row()) {
            selected.removeAt(i--);
            continue;
        }
        last_row = selected[i]->row();
    }

    for(QTableWidgetItem* item: selected) {
        ui->tableWidgetCredentials->setItem(item->row(), 2, new QTableWidgetItem(pass_manager.generate_password(pass_len)));
    }
}

void MainWindow::search(const QString& input) {
    for(int i = 0; i < ui->tableWidgetCredentials->rowCount(); i++) {
        ui->tableWidgetCredentials->showRow(i);
    }

    if(input.size() == 0) {
        return;
    }

    for(int i = 0; i < ui->tableWidgetCredentials->rowCount(); i++) {
        if(!ui->tableWidgetCredentials->item(i, 0)->text().contains(input)) {
            ui->tableWidgetCredentials->hideRow(i);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* e) {
    if(db_changes_saved) {
        e->accept();
        return;
    }

    QMessageBox exit_msg_box;
    exit_msg_box.setIcon(QMessageBox::Warning);
    exit_msg_box.setText("You didn't save changes!");
    exit_msg_box.setInformativeText("Do you want to save before exiting?");
    exit_msg_box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    exit_msg_box.setDefaultButton(QMessageBox::Yes);

    switch(exit_msg_box.exec()) {
    case QMessageBox::Save:
        save_db();
        e->accept();
        break;
    case QMessageBox::Discard:
        e->accept();
        break;
    case QMessageBox::Cancel:
        e->ignore();
        break;
    default:
        e->ignore();
        break;
    }
}

void MainWindow::check_db() {
    if(pass_manager.db_exists()) {
        return;
    }

    QInputDialog input_dlg;
    input_dlg.resize(400, 200);
    input_dlg.setWindowTitle("Database Not Found");
    input_dlg.setLabelText("Enter password for your new database:");
    input_dlg.setTextEchoMode(QLineEdit::Password);

    if(input_dlg.exec() == 0) {
        close();
        return;
    }

    QString pwd = input_dlg.textValue();
    if(pwd.length() > 0) {
        pass_manager.enc_key = pwd;
        if(!pass_manager.create_tables()) {
            return;
        }
    }
    check_db();
}

void MainWindow::on_spinBoxPasswordLength_valueChanged(int len) {
    pass_len = len;
}

void MainWindow::on_tableWidgetCredentials_itemChanged() {
    db_changes_saved = false;
}

void MainWindow::on_tableWidgetCredentials_itemSelectionChanged() {
    QList<QTableWidgetItem*> selected = ui->tableWidgetCredentials->selectedItems();

    int count = 0;
    int last_row = -1;
    for(QTableWidgetItem* item : selected) {
        if(last_row != item->row()) {
            count++;
        }
        last_row = item->row();
    }

    if(count > 1) {
        ui->pushButtonRemoveEntry->setText("Remove Selected Entries");
    } else {
        ui->pushButtonRemoveEntry->setText("Remove Selected Entry");
    }
}
