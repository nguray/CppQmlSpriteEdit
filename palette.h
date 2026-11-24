#ifndef PALETTE_H
#define PALETTE_H

#include <QObject>
#include <QPainter>
#include <QColor>
#include <QPoint>
#include <QQuickPaintedItem>
#include <vector>
#include "colorrect.h"

class Palette : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

public:

    explicit Palette(QQuickItem *parent = nullptr);
    ~Palette();

    void paint(QPainter *painter) override;

protected:
    // Override mouse event handlers
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;

signals:
    void valueChanged();
    void foregroundColorChanged(QColor c); // Custom signal exposed to QML
    void backgroundColorChanged(QColor c);
    void clicked(); // Custom signal exposed to QML

private:

    bool m_hovered = false;

    void        drawColors(QPainter *painter);
    void        setDefaultPalette();

    QColor      myGridColor = Qt::black;
    ColorRect   foreGroundColor;
    ColorRect   backGroundColor;

    ColorRect   *dragColor = nullptr;

    int nbColumns = 16;
    int nbRows = 2;
    int cellSize = 16;
    QPoint startPt;

    std::vector<ColorRect *> tblColors;

};

#endif // PALETTE_H
