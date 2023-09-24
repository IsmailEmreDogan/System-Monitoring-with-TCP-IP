#ifndef WINDOWSERVER_H
#define WINDOWSERVER_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "cpuserver.h"
#include "memserver.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QAreaSeries>

namespace Ui {
class windowserver;
}

class windowserver : public QMainWindow
{
    Q_OBJECT

public:
    explicit windowserver(QWidget *parent = nullptr);
    ~windowserver();

private slots:
    void onMemoryDataPartReceived(QByteArray dataPart);
    void onCPUDataPartReceived(QByteArray dataPart);

private:
    void setupCharts();
    void setupLayouts();
    void setupMainWindowLayout();

    Ui::windowserver *ui;
    MemoryServer* clientMemoryInstance;
    CpuServer* clientCPUInstance;

    QLineSeries* memorySeries;
    QLineSeries* cpuSeries;
    QChartView* memoryChartView;
    QChartView* cpuChartView;

    // Bu iki satırı ekleyin:
    QValueAxis* memoryXAxis;
    QValueAxis* cpuXAxis;
    int timeCounter;
    QAreaSeries *memoryArea;
    QAreaSeries *cpuArea;

};

#endif // WINDOWSERVER_H
