#include "recordergui.h"
#include "ui_recordergui.h"

#include <log4cxx/logger.h>

#include <QMessageBox>
#include <QFileDialog>

using namespace log4cxx;

RecorderGUI::RecorderGUI(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::RecorderGUI)
{
    ui->setupUi(this);
    ioBridge = new IOBridge(this);

    baseStylesheet = this->styleSheet();
    this->setStyleSheet(baseStylesheet + "QFrame { border-color: gray; }");

    this->initSignals();

    hourlyTimecode = false;
    this->ui->labelSignalError->setVisible(false);
}

RecorderGUI::~RecorderGUI()
{
    this->disableSignals();

    delete ui;
}

void RecorderGUI::initSignals()
{
    this->connect(ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(startSlot()));
    this->connect(ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stopSlot()));

    this->connect(ui->preview, SIGNAL(setValues(QByteArray)), ui->audioMeter, SLOT(setValues(QByteArray)));
    this->connect(&timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    this->connect(&hourlyTimer, SIGNAL(timeout()), this, SLOT(hourlyTimeUpdate()));

    this->connect(ui->pushButtonTimecode, SIGNAL(clicked()), this, SLOT(timecodeSlot()));
    this->connect(ui->pushButtonChoosePath, SIGNAL(clicked()), this, SLOT(chooseRecordPath()));
}

void RecorderGUI::disableSignals()
{
    this->disconnect(ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(startSlot()));
    this->disconnect(ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stopSlot()));

    this->disconnect(ui->preview, SIGNAL(setValues(QByteArray)), ui->audioMeter, SLOT(setValues(QByteArray)));
    this->disconnect(&timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    this->disconnect(&hourlyTimer, SIGNAL(timeout()), this, SLOT(hourlyTimeUpdate()));

    this->disconnect(ui->pushButtonTimecode, SIGNAL(clicked()), this, SLOT(timecodeSlot()));
    this->disconnect(ui->pushButtonChoosePath, SIGNAL(clicked()), this, SLOT(chooseRecordPath()));

    this->disconnect(pushButtonStartShortcut, SIGNAL(activated()), ui->pushButtonStart, SLOT(click()));
    this->disconnect(pushButtonStopShortcut, SIGNAL(activated()), ui->pushButtonStop, SLOT(click()));
}

void RecorderGUI::initShortcuts(const QString& startShortcut, const QString& stopShortcut, const QString& channelLabel)
{
    pushButtonStartShortcut = new QShortcut(QKeySequence(startShortcut), this);
    this->connect(pushButtonStartShortcut, SIGNAL(activated()), ui->pushButtonStart, SLOT(click()));
    pushButtonStopShortcut = new QShortcut(QKeySequence(stopShortcut), this);
    this->connect(pushButtonStopShortcut, SIGNAL(activated()), ui->pushButtonStop, SLOT(click()));

    QStringList str = channelLabel.split(" ");
    this->ui->labelRecorder->setText(this->ui->labelRecorder->text() + "\n\n" + str[1]);
    this->debugName = channelLabel;
}

void RecorderGUI::disableShortcuts()
{
    this->disconnect(pushButtonStartShortcut, SIGNAL(activated()), ui->pushButtonStart, SLOT(click()));
    this->disconnect(pushButtonStopShortcut, SIGNAL(activated()), ui->pushButtonStop, SLOT(click()));

    delete pushButtonStartShortcut;
    delete pushButtonStopShortcut;
}

const bool RecorderGUI::isStopped() const
{
    if(ioBridge == NULL)
        return true;

    return !ioBridge->isRecording();
}

// PushButton Slots

void RecorderGUI::startSlot()
{
    QString name = ui->lineEditClipName->text();
    if(name != "" && name != "Insert Clip Name")
    {
#ifdef DECKLINK
        if(ioBridge != NULL)
        {
            if(ioBridge->getDecklinkInput() != NULL || ioBridge->getDecklinkOutput() != NULL)
            {
                if(QFileInfo(ui->labelRecordPath->text() + name + ".mxf").exists())
                {
                    int result = QMessageBox::question((QWidget*)this->parent(), "File exists", "A file with the same name already exists, overwrite?", QMessageBox::No, QMessageBox::Yes);
                    if(result == QMessageBox::No)
                        return;

                    LOG4CXX_WARN(Logger::getLogger(debugName.toStdString()), "Overwriting file: " + ui->labelRecordPath->text().toStdString() + name.toStdString() + ".mxf");
                }

                if(ioBridge->startRecording(ui->labelRecordPath->text(), name, ".mxf", ui->pushButtonTimecode->text().toStdString().c_str()))
                {
                    ui->lineEditClipName->setText("Recording: " + ui->labelRecordPath->text() + name + ".mxf");
                    LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Started recording file: \"" + name.toStdString() + "\" with start TC: " + ui->pushButtonTimecode->text().toStdString());

                    timer.start(10);

                    ui->pushButtonStart->setEnabled(false);
                    ui->pushButtonStop->setEnabled(true);
                    ui->pushButtonChoosePath->setEnabled(false);
                    ui->pushButtonTimecode->setEnabled(false);
                    ui->lineEditClipName->setEnabled(false);
                    this->setStyleSheet(baseStylesheet + "QFrame { border-color: darkRed; }");

                    return;
                }

                LOG4CXX_ERROR(Logger::getLogger(debugName.toStdString()), "Error initializing recording");
                QMessageBox::critical((QWidget*)this->parent(), "Erro", "Error initializing recording");
            }
        }
#endif
        LOG4CXX_ERROR(Logger::getLogger(debugName.toStdString()), "There are no active inputs");
        QMessageBox::critical((QWidget*)this->parent(), "Erro", "There are no active inputs");
    }
    else QMessageBox::critical((QWidget*)this->parent(), "Erro", "Invalid file name");
}

