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
}

void EditArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Update value based on horizontal click position (0 to 100)
        int newVal = static_cast<int>((event->position().x() / width()) * 100);
        setValue(std::clamp(newVal, 0, 100));

        emit clicked();
        event->accept(); // Prevents the event from propagating further
    }
}

void EditArea::mouseMoveEvent(QMouseEvent *event)
{
    // Automatically called while dragging with LeftButton pressed
    int newVal = static_cast<int>((event->position().x() / width()) * 100);
    setValue(std::clamp(newVal, 0, 100));
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

void EditArea::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);

    // Draw background circle
    QPen bgPen(QColor("#e0e0e0"), 12);
    painter->setPen(bgPen);
    painter->drawArc(15, 15, width() - 30, height() - 30, 0, 360 * 16);

    // Draw progress arc based on m_value (0 to 100)
    QPen progressPen(QColor("#2196F3"), 12);
    painter->setPen(progressPen);
    int spanAngle = -(m_value / 100.0) * 360 * 16;
    painter->drawArc(15, 15, width() - 30, height() - 30, 90 * 16, spanAngle);
}
