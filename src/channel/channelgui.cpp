#include "channelgui.h"
#include "ui_channelgui.h"

#include <log4cxx/logger.h>
#include <QMessageBox>

using namespace log4cxx;

ChannelGUI::ChannelGUI(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ChannelGUI)
{
    ui->setupUi(this);
    player = new Player(this);

    baseStylesheet = this->styleSheet();
    this->setStyleSheet(baseStylesheet + "QFrame { border-color: gray; }");

    chainPlayList = false;
    maximumDuration = QTime(23, 59, 59, 0);

    activeFilter = 0;

    this->initSignals();
    this->initPreview();

    this->ui->sliderSeek->setVisible(false);
    this->ui->labelSpeed->setVisible(false);
    this->ui->labelNoRef->setVisible(false);
    this->ui->pushButtonReverse->setVisible(false);
    this->ui->pushButtonForward->setVisible(false);
}

ChannelGUI::~ChannelGUI()
{
    this->disableSignals();

    delete ui;
    delete player;
}

void ChannelGUI::initSignals()
{
    this->connect(ui->pushButtonTake, SIGNAL(clicked()), this, SLOT(takeSlot()));
    this->connect(ui->pushButtonDrop, SIGNAL(clicked()), this, SLOT(dropSlot()));
    this->connect(ui->pushButtonRecue, SIGNAL(clicked()), this, SLOT(recueSlot()));
    this->connect(ui->pushButtonLoop, SIGNAL(clicked()), this, SLOT(loopSlot()));

    this->connect(ui->pushButtonPlayPause, SIGNAL(clicked()), this, SLOT(playPauseSlot()));
    this->connect(ui->pushButtonNextFrame, SIGNAL(clicked()), this, SLOT(nextFrameSlot()));
    this->connect(ui->pushButtonPreviousFrame, SIGNAL(clicked()), this, SLOT(previousFrameSlot()));
    this->connect(ui->pushButtonForward, SIGNAL(clicked()), this, SLOT(forwardSlot()));
    this->connect(ui->pushButtonReverse, SIGNAL(clicked()), this, SLOT(reverseSlot()));
    this->connect(ui->sliderSeek, SIGNAL(valueChanged(int)), this, SLOT(seekSlot(int)));

    this->connect(ui->preview, SIGNAL(setValues(QByteArray)), ui->audioMeter, SLOT(setValues(QByteArray)));
}

void ChannelGUI::disableSignals()
{
    this->disconnect(ui->pushButtonTake, SIGNAL(clicked()), this, SLOT(takeSlot()));
    this->disconnect(ui->pushButtonDrop, SIGNAL(clicked()), this, SLOT(dropSlot()));
    this->disconnect(ui->pushButtonRecue, SIGNAL(clicked()), this, SLOT(recueSlot()));
    this->disconnect(ui->pushButtonLoop, SIGNAL(clicked()), this, SLOT(loopSlot()));

    this->disconnect(ui->pushButtonPlayPause, SIGNAL(clicked()), this, SLOT(playPauseSlot()));
    this->disconnect(ui->pushButtonNextFrame, SIGNAL(clicked()), this, SLOT(nextFrameSlot()));
    this->disconnect(ui->pushButtonPreviousFrame, SIGNAL(clicked()), this, SLOT(previousFrameSlot()));
    this->disconnect(ui->pushButtonForward, SIGNAL(clicked()), this, SLOT(forwardSlot()));
    this->disconnect(ui->pushButtonReverse, SIGNAL(clicked()), this, SLOT(reverseSlot()));
    this->disconnect(ui->sliderSeek, SIGNAL(valueChanged(int)), this, SLOT(seekSlot(int)));

    this->disconnect(ui->preview, SIGNAL(setValues(QByteArray)), ui->audioMeter, SLOT(setValues(QByteArray)));

    disableShortcuts();
}

void ChannelGUI::initPreview()
{
#ifdef GUI
    if(player != NULL)
        player->togglePreviewVideo(ui->preview);
    QTimer::singleShot(100, this, SLOT(loadBars()));
#endif
}