void RecorderGUI::stopSlot()
{
    if(ioBridge != NULL)
    {
        int result = QMessageBox::question((QWidget*)this->parent(), "Stop Recording", "Are you sure you want to stop the recording?", QMessageBox::No, QMessageBox::Yes);
        if(result == QMessageBox::Yes)
        {
            timer.stop();
            QTime finalTime = QTime(0, 0, 0, 0).addMSecs(ioBridge->stopRecording(false));

            QStringList splitDuration = finalTime.toString("hh:mm:ss.zzz").split(".");
            int framesDuration = splitDuration.at(1).toInt() * 25 / 1000;
            QString duration = splitDuration[0] + ":" + QString::number(framesDuration).rightJustified(2, '0');

            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Stop recording at TC: " + ui->pushButtonTimecode->text().toStdString() + " with duration: " + duration.toStdString());

            ui->pushButtonStart->setEnabled(true);
            ui->pushButtonStop->setEnabled(false);
            ui->pushButtonChoosePath->setEnabled(true);
            ui->lineEditClipName->setEnabled(true);
            ui->lineEditClipName->setText("Insert Clip Name");
            ui->labelDuration->setText("00:00:00:00");
            this->setStyleSheet(baseStylesheet + "QFrame { border-color: gray; }");
            this->ui->labelSignalError->setVisible(false);

            if(ioBridge->getDecklinkInput() != NULL)
                ui->pushButtonTimecode->setEnabled(true);
        }
    }
}

void RecorderGUI::timecodeSlot()
{
    hourlyTimecode = !hourlyTimecode;

    if(hourlyTimecode)
    {
#ifdef DECKLINK
        if(ioBridge != NULL)
        {
            const DeckLinkInput* inputDL = ioBridge->getDecklinkInput();
            if(inputDL != NULL)
                this->disconnect(inputDL, SIGNAL(timecodeReceived(QString)), this, SLOT(updateTimecode(QString)));
        }
#endif
        hourlyTimer.start(10);
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Changed to hourly timecode");
    }
    else
    {
        hourlyTimer.stop();
        ui->pushButtonTimecode->setText("00:00:00:00");

#ifdef DECKLINK
        if(ioBridge != NULL)
        {
            const DeckLinkInput* inputDL = ioBridge->getDecklinkInput();
            if(inputDL != NULL)
                this->connect(inputDL, SIGNAL(timecodeReceived(QString)), this, SLOT(updateTimecode(QString)));
        }
#endif
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Changed to embedded timecode");
    }
}

void RecorderGUI::signalErrorSlot()
{
    if(!ui->pushButtonStart->isEnabled())
    {
        if(!ui->labelSignalError->isVisible())
            LOG4CXX_ERROR(Logger::getLogger(debugName.toStdString()), "Error in video signal at TC: " + ui->pushButtonTimecode->text().toStdString());
        this->ui->labelSignalError->setVisible(true);
        if(!hourlyTimer.isActive())
            ui->pushButtonTimecode->setText("00:00:00:00");

        // When getting a signal error reboot the interface and if this interface is bridged, reboot the bridge
        ioBridge->rebootInterface();
    }
}

void RecorderGUI::chooseRecordPath()
{
    QFileDialog fileDialog;
    fileDialog.setWindowTitle("Choose path...");
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    fileDialog.setDirectory(ui->labelRecordPath->text());

    QStringList fileNames;
    if(fileDialog.exec())
        fileNames = fileDialog.selectedFiles();

    QString path = "";
    if(!fileNames.empty())
        path = fileNames.first();

    if(path != "")
        changeRecordPath(path.replace("/", "\\"));
}

void RecorderGUI::changeRecordPath(const QString& path)
{
    if(path != "")
    {
        // Correct trailing backslash
        path.right(1) == "\\" ? ui->labelRecordPath->setText(path) : ui->labelRecordPath->setText(path + "\\");
        emit changedPath(path);
    }
}

