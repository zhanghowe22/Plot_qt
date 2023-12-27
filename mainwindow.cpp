#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    // 创建按钮
    fileButton = new QPushButton("Open File", this);

    // 创建绘图区域
    customPlot = new QCustomPlot(this);

    // 连接按钮的点击事件到槽函数
    connect(fileButton, &QPushButton::clicked, this, &MainWindow::openBinaryFileAndPlot);

    // 使用布局管理器将按钮和绘图区域添加到主窗口
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(fileButton);
    layout->addWidget(customPlot);

    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // 获取主屏幕分辨率
    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    QRect screenGeometry = primaryScreen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    // 设置主界面的默认大小为屏幕宽度的一半，高度的一半
    this->resize(screenWidth / 2, screenHeight / 2);

    //    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

// 解析数据
void readDataPacket(QDataStream& in, QVector<double>& xData, QVector<double>& yData) {
    // 跳过第一个字节
    in.skipRawData(2);

    // 读取4个字节作为X轴（时间）
    qint32 xValue;
    in >> xValue;

    // 跳过后续90个字节
    in.skipRawData(86);

    // 读取2个字节作为Y轴（数值）
    qint16 yValue;
    in >> yValue;

    in.skipRawData(778 - 94);

    // 添加数据到绘图数据向量
    xData.append(xValue);
    yData.append(yValue);
}

// 打开文件、解析、绘图
void MainWindow::openBinaryFileAndPlot() {
    // Step 1: 弹窗选择二进制文件
    QString filePath = QFileDialog::getOpenFileName(this, "Choose binary file", QString(),
                                                    "REC Files (*.dat);;VIB Files (*.vibdat);;All Files (*)");

    if (filePath.isEmpty()) {
        qDebug() << "User canceled the operation.";
        return;
    }

    // Step 2: 读取二进制文件中的多包数据
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Open file error. " << file.errorString();
        return;
    }

    // Step 3: 解析数据并准备绘制曲线图的数据
    QVector<double> xData, yData;
    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);

    // 从文件的开头开始读取数据
    while (!in.atEnd()) {
        readDataPacket(in, xData, yData);
    }

    file.close();

    // Step 4: 使用QCustomPlot绘制曲线图
    customPlot->addGraph();
    customPlot->graph(0)->setData(xData, yData);

    // 获取主坐标轴
    QCPAxisRect* mainAxisRect = customPlot->axisRect();

    // 启用局部放大和拖动
    mainAxisRect->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    mainAxisRect->setRangeDrag(Qt::Horizontal | Qt::Vertical);

    customPlot->xAxis->setLabel("Time");
    customPlot->yAxis->setLabel("Value");
    customPlot->rescaleAxes();

    // 可缩放
    customPlot->setInteraction(QCP::iRangeZoom, true);

    // 可拖动
    customPlot->setInteraction(QCP::iRangeDrag, true);

    // 曲线可选
    customPlot->setInteraction(QCP::iSelectPlottables, true);

    customPlot->replot();
}
