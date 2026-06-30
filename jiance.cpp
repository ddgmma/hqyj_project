#include "jiance.h"
#include "ui_jiance.h"

// 硬件类头文件
#include "fsmpLed.h"
#include "fsmpFan.h"
#include "fsmpBeeper.h"
#include "fsmpVibrator.h"
#include "fsmpTempHum.h"
#include "fsmpLight.h"

// 主窗口头文件
#include "mainwindow.h"

// ========== 自动控制阈值 ==========
#define LIGHT_THRESHOLD_1    70   // 光照≥70全关，低于此值开1盏灯
#define LIGHT_THRESHOLD_2    61   // 低于此值开2盏灯（对应41~60区间）
#define LIGHT_THRESHOLD_3    41   // 低于此值3盏灯全开（对应≤40区间）

#define FAN_TEMP_THRESHOLD   30.0
#define BEEPER_TEMP_THRESHOLD 35.0
#define HUMIDITY_LOW_THRESHOLD 50.0

// 构造函数
jiance::jiance(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::jiance)
    , m_mainWin(nullptr)
{
    ui->setupUi(this);

    // 硬件初始化
    m_leds     = new fsmpLeds(this);
    m_fan      = new fsmpFan(this);
    m_beeper   = new fsmpBeeper("/dev/input/event0", this);
    m_vibrator = new fsmpVibrator(this);
    m_tempHum  = new fsmpTempHum(this);
    m_light    = new fsmpLight(this);

    // 硬件默认参数
    m_fan->setSpeed(200);
    m_beeper->setRate(1000);
    m_vibrator->setParameter(0xFFFF, 60000);

    // 初始化定时器（不自动启动，窗口显示时启动）
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &jiance::onTimerTimeout);

    // 绑定切换按钮
    connect(ui->pushButton_change, &QPushButton::clicked, this, &jiance::on_pushButton_change_clicked);
}

// 析构函数
jiance::~jiance()
{
    // 停止定时器
    stopAutoDetect();

    // 关闭所有硬件
    m_leds->off(fsmpLeds::LED1);
    m_leds->off(fsmpLeds::LED2);
    m_leds->off(fsmpLeds::LED3);
    m_fan->stop();
    m_beeper->stop();
    m_vibrator->stop();

    // 释放硬件资源
    delete m_leds;
    delete m_fan;
    delete m_beeper;
    delete m_vibrator;
    delete m_tempHum;
    delete m_light;

    delete ui;
}

// 设置主窗口指针
void jiance::setMainWindow(MainWindow *win)
{
    m_mainWin = win;
}

// 启动自动检测
void jiance::startAutoDetect()
{
    if(!m_timer->isActive()) {
        m_timer->start(1000);
    }
}

// 停止自动检测
void jiance::stopAutoDetect()
{
    if(m_timer->isActive()) {
        m_timer->stop();
    }
}

// 窗口隐藏事件：自动停止检测
void jiance::hideEvent(QHideEvent *event)
{
    stopAutoDetect();
    QWidget::hideEvent(event);
}

// 窗口显示事件：自动恢复检测
void jiance::showEvent(QShowEvent *event)
{
    startAutoDetect();
    QWidget::showEvent(event);
}

// ===================== 切回手动控制主窗口 =====================
void jiance::on_pushButton_change_clicked()
{
    // 停止自动检测功能
    stopAutoDetect();

    // 主窗口有效则显示并置顶
    if(m_mainWin != nullptr)
    {
        m_mainWin->show();
        m_mainWin->raise();
    }

    // 隐藏当前自动检测窗口
    this->hide();
}

// ===================== 核心自动检测逻辑（定时执行） =====================
void jiance::onTimerTimeout()
{
    // 1. 读取传感器数据
    double temp = m_tempHum->temperature();
    double humi = m_tempHum->humidity();
    int lightVal = m_light->getValue();

    // 2. 更新界面显示
    ui->labelHum->display(humi);
    ui->labelLight->display(lightVal);
    ui->labelTemp->display(temp);

    // 3. 光照分级控制LED
    if (lightVal < LIGHT_THRESHOLD_3) {
        // ≤40：全开
        m_leds->on(fsmpLeds::LED1);
        m_leds->on(fsmpLeds::LED2);
        m_leds->on(fsmpLeds::LED3);
    } else if (lightVal < LIGHT_THRESHOLD_2) {
        // 41~60：开2盏
        m_leds->on(fsmpLeds::LED1);
        m_leds->on(fsmpLeds::LED2);
        m_leds->off(fsmpLeds::LED3);
    } else if (lightVal < LIGHT_THRESHOLD_1) {
        // 61~69：开1盏
        m_leds->on(fsmpLeds::LED1);
        m_leds->off(fsmpLeds::LED2);
        m_leds->off(fsmpLeds::LED3);
    } else {
        // ≥70：全关
        m_leds->off(fsmpLeds::LED1);
        m_leds->off(fsmpLeds::LED2);
        m_leds->off(fsmpLeds::LED3);
    }

    // 4. 温度控制风扇：高于阈值开启散热
    if (temp > FAN_TEMP_THRESHOLD) {
        m_fan->start();
    } else {
        m_fan->stop();
    }

    // 5. 温湿度控制蜂鸣器：高温或低湿度触发报警
    if (temp > BEEPER_TEMP_THRESHOLD || humi < HUMIDITY_LOW_THRESHOLD) {
        m_beeper->start();
    } else {
        m_beeper->stop();
    }
}