void ChannelGUI::initShortcuts(const QString& takeShortcut, const QString& dropShortcut, const QString& recueShortcut, const QString& loopShortcut, const QString& channelLabel)
{
    pushButtonTakeShortcut = new QShortcut(QKeySequence(takeShortcut), this);
    this->connect(pushButtonTakeShortcut, SIGNAL(activated()), ui->pushButtonTake, SLOT(click()));
    pushButtonDropShortcut = new QShortcut(QKeySequence(dropShortcut), this);
    this->connect(pushButtonDropShortcut, SIGNAL(activated()), ui->pushButtonDrop, SLOT(click()));
    pushButtonRecueShortcut = new QShortcut(QKeySequence(recueShortcut), this);
    this->connect(pushButtonRecueShortcut, SIGNAL(activated()), ui->pushButtonRecue, SLOT(click()));
    pushButtonLoopShortcut = new QShortcut(QKeySequence(loopShortcut), this);
    this->connect(pushButtonLoopShortcut, SIGNAL(activated()), ui->pushButtonLoop, SLOT(click()));

    QStringList str = channelLabel.split(" ");
    this->ui->labelChannel->setText(this->ui->labelChannel->text() + "\n\n" + str[1]);
    this->debugName = channelLabel;

    this->ui->labelNoRef->setVisible(false);
}

void ChannelGUI::disableShortcuts()
{
    this->disconnect(pushButtonTakeShortcut, SIGNAL(activated()), ui->pushButtonTake, SLOT(click()));
    this->disconnect(pushButtonDropShortcut, SIGNAL(activated()), ui->pushButtonDrop, SLOT(click()));
    this->disconnect(pushButtonRecueShortcut, SIGNAL(activated()), ui->pushButtonRecue, SLOT(click()));
    this->disconnect(pushButtonLoopShortcut, SIGNAL(activated()), ui->pushButtonLoop, SLOT(click()));

    delete pushButtonTakeShortcut;
    delete pushButtonDropShortcut;
    delete pushButtonRecueShortcut;
    delete pushButtonLoopShortcut;
}

const bool ChannelGUI::isStopped() const
{
    if(player == NULL)
        return true;

    return !player->isPlaying() && !player->isPaused();
}

void ChannelGUI::take()
{
    if(player != NULL && player->isLoaded())
    {
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Play media");

        this->setStyleSheet(baseStylesheet + "QFrame { border-color: darkRed; }");
        ui->pushButtonTake->setStyleSheet("QPushButton { background-color: red; border-color: darkRed; }");
        ui->pushButtonPlayPause->setStyleSheet("QPushButton { border: none; background: url(:/images/buttons/pause.png); } QPushButton:pressed { background: url(:/images/buttons/pause_pressed.png); }");
        this->ui->labelSpeed->setVisible(true);
        this->ui->labelNoRef->setVisible(false);

        emit itemPlaying();

        if(!channelTimer.isActive())
            channelTimer.start(10);
    }
}

// PushButton Slots

void ChannelGUI::takeSlot()
{
    if(player != NULL && player->isLoaded() && !player->isPlaying())
    {
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Take pressed");

        player->take();
        take();
    }
}

void ChannelGUI::drop(const bool immediate)
{
    if(player != NULL && player->isLoaded())
    {
        player->dropMedia(immediate);

        if(channelTimer.isActive())
        {
            this->disconnect(&channelTimer, SIGNAL(timeout()), this, SLOT(channelStep()));
            channelTimer.stop();
        }

        ui->labelClipName->setText("No Clip Loaded");
        ui->labelClipDuration->setText("00:00:00:00");
        ui->labelClipRemaining->setText("00:00:00:00");
        this->setStyleSheet(baseStylesheet + "QFrame { border-color: gray; }");
        ui->pushButtonTake->setStyleSheet("QPushButton { background-color: lightGray; border-color: gray; }");
        ui->pushButtonPlayPause->setStyleSheet("QPushButton { border: none; background: url(:/images/buttons/play.png); } QPushButton:pressed { background: url(:/images/buttons/play_pressed.png); }");
        this->ui->labelSpeed->setVisible(false);
        this->ui->labelSpeed->setText("1x");
        this->ui->sliderSeek->setVisible(false);
        this->ui->sliderSeek->setValue(0);
        this->ui->labelNoRef->setVisible(false);
        QTimer::singleShot(100, ui->preview, SLOT(stopAudio()));

        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Dropped media");
    }
}

void ChannelGUI::dropSlot()
{
    if(player != NULL && player->isLoaded())
    {
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Drop pressed");

        ui->pushButtonDrop->blockSignals(true);
        drop(true);
        emit loadNextItem();
        QTimer::singleShot(QApplication::doubleClickInterval(), this, SLOT(dropReactivate()));
    }
}

void ChannelGUI::dropReactivate()
{
    ui->pushButtonDrop->blockSignals(false);
}

