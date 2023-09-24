#include "clientwindow.h"
#include "ui_clientwindow.h" // Bu satırı doğru bir şekilde eklemeye devam edin.

ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow),
    clientMemoryInstance(new ClientMemory(this)),
    clientCPUInstance(new CpuClient(this))
{
    ui->setupUi(this);

    int initialMemoryUsage = clientMemoryInstance->retrieveMemoryUsage();
    ui->barclient->setValue(initialMemoryUsage);

    int initialCPUUsage = clientCPUInstance->retrieveCPUUsage();
    ui->barclient_2->setValue(initialCPUUsage);

    connect(clientMemoryInstance, &ClientMemory::memoryUsageUpdated, this, &ClientWindow::onMemoryUsageUpdated);
    connect(clientCPUInstance, &ClientCPU::cpuUsageUpdated, this, &ClientWindow::onCPUUsageUpdated);
}

ClientWindow::~ClientWindow()
{
    delete clientMemoryInstance;
    delete clientCPUInstance;
}

void ClientWindow::onMemoryUsageUpdated(int percentage) {
    ui->barclient->setValue(percentage);
}

void ClientWindow::onCPUUsageUpdated(int percentage) {
    ui->barclient_2->setValue(percentage);
}
