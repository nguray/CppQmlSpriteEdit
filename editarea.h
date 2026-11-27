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

public:
    explicit EditArea(QQuickItem *parent = nullptr);
    ~EditArea();

    void paint(QPainter *painter) override;
    void setPreviousMode();

protected:
    // Override mouse event handlers
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

signals:
    // Declare the signal here. You don't implement this in the .cpp file.
    void imageChanged();
    void enableSave(bool fEnableSave);
    void pickImageColor(QColor c);
    void statusMessageRequested(const QString &message, int timeout = 0);
    void statusClearRequested();

public slots:
    void onForegroundColorChanged(QColor c);
    void onMenuActionTriggered();
    void onCopyRectangle();
    void onCutRectangle();
    void onPaste();
    void setPencilMode();
    void setSelectRectMode();
    void setRectangleMode();
    void setEllipseMode();
    void setPasteMode();
    void setFillMode();

private slots:
    void toggleFlash();


private:

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
    void initZoomPan(QSizeF canvasSize);


    EditMode        *prevEditMode=NULL;
    EditMode        *curEditMode=NULL;
    PencilMode      *pencilMode;
    RectangleMode   *rectangleMode;
    EllipseMode     *ellipseMode;
    SelectRectMode  *selectRectMode;
    PasteMode       *pasteMode;
    FillMode        *fillMode;


};
#endif // EDITAREA_H
