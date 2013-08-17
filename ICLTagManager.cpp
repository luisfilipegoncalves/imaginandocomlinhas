#include "ICLTagManager.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDataStream>
#include <QDebug>
#include <QUuid>
#include <QVariantMap>
#include <QVariant>

ICLTagManager *ICLTagManager::_instance = 0;

ICLTagManager::ICLTagManager(QObject *parent) :
    QObject(parent)
{
    _instance = this;

    _model = new TagsModel(this);
    _proxyModelFilterTags = new QSortFilterProxyModel(this);
    _proxyModelFilterTags->setSourceModel(_model);
    _proxyModelFilterTags->setFilterCaseSensitivity(Qt::CaseInsensitive);
    _proxyModelFilterTags->setSortCaseSensitivity(Qt::CaseInsensitive);

    loadTags();

    qDebug() << "created ICLTagManager!";
}

ICLTagManager::~ICLTagManager()
{

}


ICLTagManager *ICLTagManager::instance()
{
    return _instance;
}

bool ICLTagManager::addTag(const QString &strNewTag)
{
    qDebug() << "Adding new tag: " << strNewTag;

    if(strNewTag.isEmpty())
        return false;

    foreach(QVariant v, _tagsList)
    {
        QVariantMap m = v.toMap();
        QString tN = m.value("name", QString()).toString();
        if(tN == strNewTag)
            return false;
    }

    QFile tagsFile("tags.json");
    bool fileOpened = tagsFile.open(QIODevice::ReadWrite);
    if(!fileOpened)
    {
        qDebug() << "Error opening tag file.";
        return false;
    }

    QByteArray fileData = tagsFile.readAll();
    tagsFile.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &error);
    if(error.error != QJsonParseError::NoError)
        qDebug() << "Error loading json file: " << error.errorString();

    QJsonArray jsonArray = doc.array();
    QVariantList docList = jsonArray.toVariantList();


    QVariantMap newTagMap;
    newTagMap.insert("id", QUuid::createUuid().toString());
    newTagMap.insert("name", strNewTag);
    newTagMap.insert("numImages", 0);

    addTagToModel(newTagMap);

    docList.append(newTagMap);

    jsonArray = QJsonArray::fromVariantList(docList);
    doc.setArray(jsonArray);

    tagsFile.remove();
    if(!tagsFile.open(QIODevice::ReadWrite))
    {
        qDebug() << "Error opening tag file.";
        return false;
    }

    qDebug() << "saving: " << doc.toJson();
    tagsFile.write(doc.toJson());
    return true;
}

QStandardItemModel *ICLTagManager::model()
{
    return _model;
}

QSortFilterProxyModel *ICLTagManager::proxyModel()
{
    return _proxyModelFilterTags;
}

QString ICLTagManager::tagNameById(const QString &id) const
{
    foreach(QVariant v, _tagsList)
    {
        QVariantMap m = v.toMap();
        QString mId = m.value("id", QString()).toString();
        if(mId == id)
            return m.value("name", QString()).toString();
    }
}

void ICLTagManager::loadTags()
{
    qDebug() << "Loading tags...";

    QFile tagsFile("tags.json");
    bool fileOpened = tagsFile.open(QIODevice::ReadWrite);
    if(!fileOpened)
    {
        qDebug() << "Error opening tag file.";
        return;
    }

    QByteArray fileData = tagsFile.readAll();
    tagsFile.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &error);
    if(error.error != QJsonParseError::NoError)
        qDebug() << "Error loading json file: " << error.errorString();

    QJsonArray jsonArray = doc.array();
    _tagsList = jsonArray.toVariantList();

    foreach(QVariant v, _tagsList)
    {
        QVariantMap m = v.toMap();
        addTagToModel(m);
    }
    _proxyModelFilterTags->sort(0);

    qDebug() << "load finished";
}

void ICLTagManager::addTagToModel(QVariantMap m)
{
    QString tagName = m.value("name", QString()).toString();
    QString tagId = m.value("id", QString()).toString();
    int numPics = m.value("numImages", 0).toInt();
    qDebug() << "Found tag: " << tagName << "id: " << tagId << "numPics: " << numPics;
    QStandardItem *item = new QStandardItem(tagName);
    item->setData(tagId, Qt::UserRole + 1);
    item->setData(numPics, Qt::UserRole + 2);
    _model->appendRow(item);
}

TagsModel::TagsModel(QObject *parent)
{
}

QHash<int, QByteArray> TagsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Qt::UserRole + 1] = "tagId";
    roles[Qt::UserRole + 2] = "numPics";
    return roles;
}

