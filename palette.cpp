#include "palette.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <QGuiApplication>
#include <QStyleHints>


Palette::Palette(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    //setWidth(150);
    //setHeight(150);

    // 1. Enable left mouse button events
    setAcceptedMouseButtons(Qt::LeftButton);

    // 2. Enable hover events (optional)
    setAcceptHoverEvents(true);

    //-- Fill colors table
    for (int i = 0; i < nbRows; i++) {
        for (int j = 0; j < nbColumns; j++) {
            int x = 2 + 2*cellSize + j*cellSize;
            int y = 1 + i*cellSize;
            tblColors.push_back(new ColorRect(x+1,y+1,cellSize-1,QColor(0,0,0,0)));
        }
    }

    setFillColor(QColor(246,230,240));

    setDefaultPalette();

    // Set up the single-shot timer
    clickTimer = new QTimer(this);
    clickTimer->setSingleShot(true);

    // Connect the timer timeout to your single click action
    connect(clickTimer, &QTimer::timeout, this, &Palette::handleSingleLeftClick);


}

Palette::~Palette()
{
    for(auto r : tblColors){
        delete r;
    }
    tblColors.clear();

    if (clickTimer){
        delete clickTimer;
    }

}

ColorRect *Palette::hitColors(QPoint p)
{
    for(auto r : tblColors){
        if (r->contains(p)){
            return r;
        }

    }
    return NULL;
}

void Palette::handleSingleLeftClick() {
    qDebug() << "Single Left click Detected!";
    if (auto cr = hitColors(startPt) ){
        // Select foreground color
        foreGroundColor.setColor(cr->getColor());
        emit foregroundColorChanged(cr->getColor());
        update();
    }else if (startPt.x()<2*cellSize){
        //-- Swap foreground <-> background
        auto c = backGroundColor.getColor();
        backGroundColor.setColor(foreGroundColor.getColor());
        foreGroundColor.setColor(c);
        emit foregroundColorChanged(c);
        update();
    }
}

void Palette::mousePressEvent(QMouseEvent *event)
{
    QPoint p = event->position().toPoint();

    fHitColor = false;
    if (event->button() == Qt::LeftButton) {

        startPt = p;
        clickTimer->start(QGuiApplication::styleHints()->mouseDoubleClickInterval());

        if (selColorRect = hitColors(p) ){
            fHitColor = true;
        }else if (foreGroundColor.contains(p)){
            fHitColor = true;
            selColorRect = &foreGroundColor;
        }

        event->accept(); // Prevents the event from propagating further
    }
}


void Palette::mouseDoubleClickEvent(QMouseEvent *event)
{
    // Disable single clich handler
    clickTimer->stop();

    if (event->button() == Qt::LeftButton) {

        QPoint p = event->position().toPoint();

        // Handle left double-click
        qDebug() << "Left mouse double-clicked at:" << event->pos();

        if (selColorRect = hitColors(p) ){
            emit colorPickerRequested(); // Tell QML to open the ColorDialog
            // if (cr!=tblColors.front()){
            //     QColorPicker picker(this);
            //     picker.setColor(cr->getColor());

            //     if (picker.exec() == QDialog::Accepted) {
            //         QColor color = picker.color();
            //         cr->setColor(color);
            //         qDebug() << "Linux user picked color:" << color.name();
            //         update();
            //     }
            // }
        }else if (foreGroundColor.contains(p)){
            selColorRect = &foreGroundColor;
            emit colorPickerRequested(); // Tell QML to open the ColorDialog
            // QColorPicker picker(this);
            // picker.setColor(foreGroundColor.getColor());

            // if (picker.exec() == QDialog::Accepted) {
            //     QColor color = picker.color();
            //     foreGroundColor.setColor(color);
            //     qDebug() << "Linux user picked color:" << color.name();
            //     emit foreGroundColorChanged(foreGroundColor.getColor());
            //     update();

            // }
        }
    }

}


void Palette::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton)){
        // Disable single clich handler
        clickTimer->stop();

        QPoint p = event->position().toPoint();
        if (fHitColor){
            QPoint v = p - startPt;
            if (v!=QPoint(0,0)){
                if (!dragColor){
                    dragColor = new ColorRect(startPt.x()-cellSize/2,startPt.y()-cellSize,cellSize,selColorRect->getColor());
                }
                dragColor->moveCenter(startPt+v);
                update();
            }
        }
        event->accept();
    }

}

void Palette::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Mouse Release button";
    qDebug() << "--------------";
    QPoint p = event->position().toPoint();
    if (event->button() == Qt::LeftButton){
        if (dragColor) {
            if (auto cr = hitColors(p) ){
                if (cr!= tblColors.front()){
                    cr->setColor(dragColor->getColor());
                }
            }
            delete dragColor;
            dragColor = nullptr;
            update();
        }
        event->accept();
    }

}

// void Palette::hoverEnterEvent(QHoverEvent *event)
// {
//     m_hovered = true;
//     update(); // Repaint to show hover state
//     event->accept();
// }

// void Palette::hoverLeaveEvent(QHoverEvent *event)
// {
//     m_hovered = false;
//     update(); // Repaint to restore normal state
//     event->accept();
// }

void Palette::drawColors(QPainter *painter)
{
    int ic = 0;
    for (int i = 0; i < nbRows; i++) {
        for (int j = 0; j < nbColumns; j++) {
            if (ic<tblColors.size()){
                auto cr = tblColors[ic];
                cr->draw(painter);
            }
            ic++;
        }
    }

}

void Palette::paint(QPainter *painter)
{
    //painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(QPen(myGridColor, 0.5, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));

    backGroundColor.draw(painter);
    foreGroundColor.draw(painter);

    drawColors(painter);

    //--
    if (dragColor){
        dragColor->draw(painter);
    }

}

void Palette::setDefaultPalette()
{
    int i = 0;
    tblColors[i++]->setColor(QColor(0x00, 0x00, 0x00, 0x00));
    // tblColors[i++].color = QColor(0x00,0x00,0x00,0xFF);
    tblColors[i++]->setColor(QColor(0xFF, 0x00, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0xFF, 0xFF, 0xFF, 0xFF));
    tblColors[i++]->setColor(QColor(0x80, 0x80, 0x80, 0xFF));
    tblColors[i++]->setColor(QColor(0xC0, 0xC0, 0xC0, 0xFF));
    tblColors[i++]->setColor(QColor(0x80, 0x00, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0xFF, 0x00, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0x80, 0x80, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0xFF, 0xFF, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0x80, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0xFF, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0x80, 0x80, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0xFF, 0xFF, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0x00, 0x80, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0x00, 0xFF, 0xFF));
    tblColors[i++]->setColor(QColor(0x80, 0x00, 0x80, 0xFF));
    tblColors[i++]->setColor(QColor(0xFF, 0x00, 0xFF, 0xFF));

    foreGroundColor = ColorRect(1,1,1.2*cellSize,QColor(0x80, 0x80, 0xF0, 0xFF));
    backGroundColor = ColorRect(1,1,2*cellSize,QColor(0x00, 0x00, 0x00, 0x00));

    emit foregroundColorChanged(foreGroundColor.getColor());

}

void Palette::onSetForeGroundColor(QColor c)
{
    foreGroundColor.setColor(c);
    update();
}
