#ifndef PALETTE_H
#define PALETTE_H

#include <QObject>
#include <QPainter>
#include <QColor>
#include <QPoint>
#include <QQuickPaintedItem>
#include <vector>
#include "colorrect.h"
#include <QTimer>
#include <QtQml/qqmlregistration.h>

class Palette : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

    // Expose the color so QML ColorDialog can read andet update it
    Q_PROPERTY(QColor selectColor READ getSelectColor WRITE setSelectColor NOTIFY selectColorChanged)

public:

    explicit Palette(QQuickItem *parent = nullptr);
    ~Palette();


    QColor getSelectColor() const {
        if (selColorRect){
            return selColorRect->getColor();
        }else{
            return QColor(0,0,0,0);
        }
    }
    void setSelectColor(const QColor &color) {
        if (selColorRect){
            if (selColorRect->getColor() == color) return;
            selColorRect->setColor(color);
            emit selectColorChanged();
            update(); // Trigger a repaint with the new color
        }
     }

    void paint(QPainter *painter) override;

protected:
    // Override mouse event handlers
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    // void hoverEnterEvent(QHoverEvent *event) override;
    // void hoverLeaveEvent(QHoverEvent *event) override;

signals:
    void valueChanged();
    void foregroundColorChanged(QColor c); // Custom signal exposed to QML
    void backgroundColorChanged(QColor c);
    void selectColorChanged();
    void colorPickerRequested();

public slots:
    void onSetForeGroundColor(QColor c);

private slots:
    void handleSingleLeftClick();


private:

    //bool m_hovered = false;
    bool   fHitColor;

    ColorRect *selColorRect = nullptr;

    ColorRect   *hitColors(QPoint p);
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

    QTimer  *clickTimer=nullptr;

};

#endif // PALETTE_H
