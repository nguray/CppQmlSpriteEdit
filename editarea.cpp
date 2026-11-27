#include "editarea.h"

#include <algorithm>

EditArea::EditArea(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setWidth(150);
    setHeight(150);
    setFocus(true); // Requests focus within its parent scope

    // 1. Enable left mouse button events
    setAcceptedMouseButtons(Qt::LeftButton|Qt::RightButton|Qt::MiddleButton);

    // 2. Enable hover events (optional)
    setAcceptHoverEvents(true);

    setFillColor(QColor(220,220,204));

    pipettePixmap = QPixmap(":/icons/pipette.png");
    pipetteCursor = QCursor(pipettePixmap, 4, 26);

    //EditMode::image = QImage(32, 32, QImage::Format_ARGB32);
    EditMode::setImage(QSharedPointer<SpriteImage>::create(32, 32, QImage::Format_ARGB32));
    EditMode::image->fill(QColor(0, 0, 0, 0));
    EditMode::backupImage();

    EditMode::paintItem = this;
    pencilMode = new PencilMode();
    rectangleMode = new RectangleMode();
    ellipseMode = new EllipseMode();
    selectRectMode = new SelectRectMode();
    pasteMode = new PasteMode();
    fillMode = new FillMode();

    curEditMode = pencilMode;
    setPencilMode();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &EditArea::toggleFlash);

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
    forceActiveFocus(); // Grabs active focus when clicked

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
            emit(pickImageColor(c));
        }
        setCursor(pipetteCursor);
    }else{

        if (curEditMode->mousePressEvent(event)){
            update();
            emit(imageChanged());
            emit statusMessageRequested(QStringLiteral("Image changed..."), 3000);
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

    //qDebug() << "Mouse move";

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
    }else{
        if (curEditMode->mouseMoveEvent(event)){
            update();
            emit(imageChanged());
            //updateSaveMenuItem();
        }

        if (cursor().shape()!=curEditMode->mouseCursor.shape()){
            setCursor(curEditMode->mouseCursor);
        }

    }

    event->accept();
}

void EditArea::mouseReleaseEvent(QMouseEvent *event)
{

    if (event->button() == Qt::MiddleButton){
        //
        unsetCursor();
    }else if (EditMode::fControlKey){
        setCursor(pipetteCursor);
    }else{
        if (curEditMode->mouseReleaseEvent(event)){
            update();
        }
        // Change EditArea widget cursor if required
        if (cursor().shape()!=curEditMode->mouseCursor.shape()){
            setCursor(curEditMode->mouseCursor);
        }
    }

    event->accept();
}

void EditArea::wheelEvent(QWheelEvent *event)
{
    // Vertical scroll delta (positive = up/forward, negative = down/backward)
    QPoint numDegrees = event->angleDelta() / 8;
    if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        if (numSteps.y()>0){
            float nv = EditMode::zoomFactor + 0.04f;
            if (nv<10.0f){
                EditMode::zoomFactor = nv;
                update();
            }
        }else{
            float nv = EditMode::zoomFactor - 0.04f;
            if (nv>0.1f){
                EditMode::zoomFactor = nv;
                update();
            }
        }
        //qDebug() << "Canvas Scale: " << EditMode::zoom;
    }

    // Horizontal scroll delta
    //int xDelta = event->angleDelta().x();

    // Mark event as accepted
    event->accept();

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

void EditArea::hoverMoveEvent(QHoverEvent *event)
{
    if (EditMode::fControlKey){
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
        if (curEditMode->hoverMoveEvent(event)){
            update();
        }
        // Change EditArea widget cursor if required
        if (cursor().shape()!=curEditMode->mouseCursor.shape()){
            setCursor(curEditMode->mouseCursor);
        }
    }
    event->accept();
}

void EditArea::keyPressEvent(QKeyEvent *event)
{
    //--
    if (event->key()==Qt::Key_Shift){
        EditMode::fShiftKey = true;
        m_timer->start(400); // Toggle every 400 milliseconds
        update();
        event->accept();
        return;
    }else if (event->key()==Qt::Key_Control){
        EditMode::fControlKey = true;
        setCursor(pipetteCursor);
        emit statusMessageRequested("Pipette Mode", 3000); // 3 seconds
        event->accept();
        return;
    }
    QQuickPaintedItem::keyPressEvent(event);
}

void EditArea::keyReleaseEvent(QKeyEvent *event)
{
    //--
    if (event->key()==Qt::Key_Shift){
        EditMode::fShiftKey = false;
        m_timer->stop();
        update();
    }else if (event->key()==Qt::Key_Control){
        EditMode::fControlKey = false;
        setCursor(Qt::ArrowCursor);
    }
    event->accept();

}

void EditArea::onMenuActionTriggered() {
    qDebug() << "Menu item triggered my C++ component!";
}

void EditArea::onForegroundColorChanged(QColor c)
{
    EditMode::foregroundColor = c;
    qDebug() << "foreground color changed " << c;
    update();
}

