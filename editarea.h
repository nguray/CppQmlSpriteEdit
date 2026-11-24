#ifndef EDITAREA_H
#define EDITAREA_H

#include <QObject>
#include <QPainter>
#include <QColor>
#include <QQuickPaintedItem>

class EditArea : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit EditArea(QQuickItem *parent = nullptr);

    int value() const { return m_value; }
    void setValue(int newValue);

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
    void clicked(); // Custom signal exposed to QML

public slots:
    void onForegroundColorChanged(QColor c);
    void onMenuActionTriggered();


private:
    int m_value = 50;
    bool m_hovered = false;
    QColor foregroundColor = Qt::blue;

};
#endif // EDITAREA_H
