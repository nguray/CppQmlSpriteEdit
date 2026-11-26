#include "sprites.h"

Sprites::Sprites(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    // 1. Enable left mouse button events
    setAcceptedMouseButtons(Qt::LeftButton);

    // 2. Enable hover events (optional)
    setAcceptHoverEvents(true);

    setFillColor(QColor(246,246,240));

}

Sprites::~Sprites()
{

}

void Sprites::mousePressEvent(QMouseEvent *event)
{

    event->accept(); // Prevents the event from propagating further

}

void Sprites::mouseMoveEvent(QMouseEvent *event)
{

    event->accept(); // Prevents the event from propagating further

}

void Sprites::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        event->accept();
    }
}

void Sprites::hoverEnterEvent(QHoverEvent *event)
{
    // m_hovered = true;
    // update(); // Repaint to show hover state
    event->accept();
}

void Sprites::hoverLeaveEvent(QHoverEvent *event)
{
    // m_hovered = false;
    // update(); // Repaint to restore normal state
    event->accept();
}

void Sprites::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);

    if (newGeometry!=oldGeometry){
        cellSize = newGeometry.height()/NB_SPRITES;
        setImplicitWidth(cellSize);
    }
}

void Sprites::drawCells(QPainter *painter)
{
    painter->setPen(QPen(QColor(128,128,128,255), 1, Qt::SolidLine, Qt::RoundCap,
                         Qt::RoundJoin));

    int y;
    int xLeft = 0;
    int xRight = cellSize - 1;
    for (int i = 0; i<=NB_SPRITES; i++){
        y = i * cellSize;
        painter->drawLine(QPoint(xLeft,y),QPoint(xRight,y));
    }

    painter->drawLine(QPoint(xLeft,0),QPoint(xLeft,y));
    painter->drawLine(QPoint(xRight,0),QPoint(xRight,y));

    cellsRect.setCoords(xLeft,0,xRight,y);

}

void Sprites::drawSelectMark(QPainter *painter)
{
    int xLeft = 1;
    int xRight = cellSize - 2;
    int yTop = iSelectCell * cellSize + 1;
    int yBottom = yTop + cellSize - 2;

    painter->setPen(QPen(QColor(255,0,0,255), 2, Qt::SolidLine, Qt::RoundCap,
                         Qt::RoundJoin));

    //-- TopLeft Corner
    painter->drawLine(QPoint(xLeft,yTop+8),QPoint(xLeft,yTop));
    painter->drawLine(QPoint(xLeft,yTop),QPoint(xLeft+8,yTop));

    //-- TopRight Corner
    painter->drawLine(QPoint(xRight-8,yTop),QPoint(xRight,yTop));
    painter->drawLine(QPoint(xRight,yTop),QPoint(xRight,yTop+8));

    //-- Bottom Right Corner
    painter->drawLine(QPoint(xRight,yBottom-8),QPoint(xRight,yBottom));
    painter->drawLine(QPoint(xRight,yBottom),QPoint(xRight-8,yBottom));

    //-- BottomLeft Corner
    painter->drawLine(QPoint(xLeft+8,yBottom),QPoint(xLeft,yBottom));
    painter->drawLine(QPoint(xLeft,yBottom),QPoint(xLeft,yBottom-8));

}

void Sprites::drawSprites(QPainter *painter)
{
    int imgWidth,imgHeight;
    int yTop = 0;
    for (auto &img : images){
        if (!img.isNull()){
            imgWidth  = img->width();
            imgHeight = img->height();
            painter->drawImage(QRect(cellSize/2 - imgWidth/2,yTop + cellSize/2 - imgHeight/2,imgWidth,imgHeight),
                               *img, img->rect());
        }
        yTop += cellSize;
    }
}


void Sprites::paint(QPainter *painter)
{
    drawCells(painter);

    drawSelectMark(painter);

    drawSprites(painter);

}



