#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

// 硬件驱动头文件
#include "fsmpLed.h"
#include "fsmpFan.h"
#include "fsmpBeeper.h"
#include "fsmpVibrator.h"
#include "fsmpTempHum.h"
#include "fsmpLight.h"

// 前置声明自动检测窗口，减少头文件依赖
class jiance;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 手动控制按钮槽函数
    void on_btnLed1_clicked();
    void on_btnLed2_clicked();
    void on_btnLed3_clicked();
    void on_btnFan_clicked();
    void on_btnBeeper_clicked();
    void on_btnVibrator_clicked();

    // 跳转自动检测界面槽函数
    void on_btnAutoDetect_clicked();

    // 定时器：环境采集 + 自动控制
    void onTimerTimeout();

private:
    Ui::MainWindow *ui;

    // 硬件设备对象指针
    fsmpLeds      *m_leds;
    fsmpFan       *m_fan;
    fsmpBeeper    *m_beeper;
    fsmpVibrator  *m_vibrator;
    fsmpTempHum   *m_tempHum;
    fsmpLight     *m_light;

    QTimer *m_timer;     // 周期采集定时器

    // 设备状态标记
    bool m_led1On     = false;
    bool m_led2On     = false;
    bool m_led3On     = false;
    bool m_fanOn      = false;
    bool m_beeperOn   = false;
    bool m_vibratorOn = false;

    // 高温报警阈值
    const double TEMP_ALARM_THRESHOLD = 35.0;

    // 自动检测窗口指针
    jiance *m_jianceWin;
};

#endif // MAINWINDOW_H
