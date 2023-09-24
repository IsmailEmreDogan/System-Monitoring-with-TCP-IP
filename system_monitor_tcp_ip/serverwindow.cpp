#include "serverwindow.h"
#include "ui_serverwindow.h"

serverwindow::serverwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::serverwindow)
{
    ui->setupUi(this);
}

serverwindow::~serverwindow()
{
    delete ui;
}