void EditArea::initZoomPan(QSizeF canvasSize)
{

    QSize imageSize = EditMode::image->size();
    int minDim = canvasSize.height();
    if (minDim>canvasSize.width()){
        minDim = canvasSize.width();
    }
    EditMode::cellSize = (minDim-4) / imageSize.width();

    EditMode::zoomFactor = 1.0f;

    auto w = EditMode::cellSize*imageSize.width();
    auto h = EditMode::cellSize*imageSize.height();
    EditMode::canvasTranslation.setX(canvasSize.width()/2.0f - w/2.0f);
    EditMode::canvasTranslation.setY(canvasSize.height()/2.0f - h/2.0f);

    EditMode::zoomCenter.setX(w/2.0f);
    EditMode::zoomCenter.setY(h/2.0f);

}

void EditArea::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);

    if (newGeometry.size() != oldGeometry.size()) {

        initZoomPan(newGeometry.size());

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

    //-- Canvas Zoom Pan settings use to paint
    // Store canvas size to trigger init Zoom Pan when get anpther image from Sprites widget if needed
    EditMode::image->canvasSize = size();
    EditMode::image->cellSize   = EditMode::cellSize;
    EditMode::image->zoomFactor = EditMode::zoomFactor;
    EditMode::image->canvasTranslation     = EditMode::canvasTranslation;
    EditMode::image->zoomCenter = EditMode::zoomCenter;

    painter->save();

    EditMode::canvasTransform.reset();
    EditMode::canvasTransform.translate(EditMode::canvasTranslation.x(),EditMode::canvasTranslation.y());


    EditMode::canvasTransform.translate(EditMode::zoomCenter.x(), EditMode::zoomCenter.y());
    EditMode::canvasTransform.scale(EditMode::zoomFactor, EditMode::zoomFactor);
    EditMode::canvasTransform.translate(-EditMode::zoomCenter.x(), -EditMode::zoomCenter.y());

    painter->setTransform(EditMode::canvasTransform);
    EditMode::canvasInverseTransform = EditMode::canvasTransform.inverted();

    auto w = EditMode::cellSize*EditMode::image->size().width();
    auto h = EditMode::cellSize*EditMode::image->size().height();
    QRect r = QRect(0,0,w,h);
    painter->fillRect(r,QColor(253,253,237));

    drawGrid(painter);

    drawPixels(painter);

    if (!EditMode::image->isNull()){
        int w = EditMode::image->size().width();
        int h = EditMode::image->size().height();
        painter->drawImage(QRect(w*EditMode::cellSize+10,4,w,h), *EditMode::image, EditMode::image->rect());

        painter->drawImage(QRect(w*EditMode::cellSize+10,128,w,h), EditMode::image_bak, EditMode::image_bak.rect());

        if (!EditMode::image_cpy.isNull()){
            int cpy_w = EditMode::image_cpy.size().width();
            int cpy_h = EditMode::image_cpy.size().height();
            painter->drawImage(QRect(w*EditMode::cellSize+10,280,cpy_w,cpy_h), EditMode::image_cpy, EditMode::image_cpy.rect());
        }
    }

    curEditMode->paintEvent(painter);

    painter->restore();

}

void EditArea::toggleFlash()
{
    pencilMode->toggleFlash();
    update();
}


void EditArea::setSelectRectMode()
{
    prevEditMode = curEditMode;
    curEditMode = selectRectMode;
    curEditMode->init();
    update();
}

void EditArea::setPencilMode()
{
    prevEditMode = curEditMode;
    curEditMode = pencilMode;
    curEditMode->init();
    update();
}

void EditArea::setRectangleMode()
{
    prevEditMode = curEditMode;
    curEditMode = rectangleMode;
    curEditMode->init();
    update();
}

void EditArea::setEllipseMode()
{
    prevEditMode = curEditMode;
    curEditMode = ellipseMode;
    curEditMode->init();
    update();
}

void EditArea::setPasteMode()
{
    prevEditMode = curEditMode;
    curEditMode = pasteMode;
    curEditMode->init();
    update();

}

void EditArea::setFillMode()
{
    prevEditMode = curEditMode;
    curEditMode = fillMode;
    curEditMode->init();
    update();

}

void EditArea::setPreviousMode()
{
    curEditMode = prevEditMode;
    curEditMode->init();
    update();

}

void EditArea::onCopyRectangle()
{
    qDebug() << "Copy Select Rectangle";
    QRect r = EditMode::selectRect->getPixRect();
    r.adjust(0,0,1,1);
    EditMode::image_cpy = EditMode::image->copy(r);
    curEditMode->init();

}

void EditArea::onCutRectangle()
{
    qDebug() << "Cut Select Rectangle";
    QRect r = EditMode::selectRect->getPixRect();
    r.adjust(0,0,1,1);
    EditMode::image_cpy = EditMode::image->copy(r);

    QPainter painter(EditMode::image.get());
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    r = EditMode::selectRect->getPixRect();
    r.adjust(0,0,1,1);
    painter.fillRect(r,EditMode::backgroundColor);
    curEditMode->init();
}

void EditArea::onPaste()
{
    qDebug() << "Paste image";
    if (!EditMode::image_cpy.isNull()){
        setPasteMode();
    }
}


