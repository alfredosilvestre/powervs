#include "decklinkcontrol.h"
#include "mainwindow.h"
#include "ui_config.h"
#include "ui_mainwindow.h"
#include "ui_playlistgui.h"
#include "ui_channelgui.h"
#include "ui_recordergui.h"

#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    config(new Ui::Dialog)
{
    ui->setupUi(this);
    config->setupUi(&configDialog);
    configDialog.setWindowFlags(Qt::Tool);
    settings = new QSettings("PowerVS", "Settings");

    this->initInterface();
    this->initSignals();
    this->initDecklink();
    this->initFormats();
    this->initCG();
    this->initModes();

    QString path = settings->value("RecordPathA").toString();
    if(path != "")
        ui->recorderA->changeRecordPath(path);
    path = settings->value("RecordPathB").toString();
    if(path != "")
        ui->recorderB->changeRecordPath(path);
    this->checkAutoloadPlaylists();
}

MainWindow::~MainWindow()
{
    disableSignals();

    delete config;
    delete ui;
    delete configShortcut;
    delete settings;
}

void MainWindow::initInterface()
{
    QDesktopWidget* desktop = QApplication::desktop();
    int width = desktop->width()/desktop->numScreens();

    int halfVCanvas = desktop->height()/2 - 15;
    int channelWidth = this->ui->channelA->width() * halfVCanvas / this->ui->channelA->height() + 10;
    int halfHCanvas = (this->ui->playListA->width() + channelWidth) / 2;

    int x = width/2 - halfHCanvas;
    if(width <= 1280)
        x = 15;

    initPlaylists(x, halfVCanvas);

    x += this->ui->playListA->width() + 10;
    int height = halfVCanvas;
    int y = halfVCanvas - this->ui->channelA->height() + 10;
    if(width <= 1280)
        height = this->ui->channelA->height();
    else y = 10;

    initChannelRecorder(x, y, height, this->ui->channelA, this->ui->recorderA);
    y += this->ui->channelA->height() + 10;
    initChannelRecorder(x, y, height, this->ui->channelB, this->ui->recorderB);

    this->ui->recorderA->setVisible(false);
    this->ui->recorderB->setVisible(false);

    x += ui->channelA->width()/2;
    y -= this->ui->channelA->height() + ui->pushButtonFillKeyTake->height() + 20;
    if(y < 0)
        y = 0;
    ui->pushButtonFillKeyTake->setGeometry(x - ui->pushButtonFillKeyTake->width() - 10, y, ui->pushButtonFillKeyTake->width(), ui->pushButtonFillKeyTake->height());
    ui->pushButtonFillKeyDrop->setGeometry(x + 10, y, ui->pushButtonFillKeyDrop->width(), ui->pushButtonFillKeyDrop->height());
}

void MainWindow::initPlaylists(int x, int height)
{
    int width = this->ui->playListA->width();

    this->ui->playListA->setGeometry(x, 10, width, height);
    this->ui->playListB->setGeometry(x, height + 20, width, height);

    int x2 = this->ui->playListA->ui->pushButtonAddFile->x();
    int width2 = this->ui->playListA->ui->pushButtonAddFile->width();
    int height2 = this->ui->playListA->ui->pushButtonAddFile->height();
    this->ui->playListA->ui->pushButtonAddFile->setGeometry(x2, height - 41, width2, height2);
    this->ui->playListB->ui->pushButtonAddFile->setGeometry(x2, height - 41, width2, height2);

    x2 = this->ui->playListA->ui->pushButtonRemoveFile->x();
    width2 = this->ui->playListA->ui->pushButtonRemoveFile->width();
    this->ui->playListA->ui->pushButtonRemoveFile->setGeometry(x2, height - 41, width2, height2);
    this->ui->playListB->ui->pushButtonRemoveFile->setGeometry(x2, height - 41, width2, height2);

    x2 = this->ui->playListA->ui->pushButtonRecuePlaylist->x();
    width2 = this->ui->playListA->ui->pushButtonRecuePlaylist->width();
    this->ui->playListA->ui->pushButtonRecuePlaylist->setGeometry(x2, height - 41, width2, height2);
    this->ui->playListB->ui->pushButtonRecuePlaylist->setGeometry(x2, height - 41, width2, height2);

    x2 = this->ui->playListA->ui->pushButtonLoop->x();
    width2 = this->ui->playListA->ui->pushButtonLoop->width();
    this->ui->playListA->ui->pushButtonLoop->setGeometry(x2, height - 41, width2, height2);
    this->ui->playListB->ui->pushButtonLoop->setGeometry(x2, height - 41, width2, height2);

    x2 = this->ui->playListA->ui->pushButtonBlock->x();
    width2 = this->ui->playListA->ui->pushButtonBlock->width();
    this->ui->playListA->ui->pushButtonBlock->setGeometry(x2, height - 41, width2, height2);
    this->ui->playListB->ui->pushButtonBlock->setGeometry(x2, height - 41, width2, height2);

    x2 = this->ui->playListA->ui->pushButtonChain->x();
    width2 = this->ui->playListA->ui->pushButtonChain->width();
    this->ui->playListA->ui->pushButtonChain->setGeometry(x2, height - 41, width2, height2);
    this->ui->playListB->ui->pushButtonChain->setGeometry(x2, height - 41, width2, height2);

    x2 = this->ui->playListA->ui->tableWidget->x();
    int y2 = this->ui->playListA->ui->tableWidget->y();
    width2 = this->ui->playListA->ui->tableWidget->width();
    this->ui->playListA->ui->tableWidget->setGeometry(x2, y2, width2, height - 70);
    this->ui->playListB->ui->tableWidget->setGeometry(x2, y2, width2, height - 70);
}

