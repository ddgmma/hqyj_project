#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jiance.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_jianceWin(nullptr)
{
    ui->setupUi(this);

    // ========== 1. 初始化所有硬件设备 ==========
    m_leds     = new fsmpLeds(this);
    m_fan      = new fsmpFan(this);
    m_beeper   = new fsmpBeeper("/dev/input/event0", this);
    m_vibrator = new fsmpVibrator(this);
    m_tempHum  = new fsmpTempHum(this);
    m_light    = new fsmpLight(this);

    // 设置硬件默认参数
    m_fan->setSpeed(200);          // 风扇默认转速 0-255
    m_beeper->setRate(1000);       // 蜂鸣器默认频率 1000Hz
    m_vibrator->setParameter(0xFFFF, 60000); // 震动马达：最大强度，持续60秒

    // ========== 2. 初始化定时器，1秒刷新一次界面数据 ==========
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimerTimeout);
    m_timer->start(1000);
}

MainWindow::~MainWindow()
{
    // 停止定时器
    m_timer->stop();

    // 关闭所有硬件
    m_leds->off(fsmpLeds::LED1);
    m_leds->off(fsmpLeds::LED2);
    m_leds->off(fsmpLeds::LED3);
    m_fan->stop();
    m_beeper->stop();
    m_vibrator->stop();

    // 安全释放自动检测窗口
    if(m_jianceWin != nullptr)
    {
        delete m_jianceWin;
        m_jianceWin = nullptr;
    }

    delete ui;
}

// ===================== 手动控制：LED1 =====================
void MainWindow::on_btnLed1_clicked()
{
    if (m_led1On) {
        m_leds->off(fsmpLeds::LED1);
        ui->btnLed1->setText("打开 LED1");
        m_led1On = false;
    } else {
        m_leds->on(fsmpLeds::LED1);
        ui->btnLed1->setText("关闭 LED1");
        m_led1On = true;
    }
}

// ===================== 手动控制：LED2 =====================
void MainWindow::on_btnLed2_clicked()
{
    if (m_led2On) {
        m_leds->off(fsmpLeds::LED2);
        ui->btnLed2->setText("打开 LED2");
        m_led2On = false;
    } else {
        m_leds->on(fsmpLeds::LED2);
        ui->btnLed2->setText("关闭 LED2");
        m_led2On = true;
    }
}

// ===================== 手动控制：LED3 =====================
void MainWindow::on_btnLed3_clicked()
{
    if (m_led3On) {
        m_leds->off(fsmpLeds::LED3);
        ui->btnLed3->setText("打开 LED3");
        m_led3On = false;
    } else {
        m_leds->on(fsmpLeds::LED3);
        ui->btnLed3->setText("关闭 LED3");
        m_led3On = true;
    }
}

// ===================== 手动控制：风扇 =====================
void MainWindow::on_btnFan_clicked()
{
    if (m_fanOn) {
        m_fan->stop();
        ui->btnFan->setText("打开风扇");
        m_fanOn = false;
    } else {
        m_fan->start();
        ui->btnFan->setText("关闭风扇");
        m_fanOn = true;
    }
}

// ===================== 手动控制：蜂鸣器 =====================
void MainWindow::on_btnBeeper_clicked()
{
    if (m_beeperOn) {
        m_beeper->stop();
        ui->btnBeeper->setText("打开蜂鸣器");
        m_beeperOn = false;
    } else {
        m_beeper->start();
        ui->btnBeeper->setText("关闭蜂鸣器");
        m_beeperOn = true;
    }
}

// ===================== 手动控制：震动马达 =====================
void MainWindow::on_btnVibrator_clicked()
{
    if (m_vibratorOn) {
        m_vibrator->stop();
        ui->btnVibrator->setText("打开震动马达");
        m_vibratorOn = false;
    } else {
        m_vibrator->start();
        ui->btnVibrator->setText("关闭震动马达");
        m_vibratorOn = true;
    }
}

// ===================== 跳转自动检测界面 =====================
void MainWindow::on_btnAutoDetect_clicked()
{
    // 窗口未创建则新建，已创建则直接显示
    if(m_jianceWin == nullptr)
    {
        m_jianceWin = new jiance();
        // 将当前主窗口指针传给自动检测窗口，用于切回
        m_jianceWin->setMainWindow(this);
        // 窗口关闭时自动置空指针，避免野指针
        connect(m_jianceWin, &jiance::destroyed, this, [=](){
            m_jianceWin = nullptr;
        });
    }

    // 显示自动检测窗口并置顶
    m_jianceWin->show();
    m_jianceWin->raise();

    // 隐藏当前手动控制主窗口
    this->hide();
}

// ===================== 定时器：仅刷新界面数据，无自动控制 =====================
void MainWindow::onTimerTimeout()
{
    // 1. 读取环境数据
    double temperature = m_tempHum->temperature();
    double lightValue  = m_light->getValue();

}
