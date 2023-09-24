#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include "memclient.h"
#include "cpuclient.h"
#include "ui_clientwindow.h"

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

private slots:
    void onMemoryUsageUpdated(int percentage);
    void onCPUUsageUpdated(int percentage);

private:
    Ui::ClientWindow *ui;
    ClientMemory *clientMemoryInstance;
    CpuClient *clientCPUInstance;
};

#endif // CLIENTWINDOW_H
