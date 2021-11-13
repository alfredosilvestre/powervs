#ifndef PLAYLISTGUI_H
#define PLAYLISTGUI_H

#include "playlist.h"

#include <QAction>
#include <QShortcut>
#include <QTableWidgetItem>

namespace Ui { class PlayListGUI; }

class PlayListGUI : public QFrame
{
    Q_OBJECT
    
    public:
        explicit PlayListGUI(QWidget* parent = 0);
        ~PlayListGUI();
        Ui::PlayListGUI* ui;

    private:
        PlayList playlist;
        QShortcut* pushButtonChainShortcut;
        QShortcut* pushButtonLoopShortcut;
        QString debugName;

        QAction* savePlaylistAction;
        QAction* loadPlaylistAction;
        QAction* clearPlaylistAction;

        QTableWidgetItem* currentItem;

    private:
        void initTable();
        void initMenu();
        void initSignals();
        void disableSignals();
        void unloadItem();
        void clearPlaylist();

    public:
        void initShortcuts(const QString& chainShortcut, const QString& loopShortcut, const QString& name);
        void disableShortcuts();
        void autoloadPlaylist();

    public slots:
        // PushButton Slots
        void recuePlaylistSlot();
        void addFileSlot();
        void removeFileSlot();
        void chainPlayListSlot();
        void loopPlayListSlot();
        void blockPlayListSlot();

        // Menu Slots
        void savePlaylistSlot() const;
        void loadPlaylistSlot();
        void clearPlaylistSlot();

        // TableWidget Slots
        void loadItem(const int index, const bool take);
        void moveItem(const int source, const int destination);

        // Channel Slots
        void itemLoaded();
        void loadNextItem();
        void itemPlaying();

    signals:
        void loadItem(const PlayListItem& item, const bool canTake);
        void recueItem();
        void playListChain(const bool chain);
};

#endif // PLAYLISTGUI_H