void MainWindow::initChannelRecorder(int x, int y, int height, ChannelGUI* channel, RecorderGUI* recorder)
{
    int x2 = channel->ui->videoFrame->x();
    int y2 = channel->ui->videoFrame->y();
    int height2 = height * channel->ui->videoFrame->height() / channel->height();
    int width2 = height * channel->ui->videoFrame->width() / channel->height();
    channel->ui->videoFrame->setGeometry(x2, y2, width2, height2);
    recorder->ui->videoFrame->setGeometry(x2, y2, width2, height2);

    y2 += height2 + 5;
    channel->ui->sliderSeek->setGeometry(x2, y2, width2, channel->ui->sliderSeek->height());
    y2 += 5;
    recorder->ui->labelPath->setGeometry(x2, y2, recorder->ui->labelPath->width(), recorder->ui->labelPath->height());
    x2 += recorder->ui->labelPath->width();
    recorder->ui->labelRecordPath->setGeometry(x2, y2, width2 - recorder->ui->labelPath->width() - recorder->ui->pushButtonChoosePath->width() - 10, recorder->ui->labelRecordPath->height());
    x2 += recorder->ui->labelRecordPath->width() + 10;
    recorder->ui->pushButtonChoosePath->setGeometry(x2, y2, recorder->ui->pushButtonChoosePath->width(), recorder->ui->pushButtonChoosePath->height());

    x2 = channel->ui->preview->x();
    y2 = channel->ui->preview->y();
    channel->ui->preview->setGeometry(x2, y2, width2 - 4, height2 - 4);
    recorder->ui->preview->setGeometry(x2, y2, width2 - 4, height2 - 4);

    x2 = channel->ui->videoFrame->x() + channel->ui->videoFrame->width() + 2;
    y2 = channel->ui->videoFrame->y();
    height2 = height * channel->ui->audioMeter->height() / channel->height();
    width2 = height * channel->ui->audioMeter->width() / channel->height();
    channel->ui->audioMeter->setGeometry(x2, y2, width2, height2);
    recorder->ui->audioMeter->setGeometry(x2, y2, width2, height2);

    x2 = channel->ui->labelClipName->x();
    y2 = channel->ui->labelClipName->y();
    height2 = channel->ui->labelClipName->height();
    width2 = channel->ui->videoFrame->width() + channel->ui->audioMeter->width() - 5;
    channel->ui->labelClipName->setGeometry(x2, y2, width2, height2);
    recorder->ui->lineEditClipName->setGeometry(x2, y2, width2, height2);

    y2 = channel->ui->labelClipDuration->y();
    width2 = width2 / 2 - 10;
    channel->ui->labelClipDuration->setGeometry(x2, y2, width2, height2);
    recorder->ui->pushButtonTimecode->setGeometry(x2, y2, width2, height2);
    x2 += width2 + 20;
    channel->ui->labelClipRemaining->setGeometry(x2, y2, width2, height2);
    recorder->ui->labelDuration->setGeometry(x2, y2, width2, height2);

    QFont font = channel->ui->pushButtonTake->font();
    if(height > channel->height())
        font.setPointSize(34);

    x2 = channel->ui->audioMeter->x() + channel->ui->audioMeter->width() + 19;
    y2 = channel->ui->pushButtonTake->y();
    height2 = height * channel->ui->pushButtonTake->height() / channel->height();
    width2 = height * channel->ui->pushButtonTake->width() / channel->height();
    channel->ui->pushButtonTake->setGeometry(x2, y2, width2, height2);
    channel->ui->pushButtonTake->setFont(font);
    recorder->ui->pushButtonStart->setGeometry(x2, y2, width2, height2);
    recorder->ui->pushButtonStart->setFont(font);

    y2 += height2 + 9;
    channel->ui->pushButtonDrop->setGeometry(x2, y2, width2, height2);
    channel->ui->pushButtonDrop->setFont(font);
    recorder->ui->pushButtonStop->setGeometry(x2, y2, width2, height2);
    recorder->ui->pushButtonStop->setFont(font);

    y2 += height2 + 9;
    channel->ui->pushButtonRecue->setGeometry(x2, y2, width2, height2);
    channel->ui->pushButtonRecue->setFont(font);

    y2 += height2 + 9;
    channel->ui->pushButtonLoop->setGeometry(x2, y2, width2, height2);
    channel->ui->pushButtonLoop->setFont(font);

    y2 += height2 + 27;
    channel->ui->labelSpeed->setGeometry(x2, y2 - 10, width2, channel->ui->labelSpeed->height());
    x2 = channel->ui->audioMeter->x() + 2*channel->ui->audioMeter->width()/3;
    recorder->ui->labelSignalError->setGeometry(x2, y2 - 15, recorder->ui->labelSignalError->width(), recorder->ui->labelSignalError->height());

    width2 = channel->ui->pushButtonReverse->width();
    height2 = channel->ui->pushButtonReverse->height();
    x2 = channel->ui->videoFrame->x() + channel->ui->videoFrame->width()/2 - 140 - width2 / 2;
    channel->ui->pushButtonReverse->setGeometry(x2, y2, width2, height2);

    x2 += 70;
    channel->ui->pushButtonPreviousFrame->setGeometry(x2, y2, width2, height2);

    x2 += 70;
    channel->ui->pushButtonPlayPause->setGeometry(x2, y2, width2, height2);

    x2 += 70;
    channel->ui->pushButtonNextFrame->setGeometry(x2, y2, width2, height2);

    x2 += 70;
    channel->ui->pushButtonForward->setGeometry(x2, y2, width2, height2);

    x2 += 70;
    channel->ui->labelNoRef->setGeometry(channel->ui->audioMeter->geometry().x(), channel->ui->labelSpeed->y(), channel->ui->labelNoRef->width(), channel->ui->labelNoRef->height());

    width2 = channel->ui->pushButtonTake->x() + channel->ui->pushButtonTake->width() + 20;
    height2 = y2 + channel->ui->pushButtonReverse->height() + 12;
    y += height - height2;
    channel->setGeometry(x, y, width2, height2);
    recorder->setGeometry(x, y, width2, height2);
}

