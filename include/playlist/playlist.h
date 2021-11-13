#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QList>
#include <QString>

extern "C"
{
    #include <libavformat/avformat.h>
}

struct PlayListItem
{
    QString name;
    QString path;
    QString duration;
    int64_t duration_ms;
};

class PlayList
{
    public:
        PlayList();
        ~PlayList();

    private:
        QList<PlayListItem> items;

    public:
        const PlayListItem addItem(const QString& path);
        void removeItem(const int index);
        const PlayListItem getItem(const int index) const;
        void moveItem(const int source, const int destination);
        void savePlaylist(const QString& path) const;
        const QList<PlayListItem> loadPlaylist(const QString& path);
};

#endif // PLAYLIST_H
