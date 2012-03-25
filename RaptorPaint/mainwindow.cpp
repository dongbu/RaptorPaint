#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connectionwindow.h"

#include <QDebug>
#include <QRgb>
#include <iostream>
#include <qmath.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cm(new ConnectionManager),
    muted(QIcon(":/userIcons/muted.png")),
    unmuted(QIcon(":/userIcons/unmuted.png"))
{
    ui->setupUi(this);

    this->cm->myImage()->fill(0x00FF0000);

    this->connect(this->cm, SIGNAL(gotTextMessage(QString)), this, SLOT(gotTextMessage(QString)));
    this->connect(ui->actionConnect_Host, SIGNAL(triggered()), this, SLOT(mnuConnect()));
    this->connect(ui->input, SIGNAL(returnPressed()), this, SLOT(txtInputReturnPressed()));

    this->connect(this->cm, SIGNAL(userJoined(QString)), this, SLOT(userJoined(QString)));
    this->connect(this->cm, SIGNAL(userLeft(QString)), this, SLOT(userLeft(QString)));

    this->connect(ui->userList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(userMuteToggle(QListWidgetItem*)));

    ui->mnuIn->setShortcut(QKeySequence("Ctrl+="));
    ui->mnuOut->setShortcut(QKeySequence("Ctrl+-"));
    ui->mnuActual->setShortcut(QKeySequence("Ctrl+0"));

    this->connect(ui->mnuIn, SIGNAL(triggered()), ui->paintArea, SLOT(zoomIn()));
    this->connect(ui->mnuOut, SIGNAL(triggered()), ui->paintArea, SLOT(zoomOut()));
    this->connect(ui->mnuActual, SIGNAL(triggered()), ui->paintArea, SLOT(zoomActual()));

    ui->paintArea->setImageStack(this->cm->getLayerPtr());

    this->connect(ui->paintArea, SIGNAL(drawHere(double,double)), this, SLOT(drawHere(double,double)));

    this->wasDragging = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawHere(double x, double y)
{
    static double lx, ly;

    QImage* img = this->cm->myImage();
    if (x < 0 || x > WIDTH || y < 0 || y > HEIGHT)
    {
        return;
    }
    if (img->isNull())
    {
        return;
    }

    QPainter painter(img);

    if (wasDragging)
    {
        double cx = x - lx;
        double cy = y - ly;
        double cc = qSqrt(cx * cx + cy * cy); // total distance between where we are now and where we were
        double dx = cx / cc;
        double dy = cy / cc;

        for (double cp = 0; cp < cc; cp++)
        {
            changeCanvas(x - dx * cp, y - dy * cp, &painter);
        }
    }
    else
    {
        this->wasDragging = true;
        changeCanvas(x, y, &painter);
    }

    lx = x;
    ly = y;
}

void MainWindow::changeCanvas(double x, double y, QPainter* painter)
{
    QRectF r(-this->toolSize / 2.0, -this->toolSize / 2.0, this->toolSize / 2.0, this->toolSize / 2.0);
    switch (this->selectedTool)
    {
    case BRUSH:
    case PENCIL:
    case ERASER:
    case TYPE:
    default:
        QRect r(x, y, 10, 10);
        painter->setBrush(Qt::SolidPattern);
        painter->setPen(QColor::fromRgb(0,0,0));
        painter->drawEllipse(r);
        break;
    }
}

void MainWindow::txtInputReturnPressed()
{
    this->cm->sendTextMessage(ui->input->text());
    ui->input->setText("");
}

void MainWindow::userMuteToggle(QListWidgetItem *item)
{
    QString user = item->text();
    bool mute = this->cm->toggleMute(user);
    item->setIcon(mute ? this->muted : this->unmuted);
}

void MainWindow::mnuConnect()
{
    this->cm->openConnectionWindow();
}

void MainWindow::userJoined(QString name)
{
    qDebug("Join");
    qDebug(name.toAscii().data());
    if (this->listItems.contains(name)) return;
    QListWidgetItem* item = new QListWidgetItem(this->unmuted, name, ui->userList);
    this->listItems[name] = item;
}

void MainWindow::userLeft(QString name)
{
    qDebug("Leave");
    qDebug(name.toAscii().data());
    if (this->listItems.contains(name))
    {
        QListWidgetItem* item = this->listItems[name];
        this->listItems.remove(name);
        delete item;
    }
}

// Got message from server
void MainWindow::gotTextMessage(QString msg)
{
    if(msg.startsWith(QString("[%1]").arg(this->cm->getName())))
    {
        msg = msg.prepend("<p class=\"me\">").append("</p>");
    }
    ui->chatLog->append(msg);
}