void MainWindow::initSignals()
{
    initSignalsA();
    initSignalsB();

    fillKeyTakeShortcut = new QShortcut(QKeySequence("+"), this);
    fillKeyDropShortcut = new QShortcut(QKeySequence("-"), this);
    ui->pushButtonFillKeyTake->setVisible(false);
    ui->pushButtonFillKeyDrop->setVisible(false);

    this->connect(config->checkBoxFillKey, SIGNAL(stateChanged(int)), this, SLOT(toggleFillKey(int)));
    this->connect(config->checkBoxAutoloadPlaylists, SIGNAL(stateChanged(int)), this, SLOT(toggleAutoloadPlaylists(int)));

    bool fillKey = settings->value("EnableFillKey").toBool();
    if(fillKey)
        config->checkBoxFillKey->setChecked(true);
    else config->checkBoxFillKey->setChecked(false);

    this->connect(config->checkBoxVideoPreviewerA, SIGNAL(stateChanged(int)), this, SLOT(toggleVideoPreviewA(int)));
    this->connect(config->checkBoxVideoPreviewerB, SIGNAL(stateChanged(int)), this, SLOT(toggleVideoPreviewB(int)));
    this->connect(config->checkBoxAudioPreviewerA, SIGNAL(stateChanged(int)), this, SLOT(toggleAudioPreviewA(int)));
    this->connect(config->checkBoxAudioPreviewerB, SIGNAL(stateChanged(int)), this, SLOT(toggleAudioPreviewB(int)));

    this->connect(config->comboBoxVideoFormatChannelA, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedFormatA(QString)));
    this->connect(config->comboBoxVideoFormatChannelB, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedFormatB(QString)));

    this->connect(config->comboBoxModeChannelA, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedModeA(QString)));
    this->connect(config->comboBoxModeChannelB, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedModeB(QString)));

    configShortcut = new QShortcut(QKeySequence("Ctrl+Shift+F12"), this);
    this->connect(configShortcut, SIGNAL(activated()), this, SLOT(configOpenSlot()));
}

void MainWindow::initSignalsA()
{
    if(config->labelChannelA->text().contains("Playout"))
    {
        this->connect(ui->channelA, SIGNAL(itemLoaded()), ui->playListA, SLOT(itemLoaded()));
        this->connect(ui->channelA, SIGNAL(itemPlaying()), ui->playListA, SLOT(itemPlaying()));
        this->connect(ui->channelA, SIGNAL(loadNextItem()), ui->playListA, SLOT(loadNextItem()));
        this->connect(ui->playListA, SIGNAL(loadItem(PlayListItem, bool)), ui->channelA, SLOT(loadItem(PlayListItem, bool)));
        this->connect(ui->playListA, SIGNAL(recueItem()), ui->channelA, SLOT(recue()));
        this->connect(ui->playListA, SIGNAL(playListChain(bool)), ui->channelA, SLOT(playListChain(bool)));
        this->connect(ui->channelA, SIGNAL(changedInterface(QString,QString)), this, SLOT(changedInterface(QString,QString)));

        this->connect(config->comboBoxInterfaceChannelA, SIGNAL(currentIndexChanged(QString)), ui->channelA, SLOT(interfaceChanged(QString)));
        this->connect(config->comboBoxVideoFormatChannelA, SIGNAL(currentIndexChanged(QString)), ui->channelA, SLOT(changedFormat(QString)));
        this->connect(config->comboBoxCGChannelA, SIGNAL(currentIndexChanged(QString)), ui->channelA, SLOT(cgChanged(QString)));
        this->connect(config->checkBoxVideoPreviewerA, SIGNAL(stateChanged(int)), ui->channelA, SLOT(togglePreviewVideo(int)));
        this->connect(config->checkBoxAudioPreviewerA, SIGNAL(stateChanged(int)), ui->channelA, SLOT(togglePreviewAudio(int)));

        ui->playListA->initShortcuts("F5", "F6", "PlayList A");
        ui->channelA->initShortcuts("F1", "F2", "F3", "F4", config->labelChannelA->text());
    }
    else
    {
        this->connect(ui->recorderA, SIGNAL(changedInterface(QString,QString)), this, SLOT(changedInterface(QString,QString)));
        this->connect(ui->recorderA, SIGNAL(changedPath(QString)), this, SLOT(changedRecordPathA(QString)));

        this->connect(config->comboBoxInterfaceChannelA, SIGNAL(currentIndexChanged(QString)), ui->recorderA, SLOT(interfaceChanged(QString)));
        this->connect(config->comboBoxVideoFormatChannelA, SIGNAL(currentIndexChanged(QString)), ui->recorderA, SLOT(changedFormat(QString)));
        this->connect(config->comboBoxCGChannelA, SIGNAL(currentIndexChanged(QString)), ui->recorderA, SLOT(cgChanged(QString)));
        this->connect(config->checkBoxVideoPreviewerA, SIGNAL(stateChanged(int)), ui->recorderA, SLOT(togglePreviewVideo(int)));
        this->connect(config->checkBoxAudioPreviewerA, SIGNAL(stateChanged(int)), ui->recorderA, SLOT(togglePreviewAudio(int)));

        this->connect(this, SIGNAL(sendFFError(int,QString)), ui->recorderA, SLOT(checkFFError(int,QString)));

        ui->recorderA->initShortcuts("F1", "F2", config->labelChannelA->text());
    }
}

