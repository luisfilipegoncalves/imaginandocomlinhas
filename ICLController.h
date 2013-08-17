#ifndef ICLCONTROLLER_H
#define ICLCONTROLLER_H

#include <QObject>
#include <QStringListModel>
#include <QSortFilterProxyModel>

class ICLTagManager;
class ICLImagesManager;
class ICLController : public QObject
{
    Q_OBJECT

public:
    explicit ICLController(QObject *parent = 0);

    ICLTagManager *tagManager();
    ICLImagesManager *imageManager();

signals:
    
public slots:
    void addNewTag(const QString &newTag);
    void addTagToImage(const QString &tagId, const QString &imageId, int index);

private:
    ICLTagManager * _tagManager;
    ICLImagesManager *_imageManager;
};

#endif // ICLCONTROLLER_H
