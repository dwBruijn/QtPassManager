#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QTableWidgetItem>
#include <QHeaderView>
#include "passmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    // Function to check if the database exists, if not, create it
    void check_db();

protected:
    // Instance of PassManager
    PassManager pass_manager;
    // Indicates if all database changes has been saved
    bool db_changes_saved = true;
    // Used by the password generator
    int pass_len = 20;
    // Function to handle exit event
    void closeEvent(QCloseEvent*) override;

protected slots:
    // Function to handle decrypt user action
    void decrypt_db();
    // Function to handle backup DB user action
    void backup_db();
    // Function to handle save user action
    void save_db();
    // Function to handle add entry user action
    void add_entry();
    // Funciton to handle remove entry user action
    void remove_entry();
    // Function to handle generate password user action
    void generate_pass();
    // Function to handle search user action
    void search(const QString&);

private:
    Ui::MainWindow *ui;

private slots:
    // Function to handle spinbox password length value change
    void on_spinBoxPasswordLength_valueChanged(int);
    // Function to handle table entry change
    void on_tableWidgetCredentials_itemChanged();
    // Function to handle table entry selection change
    void on_tableWidgetCredentials_itemSelectionChanged();
};
#endif // MAINWINDOW_H
