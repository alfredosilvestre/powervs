#ifndef PLAYLISTTABLE_H
#define PLAYLISTTABLE_H

#include "playlist.h"

#include <QTableWidget>

class PlayListTable : public QTableWidget
{
    Q_OBJECT

    public:
        explicit PlayListTable(QWidget* parent = 0);
        ~PlayListTable();

    private:
        QTableWidgetItem* doubleClickItem;
        bool canClear;
        int channel;

    private:
        void moveRow(const int source, const int destination);

    public:
        void addItem(const PlayListItem& item);
        void removeItem(const int currentIndex);
        void dragEnterEvent(QDragEnterEvent* event);
        bool dropMimeData(int row, int column, const QMimeData* data, Qt::DropAction action);
        void mouseDoubleClickEvent(QMouseEvent* event);
        void leaveEvent(QEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);

    public slots:
        void itemDoubleClickedSlot(QTableWidgetItem* item);

    signals:
        void loadItem(const int index, const bool canTake);
        void moveItem(const int source, const int destination);
};

#endif // PLAYLISTTABLE_H
