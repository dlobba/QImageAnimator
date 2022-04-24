#include "QImageAnimator.h"
#include "./ui_QImageAnimator.h"

#include <QtGui/QPixmap>
#include <QtGui/QScreen>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QPushButton>

#include "QClipPlayer.h"
#include "ScreenAreaSelection.h"
#include <QStandardItemModel>

#include <gif-h/gif.h>

static constexpr const int kRecordingIntervalDefault = 1000; // ms

QImageAnimator::QImageAnimator(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::QImageAnimator), mStatus(RecordingStatus::RecordingIdle),
      mSamplingInterval(0), mAnimationInterval(0), mSizeOutput({0, 0}), mScreenshotNumber(0)

{
    ui->setupUi(this);

    // populate resolution combo
    ui->resolutionCBox->addItem("Native", -1);
    ui->resolutionCBox->addItem("720p", 720);
    ui->resolutionCBox->addItem("480p", 480);
    ui->resolutionCBox->addItem("360p", 360);
    ui->resolutionCBox->addItem("240p", 240);
    ui->resolutionCBox->addItem("144p", 144);

    ui->outDirValueLabel->setText(tr("Select a Folder..."));
    connect(ui->outDirSelectButton,
            &QPushButton::clicked,
            this,
            &QImageAnimator::SelectDestinationDirectory);

    connect(ui->recordingStartStopButton, &QPushButton::clicked, [this] {
        if (mStatus == RecordingStatus::RecordingIdle) {
            StartRecording();
        } else {
            StopRecording();
        }
    });

    mRecordingTimer = new QTimer(this);
    mRecordingTimer->setSingleShot(false);
    mRecordingTimer->setInterval(kRecordingIntervalDefault);
    connect(mRecordingTimer, &QTimer::timeout, this, &QImageAnimator::TakeScreenshot);

    mFilesModel = new QStandardItemModel(0, 1, ui->recordingFilesView);
    ui->recordingFilesView->setModel(mFilesModel);

    connect(ui->recordingFilesView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this,
            &QImageAnimator::OnFilesViewEntryChanged);

    ui->recordingView->setScene(new QGraphicsScene(ui->recordingView));

    mPlayer = new QClipPlayer(this);
    ui->viewContainer->layout()->addWidget(mPlayer);
    mPlayer->hide(); // hidden by default

    connect(ui->previewButton, &QPushButton::clicked, this, &QImageAnimator::StartStopGif);
    connect(ui->exportFileButton, &QPushButton::clicked, this, &QImageAnimator::OnSaveButtonClicked);

    // controls page
    connect(ui->gotoGalleryButton, &QPushButton::clicked, [this] {
        ui->stackedWidget->setCurrentWidget(ui->galleryPage);
    });
    ui->gotoGalleryButton->setEnabled(false);

    connect(ui->areaSelectionButton, &QPushButton::clicked, this, &QImageAnimator::OnSelectAreaButtonClicked);

    // gallery page
    connect(ui->gotoExportButton, &QPushButton::clicked, [this] {
        ui->stackedWidget->setCurrentWidget(ui->controlsPage);
    });
}

QImageAnimator::~QImageAnimator()
{
    delete ui;
}

void QImageAnimator::SaveGif()
{
    mAnimationInterval = ui->animationIntervalSpinbox->value();
    auto delay = mAnimationInterval / 10;
    auto bitDepth = 8;
    GifWriter writer = {};
    GifBegin(&writer,
             mExportFilePath.toUtf8().data(),
             mSizeOutput.width(),
             mSizeOutput.height(),
             delay,
             bitDepth,
             false);

    for (auto row = 0; row < mFilesModel->rowCount(); row++) {
        auto entry = mFilesModel->item(row, 0);
        auto idx = entry->data().toInt();

        if (entry->checkState() != Qt::CheckState::Checked) {
            continue;
        }

        if (idx >= 0 && idx < mFrames.size()) {
            auto frame = mFrames.at(idx);
            if (frame && !frame->isNull()) {
                auto image = frame->scaled(mSizeOutput.width(), mSizeOutput.height()).toImage();
                image.convertTo(QImage::Format::Format_RGBA8888);
                const auto bits = image.constBits();
                GifWriteFrame(&writer, bits, image.width(), image.height(), delay, bitDepth, false);
            }
        }
    }

    GifEnd(&writer);
}

void QImageAnimator::StartRecording()
{
    mSamplingInterval = ui->samplingIntervalSpinBox->value();

    if (ui->areaFullScreenCBox->isChecked()) {
        mScreenSelectionArea.setRect(0,0,-1,-1);
    } else if (!mScreenSelectionArea.isValid()) {
        return;
    }

    auto screen = QGuiApplication::primaryScreen();
    if (!screen) {
        return;
    }

    auto height = ui->resolutionCBox->currentData().toInt();
    auto frame = screen->grabWindow(0,
                                    mScreenSelectionArea.x(),
                                    mScreenSelectionArea.y(),
                                    mScreenSelectionArea.width(),
                                    mScreenSelectionArea.height());
    if (height > 0) {
        frame = frame.scaledToHeight(height);
    }
    mSizeOutput = frame.size();

    // block navigation on controlsPage
    ui->gotoGalleryButton->setEnabled(false);
    ui->areaFullScreenCBox->setEnabled(false);
    ui->areaSelectionButton->setEnabled(false);
    mStatus = RecordingStarted;

    mFilesModel->removeRows(0, mFilesModel->rowCount());
    while (!mFrames.empty()) {
        delete mFrames.at(0);
        mFrames.pop_front();
    }
    mScreenshotNumber = 0;

    mRecordingTimer->setInterval(mSamplingInterval);
    mRecordingTimer->start();
    ui->recordingStartStopButton->setText(tr("Stop"));
}

