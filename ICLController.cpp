#include "ICLController.h"
#include "ICLTagManager.h"
#include "ICLImagesManager.h"
#include <QDebug>

ICLController::ICLController(QObject *parent) :
    QObject(parent)
{
    _tagManager = new ICLTagManager(this);
    _imageManager = new ICLImagesManager(this);

//    _viewer.rootContext()->setContextProperty("tagsModel", _tagManager->proxyModel());
    //    _viewer.rootContext()->setContextProperty("imagesModel", _imageManager->proxyModel());
}

ICLTagManager *ICLController::tagManager()
{
    return _tagManager;
}

ICLImagesManager *ICLController::imageManager()
{
    return _imageManager;
}

void ICLController::addNewTag(const QString &newTag)
{
    qDebug() << "Add new tag: " << newTag;
    _tagManager->addTag(newTag);
}

void ICLController::addTagToImage(const QString &tagId, const QString &imageId, int modelIndex)
{
    qDebug() << "add tag: " << tagId << " To image: " << imageId << " QModelIndex: " << modelIndex;
    QString tagName = _tagManager->tagNameById(tagId);
    qDebug() << "tagName: " << tagName;

    _imageManager->addTagToImage(tagName, tagId , imageId, modelIndex);
}