void ChannelGUI::recue()
{
    if(player != NULL && player->isLoaded())
    {
        player->recue();

        this->setStyleSheet(baseStylesheet + "QFrame { border-color: darkGreen; }");
        ui->pushButtonTake->setStyleSheet("QPushButton { background-color: lime; border-color: green; }");
        ui->pushButtonPlayPause->setStyleSheet("QPushButton { border: none; background: url(:/images/buttons/play.png); } QPushButton:pressed { background: url(:/images/buttons/play_pressed.png); }");
        this->ui->labelSpeed->setText("1x");
        this->ui->labelSpeed->setVisible(false);
        this->ui->sliderSeek->blockSignals(true);
        this->ui->sliderSeek->setValue(0);
        this->ui->sliderSeek->blockSignals(false);
        channelStep();
        QTimer::singleShot(100, ui->preview, SLOT(stopAudio()));

        emit itemLoaded();

        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Recued media");
    }
}

void ChannelGUI::recueSlot()
{
    if(player != NULL && player->isLoaded())
    {
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Recue pressed");

        ui->pushButtonTake->blockSignals(true);
        ui->pushButtonPlayPause->blockSignals(true);
        ui->pushButtonRecue->blockSignals(true);

        recue();
        QTimer::singleShot(QApplication::doubleClickInterval(), this, SLOT(recueReactivate()));

        if(channelTimer.isActive())
            channelTimer.stop();
    }
}

void ChannelGUI::recueReactivate()
{
    ui->pushButtonTake->blockSignals(false);
    ui->pushButtonPlayPause->blockSignals(false);
    ui->pushButtonRecue->blockSignals(false);
}

void ChannelGUI::loopSlot()
{
    if(ui->pushButtonLoop->isChecked())
    {
        if(player != NULL)
            player->toggleLoop(true);
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Loop activated");
    }
    else
    {
        if(player != NULL)
            player->toggleLoop(false);
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Loop deactivated");
    }
}

void ChannelGUI::playPauseSlot()
{
    if(player != NULL && player->isLoaded())
    {
        if(player->isPlaying())
        {
            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Pause pressed");

            player->pause(true);
            this->setStyleSheet(baseStylesheet + "QFrame { border-color: darkGoldenrod; }");
            ui->pushButtonTake->setStyleSheet("QPushButton { background-color: gold; border-color: darkGoldenrod; }");
            ui->pushButtonPlayPause->setStyleSheet("QPushButton { border: none; background: url(:/images/buttons/play.png); } QPushButton:pressed { background: url(:/images/buttons/play_pressed.png); }");
            this->ui->labelSpeed->setVisible(false);
			this->ui->labelSpeed->setText("1x");

            if(channelTimer.isActive())
                channelTimer.stop();
        }
        else
        {
            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Play pressed");

            player->take();
            take();

            if(!channelTimer.isActive())
                channelTimer.start(10);
        }
    }
}

void ChannelGUI::nextFrameSlot()
{
    if(player != NULL && player->isLoaded() && !player->isPlaying())
    {
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Next frame pressed");

        player->nextFrame();
        channelStep();
    }
}

void ChannelGUI::previousFrameSlot()
{
    if(player != NULL && player->isLoaded() && !player->isPlaying())
    {
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Previous frame pressed");

        player->previousFrame();
        channelStep();
    }
}

void ChannelGUI::forwardSlot()
{
    if(player != NULL && player->isLoaded() && player->isPlaying())
    {
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Forward pressed");

        this->ui->labelSpeed->setText(QString::number(player->forward()) + "x");
    }
}

void ChannelGUI::reverseSlot()
{
    if(player != NULL && player->isLoaded() && player->isPlaying())
    {
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Reverse pressed");

        this->ui->labelSpeed->setText(QString::number(player->reverse()) + "x");
    }
}

void ChannelGUI::seekSlot(const int value)
{
    if(player != NULL && player->isLoaded() && !player->isPlaying())
    {
        player->seek(value);

        ui->sliderSeek->blockSignals(true);
        channelStep();
        ui->sliderSeek->blockSignals(false);

        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Seek value changed");
    }
}

// PlayList Slots

void ChannelGUI::loadItem(const PlayListItem& item, const bool canTake)
{
    if(player != NULL && !player->isPlaying())
    {
        if(player->isLoaded())
            drop(true);

        if(item.path == "")
            return;

        int64_t duration_ms = player->loadMedia(item.path);
        if(duration_ms != -1)
        {
            this->ui->sliderSeek->setVisible(true);
            this->ui->sliderSeek->setMaximum(item.duration_ms);

            ui->labelClipName->setText(item.name);
            clipDuration = QTime::fromString(item.duration, "hh:mm:ss.zzz");

            emit itemLoaded();

            this->setStyleSheet(baseStylesheet + "QFrame { border-color: darkGreen; }");
            ui->pushButtonTake->setStyleSheet("QPushButton { background-color: lime; border-color: green; }");

            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Loaded clip: " + item.name.toStdString());

            if(!channelTimer.isActive())
            {
                this->connect(&channelTimer, SIGNAL(timeout()), this, SLOT(channelStep()));
                channelStep();
            }

            if(chainPlayList && canTake)
                takeSlot();
        }
        else
        {
            QString msg("Error loading file \"" + item.name + "\"");
            QMessageBox::critical((QWidget*)this->parent(), "Error", msg);
            LOG4CXX_ERROR(Logger::getLogger(debugName.toStdString()), msg.toStdString());
        }
    }
}

