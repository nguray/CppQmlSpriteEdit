#include "palette.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <QGuiApplication>
#include <QStyleHints>


Palette::Palette(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    //setWidth(150);
    //setHeight(150);

    // 1. Enable left mouse button events
    setAcceptedMouseButtons(Qt::LeftButton);

    // 2. Enable hover events (optional)
    setAcceptHoverEvents(true);

    //-- Fill colors table
    for (int i = 0; i < nbRows; i++) {
        for (int j = 0; j < nbColumns; j++) {
            int x = 2 + 2*cellSize + j*cellSize;
            int y = 1 + i*cellSize;
            tblColors.push_back(new ColorRect(x+1,y+1,cellSize-1,QColor(0,0,0,0)));
        }
    }

    setFillColor(QColor(246,230,240));

    setDefaultPalette();

    // Set up the single-shot timer
    clickTimer = new QTimer(this);
    clickTimer->setSingleShot(true);

    // Connect the timer timeout to your single click action
    connect(clickTimer, &QTimer::timeout, this, &Palette::handleSingleLeftClick);


}

Palette::~Palette()
{
    for(auto r : tblColors){
        delete r;
    }
    tblColors.clear();

    if (clickTimer){
        delete clickTimer;
    }

}

ColorRect *Palette::hitColors(QPoint p)
{
    for(auto r : tblColors){
        if (r->contains(p)){
            return r;
        }

    }
    return NULL;
}

void Palette::handleSingleLeftClick() {
    qDebug() << "Single Left click Detected!";
    if (auto cr = hitColors(startPt) ){
        // Select foreground color
        foregroundColor.setColor(cr->getColor());
        emit foregroundColorChanged(cr->getColor());
        update();
    }else if (startPt.x()<2*cellSize){
        //-- Swap foreground <-> background
        auto c = backgroundColor.getColor();
        backgroundColor.setColor(foregroundColor.getColor());
        foregroundColor.setColor(c);
        emit foregroundColorChanged(c);
        update();
    }
}

void Palette::mousePressEvent(QMouseEvent *event)
{
    QPoint p = event->position().toPoint();

    fHitColor = false;
    if (event->button() == Qt::LeftButton) {

        startPt = p;
        clickTimer->start(QGuiApplication::styleHints()->mouseDoubleClickInterval());

        if (selColorRect = hitColors(p) ){
            fHitColor = true;
        }else if (foregroundColor.contains(p)){
            fHitColor = true;
            selColorRect = &foregroundColor;
        }

        event->accept(); // Prevents the event from propagating further
    }
}


void Palette::mouseDoubleClickEvent(QMouseEvent *event)
{
    // Disable single clich handler
    clickTimer->stop();

    if (event->button() == Qt::LeftButton) {

        QPoint p = event->position().toPoint();

        // Handle left double-click
        qDebug() << "Left mouse double-clicked at:" << event->pos();

        if (selColorRect = hitColors(p) ){
            emit colorPickerRequested(); // Tell QML to open the ColorDialog
            // if (cr!=tblColors.front()){
            //     QColorPicker picker(this);
            //     picker.setColor(cr->getColor());

            //     if (picker.exec() == QDialog::Accepted) {
            //         QColor color = picker.color();
            //         cr->setColor(color);
            //         qDebug() << "Linux user picked color:" << color.name();
            //         update();
            //     }
            // }
        }else if (foregroundColor.contains(p)){
            selColorRect = &foregroundColor;
            emit colorPickerRequested(); // Tell QML to open the ColorDialog
            // QColorPicker picker(this);
            // picker.setColor(foregroundColor.getColor());

            // if (picker.exec() == QDialog::Accepted) {
            //     QColor color = picker.color();
            //     foregroundColor.setColor(color);
            //     qDebug() << "Linux user picked color:" << color.name();
            //     emit foregroundColorChanged(foregroundColor.getColor());
            //     update();

            // }
        }
    }

}


void Palette::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton)){
        // Disable single clich handler
        clickTimer->stop();

        QPoint p = event->position().toPoint();
        if (fHitColor){
            QPoint v = p - startPt;
            if (v!=QPoint(0,0)){
                if (!dragColor){
                    dragColor = new ColorRect(startPt.x()-cellSize/2,startPt.y()-cellSize,cellSize,selColorRect->getColor());
                }
                dragColor->moveCenter(startPt+v);
                update();
            }
        }
        event->accept();
    }

}

void Palette::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Mouse Release button";
    qDebug() << "--------------";
    QPoint p = event->position().toPoint();
    if (event->button() == Qt::LeftButton){
        if (dragColor) {
            if (auto cr = hitColors(p) ){
                if (cr!= tblColors.front()){
                    cr->setColor(dragColor->getColor());
                }
            }
            delete dragColor;
            dragColor = nullptr;
            update();
        }
        event->accept();
    }

}

// void Palette::hoverEnterEvent(QHoverEvent *event)
// {
//     m_hovered = true;
//     update(); // Repaint to show hover state
//     event->accept();
// }

// void Palette::hoverLeaveEvent(QHoverEvent *event)
// {
//     m_hovered = false;
//     update(); // Repaint to restore normal state
//     event->accept();
// }

void Palette::drawColors(QPainter *painter)
{
    int ic = 0;
    for (int i = 0; i < nbRows; i++) {
        for (int j = 0; j < nbColumns; j++) {
            if (ic<tblColors.size()){
                auto cr = tblColors[ic];
                cr->draw(painter);
            }
            ic++;
        }
    }

}

