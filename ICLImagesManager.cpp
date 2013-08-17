#include "ICLImagesManager.h"
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMimeDatabase>
#include <QDir>
#include <QDirIterator>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


ICLImagesManager::ICLImagesManager(QObject *parent) :
    QObject(parent)
{
    _model = new ImagesModel(this);
    _proxyModelFilterImages = new QSortFilterProxyModel(this);
    _proxyModelFilterImages->setSourceModel(_model);

    loadDB();

    connect(&_watcher, SIGNAL(finished()), this, SLOT(fileSystemInfoReady()));
    getRootDirectory("C:\\Users\\User\\Pictures\\mytestpics");
//    getRootDirectory("/Users/luisfilipe/Pictures/testeEva");
}

ICLImagesManager::~ICLImagesManager()
{
    saveDB();
}

QStandardItemModel *ICLImagesManager::model()
{
    return _model;
}

QSortFilterProxyModel *ICLImagesManager::proxyModel()
{
    return _proxyModelFilterImages;
}

extern QStringList entryList(const QString &path)
{
    QMimeDatabase db;

    QDirIterator it(path, QDirIterator::Subdirectories);
    QStringList list;
    while (it.hasNext()) {
        QString f = it.next();
        QMimeType t = db.mimeTypeForFile(f);
        QString mymeTypeName = t.name();
        if(mymeTypeName.contains("image/"))
        {
            //qDebug() << "f = " << f << " type: " << t.name() << t.aliases();
            QFile imageF(f);
            if(imageF.open(QIODevice::ReadOnly))
            {
                QByteArray res = QCryptographicHash::hash(imageF.readAll(), QCryptographicHash::Md5);
                QString resHex = res.toHex();
                list.append(f);
                list.append(resHex);
            }
        }
    }
    return list;
}


void ICLImagesManager::getRootDirectory(const QString & dirPath)
{
    qDebug() << "getting root directory... " << dirPath;
    if(dirPath.isEmpty())
        return;

    QFileInfo info(dirPath);
    if(!info.exists() || !info.isDir())
        return;

    QFuture<QStringList> future = QtConcurrent::run(entryList, dirPath);
    _watcher.setFuture(future);
}

void ICLImagesManager::fileSystemInfoReady()
{
    qDebug() << "fileSystemInfoReady...";
    QStringList results = _watcher.result();
    //qDebug() << "results size: " << results.size() << results;

    for(int i=0; i<results.size(); i+=2)
    {
        QString filePath = results.at(i);
        QString fileHash = results.at(i+1);
        addImageToDB(fileHash, filePath);

    }

    saveDB();
}

void ICLImagesManager::loadDB()
{
    qDebug() << "Loading images...";

    QFile imagesFile("images.json");
    bool fileOpened = imagesFile.open(QIODevice::ReadWrite);
    if(!fileOpened)
    {
        qDebug() << "Error opening images file.";
        return;
    }

    QByteArray fileData = imagesFile.readAll();
    imagesFile.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &error);
    if(error.error != QJsonParseError::NoError)
        qDebug() << "Error loading json file: " << error.errorString();

    QJsonObject jsonObj = doc.object();
    _imagesMap = jsonObj.toVariantMap();

    foreach(QString imageHash, _imagesMap.keys())
    {
        qDebug() << "Got image key: " << imageHash;
        QVariantMap m = _imagesMap.value(imageHash).toMap();
        addImageToModel(imageHash, m);
    }
}

void ICLImagesManager::saveDB()
{
    qDebug() << "Saving db...";

    QFile imagesFile("images.json");
    if(imagesFile.exists())
        imagesFile.remove();

    QJsonDocument doc;
    QJsonObject obj = QJsonObject::fromVariantMap(_imagesMap);
    doc.setObject(obj);

    if(!imagesFile.open(QIODevice::ReadWrite))
    {
        qDebug() << "Error opening images file...";
        return;
    }

    imagesFile.write(doc.toJson());


}

bool ICLImagesManager::addImageToDB(const QString &fileHash, const QString &filePath)
{
    if(_imagesMap.contains(fileHash))
        return false;

    QVariantMap newImageMap;
    newImageMap.insert("filePath", filePath);
    newImageMap.insert("rating", 0);
    QStringList tags;
    newImageMap.insert("tagsList", tags);
    QString strLisJoin = tags.join(", ");
    newImageMap.insert("tagsString", strLisJoin);
    _imagesMap.insert(fileHash, newImageMap);

    addImageToModel(fileHash, newImageMap);

    return true;
}

void ICLImagesManager::addImageToModel(QString imageHash, QVariantMap map)
{
    QStandardItem *item = new QStandardItem(map.value("filePath").toString());
    item->setData(imageHash, Qt::UserRole + 1);
    item->setData(QIcon(map.value("filePath").toString()), Qt::DecorationRole);
    item->setData(map.value("rating").toInt(), Qt::UserRole + 2);
    item->setData(map.value("tagsString").toString(), Qt::UserRole + 3);
    item->setData(map.value("tagsList").toStringList(), Qt::UserRole + 4);
    item->setData(map.value("filePath").toString(), Qt::UserRole + 5);
    _model->appendRow(item);
}

void ICLImagesManager::addTagToImage(const QString &tagName, const QString &tagId, const QString &imageId, int modelIndex)
{
    QVariantMap currentImageMap = _imagesMap.value(imageId).toMap();
    QStringList tagsList = currentImageMap.value("tagsList", QStringList()).toStringList();
    tagsList.append(tagId);

    QStandardItem *item = _model->item(modelIndex, 0);
    item->setData(tagName,  Qt::UserRole + 3);
    item->setData(tagsList,  Qt::UserRole + 4);
}
