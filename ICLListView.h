#ifndef ICLLISTVIEW_H
#define ICLLISTVIEW_H

#include <QListView>

class ICLListView : public QListView
{
    Q_OBJECT
public:
    explicit ICLListView(QWidget *parent = 0);
    
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);

signals:
    void dropedImagesAtTag(QString tagId);

public slots:
    
};

#endif // ICLLISTVIEW_H
