#ifndef QIMAGEANIMATOR_H
#define QIMAGEANIMATOR_H

#include <QMainWindow>
#include <QString>

#include <QtCore/QTimer>
#include <QtCore/QVector>
#include <QtCore/QPointer>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui { class QImageAnimator; }
QT_END_NAMESPACE

class QClipPlayer;
class QStandardItemModel;

class QImageAnimator : public QMainWindow
{
    Q_OBJECT

public:
    QImageAnimator(QWidget *parent = nullptr);
    ~QImageAnimator();

private:
    enum RecordingStatus { RecordingIdle, RecordingStarted };

private:
    void SaveGif();

    void StartRecording();
    void StopRecording();

    void PopulateFileView();
    void LoadFramePreview(const QPixmap* aFrame);

private slots:
    void OnSelectAreaButtonClicked();
    void SelectDestinationDirectory();
    void OnSaveButtonClicked();
    void TakeScreenshot();
    void OnFilesViewEntryChanged(const QModelIndex &aCurrent, const QModelIndex &aPrevious);
    void StartStopGif();

private:
    Ui::QImageAnimator *ui;
    RecordingStatus mStatus;

    unsigned int mSamplingInterval;
    unsigned int mAnimationInterval;

    QString mFramesDestinationDir;
    QString mExportFilePath;

    QRect mScreenSelectionArea;
    QSize mSizeOutput;

    unsigned long long int mScreenshotNumber;
    QStandardItemModel* mFilesModel;
    QTimer* mRecordingTimer;

    QClipPlayer* mPlayer;
    QVector<QPixmap*> mFrames;
};
#endif // QIMAGEANIMATOR_H
