#ifndef EDITAREA_H
#define EDITAREA_H

#include <QObject>
#include <QPainter>
#include <QColor>
#include <QQuickPaintedItem>
#include <QImage>
#include <QTimer>
#include <QPixmap>
#include <QCursor>

#include "editmode.h"
#include "pencilmode.h"
#include "rectanglemode.h"
#include "ellipsemode.h"
#include "selectrectmode.h"
#include "pastemode.h"
#include "fillmode.h"
#include "spriteimage.h"

class EditArea : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit EditArea(QQuickItem *parent = nullptr);
    ~EditArea();

    int value() const { return m_value; }
    void setValue(int newValue);

    void paint(QPainter *painter) override;

    void setSelectRectMode();
    void setPencilMode();
    void setRectangleMode();
    void setEllipseMode();
    void setPasteMode();
    void setFillMode();

protected:
    // Override mouse event handlers
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

signals:
    void valueChanged();
    //void clicked(); // Custom signal exposed to QML
    void imageChanged();
    void enableSave(bool fEnableSave);
    void pickImageColor(QColor c);
    // Declare the signal here. You don't implement this in the .cpp file.
    void showStatusMessage(const QString &message, int timeout = 0);

public slots:
    void onForegroundColorChanged(QColor c);
    void onMenuActionTriggered();


private:
    int m_value = 50;
    bool m_hovered = false;
    QColor foregroundColor = Qt::blue;

    QTimer  *m_timer;
    QPixmap pipettePixmap;
    QCursor pipetteCursor;

    QPointF lastMousePos;

    bool    modified = false;
    int     myPenWidth = 1;

    QColor  myGridColor = Qt::black;
    QPoint  lastPoint;

    void drawGrid(QPainter *painter);
    void drawPixels(QPainter *painter);


    EditMode        *curEditMode=NULL;
    PencilMode      *pencilMode;
    RectangleMode   *rectangleMode;
    EllipseMode     *ellipseMode;
    SelectRectMode  *selectRectMode;
    PasteMode       *pasteMode;
    FillMode        *fillMode;


};
#endif // EDITAREA_H