IOBridge* RecorderGUI::getIOBridge() const
{
    return ioBridge;
}

const void RecorderGUI::setIOBridge(IOBridge* ioBridge)
{
    this->ioBridge->setIOBridge(ioBridge);
}

void RecorderGUI::checkFFError(int addr, QString message)
{
    if(ioBridge != NULL)
    {
        QString newFilename = ioBridge->checkFFError(addr, ui->pushButtonTimecode->text().toStdString().c_str());
        if(newFilename != "")
        {
            LOG4CXX_ERROR(Logger::getLogger(debugName.toStdString()), "FFError while recording (" + message.toStdString() + "), restarting recording");
            ui->lineEditClipName->setText("Recording: " + newFilename);
            ui->labelDuration->setText("00:00:00:00");
        }
    }
}

// Helper Slots

void RecorderGUI::loadBars()
{
    if(ioBridge != NULL && !ioBridge->isRecording())
        ui->preview->loadBars();
}

void RecorderGUI::timeUpdate()
{
    if(ioBridge != NULL)
    {
        QTime t = QTime(0, 0, 0, 0).addMSecs(ioBridge->getCurrentRecordTime());
        QStringList splitDuration = t.toString("hh:mm:ss.zzz").split(".");
        int framesDuration = splitDuration.at(1).toInt() * 25 / 1000;
        ui->labelDuration->setText(splitDuration[0] + ":" + QString::number(framesDuration).rightJustified(2, '0'));
    }
}

void RecorderGUI::hourlyTimeUpdate()
{
    QStringList splitDuration = QTime::currentTime().toString("hh:mm:ss.zzz").split(".");
    int framesDuration = splitDuration.at(1).toInt() * 25 / 1000;
    QString timecode = splitDuration[0] + ":" + QString::number(framesDuration).rightJustified(2, '0');

    ui->pushButtonTimecode->setText(timecode);
    emit timecodeReceived(timecode);
}

void RecorderGUI::updateTimecode(const QString& timecode)
{
    ui->pushButtonTimecode->setText(timecode);
    emit timecodeReceived(timecode);
}

// Config Slots

void RecorderGUI::interfaceChanged(const QString& inter)
{
    if(ioBridge != NULL && !ioBridge->isRecording())
    {
#ifdef DECKLINK
        const DeckLinkInput* inputDL = ioBridge->getDecklinkInput();
        if(inputDL != NULL)
        {
            if(!hourlyTimecode)
                this->disconnect(inputDL, SIGNAL(timecodeReceived(QString)), this, SLOT(updateTimecode(QString)));
            this->disconnect(inputDL, SIGNAL(signalError()), this, SLOT(signalErrorSlot()));
        }

        if(debugName.contains("Ingest"))
        {
            QString interf = ioBridge->setDeckLinkInput(inter);

            inputDL = ioBridge->getDecklinkInput();
            if(inputDL != NULL)
            {
                if(!hourlyTimecode)
                    this->connect(inputDL, SIGNAL(timecodeReceived(QString)), this, SLOT(updateTimecode(QString)));
                this->connect(inputDL, SIGNAL(signalError()), this, SLOT(signalErrorSlot()));
            }

            emit changedInterface(interf, ui->labelRecorder->text());
            ui->pushButtonTimecode->setEnabled(true);

            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Decklink input changed to: " + interf.toStdString());
        }
        else
        {
            QString interf = ioBridge->setDeckLinkOutput(inter);
            emit changedInterface(interf, ui->labelRecorder->text());
            ui->pushButtonTimecode->setEnabled(false);
            if(hourlyTimecode)
                timecodeSlot();

            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Decklink output changed to: " + interf.toStdString());
        }
#else
        Q_UNUSED(inter);
#endif
    }
}

void RecorderGUI::changedFormat(const QString& format)
{
    if(ioBridge != NULL && !ioBridge->isRecording())
    {
        ioBridge->changeFormat(format);
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Format changed to: " + format.toStdString());
    }
}

void RecorderGUI::cgChanged(const QString& cg)
{
    ioBridge->activateFilter(cg);
    LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Activated filter: " + cg.toStdString());
}

void RecorderGUI::togglePreviewVideo(const int state)
{
#ifdef GUI
    if(ioBridge != NULL && !ioBridge->isRecording())
    {
        if(state == 2)
        {
            ioBridge->togglePreviewVideo(ui->preview);
        }
        else if(state == 0)
        {
            ioBridge->togglePreviewVideo(NULL);
            QTimer::singleShot(100, this, SLOT(loadBars()));
        }
    }
#else
    Q_UNUSED(state);
#endif
}

void RecorderGUI::togglePreviewAudio(const int state)
{
    Q_UNUSED(state);
}
