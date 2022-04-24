#include "ScreenAreaSelection.h"
#include <QMouseEvent>
#include <QRubberBand>

constexpr const unsigned int kMinSize = 50; // px

ScreenAreaSelection::ScreenAreaSelection(QWidget *aParent) : QDialog{aParent}
{
    setWindowOpacity(0.5);
    showFullScreen();
    mRubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

void ScreenAreaSelection::mousePressEvent(QMouseEvent *event)
{
    mOrigin = event->pos();

    mRubberBand->setGeometry(QRect(mOrigin, QSize()));
    mRubberBand->show();
}

void ScreenAreaSelection::mouseMoveEvent(QMouseEvent *event)
{
    mRubberBand->setGeometry(QRect(mOrigin, event->pos()).normalized());
}

void ScreenAreaSelection::mouseReleaseEvent(QMouseEvent *)
{
    mRubberBand->hide();
    auto selectionSize = mRubberBand->size();
    if (selectionSize.height() < kMinSize || selectionSize.width() < kMinSize) {
        return;
    }

    emit ScreenAreaSelected(mRubberBand->geometry());
    accept();
}