void MainWindow::initSignalsB()
{
    if(config->labelChannelB->text().contains("Playout"))
    {
        this->connect(ui->channelB, SIGNAL(itemLoaded()), ui->playListB, SLOT(itemLoaded()));
        this->connect(ui->channelB, SIGNAL(itemPlaying()), ui->playListB, SLOT(itemPlaying()));
        this->connect(ui->channelB, SIGNAL(loadNextItem()), ui->playListB, SLOT(loadNextItem()));
        this->connect(ui->playListB, SIGNAL(loadItem(PlayListItem, bool)), ui->channelB, SLOT(loadItem(PlayListItem, bool)));
        this->connect(ui->playListB, SIGNAL(recueItem()), ui->channelB, SLOT(recue()));
        this->connect(ui->playListB, SIGNAL(playListChain(bool)), ui->channelB, SLOT(playListChain(bool)));
        this->connect(ui->channelB, SIGNAL(changedInterface(QString,QString)), this, SLOT(changedInterface(QString,QString)));

        this->connect(config->comboBoxInterfaceChannelB, SIGNAL(currentIndexChanged(QString)), ui->channelB, SLOT(interfaceChanged(QString)));
        this->connect(config->comboBoxVideoFormatChannelB, SIGNAL(currentIndexChanged(QString)), ui->channelB, SLOT(changedFormat(QString)));
        this->connect(config->comboBoxCGChannelB, SIGNAL(currentIndexChanged(QString)), ui->channelB, SLOT(cgChanged(QString)));
        this->connect(config->checkBoxVideoPreviewerB, SIGNAL(stateChanged(int)), ui->channelB, SLOT(togglePreviewVideo(int)));
        this->connect(config->checkBoxAudioPreviewerB, SIGNAL(stateChanged(int)), ui->channelB, SLOT(togglePreviewAudio(int)));

        ui->playListB->initShortcuts("F7", "F8", "PlayList B");
        ui->channelB->initShortcuts("F9", "F10", "F11", "F12", config->labelChannelB->text());
    }
    else
    {
        this->connect(ui->recorderB, SIGNAL(changedInterface(QString,QString)), this, SLOT(changedInterface(QString,QString)));
        this->connect(ui->recorderB, SIGNAL(changedPath(QString)), this, SLOT(changedRecordPathB(QString)));

        this->connect(config->comboBoxInterfaceChannelB, SIGNAL(currentIndexChanged(QString)), ui->recorderB, SLOT(interfaceChanged(QString)));
        this->connect(config->comboBoxVideoFormatChannelB, SIGNAL(currentIndexChanged(QString)), ui->recorderB, SLOT(changedFormat(QString)));
        this->connect(config->comboBoxCGChannelB, SIGNAL(currentIndexChanged(QString)), ui->recorderB, SLOT(cgChanged(QString)));
        this->connect(config->checkBoxVideoPreviewerB, SIGNAL(stateChanged(int)), ui->recorderB, SLOT(togglePreviewVideo(int)));
        this->connect(config->checkBoxAudioPreviewerB, SIGNAL(stateChanged(int)), ui->recorderB, SLOT(togglePreviewAudio(int)));

        this->connect(this, SIGNAL(sendFFError(int,QString)), ui->recorderB, SLOT(checkFFError(int,QString)));

        ui->recorderB->initShortcuts("F9", "F10", config->labelChannelB->text());
    }
}

void MainWindow::disableSignals()
{
    disableSignalsA();
    disableSignalsB();

    this->disconnect(config->checkBoxVideoPreviewerA, SIGNAL(stateChanged(int)), this, SLOT(toggleVideoPreviewA(int)));
    this->disconnect(config->checkBoxVideoPreviewerB, SIGNAL(stateChanged(int)), this, SLOT(toggleVideoPreviewB(int)));
    this->disconnect(config->checkBoxAudioPreviewerA, SIGNAL(stateChanged(int)), this, SLOT(toggleAudioPreviewA(int)));
    this->disconnect(config->checkBoxAudioPreviewerB, SIGNAL(stateChanged(int)), this, SLOT(toggleAudioPreviewB(int)));

    this->disconnect(configShortcut, SIGNAL(activated()), this, SLOT(configOpenSlot()));

    this->disconnect(config->comboBoxVideoFormatChannelA, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedFormatA(QString)));
    this->disconnect(config->comboBoxVideoFormatChannelB, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedFormatB(QString)));

    this->connect(config->comboBoxModeChannelA, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedModeA(QString)));
    this->connect(config->comboBoxModeChannelB, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedModeB(QString)));
}

void MainWindow::disableSignalsA()
{
    if(config->labelChannelA->text().contains("Playout"))
    {
        this->disconnect(ui->channelA, SIGNAL(itemLoaded()), ui->playListA, SLOT(itemLoaded()));
        this->disconnect(ui->channelA, SIGNAL(itemPlaying()), ui->playListA, SLOT(itemPlaying()));
        this->disconnect(ui->channelA, SIGNAL(loadNextItem()), ui->playListA, SLOT(loadNextItem()));
        this->disconnect(ui->playListA, SIGNAL(loadItem(PlayListItem, bool)), ui->channelA, SLOT(loadItem(PlayListItem, bool)));
        this->disconnect(ui->playListA, SIGNAL(recueItem()), ui->channelA, SLOT(recue()));
        this->disconnect(ui->playListA, SIGNAL(playListChain(bool)), ui->channelA, SLOT(playListChain(bool)));
        this->disconnect(ui->channelA, SIGNAL(changedInterface(QString,QString)), this, SLOT(changedInterface(QString,QString)));

        this->disconnect(config->comboBoxInterfaceChannelA, SIGNAL(currentIndexChanged(QString)), ui->channelA, SLOT(interfaceChanged(QString)));
        this->disconnect(config->comboBoxVideoFormatChannelA, SIGNAL(currentIndexChanged(QString)), ui->channelA, SLOT(changedFormat(QString)));
        this->disconnect(config->comboBoxCGChannelA, SIGNAL(currentIndexChanged(QString)), ui->channelA, SLOT(cgChanged(QString)));
        this->disconnect(config->checkBoxVideoPreviewerA, SIGNAL(stateChanged(int)), ui->channelA, SLOT(togglePreviewVideo(int)));
        this->disconnect(config->checkBoxAudioPreviewerA, SIGNAL(stateChanged(int)), ui->channelA, SLOT(togglePreviewAudio(int)));

        ui->channelA->disableShortcuts();
        ui->playListA->disableShortcuts();
    }
    else
    {
        this->disconnect(ui->recorderA, SIGNAL(changedInterface(QString,QString)), this, SLOT(changedInterface(QString,QString)));
        this->disconnect(ui->recorderA, SIGNAL(changedPath(QString)), this, SLOT(changedRecordPathA(QString)));

        this->disconnect(config->comboBoxInterfaceChannelA, SIGNAL(currentIndexChanged(QString)), ui->recorderA, SLOT(interfaceChanged(QString)));
        this->disconnect(config->comboBoxVideoFormatChannelA, SIGNAL(currentIndexChanged(QString)), ui->recorderA, SLOT(changedFormat(QString)));
        this->disconnect(config->comboBoxCGChannelA, SIGNAL(currentIndexChanged(QString)), ui->recorderA, SLOT(cgChanged(QString)));
        this->disconnect(config->checkBoxVideoPreviewerA, SIGNAL(stateChanged(int)), ui->recorderA, SLOT(togglePreviewVideo(int)));
        this->disconnect(config->checkBoxAudioPreviewerA, SIGNAL(stateChanged(int)), ui->recorderA, SLOT(togglePreviewAudio(int)));

        this->disconnect(this, SIGNAL(sendFFError(int,QString)), ui->recorderA, SLOT(checkFFError(int,QString)));

        ui->recorderA->disableShortcuts();
    }
}

