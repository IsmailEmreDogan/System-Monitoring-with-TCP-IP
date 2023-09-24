#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>

namespace Ui {
class serverwindow;
}

class serverwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit serverwindow(QWidget *parent = nullptr);
    ~serverwindow();

private:
    Ui::serverwindow *ui;
};

#endif // SERVERWINDOW_H
