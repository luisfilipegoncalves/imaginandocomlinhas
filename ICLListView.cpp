#include "ICLListView.h"
#include <QDragMoveEvent>
#include <QDebug>
#include <QMimeData>

ICLListView::ICLListView(QWidget *parent) :
    QListView(parent)
{
}

void ICLListView::dragMoveEvent(QDragMoveEvent *e)
{
    if(e->source() != this && e->dropAction())
        e->accept();
}

void ICLListView::dropEvent(QDropEvent *e)
{
    QString tagId = indexAt(e->pos()).data(Qt::UserRole + 1).toString();
    qDebug() << "Drop Event at tag id: " << tagId;
    emit dropedImagesAtTag(tagId);

}
