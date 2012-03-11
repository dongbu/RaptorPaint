#include "glwindow.h"
#include <iostream>
#include <QMouseEvent>
#include <QApplication>

GLWindow::GLWindow(QWidget* parent) :
    QGLWidget(parent), stack(0)
{
    this->xOffset = 100.0;
    this->yOffset = 100.0;
    this->zoomFactor = 1.0;
}

void GLWindow::setImageStack(QMap<QString, QImage>* stack = 0)
{
    this->stack = stack;
}

void GLWindow::adjustOffset(double x, double y)
{
    this->xOffset += x;
    this->yOffset += y;
    this->repaint();
}

void GLWindow::adjustZoom(double amount)
{
    this->zoomFactor += amount / 10.0;
    this->repaint();
}

void GLWindow::mouseMoveEvent(QMouseEvent* e)
{
    if (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier))
    {
        static QPoint lastLocation;
        static bool lastLocationKnown = false;

        if (e->buttons().testFlag(Qt::LeftButton))
        {
            if (lastLocationKnown)
            {
                double dx = e->pos().x() - lastLocation.x();
                double dy = e->pos().y() - lastLocation.y();
                this->adjustOffset(dx, dy);
            }
            else
            {
                lastLocationKnown = true;
            }
            lastLocation = e->pos();
        }
        else
        {
            lastLocationKnown = false;
        }
    }
}

void GLWindow::initializeGL()
{
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void GLWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double w = (double)this->width();
    double h = (double)this->height();

    std::cout << w << std::endl;
    std::cout << h << std::endl;

    glOrtho(0, w, h, 0, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(this->xOffset, this->yOffset, 0);

    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
    glVertex3i(0, 0, 100);
    glVertex3i(3000, 0, 100);
    glVertex3i(3000, 3000, 100);
    glVertex3i(0, 3000, 100);
    glEnd();

    if (this->stack != 0)
    {
        QMap<QString, QImage>* images = this->stack;
        foreach (QString key, images->keys())
        {
            QImage gldata = QGLWidget::convertToGLFormat((*images)[key]);
            glDrawPixels(gldata.width(), gldata.height(), GL_RGBA, GL_UNSIGNED_BYTE, gldata.bits());
        }
    }
}