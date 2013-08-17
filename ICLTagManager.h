#ifndef ICLTAGMANAGER_H
#define ICLTAGMANAGER_H

#include <QObject>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class TagsModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit TagsModel(QObject *parent = 0);

protected:
    QHash<int, QByteArray> roleNames() const;
};


class ICLTagManager : public QObject
{
    Q_OBJECT
public:

    explicit ICLTagManager(QObject *parent = 0);

    static ICLTagManager *instance();

    ~ICLTagManager();

    bool addTag(const QString &newTag);

    QStandardItemModel * model();
    QSortFilterProxyModel *proxyModel();

    QString tagNameById(const QString &id) const;

signals:
    
public slots:

private:
    void loadTags();
    void addTagToModel(QVariantMap tagMap);


private:
    QVariantList _tagsList;
    TagsModel *_model;
    QSortFilterProxyModel *_proxyModelFilterTags;
    static ICLTagManager *_instance;

};

#endif // ICLTAGMANAGER_H