void QImageAnimator::StopRecording()
{
    mStatus = RecordingIdle;
    mRecordingTimer->stop();
    ui->recordingStartStopButton->setText(tr("Start"));

    // enable navigation
    ui->gotoGalleryButton->setEnabled(true);
    ui->areaFullScreenCBox->setEnabled(true);
    ui->areaSelectionButton->setEnabled(true);
    mScreenSelectionArea = QRect(); // set invalid rect
}

void QImageAnimator::LoadFramePreview(const QPixmap *aFrame)
{
    if (!aFrame || aFrame->isNull()) {
        return;
    }
    auto frame = aFrame->scaledToWidth(ui->viewContainer->width(),
                                       Qt::TransformationMode::SmoothTransformation);
    ui->recordingView->scene()->clear();
    ui->recordingView->scene()->addPixmap(frame);
}

void QImageAnimator::OnSelectAreaButtonClicked()
{
    ScreenAreaSelection *area = new ScreenAreaSelection(this);
    area->setAttribute(Qt::WA_DeleteOnClose);

    connect(area, &ScreenAreaSelection::ScreenAreaSelected, [this](const auto aRect) {
        mScreenSelectionArea = aRect;
    });

    area->exec();
    return;
}

void QImageAnimator::SelectDestinationDirectory()
{
    auto startDir = QDir::homePath();
    if (!mFramesDestinationDir.isEmpty()) {
        startDir = mFramesDestinationDir;
    } else {
        auto previousDir = QFileInfo(mExportFilePath).dir();
        if (previousDir.isAbsolute() && previousDir.exists()) {
            startDir = previousDir.absolutePath();
        }
    }
    mFramesDestinationDir = QFileDialog::getExistingDirectory(this,
                                                              tr("Select output folder"),
                                                              startDir,
                                                              QFileDialog::Option::ShowDirsOnly);
    if (!mFramesDestinationDir.isEmpty()) {
        ui->outDirValueLabel->setText(mFramesDestinationDir);
    }
}

void QImageAnimator::OnSaveButtonClicked()
{
    auto startDir = QDir::homePath();
    auto previousDir = QFileInfo(mExportFilePath).dir();
    if (previousDir.isAbsolute() && previousDir.exists()) {
        startDir = previousDir.absolutePath();
    }
    auto path = QFileDialog::getSaveFileName(this,
                                             tr("Select output folder"),
                                             startDir,
                                             QString("*.gif"),
                                             nullptr,
                                             QFileDialog::Option::ShowDirsOnly);
    if (path.isEmpty()) {
        return;
    }
    mExportFilePath = path;
    SaveGif();
}

void QImageAnimator::TakeScreenshot()
{
    auto screen = QGuiApplication::primaryScreen();
    if (!screen) {
        return;
    }
    QPixmap *frame = new QPixmap(
        screen
            ->grabWindow(0,
                         mScreenSelectionArea.x(),
                         mScreenSelectionArea.y(),
                         mScreenSelectionArea.width(),
                         mScreenSelectionArea.height())
            .scaledToHeight(mSizeOutput.height(), Qt::TransformationMode::SmoothTransformation));

    if (frame->width() > mSizeOutput.width()) {
        mSizeOutput.setWidth(frame->width());
    }

    auto item = new QStandardItem(QString("shot_%1").arg(mScreenshotNumber));
    item->setData(mScreenshotNumber);
    item->setCheckable(true);
    item->setCheckState(Qt::CheckState::Checked);
    mFilesModel->appendRow(item);

    mFrames.append(frame);
    mScreenshotNumber++;
}

void QImageAnimator::OnFilesViewEntryChanged(const QModelIndex &aCurrent,
                                            const QModelIndex &aPrevious)
{
    ui->recordingView->show();
    mPlayer->hide();
    ui->viewContainer->adjustSize();

    auto item = mFilesModel->itemFromIndex(aCurrent);
    if (item && item->data().isValid()) {
        auto idx = item->data().toInt();
        if (idx >= 0 && idx < mFrames.count()) {
            auto framePtr = mFrames.at(idx);
            if (framePtr) {
                LoadFramePreview(framePtr);
            }
        }
    }
}

void QImageAnimator::StartStopGif()
{
    mPlayer->Stop();
    QVector<QPixmap> clipFrames;
    for (auto row = 0; row < mFilesModel->rowCount(); row++) {
        auto entry = mFilesModel->item(row, 0);
        auto idx = entry->data().toInt();

        if (entry->checkState() != Qt::CheckState::Checked) {
            continue;
        }
        if (idx >= 0 && idx < mFrames.size()) {
            auto frame = mFrames.at(idx);
            if (frame && !frame->isNull()) {
                clipFrames.append(*frame);
            }
        }
    }
    if (clipFrames.isEmpty()) {
        return;
    }

    ui->recordingView->hide();
    mPlayer->show();

    mPlayer->SetClipImages(clipFrames);
    mAnimationInterval = ui->animationIntervalSpinbox->value();
    mPlayer->SetFrameDelay(mAnimationInterval);
    mPlayer->Play();
}
