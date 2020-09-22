#ifndef MOTIONCONTROL_H
#define MOTIONCONTROL_H

#include <QMainWindow>
#include <QtSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MotionControl; }
QT_END_NAMESPACE

class MotionControl : public QMainWindow
{
    Q_OBJECT

public:
    MotionControl(QWidget *parent = nullptr);
    ~MotionControl();



private slots:
    void on_connectButton_clicked();
    void receiveInfo();

    void on_moveButton_clicked();

    void on_speedSlider_sliderMoved(int position);

    void on_pushButton_clicked();

    void on_eraseButton_clicked();

private:
    Ui::MotionControl *ui;
    QSerialPort *m_serialPort;


};
#endif // MOTIONCONTROL_H
