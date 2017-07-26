#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QSqlQuery>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fieldList = "Gene,Chromosome,Sequence,SNP,miRNA,AlleleNum,Alleles,\"MAF_>0.05\",Location,Mutation_ProteinPos,Mutation_Type";
    comboBoxSelection = "Gene";
    geneOn = true;
    miRNAON = false;
    snpON = false;

    miRNA_SNP_Database = QSqlDatabase::addDatabase("QSQLITE");
    miRNA_SNP_Database.setDatabaseName("miRNA_SNP.sqlite");
    miRNA_SNP_Database.open();
    tables = miRNA_SNP_Database.tables(QSql::Tables);

    model = new QSqlQueryModel;
    query = "";
    for (int i = 0; i < tables.size(); i++){
        query += "SELECT " + fieldList + " FROM " + tables.at(i) + " ";
        query += "UNION ALL ";
    }
    query.remove(query.size()-11,11);
    //    qDebug() << query;
    model->setQuery(query);

    for(int i=0;i<4;i++)
        model->fetchMore();

    model->setHeaderData(7, Qt::Horizontal, tr("MAF >0.05"));
    model->setHeaderData(8, Qt::Horizontal, tr("SNP Location"));
    model->setHeaderData(9, Qt::Horizontal, tr("Residue Number"));
    model->setHeaderData(10, Qt::Horizontal, tr("Mutation Type"));
    ui->mainTable->setModel(model);
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
    query = "";
    for (int i = 0; i < tables.size(); i++){
        query += "SELECT " + fieldList + " FROM " + tables.at(i) + " ";
        if(geneOn){
            query += "WHERE Gene LIKE '%" + searchParam + "%' ";
        }else if(miRNAON){
            query += "WHERE miRNA LIKE '%" + searchParam + "%' ";
        }else if(snpON){
            query += "WHERE SNP LIKE '%" + searchParam + "%' ";
        }
        query += "UNION ALL ";
    }
    query.remove(query.size()-11,11);
    //    qDebug() << query;
    model->setQuery(query);

    for(int i=0;i<4;i++)
        model->fetchMore();

    ui->mainTable->setModel(model);
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
    if(comboBoxSelection.compare("Gene")==0){
        geneOn = true;
        miRNAON = false;
        snpON = false;
    }else if(comboBoxSelection.compare("miRNA")==0){
        geneOn = false;
        miRNAON = true;
        snpON = false;
    }else if(comboBoxSelection.compare("SNP")==0){
        geneOn = false;
        miRNAON = false;
        snpON = true;
    }
}

void MainWindow::on_resetButton_clicked()
{
    query = "";
    for (int i = 0; i < tables.size(); i++){
        query += "SELECT " + fieldList + " FROM " + tables.at(i) + " ";
        query += "UNION ALL ";
    }
    query.remove(query.size()-11,11);
    model->setQuery(query);
    ui->searchBar->setText("");

    for(int i=0;i<4;i++)
        model->fetchMore();

    ui->mainTable->setModel(model);
    rowResults = model->rowCount();
    if(rowResults<=1000)
        ui->resultsOutput->setText(QString::number(rowResults));
    else
        ui->resultsOutput->setText(">1000");
}
