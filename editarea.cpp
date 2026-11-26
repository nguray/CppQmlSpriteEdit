#include "editarea.h"

#include <algorithm>

EditArea::EditArea(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setWidth(150);
    setHeight(150);

    // 1. Enable left mouse button events
    setAcceptedMouseButtons(Qt::LeftButton);

    // 2. Enable hover events (optional)
    setAcceptHoverEvents(true);

    setFillColor(QColor(220,220,204));

    pipettePixmap = QPixmap(":/icons/pipette.png");
    pipetteCursor = QCursor(pipettePixmap, 4, 26);

    //EditMode::image = QImage(32, 32, QImage::Format_ARGB32);
    EditMode::setImage(QSharedPointer<SpriteImage>::create(32, 32, QImage::Format_ARGB32));
    EditMode::image->fill(QColor(0, 0, 0, 0));
    EditMode::backupImage();

    pencilMode = new PencilMode();
    rectangleMode = new RectangleMode();
    ellipseMode = new EllipseMode();
    selectRectMode = new SelectRectMode();
    pasteMode = new PasteMode();
    fillMode = new FillMode();

    setPencilMode();


}


EditArea::~EditArea()
{
    if (pencilMode) delete pencilMode;
    if (rectangleMode) delete rectangleMode;
    if (ellipseMode) delete ellipseMode;
    if (selectRectMode) delete selectRectMode;
    if (pasteMode) delete pasteMode;
    if (fillMode) delete fillMode;

}

void EditArea::mousePressEvent(QMouseEvent *event)
{
    // if (event->button() == Qt::LeftButton) {
    //     // Update value based on horizontal click position (0 to 100)
    //     int newVal = static_cast<int>((event->position().x() / width()) * 100);
    //     setValue(std::clamp(newVal, 0, 100));

    //     emit clicked();
    // }

    if (event->button() == Qt::MiddleButton){
        // Store Starting position for computing canvas translation when mouse is moved
        lastMousePos = event->position();
        setCursor(Qt::SizeAllCursor);

    }else if (EditMode::fControlKey){
        QPointF canvasPos = EditMode::canvasInverseTransform.map(event->position());
        auto pt = canvasPos.toPoint();
        auto [fOk, c] = EditMode::getPixelColor(pt);
        if (fOk){
            EditMode::foregroundColor = c;
            //emit(pickImageColor(c));
        }
        setCursor(pipetteCursor);
    }else{
        if (curEditMode->mousePressEvent(event)){
            update();
            emit(imageChanged());
            //updateSaveMenuItem();
        }
        // Change EditArea widget cursor if required
        if (cursor().shape()!=curEditMode->mouseCursor.shape()){
            setCursor(curEditMode->mouseCursor);
        }
    }

    event->accept(); // Prevents the event from propagating further


}

void EditArea::mouseMoveEvent(QMouseEvent *event)
{
    // Automatically called while dragging with LeftButton pressed
    //int newVal = static_cast<int>((event->position().x() / width()) * 100);
    //setValue(std::clamp(newVal, 0, 100));

    if ((event->buttons() & Qt::MiddleButton)){
        // Check whether the mouse cursor is within the widget
        auto r = boundingRect();
        if (r.contains(event->position().toPoint())){
            // Compute canvas translation
            EditMode::canvasTranslation += event->position() - lastMousePos;
            lastMousePos = event->position();
            setCursor(Qt::SizeAllCursor);
            update();
        }
    }else if (EditMode::fControlKey){
        QPointF canvasPos = EditMode::canvasInverseTransform.map(event->position());
        auto pt = canvasPos.toPoint();
        auto pix = EditMode::Pos2Pixel(pt);
        QRect r = EditMode::image->rect();
        if (r.contains(pix)){
            setCursor(pipetteCursor);
        }else{
            unsetCursor();
        }
    }else{
        if (curEditMode->mouseMoveEvent(event)){
            update();
            emit(imageChanged());
            //updateSaveMenuItem();
        }

        // Change EditArea widget cursor if required
        if (cursor().shape()!=curEditMode->mouseCursor.shape()){
            setCursor(curEditMode->mouseCursor);
        }
    }

    event->accept();
}

void EditArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        event->accept();
    }
}

void EditArea::hoverEnterEvent(QHoverEvent *event)
{
    m_hovered = true;
    update(); // Repaint to show hover state
    event->accept();
}

void EditArea::hoverLeaveEvent(QHoverEvent *event)
{
    m_hovered = false;
    update(); // Repaint to restore normal state
    event->accept();
}


void EditArea::setValue(int newValue)
{
    if (m_value == newValue)
        return;
    m_value = newValue;
    emit valueChanged();
    update(); // Triggers a repaint when the value changes
}

void EditArea::onMenuActionTriggered() {
    qDebug() << "Menu item triggered my C++ component!";
}

void EditArea::onForegroundColorChanged(QColor c)
{
    foregroundColor = c;
    qDebug() << "foreground color changed";
    update();
}


void EditArea::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);

    if (newGeometry.size() != oldGeometry.size()) {
        // Handle resize logic here
        auto minDim = newGeometry.height();
        if (minDim>newGeometry.width()){
            minDim = newGeometry.width();
        }
        QSize imageSize = EditMode::image->size();
        EditMode::cellSize = (((int) minDim)-4) / imageSize.width();

        update(); // Trigger a repaint if needed
    }
}

void EditArea::drawGrid(QPainter *painter)
{
    int x,y;
    int xLeft = 0;
    int yTop = 0;
    painter->setPen(QPen(myGridColor, 0, Qt::SolidLine, Qt::RoundCap,
                         Qt::RoundJoin));
    for (int i=0;i<=EditMode::image->size().height();i++){
        for (int j=0;j<=EditMode::image->size().width();j++){
            x = xLeft + j * EditMode::cellSize;
            y = yTop + i * EditMode::cellSize;
            painter->drawPoint(QPoint(x,y));
        }
    }

}

void EditArea::drawPixels(QPainter *painter)
{
    int w = EditMode::image->size().width();
    int h = EditMode::image->size().height();
    int s = EditMode::cellSize - 2;
    for (int row = 0; row < h; row++)
    {
        for (int col = 0; col < w; col++)
        {

            QColor c = EditMode::image->pixelColor(col,row);

            if (c.alpha()){
                int x = col*EditMode::cellSize+1;
                int y = row*EditMode::cellSize+1;

                painter->setBrush(QBrush(c));
                painter->setPen(QPen(c));
                painter->drawRect(x,y,s,s);
            }
        }

    }

}


void EditArea::paint(QPainter *painter)
{
    //painter->setRenderHint(QPainter::Antialiasing);

    // Draw background circle
    // QPen bgPen(QColor("#e0e0e0"), 12);
    // painter->setPen(bgPen);
    // painter->drawArc(15, 15, width() - 30, height() - 30, 0, 360 * 16);

    // Draw progress arc based on m_value (0 to 100)
    // QPen progressPen(QColor("#2196F3"), 12);
    // painter->setPen(progressPen);
    // int spanAngle = -(m_value / 100.0) * 360 * 16;
    // painter->drawArc(15, 15, width() - 30, height() - 30, 90 * 16, spanAngle);

    drawGrid(painter);

    drawPixels(painter);

}


void EditArea::setSelectRectMode()
{
    curEditMode = selectRectMode;
    curEditMode->init();
    update();
}

void EditArea::setPencilMode()
{
    curEditMode = pencilMode;
    curEditMode->init();
    update();
}

void EditArea::setRectangleMode()
{
    curEditMode = rectangleMode;
    curEditMode->init();
    update();
}

void EditArea::setEllipseMode()
{
    curEditMode = ellipseMode;
    curEditMode->init();
    update();
}

void EditArea::setPasteMode()
{
    curEditMode = pasteMode;
    curEditMode->init();
    update();

}

void EditArea::setFillMode()
{
    curEditMode = fillMode;
    curEditMode->init();
    update();

}


