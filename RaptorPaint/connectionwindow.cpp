#include "connectionwindow.h"
#include "ui_connectionwindow.h"
#include <QMessageBox>

ConnectionWindow::ConnectionWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);

    this->connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    this->connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}

void ConnectionWindow::cancelClicked()
{
    ui->ipAddress->setText("");
    ui->userName->setText("");
    this->close();
    this->deleteLater();
}

void ConnectionWindow::connectClicked()
{
    if (!ui->ipAddress->text().length() == 0 && !ui->userName->text().length() == 0)
    {
        this->connectionReady(ui->userName->text(), ui->ipAddress->text());
        this->close();
        this->deleteLater();
    }
    else
    {
        QMessageBox msgbox("Error", "Must specify a server and username", QMessageBox::Critical, 0,0,0,this, Qt::Sheet);
        msgbox.setWindowTitle("Error");
        msgbox.setText("Must specifiy a server and username");
        msgbox.exec();
    }
}
