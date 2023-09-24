#include "windowclient.h"
#include "ui_windowclient.h"
#include "memclient.h"
#include "cpuclient.h"

windowclient::windowclient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::windowclient),
    clientMemoryInstance(new ClientMemory(this)),
    clientCPUInstance(new CpuClient(this))
{
    ui->setupUi(this);

    int initialMemoryUsage = clientMemoryInstance->retrieveMemoryUsage();
    ui->barclient->setValue(initialMemoryUsage);

    int initialCPUUsage = clientCPUInstance->retrieveCpuUsage();
    ui->barclient_2->setValue(initialCPUUsage);

    connect(clientMemoryInstance, &ClientMemory::memoryUsageUpdated, this, &windowclient::onMemoryUsageUpdated);
    connect(clientCPUInstance, &CpuClient::cpuUsageUpdated, this, &windowclient::onCPUUsageUpdated);
}

windowclient::~windowclient()
{
    delete clientMemoryInstance;
    delete clientCPUInstance;
}

void windowclient::onMemoryUsageUpdated(int percentage) {
    ui->barclient->setValue(percentage);
}

void windowclient::onCPUUsageUpdated(int percentage) {
    ui->barclient_2->setValue(percentage);
}
