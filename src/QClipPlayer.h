#ifndef QCLIPPLAYER_H
#define QCLIPPLAYER_H

#include <QWidget>
#include <QPixmap>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsItem;
class QTimer;

class QClipPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit QClipPlayer(QWidget *aParent = nullptr);

    void SetClipImages(const QVector<QPixmap>&);


    void Play();
    void Stop();

    void SetFrameDelay(const unsigned int&);
    unsigned int FrameDelay() const;

private slots:
    void ShowNextFrame();

private:
    QGraphicsView* mView;
    QGraphicsScene* mScene;
    QTimer* mClipTimer;

    QVector<QGraphicsItem*> mFrames;
    unsigned int mCurrentFrameIdx;
    unsigned int mFrameDelay;
};

#endif // QCLIPPLAYER_H