void MainWindow::disableSignalsB()
{
    if(config->labelChannelB->text().contains("Playout"))
    {
        this->disconnect(ui->channelB, SIGNAL(itemLoaded()), ui->playListB, SLOT(itemLoaded()));
        this->disconnect(ui->channelB, SIGNAL(itemPlaying()), ui->playListB, SLOT(itemPlaying()));
        this->disconnect(ui->channelB, SIGNAL(loadNextItem()), ui->playListB, SLOT(loadNextItem()));
        this->disconnect(ui->playListB, SIGNAL(loadItem(PlayListItem, bool)), ui->channelB, SLOT(loadItem(PlayListItem, bool)));
        this->disconnect(ui->playListB, SIGNAL(recueItem()), ui->channelB, SLOT(recue()));
        this->disconnect(ui->playListB, SIGNAL(playListChain(bool)), ui->channelB, SLOT(playListChain(bool)));
        this->disconnect(ui->channelB, SIGNAL(changedInterface(QString,QString)), this, SLOT(changedInterface(QString,QString)));

        this->disconnect(config->comboBoxInterfaceChannelB, SIGNAL(currentIndexChanged(QString)), ui->channelB, SLOT(interfaceChanged(QString)));
        this->disconnect(config->comboBoxVideoFormatChannelB, SIGNAL(currentIndexChanged(QString)), ui->channelB, SLOT(changedFormat(QString)));
        this->disconnect(config->comboBoxCGChannelB, SIGNAL(currentIndexChanged(QString)), ui->channelB, SLOT(cgChanged(QString)));
        this->disconnect(config->checkBoxVideoPreviewerB, SIGNAL(stateChanged(int)), ui->channelB, SLOT(togglePreviewVideo(int)));
        this->disconnect(config->checkBoxAudioPreviewerB, SIGNAL(stateChanged(int)), ui->channelB, SLOT(togglePreviewAudio(int)));

        ui->channelB->disableShortcuts();
        ui->playListB->disableShortcuts();
    }
    else
    {
        this->disconnect(ui->recorderB, SIGNAL(changedInterface(QString,QString)), this, SLOT(changedInterface(QString,QString)));
        this->disconnect(ui->recorderB, SIGNAL(changedPath(QString)), this, SLOT(changedRecordPathB(QString)));

        this->disconnect(config->comboBoxInterfaceChannelB, SIGNAL(currentIndexChanged(QString)), ui->recorderB, SLOT(interfaceChanged(QString)));
        this->disconnect(config->comboBoxVideoFormatChannelB, SIGNAL(currentIndexChanged(QString)), ui->recorderB, SLOT(changedFormat(QString)));
        this->disconnect(config->comboBoxCGChannelB, SIGNAL(currentIndexChanged(QString)), ui->recorderB, SLOT(cgChanged(QString)));
        this->disconnect(config->checkBoxVideoPreviewerB, SIGNAL(stateChanged(int)), ui->recorderB, SLOT(togglePreviewVideo(int)));
        this->disconnect(config->checkBoxAudioPreviewerB, SIGNAL(stateChanged(int)), ui->recorderB, SLOT(togglePreviewAudio(int)));

        this->connect(this, SIGNAL(sendFFError(int,QString)), ui->recorderB, SLOT(checkFFError(int,QString)));

        ui->recorderB->disableShortcuts();
    }
}

void MainWindow::initDecklink()
{
    if(DeckLinkControl::init())
    {
        // Initialize the devices list
        deckLinkDevices.append(DeckLinkControl::getOutputList());
        deckLinkDevices.append(DeckLinkControl::getInputList());

        config->comboBoxInterfaceChannelA->blockSignals(true);
        config->comboBoxInterfaceChannelB->blockSignals(true);

        config->comboBoxInterfaceChannelA->addItem("");
        config->comboBoxInterfaceChannelB->addItem("");

        if(config->labelChannelA->text().contains("Ingest"))
            config->comboBoxInterfaceChannelA->addItems(DeckLinkControl::getInputList());
        else config->comboBoxInterfaceChannelA->addItems(DeckLinkControl::getOutputList());

        if(config->labelChannelB->text().contains("Ingest"))
            config->comboBoxInterfaceChannelB->addItems(DeckLinkControl::getInputList());
        else config->comboBoxInterfaceChannelB->addItems(DeckLinkControl::getOutputList());

        config->comboBoxInterfaceChannelA->blockSignals(false);
        config->comboBoxInterfaceChannelB->blockSignals(false);

        // Set the default interfaces
        config->comboBoxInterfaceChannelA->setCurrentIndex(1);
        config->comboBoxInterfaceChannelB->setCurrentIndex(1);
    }
}

