#ifndef RECORDERGUI_H
#define RECORDERGUI_H

#include "iobridge.h"

#include <QShortcut>
#include <QTableWidgetItem>
#include <QTime>
#include <QTimer>

namespace Ui { class RecorderGUI; }

class RecorderGUI : public QFrame
{
    Q_OBJECT
    
    public:
        explicit RecorderGUI(QWidget* parent = 0);
        ~RecorderGUI();
        Ui::RecorderGUI* ui;

    private:
        QString baseStylesheet;

        IOBridge* ioBridge;
        QTimer timer;
        QTimer hourlyTimer;

        bool hourlyTimecode;

        QShortcut* pushButtonStartShortcut;
        QShortcut* pushButtonStopShortcut;

        QString debugName;

    private:
        void initSignals();
        void disableSignals();

    public:
        void initShortcuts(const QString& startShortcut, const QString& stopShortcut, const QString& channelLabel);
        void disableShortcuts();
        const bool isStopped() const;
        void changeRecordPath(const QString& path);
        IOBridge* getIOBridge() const;
        const void setIOBridge(IOBridge* ioBridge);

    public slots:
        // PushButton Slots
        void startSlot();
        void stopSlot();
        void timecodeSlot();
        void signalErrorSlot();
        void chooseRecordPath();

        // Helper Slots
        void loadBars();
        void timeUpdate();
        void hourlyTimeUpdate();
        void updateTimecode(const QString& timecode);

        // Config Slots
        void interfaceChanged(const QString& inter);
        void changedFormat(const QString& format);
        void cgChanged(const QString& cg);
        void togglePreviewVideo(const int state);
        void togglePreviewAudio(const int state);

        void checkFFError(int addr, QString message);

    signals:
        void changedInterface(const QString& interf, const QString& ch);
        void changedPath(const QString& path);
        void timecodeReceived(QString timecode);
};

#endif // RECORDERGUI_H
