#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_searchButton_clicked();

    void on_searchBar_textChanged(const QString &arg1);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_resetButton_clicked();

private:
    Ui::MainWindow *ui;
    QString searchParam;
    QSqlDatabase miRNA_SNP_Database;
    QSqlQueryModel *model;
    QString query;
    QString fieldList;
    QStringList tables;
    QString chromosomeSearch;
    QString comboBoxSelection;
    bool geneOn;
    bool miRNAON;
    bool snpON;
    int rowResults;
};

#endif // MAINWINDOW_H
