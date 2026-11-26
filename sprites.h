#ifndef SPRITES_H
#define SPRITES_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QColor>
#include <QRect>
#include <QPoint>
#include <QSharedPointer>
#include "spriteimage.h"
#include <array>

class Sprites : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

public:

    enum {
        NB_SPRITES = 5
    };

    Sprites(QQuickItem *parent = nullptr);
    ~Sprites();


    void paint(QPainter *painter) override;

protected:
    // Override mouse event handlers
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

signals:

public slots:

private:

    int cellSize      = 64;
    int iSelectCell   = 0;
    int iSelectCellPopupMenu = 0;
    QRect cellsRect;

    std::array<QSharedPointer<SpriteImage>,NB_SPRITES> images;

    void drawCells(QPainter *painter);
    void drawSelectMark(QPainter *painter);
    void drawSprites(QPainter *painter);

};

#endif // SPRITES_H
