#include "palette.h"


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

    setFillColor(QColor(246,246,240));

    setDefaultPalette();

}

Palette::~Palette()
{
    for(auto r : tblColors){
        delete r;
    }
    tblColors.clear();

    // if (clickTimer){
    //     delete clickTimer;
    // }

}

void Palette::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Update value based on horizontal click position (0 to 100)
        int newVal = static_cast<int>((event->position().x() / width()) * 100);

        emit foregroundColorChanged(Qt::red);
        qDebug() << "foreground color picked";

        update();

        event->accept(); // Prevents the event from propagating further
    }
}

void Palette::mouseMoveEvent(QMouseEvent *event)
{
    // Automatically called while dragging with LeftButton pressed
    int newVal = static_cast<int>((event->position().x() / width()) * 100);

    event->accept();
}

void Palette::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        event->accept();
    }
}

void Palette::hoverEnterEvent(QHoverEvent *event)
{
    m_hovered = true;
    update(); // Repaint to show hover state
    event->accept();
}

void Palette::hoverLeaveEvent(QHoverEvent *event)
{
    m_hovered = false;
    update(); // Repaint to restore normal state
    event->accept();
}

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