void MainWindow::initFormats()
{
    config->comboBoxVideoFormatChannelA->blockSignals(true);
    config->comboBoxVideoFormatChannelB->blockSignals(true);

    // Initialize the formats list
    config->comboBoxVideoFormatChannelA->addItem("PAL");
    config->comboBoxVideoFormatChannelA->addItem("PAL 16:9");
    config->comboBoxVideoFormatChannelA->addItem("720p50");
    config->comboBoxVideoFormatChannelA->addItem("720p5994");
    config->comboBoxVideoFormatChannelA->addItem("1080p25");
    config->comboBoxVideoFormatChannelA->addItem("1080i50");
    config->comboBoxVideoFormatChannelA->addItem("1080i5994");
    config->comboBoxVideoFormatChannelB->addItem("PAL");
    config->comboBoxVideoFormatChannelB->addItem("PAL 16:9");
    config->comboBoxVideoFormatChannelB->addItem("720p50");
    config->comboBoxVideoFormatChannelB->addItem("720p5994");
    config->comboBoxVideoFormatChannelB->addItem("1080p25");
    config->comboBoxVideoFormatChannelB->addItem("1080i50");
    config->comboBoxVideoFormatChannelB->addItem("1080i5994");

    config->comboBoxVideoFormatChannelA->blockSignals(false);
    config->comboBoxVideoFormatChannelB->blockSignals(false);

    // Disable previews if necessary
    bool disableVideoPreviewerA = settings->value("DisableVideoPreviewerA").toBool();
    if(disableVideoPreviewerA)
        config->checkBoxVideoPreviewerA->setChecked(false);
    bool disableVideoPreviewerB = settings->value("DisableVideoPreviewerB").toBool();
    if(disableVideoPreviewerB)
        config->checkBoxVideoPreviewerB->setChecked(false);
    bool disableAudioPreviewerA = settings->value("DisableAudioPreviewerA").toBool();
    if(disableAudioPreviewerA)
        config->checkBoxAudioPreviewerA->setChecked(false);
    bool disableAudioPreviewerB = settings->value("DisableAudioPreviewerB").toBool();
    if(disableAudioPreviewerB)
        config->checkBoxAudioPreviewerB->setChecked(false);

    // Set the default formats
    QString format = settings->value("ChannelFormatA").toString();
    int index = config->comboBoxVideoFormatChannelA->findText(format);
    if(index != -1)
       config->comboBoxVideoFormatChannelA->setCurrentIndex(index);
    else config->comboBoxVideoFormatChannelA->setCurrentIndex(0);

    format = settings->value("ChannelFormatB").toString();
    index = config->comboBoxVideoFormatChannelB->findText(format);
    if(index != -1)
       config->comboBoxVideoFormatChannelB->setCurrentIndex(index);
    else config->comboBoxVideoFormatChannelB->setCurrentIndex(0);
}

void MainWindow::initCG()
{
    config->comboBoxCGChannelA->blockSignals(true);
    config->comboBoxCGChannelB->blockSignals(true);

    // Initialize the CG list
    config->comboBoxCGChannelA->addItem("");
    config->comboBoxCGChannelB->addItem("");

    QStringList cgList = QDir("cg").entryList(QDir::Files);
    foreach(QString cg, cgList)
    {
        config->comboBoxCGChannelA->addItem(cg);
        config->comboBoxCGChannelB->addItem(cg);
    }

    config->comboBoxCGChannelA->blockSignals(false);
    config->comboBoxCGChannelB->blockSignals(false);
}

void MainWindow::initModes()
{
    config->comboBoxModeChannelA->blockSignals(true);
    config->comboBoxModeChannelB->blockSignals(true);

    // Initialize the Modes list
    config->comboBoxModeChannelA->addItem("Playout");
    config->comboBoxModeChannelA->addItem("Ingest");
    config->comboBoxModeChannelB->addItem("Playout");
    config->comboBoxModeChannelB->addItem("Ingest");

    config->comboBoxModeChannelA->blockSignals(false);
    config->comboBoxModeChannelB->blockSignals(false);

    QString mode = settings->value("ModeA").toString();
    int index = config->comboBoxModeChannelA->findText(mode);
    if(index != -1)
       config->comboBoxModeChannelA->setCurrentIndex(index);
    else config->comboBoxModeChannelA->setCurrentIndex(0);

    mode = settings->value("ModeB").toString();
    index = config->comboBoxModeChannelB->findText(mode);
    if(index != -1)
       config->comboBoxModeChannelB->setCurrentIndex(index);
    else config->comboBoxModeChannelB->setCurrentIndex(0);
}

void MainWindow::toggleVideoPreviewA(const int state)
{
    if(state == 2)
        settings->setValue("DisableVideoPreviewerA", false);
    else if(state == 0)
        settings->setValue("DisableVideoPreviewerA", true);
}

void MainWindow::toggleVideoPreviewB(const int state)
{
    if(state == 2)
        settings->setValue("DisableVideoPreviewerB", false);
    else if(state == 0)
        settings->setValue("DisableVideoPreviewerB", true);
}

void MainWindow::toggleAudioPreviewA(const int state)
{
    if(state == 2)
        settings->setValue("DisableAudioPreviewerA", false);
    else if(state == 0)
        settings->setValue("DisableAudioPreviewerA", true);
}

void MainWindow::toggleAudioPreviewB(const int state)
{
    if(state == 2)
        settings->setValue("DisableAudioPreviewerB", false);
    else if(state == 0)
        settings->setValue("DisableAudioPreviewerB", true);
}

