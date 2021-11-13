#include "playlisttable.h"

#include <QMouseEvent>

PlayListTable::PlayListTable(QWidget* parent) :
    QTableWidget(parent)
{
    doubleClickItem = NULL;
    canClear = true;
    channel = 0;

    this->connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(itemDoubleClickedSlot(QTableWidgetItem*)));
}

PlayListTable::~PlayListTable()
{
    for(int i=0; i<this->rowCount(); i++)
    {
        delete this->takeItem(i, 0);
        delete this->takeItem(i, 1);
    }

    this->disconnect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(itemDoubleClickedSlot(QTableWidgetItem*)));
}

void PlayListTable::addItem(const PlayListItem& item)
{
    QTableWidgetItem* nameItem = new QTableWidgetItem(item.name);
    nameItem->setTextAlignment(Qt::AlignHCenter);
    int rows = this->rowCount();
    this->setRowCount(rows + 1);
    nameItem->setBackgroundColor(QColor("white"));
    this->setItem(rows, 0, nameItem);

    QStringList split = item.duration.split(".");
    int frames = split.at(1).toInt() * 25 / 1000;
    QTableWidgetItem* durationItem = new QTableWidgetItem(split[0] + ":" + QString::number(frames).rightJustified(2, '0'));
    durationItem->setTextAlignment(Qt::AlignHCenter);
    durationItem->setBackgroundColor(QColor("white"));
    this->setItem(rows, 1, durationItem);
}

void PlayListTable::removeItem(const int currentIndex)
{
    delete this->takeItem(currentIndex, 0);
    delete this->takeItem(currentIndex, 1);
    this->removeRow(currentIndex);
}

void PlayListTable::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->source() == this)
        event->acceptProposedAction();
}

bool PlayListTable::dropMimeData(int row, int, const QMimeData*, Qt::DropAction)
{
    if(this->selectedItems().size() > 0)
    {
        int sourceRow = this->row(this->selectedItems()[0]);
        QColor color = this->selectedItems()[0]->backgroundColor();

        if(color != QColor("darkGreen") && color != QColor("darkRed"))
        {
            int destinationRow = row;

            if(sourceRow != destinationRow)
            {
                QList<QTableWidgetItem*> sourceCells;
                sourceCells.push_back(this->takeItem(sourceRow, 0));
                sourceCells.push_back(this->takeItem(sourceRow, 1));

                if(sourceRow > destinationRow)
                {
                    for(int i=sourceRow-1; i>=destinationRow; i--)
                        this->moveRow(i, i+1);
                }
                else
                {
                    for(int i=sourceRow+1; i<destinationRow; i++)
                        this->moveRow(i, i-1);
                    destinationRow--;
                }

                this->setItem(destinationRow, 0, sourceCells[0]);
                this->setItem(destinationRow, 1, sourceCells[1]);
            }
        }
    }

    return false;
}

void PlayListTable::moveRow(const int source, const int destination)
{
    QList<QTableWidgetItem*> destinationCells;
    destinationCells.push_back(this->takeItem(source, 0));
    destinationCells.push_back(this->takeItem(source, 1));

    this->setItem(destination, 0, destinationCells[0]);
    this->setItem(destination, 1, destinationCells[1]);

    emit moveItem(source, destination);
}

void PlayListTable::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTableWidget::mouseDoubleClickEvent(event);
    if(doubleClickItem != NULL)
    {
        emit loadItem(this->currentRow(), false);
        doubleClickItem = NULL;
    }
}

void PlayListTable::leaveEvent(QEvent* event)
{
    QTableWidget::leaveEvent(event);
    if(canClear)
    {
        if(this->selectionMode() == QAbstractItemView::MultiSelection && this->selectedItems().count() > 0)
        {
            for(int i=0; i<this->rowCount(); i++)
            {
                QTableWidgetItem* item = this->item(i, 0);
                if(item->backgroundColor() == QColor("yellow"))
                    item->setBackgroundColor(QColor("white"));

                item = this->item(i, 1);
                if(item->backgroundColor() == QColor("yellow"))
                    item->setBackgroundColor(QColor("white"));
            }

            foreach(QTableWidgetItem* item, this->selectedItems())
            {
                if(item->font().bold())
                {
                    item->setBackgroundColor(QColor("darkGreen"));
                    item->setTextColor(QColor("white"));
                }
                else
                {
                    item->setBackgroundColor(QColor("yellow"));
                    item->setTextColor(QColor("black"));
                }
            }
        }
        this->clearSelection();
    }
}

void PlayListTable::mousePressEvent(QMouseEvent* event)
{
    QTableWidget::mousePressEvent(event);
    canClear = false;
}

void PlayListTable::mouseReleaseEvent(QMouseEvent* event)
{
    QTableWidget::mouseReleaseEvent(event);
    canClear = true;
}

void PlayListTable::itemDoubleClickedSlot(QTableWidgetItem* item)
{
    doubleClickItem = item;
}
