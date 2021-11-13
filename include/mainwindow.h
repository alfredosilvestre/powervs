#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "playlistgui.h"
#include "channelgui.h"
#include "recordergui.h"

#include <QDialog>
#include <QMainWindow>
#include <QSettings>
#include <QShortcut>

namespace Ui { class MainWindow; class Dialog; }

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    public:
        explicit MainWindow(QWidget* parent = 0);
        ~MainWindow();
        void closeEvent(QCloseEvent* event);
        void checkFFError(int addr, QString message);

    private:
        Ui::MainWindow* ui;

        QSettings* settings;
        QStringList deckLinkDevices;
        QDialog configDialog;
        Ui::Dialog* config;
        QShortcut* configShortcut;
        QShortcut* fillKeyTakeShortcut;
        QShortcut* fillKeyDropShortcut;
        QMutex modeMutexA;
        QMutex modeMutexB;

    private:
        void initInterface();
        void initPlaylists(int x, int height);
        void initChannelRecorder(int x, int y, int height, ChannelGUI* channel, RecorderGUI* recorder);
        void initSignals();
        void initSignalsA();
        void initSignalsB();
        void disableSignals();
        void disableSignalsA();
        void disableSignalsB();
        void initDecklink();
        void initFormats();
        void initCG();
        void initModes();
        void enableFillKey();
        void disableFillKey();
        void checkAutoloadPlaylists();

    public slots:
        void toggleVideoPreviewA(const int state);
        void toggleVideoPreviewB(const int state);
        void toggleAudioPreviewA(const int state);
        void toggleAudioPreviewB(const int state);
        void toggleFillKey(const int state);
        void toggleAutoloadPlaylists(const int state);
        void changedInterface(const QString& interf, const QString& channel);
        void changedFormatA(const QString& format);
        void changedFormatB(const QString& format);
        void changedModeA(const QString& mode);
        void changedModeB(const QString& mode);
        void changedRecordPathA(const QString& path);
        void changedRecordPathB(const QString& path);
        void configOpenSlot();

    signals:
        void sendFFError(int addr, QString message);
};

#endif // MAINWINDOW_H