void MainWindow::toggleFillKey(const int state)
{
    if(state == 2)
    {
        settings->setValue("EnableFillKey", true);
        enableFillKey();
    }
    else if(state == 0)
    {
        settings->setValue("EnableFillKey", false);
        disableFillKey();
    }
}

void MainWindow::toggleAutoloadPlaylists(const int state)
{
    if(state == 2)
        settings->setValue("AutoloadPlaylists", true);
    else if(state == 0)
        settings->setValue("AutoloadPlaylists", false);
}

void MainWindow::changedInterface(const QString& interf, const QString& channel)
{
    QComboBox* comboBoxInterface;
    QString comboboxType;
    if(channel.contains("A", Qt::CaseSensitive))
    {
        comboBoxInterface = config->comboBoxInterfaceChannelB;
        comboboxType = config->labelChannelB->text();
    }
    else if(channel.contains("B", Qt::CaseSensitive))
    {
        comboBoxInterface = config->comboBoxInterfaceChannelA;
        comboboxType = config->labelChannelA->text();
    }
    else return;

    // There can only be one active input/output on each device, so we filter the list here
    foreach (const QString &str, deckLinkDevices)
    {
        int index = comboBoxInterface->findText(str);
        if (interf != "" && str.contains("(" + interf + ")"))
            comboBoxInterface->removeItem(index);
        else if(index == -1)
        {
            if(comboboxType.contains("Playout") && str.contains("Output"))
                comboBoxInterface->addItem(str);
            else if(comboboxType.contains("Ingest") && str.contains("Input"))
                comboBoxInterface->addItem(str);
            else if(comboboxType.contains("Bridge") && str.contains("Output"))
                comboBoxInterface->addItem(str);
        }
    }
}

void MainWindow::changedFormatA(const QString& format)
{
    settings->setValue("ChannelFormatA", format);

    config->comboBoxVideoFormatChannelB->blockSignals(true);
    if(config->labelChannelB->text().contains("Bridge"))
        config->comboBoxVideoFormatChannelB->setCurrentIndex(config->comboBoxVideoFormatChannelA->findText(format));
    config->comboBoxVideoFormatChannelB->blockSignals(false);
}

void MainWindow::changedFormatB(const QString& format)
{
    settings->setValue("ChannelFormatB", format);

    config->comboBoxVideoFormatChannelA->blockSignals(true);
    if(config->labelChannelA->text().contains("Bridge"))
        config->comboBoxVideoFormatChannelA->setCurrentIndex(config->comboBoxVideoFormatChannelB->findText(format));
    config->comboBoxVideoFormatChannelA->blockSignals(false);
}

void MainWindow::changedRecordPathA(const QString& path)
{
    settings->setValue("RecordPathA", path);
}

void MainWindow::changedRecordPathB(const QString& path)
{
    settings->setValue("RecordPathB", path);
}

void MainWindow::changedModeA(const QString& mode)
{
    modeMutexA.lock();

    int cg = config->comboBoxCGChannelA->currentIndex();
    config->comboBoxCGChannelA->setCurrentIndex(0);

    ui->recorderB->setIOBridge(NULL);
    config->comboBoxInterfaceChannelA->setCurrentIndex(0);

    disableSignalsA();

    int format = config->comboBoxVideoFormatChannelA->currentIndex();
    config->comboBoxVideoFormatChannelA->setEnabled(true);
    if(mode == "Bridge")
    {
        format = config->comboBoxVideoFormatChannelB->currentIndex();
        config->comboBoxVideoFormatChannelA->setEnabled(false);
    }
    config->comboBoxVideoFormatChannelA->setCurrentIndex(0);

    config->labelChannelA->setText(mode + " A");
    settings->setValue("ModeA", mode);

    if(mode == "Playout")
    {
        ui->playListA->setVisible(true);
        ui->channelA->setVisible(true);
        ui->recorderA->setVisible(false);
    }
    else
    {
        ui->playListA->setVisible(false);
        ui->channelA->setVisible(false);
        ui->recorderA->setVisible(true);
    }

    config->comboBoxInterfaceChannelA->clear();
    config->comboBoxInterfaceChannelA->addItem("");
    if(mode == "Ingest")
        config->comboBoxInterfaceChannelA->addItems(DeckLinkControl::getInputList());
    else config->comboBoxInterfaceChannelA->addItems(DeckLinkControl::getOutputList());

    QString inter = config->comboBoxInterfaceChannelB->currentText();
    inter.remove(QRegExp(".*\\("));
    inter.remove(")");
    changedInterface(inter, "B");

    initSignalsA();

    config->comboBoxInterfaceChannelA->setCurrentIndex(1);
    config->comboBoxVideoFormatChannelA->setCurrentIndex(format);
    config->comboBoxCGChannelA->setCurrentIndex(cg);

    int videoPreview = config->checkBoxVideoPreviewerA->isChecked() ? 2 : 0;
    int audioPreview = config->checkBoxVideoPreviewerA->isChecked() ? 2 : 0;
    if(mode == "Playout")
    {
        ui->channelA->togglePreviewVideo(videoPreview);
        ui->channelA->togglePreviewAudio(audioPreview);
    }
    else
    {
        ui->recorderA->togglePreviewVideo(videoPreview);
        ui->recorderA->togglePreviewAudio(audioPreview);
    }

    if(mode == "Bridge")
        ui->recorderB->setIOBridge(ui->recorderA->getIOBridge());

    if(mode == "Ingest")
        config->comboBoxModeChannelB->addItem("Bridge");
    else
    {
        int index = config->comboBoxModeChannelB->findText("Bridge");
        if(index != -1)
            config->comboBoxModeChannelB->removeItem(index);
    }

    disableFillKey();
    enableFillKey();

    modeMutexA.unlock();
}

