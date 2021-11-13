#ifndef CHANNELGUI_H
#define CHANNELGUI_H

#include "player.h"
#include "playlist.h"
#include "decklinkcontrol.h"

#include <QFrame>
#include <QShortcut>
#include <QTime>
#include <QTimer>

namespace Ui { class ChannelGUI; }

class ChannelGUI : public QFrame
{
    Q_OBJECT
    
    public:
        explicit ChannelGUI(QWidget* parent = 0);
        ~ChannelGUI();
        Ui::ChannelGUI* ui;

    private:
        QString baseStylesheet;

        Player* player;
        QShortcut* pushButtonTakeShortcut;
        QShortcut* pushButtonDropShortcut;
        QShortcut* pushButtonRecueShortcut;
        QShortcut* pushButtonLoopShortcut;

        QString debugName;

        bool chainPlayList;
        QTime clipDuration;
        QTime maximumDuration;
        QTimer channelTimer;

        int activeFilter;

    private:
        void initSignals();
        void disableSignals();
        void initPreview();
        void take();
        void drop(const bool immediate);

        // CG functions
        void clearFilters();
        void cgDeactivate();

    public:
        void initShortcuts(const QString& takeShortcut, const QString& dropShortcut, const QString& recueShortcut, const QString& loopShortcut, const QString& channelLabel);
        void disableShortcuts();
        const bool isStopped() const;

    public slots:
        // PushButton Slots
        void takeSlot();
        void dropSlot();
        void dropReactivate();
        void recueSlot();
        void recueReactivate();
        void loopSlot();

        void playPauseSlot();
        void nextFrameSlot();
        void previousFrameSlot();
        void forwardSlot();
        void reverseSlot();
        void seekSlot(const int value);

        // PlayList Slots
        void loadItem(const PlayListItem& item, const bool canTake);
        void playListChain(const bool chain);
        void recue();

        // Helper Slots
        void loadBars();
        void channelStep();
        void genlockErrorSlot();

        // Config Slots
        void interfaceChanged(const QString& inter);
        void changedFormat(const QString& format);
        void cgChanged(const QString& cg);
        void togglePreviewVideo(const int state);
        void togglePreviewAudio(const int state);

    signals:
        void itemLoaded();
        void loadNextItem();
        void itemPlaying();
        void changedInterface(const QString& interf, const QString& ch);
};

#endif // CHANNELGUI_H
