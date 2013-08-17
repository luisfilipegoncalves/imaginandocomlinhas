#ifndef ICLMAINWINDOW_H
#define ICLMAINWINDOW_H

#include <QMainWindow>
#include "ICLController.h"

namespace Ui {
class ICLMainWindow;
}

class ICLMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ICLMainWindow(QWidget *parent = 0);
    ~ICLMainWindow();
    
private slots:
    void addNewTag();
    void addImagesToTag(QString tagId);

private:
    Ui::ICLMainWindow *ui;
    ICLController controler;
};

#endif // ICLMAINWINDOW_H
