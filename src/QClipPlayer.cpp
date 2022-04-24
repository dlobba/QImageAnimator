#include "QClipPlayer.h"

#include <QDebug>
#include <QtCore/QTimer>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QVBoxLayout>

constexpr const unsigned int kDefaultFrameDelay = 1000; // ms

constexpr const QSize kMinSize(480, 360);

QClipPlayer::QClipPlayer(QWidget *aParent)
    : QWidget{aParent}, mCurrentFrameIdx(0), mFrameDelay(kDefaultFrameDelay)
{
    setMinimumSize(kMinSize);

    setLayout(new QVBoxLayout(this));

    mClipTimer = new QTimer(this);

    mView = new QGraphicsView(this);
    mScene = new QGraphicsScene(mView);
    mView->setScene(mScene);

    mClipTimer->setInterval(mFrameDelay);
    mClipTimer->setSingleShot(false);
    connect(mClipTimer, &QTimer::timeout, this, &QClipPlayer::ShowNextFrame);

    layout()->addWidget(mView);
}

void QClipPlayer::SetClipImages(const QVector<QPixmap> &aClipFrames)
{
    mFrames.clear();
    mScene->clear();

    for (const auto &clipFrame : aClipFrames) {
        auto frame = mScene->addPixmap(clipFrame);
        frame->hide();
        mFrames.append(frame);
    }
    mCurrentFrameIdx = 0;
}

void QClipPlayer::Play()
{
    if (!mClipTimer->isActive()) {
        mClipTimer->start();
    }
}

void QClipPlayer::Stop()
{
    if (mClipTimer->isActive()) {
        mClipTimer->stop();
    }
}

void QClipPlayer::SetFrameDelay(const unsigned int &aFrameDelay)
{
    mFrameDelay = aFrameDelay;
    mClipTimer->setInterval(mFrameDelay);
}

unsigned int QClipPlayer::FrameDelay() const
{
    return mFrameDelay;
}

void QClipPlayer::ShowNextFrame()
{
    if (mFrames.size() == 1) {
        mFrames.at(0)->show();
    } else if (mFrames.size() > 1) {
        auto previousFrameIdx = mCurrentFrameIdx == 0 ? mFrames.size() - 1 : mCurrentFrameIdx - 1;
        mFrames.at(previousFrameIdx)->hide();
        mFrames.at(mCurrentFrameIdx)->show();
        mScene->setFocusItem(mFrames.at(mCurrentFrameIdx));

        mCurrentFrameIdx = mCurrentFrameIdx == mFrames.size() - 1 ? 0 : mCurrentFrameIdx + 1;
    }
}
