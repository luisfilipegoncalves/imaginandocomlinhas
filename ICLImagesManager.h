#ifndef ICLIMAGESMANAGER_H
#define ICLIMAGESMANAGER_H

#include <QObject>
#include <QFutureWatcher>
#include <QStringList>

#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class ImagesModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit ImagesModel(QObject *parent = 0){}

protected:
    QHash<int, QByteArray> roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[Qt::DisplayRole] = "imagePath";
        roles[Qt::UserRole + 1] = "hash";
        roles[Qt::UserRole + 2] = "rating";
        roles[Qt::UserRole + 3] = "tagsString";
        roles[Qt::UserRole + 4] = "tagsList";
        return roles;
    }
};


class ICLImagesManager : public QObject
{
    Q_OBJECT
public:
    explicit ICLImagesManager(QObject *parent = 0);
    ~ICLImagesManager();

    QStandardItemModel * model();
    QSortFilterProxyModel *proxyModel();

    void getRootDirectory(const QString &dirPath);
    void addTagToImage(const QString &tagName, const QString &tagId, const QString &imageId, int modelIndex);

signals:
    
public slots:

private slots:
    void fileSystemInfoReady();

private:
    void loadDB();
    void saveDB();
    bool addImageToDB(const QString &fileHash, const QString &filePath);
    void addImageToModel(QString imageHash, QVariantMap map);

private:
    QFutureWatcher<QStringList> _watcher;
    ImagesModel *_model;
    QSortFilterProxyModel *_proxyModelFilterImages;

    QVariantMap _imagesMap;
};

#endif // ICLIMAGESMANAGER_H
