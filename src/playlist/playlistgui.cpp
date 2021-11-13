#include "playlistgui.h"
#include "ui_playlistgui.h"

#include <QFileDialog>
#include <QMessageBox>

#include <log4cxx/logger.h>

using namespace log4cxx;

PlayListGUI::PlayListGUI(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PlayListGUI)
{
    ui->setupUi(this);

    currentItem = NULL;
    this->initTable();
    this->initMenu();
    this->initSignals();
}

PlayListGUI::~PlayListGUI()
{
    this->disableSignals();

    delete ui;

    delete savePlaylistAction;
    delete loadPlaylistAction;
    delete clearPlaylistAction;
}

void PlayListGUI::initTable()
{
    ui->tableWidget->setColumnCount(2);
    QStringList horizontalHeader;
    horizontalHeader.append("Clip Name");
    horizontalHeader.append("Duration");
    ui->tableWidget->setHorizontalHeaderLabels(horizontalHeader);

    ui->tableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Custom);
    ui->tableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Custom);

    ui->tableWidget->horizontalHeader()->resizeSection(0, 303);
    ui->tableWidget->horizontalHeader()->resizeSection(1, 130);
}

void PlayListGUI::initMenu()
{
    savePlaylistAction = new QAction("Save Playlist", this);
    this->addAction(savePlaylistAction);
    this->connect(savePlaylistAction, SIGNAL(triggered()), this, SLOT(savePlaylistSlot()));
    loadPlaylistAction = new QAction("Load Playlist", this);
    this->addAction(loadPlaylistAction);
    this->connect(loadPlaylistAction, SIGNAL(triggered()), this, SLOT(loadPlaylistSlot()));
    clearPlaylistAction = new QAction("Clear Playlist", this);
    this->addAction(clearPlaylistAction);
    this->connect(clearPlaylistAction, SIGNAL(triggered()), this, SLOT(clearPlaylistSlot()));
    this->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void PlayListGUI::initSignals()
{
    this->connect(ui->pushButtonRecuePlaylist, SIGNAL(clicked()), this, SLOT(recuePlaylistSlot()));
    this->connect(ui->pushButtonAddFile, SIGNAL(clicked()), this, SLOT(addFileSlot()));
    this->connect(ui->pushButtonRemoveFile, SIGNAL(clicked()), this, SLOT(removeFileSlot()));
    this->connect(ui->pushButtonChain, SIGNAL(clicked()), this, SLOT(chainPlayListSlot()));
    this->connect(ui->pushButtonLoop, SIGNAL(clicked()), this, SLOT(loopPlayListSlot()));
    this->connect(ui->pushButtonBlock, SIGNAL(clicked()), this, SLOT(blockPlayListSlot()));
    this->connect(ui->tableWidget, SIGNAL(loadItem(int, bool)), this, SLOT(loadItem(int, bool)));
    this->connect(ui->tableWidget, SIGNAL(moveItem(int,int)), this, SLOT(moveItem(int,int)));
}

void PlayListGUI::disableSignals()
{
    this->disconnect(ui->pushButtonRecuePlaylist, SIGNAL(clicked()), this, SLOT(recuePlaylistSlot()));
    this->disconnect(ui->pushButtonAddFile, SIGNAL(clicked()), this, SLOT(addFileSlot()));
    this->disconnect(ui->pushButtonRemoveFile, SIGNAL(clicked()), this, SLOT(removeFileSlot()));
    this->disconnect(ui->pushButtonChain, SIGNAL(clicked()), this, SLOT(chainPlayListSlot()));
    this->disconnect(ui->pushButtonLoop, SIGNAL(clicked()), this, SLOT(loopPlayListSlot()));
    this->disconnect(ui->pushButtonBlock, SIGNAL(clicked()), this, SLOT(blockPlayListSlot()));
    this->disconnect(ui->tableWidget, SIGNAL(loadItem(int, bool)), this, SLOT(loadItem(int, bool)));
    this->disconnect(ui->tableWidget, SIGNAL(moveItem(int,int)), this, SLOT(moveItem(int,int)));

    this->disconnect(savePlaylistAction, SIGNAL(triggered()), this, SLOT(savePlaylistSlot()));
    this->disconnect(loadPlaylistAction, SIGNAL(triggered()), this, SLOT(loadPlaylistSlot()));

    this->disconnect(pushButtonChainShortcut, SIGNAL(activated()), ui->pushButtonChain, SLOT(click()));
    this->disconnect(pushButtonLoopShortcut, SIGNAL(activated()), ui->pushButtonLoop, SLOT(click()));
}

void PlayListGUI::initShortcuts(const QString& chainShortcut, const QString& loopShortcut, const QString& name)
{
    pushButtonChainShortcut = new QShortcut(QKeySequence(chainShortcut), this);
    this->connect(pushButtonChainShortcut, SIGNAL(activated()), ui->pushButtonChain, SLOT(click()));
    pushButtonLoopShortcut = new QShortcut(QKeySequence(loopShortcut), this);
    this->connect(pushButtonLoopShortcut, SIGNAL(activated()), ui->pushButtonLoop, SLOT(click()));
    this->debugName = name;
}

void PlayListGUI::disableShortcuts()
{
    this->disconnect(pushButtonChainShortcut, SIGNAL(activated()), ui->pushButtonChain, SLOT(click()));
    this->disconnect(pushButtonLoopShortcut, SIGNAL(activated()), ui->pushButtonLoop, SLOT(click()));

    delete pushButtonChainShortcut;
    delete pushButtonLoopShortcut;
}

void PlayListGUI::autoloadPlaylist()
{
    QList<PlayListItem> list = playlist.loadPlaylist(QApplication::applicationDirPath() + "/" + debugName + "_bck.pvs");
    foreach(PlayListItem item, list)
    {
        ui->tableWidget->addItem(item);
    }

    LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Autoloaded " + QString::number(ui->tableWidget->rowCount()).toStdString() + " items");
}

void PlayListGUI::unloadItem()
{
    if(currentItem != NULL)
    {
        currentItem->setBackgroundColor(QColor("gray"));
        currentItem->setTextColor(QColor("black"));
        QFont font;
        font.setBold(false);
        currentItem->setFont(font);
        QTableWidgetItem* tableItem = ui->tableWidget->item(currentItem->row(), 1);
        tableItem->setBackgroundColor(QColor("gray"));
        tableItem->setTextColor(QColor("black"));
        tableItem->setFont(font);

        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Unloaded item: " + currentItem->text().toStdString());
    }
}

// PushButton Slots

void PlayListGUI::recuePlaylistSlot()
{
    if(ui->tableWidget->rowCount() > 0)
    {
        // If there is anything playing do nothing
        for(int i=0; i<ui->tableWidget->rowCount(); i++)
        {
            QTableWidgetItem* tableItem = ui->tableWidget->item(i, 0);
            if(tableItem->backgroundColor() == QColor("darkRed"))
                return;
        }

        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Recued playlist");

        loadItem(0, ui->pushButtonLoop->isChecked());

        for(int i=1; i<ui->tableWidget->rowCount(); i++)
        {
            QTableWidgetItem* tableItem = ui->tableWidget->item(i, 0);
            tableItem->setBackgroundColor(QColor("white"));
            tableItem->setTextColor(QColor("black"));
            QFont font;
            font.setBold(false);
            tableItem->setFont(font);
            tableItem = ui->tableWidget->item(i, 1);
            tableItem->setBackgroundColor(QColor("white"));
            tableItem->setTextColor(QColor("black"));
            tableItem->setFont(font);
        }
    }
}

void PlayListGUI::addFileSlot()
{
    QFileDialog fileDialog;
    fileDialog.setWindowTitle("Add Video File...");
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setFilter(tr("Videos (*.mov *.mp4 *.mxf *.avi *.m4v);;All Files (*.*)"));

    QStringList fileNames;
    if(fileDialog.exec())
        fileNames = fileDialog.selectedFiles();

    foreach(const QString &str, fileNames)
    {
        PlayListItem item = playlist.addItem(str);
        if(item.duration == "00:00:00.000")
        {
            QString msg("Error adding file \"" + item.name + "\"");
            QMessageBox::critical((QWidget*)this->parent(), "Error", msg);
            LOG4CXX_ERROR(Logger::getLogger(debugName.toStdString()), msg.toStdString());
        }
        else
        {
            ui->tableWidget->addItem(item);
            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Added item: " + item.path.toStdString());
        }
    }

    this->playlist.savePlaylist(QApplication::applicationDirPath() + "/" + debugName + "_bck.pvs");
    LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Autosaved playlist");
}

void PlayListGUI::removeFileSlot()
{
    int currentIndex = ui->tableWidget->currentRow();
    if(currentIndex > -1)
    {
        QTableWidgetItem* tableItem = ui->tableWidget->item(currentIndex, 0);
        if(tableItem->backgroundColor() != QColor("darkGreen") && tableItem->backgroundColor() != QColor("darkRed"))
        {
            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Removed item: " + tableItem->text().toStdString());

            if(currentItem == tableItem)
                currentItem = NULL;

            playlist.removeItem(currentIndex);
            ui->tableWidget->removeItem(currentIndex);

            this->playlist.savePlaylist(QApplication::applicationDirPath() + "/" + debugName + "_bck.pvs");
            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Autosaved playlist");
        }
    }
}

void PlayListGUI::chainPlayListSlot()
{
    ui->pushButtonChain->isChecked() ? ui->pushButtonLoop->raise() : ui->pushButtonLoop->lower();
    ui->pushButtonLoop->setEnabled(ui->pushButtonChain->isChecked());
    if(!ui->pushButtonChain->isChecked() && ui->pushButtonLoop->isChecked())
    {
        ui->pushButtonLoop->setChecked(false);
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Loop deactivated");
    }

    emit playListChain(ui->pushButtonChain->isChecked() || ui->pushButtonBlock->isChecked());
    if(ui->pushButtonChain->isChecked())
    {
        ui->pushButtonBlock->setChecked(false);
        blockPlayListSlot();
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Chain activated");
    }
    else LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Chain deactivated");
}

void PlayListGUI::loopPlayListSlot()
{
    if(ui->pushButtonLoop->isChecked())
    {
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Loop activated");
    }
    else LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Loop deactivated");
}

void PlayListGUI::blockPlayListSlot()
{
    emit playListChain(ui->pushButtonChain->isChecked() || ui->pushButtonBlock->isChecked());
    if(ui->pushButtonBlock->isChecked())
    {
        ui->pushButtonChain->setChecked(false);
        chainPlayListSlot();

        this->ui->tableWidget->setSelectionMode(QAbstractItemView::MultiSelection);
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Block activated");
    }
    else
    {
        this->ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        for(int i=0; i<this->ui->tableWidget->rowCount(); i++)
        {
            QTableWidgetItem* item = this->ui->tableWidget->item(i, 0);
            if(item->backgroundColor() == QColor("yellow"))
            {
                if(item->font().bold())
                {
                    item->setBackgroundColor(QColor("darkGreen"));
                    item->setTextColor(QColor("white"));
                }
                else
                {
                    item->setBackgroundColor(QColor("white"));
                    item->setTextColor(QColor("black"));
                }
            }

            item = this->ui->tableWidget->item(i, 1);
            if(item->backgroundColor() == QColor("yellow"))
            {
                if(item->font().bold())
                {
                    item->setBackgroundColor(QColor("darkGreen"));
                    item->setTextColor(QColor("white"));
                }
                else
                {
                    item->setBackgroundColor(QColor("white"));
                    item->setTextColor(QColor("black"));
                }
            }
        }

        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Block deactivated");
    }
}

// Menu Slots

void PlayListGUI::savePlaylistSlot() const
{
    if(ui->tableWidget->rowCount() > 0)
    {
        QString path = QFileDialog::getSaveFileName((QWidget*)this->parent(), tr("Save playlist..."), "", tr("PowerVS Playlist (*.pvs)"));
        if(path != "")
        {
            this->playlist.savePlaylist(path);
            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Saved " + QString::number(ui->tableWidget->rowCount()).toStdString() + " items to playlist: " + path.toStdString());
        }
    }
}

void PlayListGUI::clearPlaylist()
{
    currentItem = NULL;
    for(int i=ui->tableWidget->rowCount()-1; i>=0; i--)
    {
        playlist.removeItem(i);
        ui->tableWidget->removeRow(i);
    }

    PlayListItem item;
    item.path = "";
    emit loadItem(item, false);
}

void PlayListGUI::loadPlaylistSlot()
{
    if(currentItem != NULL && currentItem->backgroundColor() == QColor("darkRed"))
        return;

    QFileDialog fileDialog;
    fileDialog.setWindowTitle("Load playlist...");
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setFilter(tr("PowerVS Playlist (*.pvs)"));

    QStringList fileNames;
    if(fileDialog.exec())
        fileNames = fileDialog.selectedFiles();

    QString path = "";
    if(!fileNames.empty())
        path = fileNames.first();

    if(path != "")
    {
        QList<PlayListItem> list = playlist.loadPlaylist(path);
        foreach(PlayListItem item, list)
        {
            ui->tableWidget->addItem(item);
        }

        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Loaded " + QString::number(ui->tableWidget->rowCount()).toStdString() + " items from playlist: " + path.toStdString());

        this->playlist.savePlaylist(QApplication::applicationDirPath() + "/" + debugName + "_bck.pvs");
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Autosaved playlist");
    }
}

void PlayListGUI::clearPlaylistSlot()
{
    if(ui->tableWidget->rowCount() > 0)
    {
        if(currentItem != NULL && currentItem->backgroundColor() == QColor("darkRed"))
            return;

        int result = QMessageBox::question((QWidget*)this->parent(), "Clear Playlist", "Are you sure you want to clear the playlist?", QMessageBox::No, QMessageBox::Yes);
        if(result == QMessageBox::Yes)
        {
            if(currentItem != NULL)
                unloadItem();

            clearPlaylist();

            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Cleared playlist");

            this->playlist.savePlaylist(QApplication::applicationDirPath() + "/" + debugName + "_bck.pvs");
            LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Autosaved playlist");
        }
    }
}

// TableWidget Slots

void PlayListGUI::loadItem(const int index, const bool canTake)
{
    QTableWidgetItem* tableItem = ui->tableWidget->item(index, 0);

    if(currentItem == NULL)
        currentItem = tableItem;

    if(currentItem->backgroundColor() != QColor("darkRed"))
    {
        if(currentItem != tableItem)
        {
            unloadItem();

            currentItem = tableItem;
            PlayListItem item = playlist.getItem(index);
            emit loadItem(item, canTake);
        }
        else if(currentItem->backgroundColor() != QColor("darkGreen"))
        {
            PlayListItem item = playlist.getItem(index);
            emit loadItem(item, canTake);
        }
    }
}

void PlayListGUI::moveItem(const int source, const int destination)
{
    LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Moved item: " + ui->tableWidget->item(destination, 0)->text().toStdString());
    playlist.moveItem(source, destination);
}

// Channel Slots

void PlayListGUI::itemLoaded()
{
    if(currentItem != NULL)
    {
        currentItem->setBackgroundColor(QColor("darkGreen"));
        currentItem->setTextColor(QColor("white"));
        QFont font;
        font.setBold(true);
        currentItem->setFont(font);
        QTableWidgetItem* tableItem = ui->tableWidget->item(currentItem->row(), 1);
        tableItem->setBackgroundColor(QColor("darkGreen"));
        tableItem->setTextColor(QColor("white"));
        tableItem->setFont(font);

        ui->tableWidget->scrollToItem(currentItem);
        LOG4CXX_INFO(Logger::getLogger(debugName.toStdString()), "Loaded item: " + currentItem->text().toStdString());
    }
}

void PlayListGUI::loadNextItem()
{
    int startIndex = 0;

    if(currentItem != NULL)
    {
        startIndex = currentItem->row();
        unloadItem();
        currentItem = NULL;
    }

    if(ui->pushButtonChain->isChecked() || ui->pushButtonBlock->isChecked())
    {
        bool loadedNext = false;
        for(int i=startIndex; i<ui->tableWidget->rowCount(); i++)
        {
            QColor color = this->ui->tableWidget->selectionMode() == QAbstractItemView::SingleSelection ? QColor("white") : QColor("yellow");
            if(ui->tableWidget->item(i, 0)->backgroundColor() == color)
            {
                loadItem(i, true);
                loadedNext = true;
                break;
            }
        }

        if(!loadedNext && ui->pushButtonLoop->isChecked())
            recuePlaylistSlot();
    }
}

void PlayListGUI::itemPlaying()
{
    if(currentItem != NULL)
    {
        currentItem->setBackgroundColor(QColor("darkRed"));
        currentItem->setTextColor(QColor("white"));
        QFont font;
        font.setBold(true);
        currentItem->setFont(font);
        QTableWidgetItem* tableItem = ui->tableWidget->item(currentItem->row(), 1);
        tableItem->setBackgroundColor(QColor("darkRed"));
        tableItem->setTextColor(QColor("white"));
        tableItem->setFont(font);
    }
}
