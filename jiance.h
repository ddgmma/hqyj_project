#ifndef JIANCE_H
#define JIANCE_H

#include <QWidget>
#include <QTimer>
#include <QHideEvent>
#include <QShowEvent>

// 前置声明，减少头文件依赖
class MainWindow;
class fsmpLeds;
class fsmpFan;
class fsmpBeeper;
class fsmpVibrator;
class fsmpTempHum;
class fsmpLight;

namespace Ui {
class jiance;
}

class jiance : public QWidget
{
    Q_OBJECT

public:
    explicit jiance(QWidget *parent = nullptr);
    ~jiance();

    // 设置主窗口指针，用于切回手动控制界面
    void setMainWindow(MainWindow *win);
    // 启停自动检测功能
    void startAutoDetect();
    void stopAutoDetect();

private slots:
    void onTimerTimeout();               // 定时自动检测
    void on_pushButton_change_clicked(); // 切换到手动控制主窗口

protected:
    // 重写窗口事件：隐藏时自动停止检测，显示时自动恢复
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::jiance *ui;
    QTimer *m_timer;
    MainWindow *m_mainWin; // 手动控制主窗口指针

    // 硬件设备指针
    fsmpLeds *m_leds;
    fsmpFan *m_fan;
    fsmpBeeper *m_beeper;
    fsmpVibrator *m_vibrator;
    fsmpTempHum *m_tempHum;
    fsmpLight *m_light;
};

#endif // JIANCE_H
