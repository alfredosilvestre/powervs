#include "playlist.h"

#include <QFileInfo>
#include <QTime>
#include <QTextStream>

PlayList::PlayList()
{
}

PlayList::~PlayList()
{
}

const PlayListItem PlayList::addItem(const QString& path)
{
    QFileInfo fileInfo(path);
    int64_t duration_ms = 0;
    QTime duration(0, 0, 0, 0);
    AVFormatContext* avFormatContext = NULL;
    AVDictionary* options = NULL;
    av_dict_set(&options, "enable_drefs", "1", 0);
    if(avformat_open_input(&avFormatContext, path.toStdString().c_str(), NULL, &options) == 0)
    {
        if(avformat_find_stream_info(avFormatContext, NULL) >= 0)
        {
            AVRational tb;
            tb.den = 1000;
            tb.num = 1;

            for(unsigned int i=0; i < avFormatContext->nb_streams; i++)
            {
                AVStream* stream = avFormatContext->streams[i];

                duration_ms = av_rescale_q(stream->duration, stream->time_base, tb);
                if(duration_ms <= 0)
                    duration_ms = avFormatContext->duration / 1000;
                if(stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                    break;
            }

            duration = duration.addMSecs(duration_ms);
        }

        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
    }

    PlayListItem item;
    item.name = fileInfo.baseName();
    item.path = path;
    item.duration = duration.toString("hh:mm:ss.zzz");
    item.duration_ms = duration_ms;

    if(item.duration != "00:00:00.000")
        this->items.append(item);

    return item;
}

void PlayList::removeItem(const int index)
{
    this->items.removeAt(index);
}

const PlayListItem PlayList::getItem(const int index) const
{
    return this->items.at(index);
}

void PlayList::moveItem(const int source, const int destination)
{
    this->items.insert(destination, this->items.takeAt(source));
}

void PlayList::savePlaylist(const QString& path) const
{
    QFile file(path);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);

    foreach(PlayListItem item, items)
    {
        stream << item.path << "\n";
    }

    file.close();
}

const QList<PlayListItem> PlayList::loadPlaylist(const QString& path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream stream(&file);
    QList<PlayListItem> newItems;

    while(!stream.atEnd())
    {
        QString line = stream.readLine();
        PlayListItem item = this->addItem(line);
        if(item.duration != "00:00:00.000")
            newItems.append(item);
    }

    file.close();

    return newItems;
}
