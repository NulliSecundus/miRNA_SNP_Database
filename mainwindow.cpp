#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QSqlQuery>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
    this->statusBar()->setSizeGripEnabled(false);

    ui->mainTable->setAlternatingRowColors(true);
    ui->mainTable->setSortingEnabled(true);

    fieldList = "Gene,Chromosome,Sequence,SNP,miRNA,AlleleNum,Alleles,\"MAF_>0.05\",Location,Mutation_ProteinPos,\"Mutation_Allele-Residue\",Mutation_Type";
    comboBoxSelection = "Gene";
    optionChar = "%";
    mutType = "";
    snpLocation = "";

    miRNA_SNP_Database = QSqlDatabase::addDatabase("QSQLITE");
    miRNA_SNP_Database.setDatabaseName("miRNA_SNP_Data.sqlite");
    miRNA_SNP_Database.open();
    tables = miRNA_SNP_Database.tables(QSql::Tables);

    model = new QSqlQueryModel;
    query = "SELECT " + fieldList + " FROM miRNA_SNP_Data";
    model->setQuery(query);

    for(int i=0;i<4;i++)
        model->fetchMore();

    model->setHeaderData(7, Qt::Horizontal, tr("MAF >0.05"));
    model->setHeaderData(8, Qt::Horizontal, tr("SNP Location"));
    model->setHeaderData(9, Qt::Horizontal, tr("Residue Number"));
    model->setHeaderData(10, Qt::Horizontal, tr("Allele-Residue"));
    model->setHeaderData(11, Qt::Horizontal, tr("Mutation Type"));

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(model);
    ui->mainTable->setModel(proxyModel);
    QRect rect = ui->mainTable->geometry();
    colWidth = ui->mainTable->columnWidth(0);
    rect.setWidth(20 + ui->mainTable->verticalHeader()->width() + colWidth * 12);
    ui->mainTable->setGeometry(rect);
    rowResults = model->rowCount();
    if(rowResults<=1000)
        ui->resultsOutput->setText(QString::number(rowResults));
    else
        ui->resultsOutput->setText(">1000");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_searchButton_clicked()
{
    query = "SELECT " + fieldList + " FROM miRNA_SNP_Data ";
    query += "WHERE " + comboBoxSelection + " LIKE '" + optionChar + searchParam + optionChar + "' ";
    query += "AND Location LIKE '%" + snpLocation + "%' ";
    query += "AND Mutation_Type LIKE '%" + mutType + "%' ";
    //    qDebug() << query;
    model->setQuery(query);

    for(int i=0;i<4;i++)
        model->fetchMore();

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(model);
    ui->mainTable->setModel(proxyModel);
    rowResults = model->rowCount();
    if(rowResults<=1000)
        ui->resultsOutput->setText(QString::number(rowResults));
    else
        ui->resultsOutput->setText(">1000");
}

void MainWindow::on_searchBar_textChanged(const QString &arg1)
{
    searchParam = arg1;
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    comboBoxSelection = arg1;
}

void MainWindow::on_resetButton_clicked()
{
    query = "SELECT " + fieldList + " FROM miRNA_SNP_Data";
    model->setQuery(query);
    ui->searchBar->setText("");
    ui->comboBox->setCurrentIndex(0);
    ui->comboBox_2->setCurrentIndex(0);
    ui->locationComboBox->setCurrentIndex(0);
    ui->mutTypeComboBox->setCurrentIndex(0);

    for(int i=0;i<4;i++)
        model->fetchMore();

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(model);
    ui->mainTable->setModel(proxyModel);
    for(int i = 0; i < model->columnCount(); i++){
        ui->mainTable->setColumnWidth(i,colWidth);
    }

    rowResults = model->rowCount();
    if(rowResults<=1000)
        ui->resultsOutput->setText(QString::number(rowResults));
    else
        ui->resultsOutput->setText(">1000");
}

void MainWindow::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare("Contains")==0){
        optionChar = "%";
    }else if(arg1.compare("Matches")==0){
        optionChar = "";
    }
}

void MainWindow::on_locationComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare("All")==0){
        snpLocation = "";
    }else if(arg1.compare("5UTR")==0){
        snpLocation = "5UTR";
    }else if(arg1.compare("Gene")==0){
        snpLocation = "gene";
    }else if(arg1.compare("3UTR")==0){
        snpLocation = "3UTR";
    }
}

void MainWindow::on_mutTypeComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare("All")==0){
        mutType = "";
    }else if(arg1.compare("Syn")==0){
        mutType = "Syn";
    }else if(arg1.compare("Mis")==0){
        mutType = "Mis";
    }
}

void MainWindow::on_exportResultsButton_clicked()
{
    if(rowResults <= 1000){
        QString filename = QFileDialog::getSaveFileName(this, tr("Export File"), QString(), tr("CSV Files (*.csv)"));
        QFile file(filename);
        if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
            QTextStream output(&file);
            output << fieldList << "\r\n";
            file.close();
        }
        if(file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QString textData;
            int columns = model->columnCount();
            for (int i = 0; i < rowResults; i++) {
                textData = "";
                for (int j = 0; j < columns; j++) {
                    textData += model->data(model->index(i,j)).toString();
                    textData += ", ";
                }
                textData += "\r\n";
                QTextStream output(&file);
                output << textData;
            }
            file.close();
            QMessageBox::information(this, "Success", "Search results have been exported");
        }
    }else{
        //system dialog: too much data to export
        QMessageBox::critical(this, tr("Error"), "Cannot export >1000 results");
    }
}
