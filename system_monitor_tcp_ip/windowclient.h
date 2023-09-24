#ifndef WINDOWCLIENT_H
#define WINDOWCLIENT_H

#include <QMainWindow>

namespace Ui {
class windowclient;
}

class ClientMemory; // Eğer bu sınıfı önceden tanımladıysanız
class CpuClient;    // Eğer bu sınıfı önceden tanımladıysanız

class windowclient : public QMainWindow
{
    Q_OBJECT

public:
    explicit windowclient(QWidget *parent = nullptr);
    ~windowclient();

private slots:
    void onMemoryUsageUpdated(int percentage);
    void onCPUUsageUpdated(int percentage);

private:
    Ui::windowclient *ui;
    ClientMemory *clientMemoryInstance;
    CpuClient *clientCPUInstance;
};

#endif // WINDOWCLIENT_H
