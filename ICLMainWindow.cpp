#include "ICLMainWindow.h"
#include "ui_ICLMainWindow.h"
#include "ICLTagManager.h"
#include "ICLImagesManager.h"
#include "ICLListView.h"
#include <QLineEdit>
#include <QtWidgets>
#include <QImageReader>
#include <QFontDatabase>

class TagDelegate : public QStyledItemDelegate
{
public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

class ImageDelegate : public QStyledItemDelegate
{
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

ICLMainWindow::ICLMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ICLMainWindow)
{
    ui->setupUi(this);
    ui->centralWidget->setProperty("class", "central");
    ui->centralWidget->setStyleSheet("QWidget[class=\"central\"]{background-color: #515151;}");

    ui->widgetTags->setProperty("class", "widgetTags");
    ui->widgetTags->setStyleSheet("QWidget[class=\"widgetTags\"]{\nbackground-color: #dadfe7;}");

    ui->listViewTags->setProperty("class", "listViewTags");
    ui->listViewTags->setStyleSheet("QWidget[class=\"listViewTags\"]{\nbackground-color: #dadfe7;}");

    ui->listViewImages->setProperty("class", "listViewImages");
    ui->listViewImages->setStyleSheet("QListView[class=\"listViewImages\"]"
                                      "{background-color: #676767;\n"
                                      "}");


    ui->listViewTags->setModel(controler.tagManager()->model());
    ui->listViewTags->setDropIndicatorShown(true);
    ui->listViewTags->setItemDelegate(new TagDelegate);

    ui->listViewImages->setModel(controler.imageManager()->model());
    ui->listViewImages->setDragEnabled(true);
    ui->listViewImages->setItemDelegate(new ImageDelegate);

    connect(ui->pushButtonAddNewTag, SIGNAL(clicked()), this, SLOT(addNewTag()));
    connect(ui->listViewTags, SIGNAL(dropedImagesAtTag(QString)), this, SLOT(addImagesToTag(QString)));
}

ICLMainWindow::~ICLMainWindow()
{
    delete ui;
}

void ICLMainWindow::addNewTag()
{
    QString tagName = ui->lineEditNewTagName->text();
    qDebug() << "tagName: " << tagName;
    if(!tagName.isEmpty())
    {
        controler.tagManager()->addTag(tagName);
    }
}

void ICLMainWindow::addImagesToTag(QString tagId)
{
   // QModelIndexList l = ui->listViewImages->selectedIndexes();
}

void ImageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QSize size = option.rect.size();

    QPen penBorder;
    penBorder.setWidthF(option.state&QStyle::State_Selected ? 1.5 : 1.); penBorder.setColor(option.state&QStyle::State_Selected ? Qt::black : Qt::gray);

    QPen penOutline;
    penOutline.setWidthF(6.0);
    penOutline.setColor(QColor(86, 148, 203));

    float radius = 5;

    QBrush brush;
    QRectF r;
    r.setSize(size - QSize(10, 35));
    r.moveCenter(option.rect.center());
    r.moveBottom(option.rect.bottom()-25);


    QString title = index.data().toString();
    QImageReader reader;
    QString fileP = index.data(Qt::UserRole + 5).toString();
    reader.setFileName(fileP);
    QSizeF thumbSize(reader.size());
    thumbSize.scale(r.size(), Qt::KeepAspectRatio);

    reader.setScaledSize(thumbSize.toSize());
    QImage image = reader.read();

    QRectF thumbRect(QPoint(), thumbSize);
    thumbRect.moveBottomLeft(QPoint(r.left() + 0.5*(r.width()-thumbRect.width()), r.bottom()));

    if(option.state&QStyle::State_Selected)
    {
        QLinearGradient g(thumbRect.topLeft(), thumbRect.bottomLeft());
        g.setColorAt(0, QColor(111, 179, 219));
        g.setColorAt(1, QColor(61, 106, 180));
        QPen pen(g, 7);

        painter->setBrush(Qt::NoBrush);
        painter->setPen(pen);
        painter->drawRoundedRect(thumbRect, radius, radius);
    }

    float s = float(thumbSize.width()) / float(image.width());

    QTransform t;
    t.translate(thumbRect.x(), thumbRect.y());
    brush.setTransform(t);
    brush.setTextureImage(QImage(":/dmapi/icons/16x16/chessboard"));

    painter->setBrush(brush);
    painter->setPen(penBorder);
    painter->drawRoundedRect(thumbRect, radius, radius);

    t.scale(s, s);
    brush.setTransform(t);
    brush.setTextureImage(image);
    painter->setBrush(brush);

    if(!option.state&QStyle::State_Selected)
    {
        QLinearGradient g(thumbRect.topLeft(), thumbRect.bottomLeft());
        QColor c(Qt::gray);
        g.setColorAt(0, c);
        g.setColorAt(1, c.darker(200));
        QPen pen(g, 2);
        painter->setPen(pen);
    }
    else
    {
        painter->setPen(Qt::NoPen);
    }

    painter->drawRoundedRect(thumbRect, radius, radius);


    painter->setOpacity(1.);

    QFont font;
    font.setPixelSize(10);
    painter->setFont(font);

    QRect textRect = option.rect;
    textRect.setTop(textRect.bottom() - 15);
    textRect.adjust(10, 0, -10, 0);

    QFontMetrics metrics(font, painter->device());
    r = metrics.boundingRect(textRect, Qt::AlignCenter|Qt::TextSingleLine, title);
    r = r.intersected(textRect);
    r.adjust(-10, -2, 10, 2);

    Qt::TextElideMode mode = Qt::ElideMiddle;
    const QStyleOptionViewItem *opt = qstyleoption_cast<const QStyleOptionViewItem*>(&option);
    if(opt) mode = opt->textElideMode;

    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::white);
    painter->drawText(textRect, Qt::AlignCenter|Qt::TextSingleLine, metrics.elidedText(title, mode, textRect.width() - 20));
    painter->restore();
}

QSize ImageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(256, 256);
}


QSize TagDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    s.rheight() = 50;
    return s;
}