void MainWindow::changedModeB(const QString& mode)
{
    modeMutexB.lock();

    ui->recorderA->setIOBridge(NULL);
    config->comboBoxInterfaceChannelB->setCurrentIndex(0);

    disableSignalsB();

    int format = config->comboBoxVideoFormatChannelB->currentIndex();
    config->comboBoxVideoFormatChannelB->setEnabled(true);
    if(mode == "Bridge")
    {
        format = config->comboBoxVideoFormatChannelA->currentIndex();
        config->comboBoxVideoFormatChannelB->setEnabled(false);
    }
    config->comboBoxVideoFormatChannelB->setCurrentIndex(0);
    int cg = config->comboBoxCGChannelB->currentIndex();
    config->comboBoxCGChannelB->setCurrentIndex(0);

    config->labelChannelB->setText(mode + " B");
    settings->setValue("ModeB", mode);

    if(mode == "Playout")
    {
        ui->playListB->setVisible(true);
        ui->channelB->setVisible(true);
        ui->recorderB->setVisible(false);
    }
    else
    {
        ui->playListB->setVisible(false);
        ui->channelB->setVisible(false);
        ui->recorderB->setVisible(true);
    }

    config->comboBoxInterfaceChannelB->clear();
    config->comboBoxInterfaceChannelB->addItem("");
    if(mode == "Ingest")
        config->comboBoxInterfaceChannelB->addItems(DeckLinkControl::getInputList());
    else config->comboBoxInterfaceChannelB->addItems(DeckLinkControl::getOutputList());

    QString inter = config->comboBoxInterfaceChannelA->currentText();
    inter.remove(QRegExp(".*\\("));
    inter.remove(")");
    changedInterface(inter, "A");

    initSignalsB();

    config->comboBoxInterfaceChannelB->setCurrentIndex(1);
    config->comboBoxVideoFormatChannelB->setCurrentIndex(format);
    config->comboBoxCGChannelB->setCurrentIndex(cg);

    int videoPreview = config->checkBoxVideoPreviewerB->isChecked() ? 2 : 0;
    int audioPreview = config->checkBoxVideoPreviewerB->isChecked() ? 2 : 0;
    if(mode == "Playout")
    {
        ui->channelB->togglePreviewVideo(videoPreview);
        ui->channelB->togglePreviewAudio(audioPreview);
    }
    else
    {
        ui->recorderB->togglePreviewVideo(videoPreview);
        ui->recorderB->togglePreviewAudio(audioPreview);
    }

    if(mode == "Bridge")
        ui->recorderA->setIOBridge(ui->recorderB->getIOBridge());

    if(mode == "Ingest")
        config->comboBoxModeChannelA->addItem("Bridge");
    else
    {
        int index = config->comboBoxModeChannelA->findText("Bridge");
        if(index != -1)
            config->comboBoxModeChannelA->removeItem(index);
    }

    disableFillKey();
    enableFillKey();

    modeMutexB.unlock();
}

void MainWindow::configOpenSlot()
{
    if(!ui->channelA->isStopped() || !ui->recorderA->isStopped())
        config->frameChannelA->setEnabled(false);
    else config->frameChannelA->setEnabled(true);

    if(!ui->channelB->isStopped() || !ui->recorderB->isStopped())
        config->frameChannelB->setEnabled(false);
    else config->frameChannelB->setEnabled(true);

    configDialog.exec();
}

void MainWindow::enableFillKey()
{
    if(config->checkBoxFillKey->isChecked() && config->labelChannelA->text().contains("Playout") && config->labelChannelB->text().contains("Playout"))
    {
        this->connect(fillKeyTakeShortcut, SIGNAL(activated()), ui->channelA, SLOT(takeSlot()));
        this->connect(fillKeyTakeShortcut, SIGNAL(activated()), ui->channelB, SLOT(takeSlot()));
        this->connect(fillKeyDropShortcut, SIGNAL(activated()), ui->channelA, SLOT(dropSlot()));
        this->connect(fillKeyDropShortcut, SIGNAL(activated()), ui->channelB, SLOT(dropSlot()));

        this->connect(ui->pushButtonFillKeyTake, SIGNAL(clicked()), fillKeyTakeShortcut, SIGNAL(activated()));
        this->connect(ui->pushButtonFillKeyDrop, SIGNAL(clicked()), fillKeyDropShortcut, SIGNAL(activated()));

        ui->pushButtonFillKeyTake->setVisible(true);
        ui->pushButtonFillKeyDrop->setVisible(true);
    }
}

void MainWindow::disableFillKey()
{
    this->disconnect(fillKeyTakeShortcut, SIGNAL(activated()), ui->channelA, SLOT(takeSlot()));
    this->disconnect(fillKeyTakeShortcut, SIGNAL(activated()), ui->channelB, SLOT(takeSlot()));
    this->disconnect(fillKeyDropShortcut, SIGNAL(activated()), ui->channelA, SLOT(dropSlot()));
    this->disconnect(fillKeyDropShortcut, SIGNAL(activated()), ui->channelB, SLOT(dropSlot()));

    this->disconnect(ui->pushButtonFillKeyTake, SIGNAL(clicked()), fillKeyTakeShortcut, SIGNAL(activated()));
    this->disconnect(ui->pushButtonFillKeyDrop, SIGNAL(clicked()), fillKeyDropShortcut, SIGNAL(activated()));

    ui->pushButtonFillKeyTake->setVisible(false);
    ui->pushButtonFillKeyDrop->setVisible(false);
}

void MainWindow::checkAutoloadPlaylists()
{
    bool autoloadPlaylists = settings->value("AutoloadPlaylists").toBool();
    if(!autoloadPlaylists)
        config->checkBoxAutoloadPlaylists->setChecked(false);
    else
    {
        ui->playListA->autoloadPlaylist();
        ui->playListB->autoloadPlaylist();
    }
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    if(ui->channelA->isStopped() && ui->channelB->isStopped() &&
       ui->recorderA->isStopped() && ui->recorderB->isStopped())
        event->accept();
    else event->ignore();
}

void MainWindow::checkFFError(int addr, QString message)
{
    emit sendFFError(addr, message);
}