void Palette::paint(QPainter *painter)
{
    //painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(QPen(myGridColor, 0.5, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));

    backgroundColor.draw(painter);
    foregroundColor.draw(painter);

    drawColors(painter);

    //--
    if (dragColor){
        dragColor->draw(painter);
    }

}

void Palette::setDefaultPalette()
{
    int i = 0;
    tblColors[i++]->setColor(QColor(0x00, 0x00, 0x00, 0x00));
    // tblColors[i++].color = QColor(0x00,0x00,0x00,0xFF);
    tblColors[i++]->setColor(QColor(0xFF, 0x00, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0xFF, 0xFF, 0xFF, 0xFF));
    tblColors[i++]->setColor(QColor(0x80, 0x80, 0x80, 0xFF));
    tblColors[i++]->setColor(QColor(0xC0, 0xC0, 0xC0, 0xFF));
    tblColors[i++]->setColor(QColor(0x80, 0x00, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0xFF, 0x00, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0x80, 0x80, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0xFF, 0xFF, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0x80, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0xFF, 0x00, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0x80, 0x80, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0xFF, 0xFF, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0x00, 0x80, 0xFF));
    tblColors[i++]->setColor(QColor(0x00, 0x00, 0xFF, 0xFF));
    tblColors[i++]->setColor(QColor(0x80, 0x00, 0x80, 0xFF));
    tblColors[i++]->setColor(QColor(0xFF, 0x00, 0xFF, 0xFF));

    foregroundColor = ColorRect(1,1,1.2*cellSize,QColor(0x80, 0x80, 0xF0, 0xFF));
    backgroundColor = ColorRect(1,1,2*cellSize,QColor(0x00, 0x00, 0x00, 0x00));

    emit foregroundColorChanged(foregroundColor.getColor());

}

void Palette::onSetForegroundColor(QColor c)
{
    foregroundColor.setColor(c);
    update();
}

unsigned char Palette::getRGBAlpha(unsigned int rgb) {
    //-----------------------------------------------------------
    return (rgb & RGB_A_MASK);
}

unsigned char Palette::getRGBRed(unsigned int rgb) {
    //-----------------------------------------------------------
    return (rgb & RGB_R_MASK) >> 24;
}

unsigned char Palette::getRGBGreen(unsigned int rgb) {
    //-----------------------------------------------------------
    return (rgb & RGB_G_MASK) >> 16;
}

unsigned char Palette::getRGBBlue(unsigned int rgb) {
    //-----------------------------------------------------------
    return (rgb & RGB_B_MASK) >> 8;
}

unsigned int Palette::RGBA(unsigned char r, unsigned char g, unsigned char b,
                           unsigned char a) {
    //-----------------------------------------------------------
    return (b << 8) | (g << 16) | (r << 24) | (a);
}

void Palette::save(QString pathName, QString fileName) {
    std::string fullPathName;
    //-------------------------------------------------
    auto  stdStrPathName = pathName.toStdString();
    auto  stdStrFileName = fileName.toStdString();

    if (stdStrPathName != "") {
        fullPathName = stdStrPathName + "/" + stdStrFileName;
    } else {
        fullPathName = stdStrFileName;
    }
    std::ofstream f(fullPathName);
    if (f.is_open()) {
        //--
        QColor qc;
        unsigned int ic;
        qc = foregroundColor.getColor();
        ic = RGBA(qc.red(), qc.green(), qc.blue(), qc.alpha());
        f << "FOREGROUND " << ic << std::endl;
        //--
        qc = backgroundColor.getColor();
        ic = RGBA(qc.red(), qc.green(), qc.blue(), qc.alpha());
        f << "BACKGROUND " << ic << std::endl;
        //--
        for (int i = 0; i < nbRows * nbColumns; i++) {
            qc = tblColors[i]->getColor();
            ic = RGBA(qc.red(), qc.green(), qc.blue(), qc.alpha());
            f << ic << std::endl;
        }
        f.close();
    }
}

bool Palette::load(QString pathName, QString fileName) {
    unsigned int ic;
    std::string strline;
    std::string strWord;
    std::string fullPathName;
    //-------------------------------------------------
    auto  stdStrPathName = pathName.toStdString();
    auto  stdStrFileName = fileName.toStdString();

    if ( stdStrPathName != "") {
        fullPathName = stdStrPathName + "/" + stdStrFileName;
    } else {
        fullPathName = stdStrFileName;
    }

    std::ifstream f(fullPathName);
    if (f.is_open()) {
        int i = 0;
        //--
        while (!f.eof()) {
            std::getline(f, strline);
            std::stringstream ss(strline);
            ss >> strWord;
            if (strWord == "FOREGROUND") {
                ss >> strWord;
                ic = std::atoi(strWord.c_str());
                foregroundColor.setColor(QColor(getRGBRed(ic), getRGBGreen(ic),
                                                getRGBBlue(ic), getRGBAlpha(ic)));
                qDebug() << "ForegroundColorChanged Signal emit" << foregroundColor.getColor();
                emit foregroundColorChanged(foregroundColor.getColor());
            } else if (strWord == "BACKGROUND") {
                ss >> strWord;
                ic = std::atoi(strWord.c_str());
                backgroundColor.setColor(QColor(getRGBRed(ic), getRGBGreen(ic),
                                                getRGBBlue(ic), getRGBAlpha(ic)));
            } else {
                ic = std::atoi(strWord.c_str());
                if (i < nbRows * nbColumns) {
                    tblColors[i]->setColor(QColor(getRGBRed(ic), getRGBGreen(ic),
                                                  getRGBBlue(ic), getRGBAlpha(ic)));
                    i++;
                } else {
                    break;
                }
            }
        }
        f.close();
        return true;
    }else{

        setDefaultPalette();

    }
    return false;
}