void ChannelGUI::playListChain(const bool chain)
{
    chainPlayList = chain;
}

// Helper Slots

void ChannelGUI::loadBars()
{
    if(player != NULL && !player->isLoaded())
        ui->preview->loadBars();
}

void ChannelGUI::channelStep()
{
    if(player != NULL)
    {
        int64_t msecs = player->getCurrentPlayTime();
        this->ui->sliderSeek->setValue(msecs);

        QTime newTime = clipDuration.addMSecs(-msecs);
        QTime newDuration = QTime(0, 0, 0, 0).addMSecs(player->getCurrentTimeCode());
        if(msecs == 0 && player->isPlaying())
            newTime = newTime.addMSecs(-10);

        if(!player->isEOF() && newTime < maximumDuration)
        {
            QStringList splitDuration = newDuration.toString("hh:mm:ss.zzz").split(".");
            int framesDuration = splitDuration.at(1).toInt() * 25 / 1000;
            ui->labelClipDuration->setText(splitDuration[0] + ":" + QString::number(framesDuration).rightJustified(2, '0'));

            QStringList split = newTime.toString("hh:mm:ss.zzz").split(".");
            int frames = split.at(1).toInt() * 25 / 1000;
            ui->labelClipRemaining->setText(split[0] + ":" + QString::number(frames).rightJustified(2, '0'));
        }
        else
        {
            ui->labelClipDuration->setText("00:00:00:00");
            ui->labelClipRemaining->setText("00:00:00:00");

            if(!ui->pushButtonLoop->isChecked())
            {
                drop(false);
                emit loadNextItem();
            }
        }
    }
}

void ChannelGUI::genlockErrorSlot()
{
    if(!ui->labelNoRef->isVisible())
    {
        LOG4CXX_ERROR(Logger::getLogger(debugName.toStdString()), "Error in genlock reference");
    }
    this->ui->labelNoRef->setVisible(true);
}

// Config Slots

void ChannelGUI::interfaceChanged(const QString& inter)
{
#ifndef DECKLINK
    Q_UNUSED(inter);
#endif

    if(player != NULL && !player->isPlaying())
    {
#ifdef DECKLINK
        const DeckLinkOutput* outputDL = player->getDecklinkOutput();
        if(outputDL != NULL)
            this->disconnect(outputDL, SIGNAL(genlockError()), this, SLOT(genlockErrorSlot()));

        QString interf = player->setDeckLinkOutput(inter);

        outputDL = player->getDecklinkOutput();
        if(outputDL != NULL)
            this->connect(outputDL, SIGNAL(genlockError()), this, SLOT(genlockErrorSlot()));
#endif
        recue();
        if(!player->isLoaded())
            ui->preview->loadBars();
#ifdef DECKLINK
        emit changedInterface(interf, ui->labelChannel->text());
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Decklink output changed to: " + interf.toStdString());
#endif
    }
}

void ChannelGUI::changedFormat(const QString& format)
{
    if(player != NULL && !player->isPlaying())
    {
        player->changeFormat(format);
        seekSlot(0);
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Format changed to: " + format.toStdString());
    }
}

void ChannelGUI::cgChanged(const QString& cg)
{
    player->activateFilter(cg);
    LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Activated filter: " + cg.toStdString());
}

void ChannelGUI::togglePreviewVideo(const int state)
{
#ifdef GUI
    if(player != NULL && !player->isPlaying())
    {
        if(state == 2)
        {
            player->togglePreviewVideo(ui->preview);
            recue();
        }
        else if(state == 0)
        {
            player->togglePreviewVideo(NULL);
            QTimer::singleShot(100, this, SLOT(loadBars()));
        }
    }
#else
    Q_UNUSED(state);
#endif
}

void ChannelGUI::togglePreviewAudio(const int state)
{
#ifdef GUI
    if(player != NULL && !player->isPlaying())
    {
        if(state == 2)
        {
            player->togglePreviewAudio(true);
            recue();
        }
        else if(state == 0)
            player->togglePreviewAudio(false);
    }
#else
    Q_UNUSED(state);
#endif
}
