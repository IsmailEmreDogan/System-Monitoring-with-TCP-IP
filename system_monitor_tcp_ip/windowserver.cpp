#include "windowserver.h"
#include "ui_windowserver.h"

windowserver::windowserver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::windowserver)
{
    ui->setupUi(this);
    timeCounter = 0;

    clientMemoryInstance = new MemoryServer(this);
    ui->barserver->setRange(0, 100);
    connect(clientMemoryInstance, &MemoryServer::dataPartReceived, this, &windowserver::onMemoryDataPartReceived);

    clientCPUInstance = new CpuServer(this);
    ui->barserver_2->setRange(0,100);
    connect(clientCPUInstance, &CpuServer::dataPartReceived, this, &windowserver::onCPUDataPartReceived);
    setupCharts();
    setupLayouts();
    setupMainWindowLayout();
}

windowserver::~windowserver()
{
    delete clientMemoryInstance;
    delete clientCPUInstance;
    delete ui;
}

void windowserver::setupCharts()
{
    memorySeries = new QLineSeries(this);    // Serileri başlatma

    cpuSeries = new QLineSeries(this);
    memorySeries->setPointsVisible(false);
    cpuSeries->setPointsVisible(false);

    // Çizgi renkleri ve kalınlıklarını ayarlama
    QPen memoryPen(QColor(66, 135, 245), 2);
    QPen cpuPen(QColor(76, 175, 80), 2);

    memorySeries->setPen(memoryPen);
    cpuSeries->setPen(cpuPen);

    memoryArea = new QAreaSeries(memorySeries);
    memoryArea->setColor(QColor(66, 135, 245, 80));
    memoryArea->setBorderColor(QColor(66, 135, 245));

    cpuArea = new QAreaSeries(cpuSeries);
    cpuArea->setColor(QColor(76, 175, 80, 80));
    cpuArea->setBorderColor(QColor(76, 175, 80));

    // Bellek için grafik oluşturma
    QChart *memoryChart = new QChart();
    memoryChart->setBackgroundBrush(QColor(220,220,220));

    memoryChart->addSeries(memoryArea);
    memoryChart->setTitle("Memory Usage");
    memoryChart->setTitleFont(QFont("default",16));

    // Bellek için Y ekseni oluşturma
    QValueAxis *memoryYAxis = new QValueAxis(this);
    memoryYAxis->setRange(0, 100);
    memoryYAxis->setTickCount(7);
    memoryYAxis->setLabelFormat("%d");
    memoryYAxis->setGridLineColor(Qt::black);
    memoryYAxis->setLinePenColor(Qt::black);

    memoryChart->addAxis(memoryYAxis, Qt::AlignLeft);
    memoryArea->attachAxis(memoryYAxis);

    // Bellek için X ekseni
    memoryXAxis = new QValueAxis(this);
    memoryXAxis->setLabelFormat("%d");
    memoryXAxis->setGridLineColor(Qt::black);
    memoryXAxis->setLinePenColor(Qt::black);

    memoryChart->addAxis(memoryXAxis, Qt::AlignBottom);
    memoryArea->attachAxis(memoryXAxis);

    // CPU için grafik oluşturma
    QChart *cpuChart = new QChart();
    cpuChart->setBackgroundBrush(QColor(220,220,220));
    cpuChart->addSeries(cpuArea);
    cpuChart->setTitle("CPU Usage");
    cpuChart->setTitleFont(QFont("default",16));


    // CPU için Y ekseni
    QValueAxis *cpuYAxis = new QValueAxis(this);
    cpuYAxis->setRange(0, 100);
    cpuYAxis->setTickCount(7);
    cpuYAxis->setLabelFormat("%d");
    cpuYAxis->setGridLineColor(Qt::black);
    cpuYAxis->setLinePenColor(Qt::black);
    cpuChart->addAxis(cpuYAxis, Qt::AlignLeft);
    cpuArea->attachAxis(cpuYAxis);

    // CPU için X ekseni
    cpuXAxis = new QValueAxis(this);
    cpuXAxis->setLabelFormat("%d");
    cpuXAxis->setGridLineColor(Qt::black);
    cpuXAxis->setLinePenColor(Qt::black);

    cpuChart->addAxis(cpuXAxis, Qt::AlignBottom);
    cpuArea->attachAxis(cpuXAxis);

    memoryChart->legend()->hide();
    cpuChart->legend()->hide();

    // Grafikleri görüntüleyicilere ekleme
    memoryChartView = new QChartView(memoryChart);
    cpuChartView = new QChartView(cpuChart);
}




void windowserver::onMemoryDataPartReceived(QByteArray dataPart)
{
    int memoryUsagePercentage = dataPart.toInt();
    ui->barserver->setValue(memoryUsagePercentage);
    memorySeries->append(timeCounter, memoryUsagePercentage);

    if (timeCounter > 10) {
        memoryXAxis->setRange(timeCounter - 10, timeCounter); // x ekseni kaydrılır.
    } else {
        memoryXAxis->setRange(0, 10);
    }

    timeCounter++;
}

void windowserver::onCPUDataPartReceived(QByteArray dataPart)
{
    int cpuUsagePercentage = dataPart.toInt();
    ui->barserver_2->setValue(cpuUsagePercentage);
    cpuSeries->append(timeCounter, cpuUsagePercentage);

    if (timeCounter > 10) {
        cpuXAxis->setRange(timeCounter - 10, timeCounter);
    } else {
        cpuXAxis->setRange(0, 10);
    }

    timeCounter++;
}

void windowserver::setupMainWindowLayout()
{
    QWidget *central = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    mainLayout->addWidget(ui->frame);
    mainLayout->addWidget(ui->frame_2);


    central->setLayout(mainLayout);
    this->setCentralWidget(central);
}


void windowserver::setupLayouts()
{

    // Frame'ler için layout'ları başlatma
    QVBoxLayout *frame1Layout = new QVBoxLayout(ui->frame);
    QVBoxLayout *frame2Layout = new QVBoxLayout(ui->frame_2);

    // Grafikleri frame'lere ekleme
    frame1Layout->addWidget(memoryChartView);
    frame2Layout->addWidget(cpuChartView);

    frame1Layout->addWidget(ui->label);
    frame2Layout->addWidget(ui->label_2);

    // Barları frame'lere ekleme
    frame1Layout->addWidget(ui->barserver);
    frame2Layout->addWidget(ui->barserver_2);



    // Frame layout'larını başlatma
    ui->frame->setLayout(frame1Layout);
    ui->frame_2->setLayout(frame2Layout);
}

