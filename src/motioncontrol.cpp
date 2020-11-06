#include "motioncontrol.h"
#include "ui_motioncontrol.h"

#include <QSerialPort>

const uint16_t crctalbeabs[] = {0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
                                0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400};

uint16_t crc16tablefast(QByteArray ptr, uint16_t len);

MotionControl::MotionControl(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MotionControl)
{
    ui->setupUi(this);
    m_serialPort = new QSerialPort();//实例化串口类一个对象
}

MotionControl::~MotionControl()
{
    delete ui;
    if (m_serialPort->isOpen())
        {
            m_serialPort->close();
        }
        delete m_serialPort;
}


void MotionControl::on_connectButton_clicked()
{
    if(m_serialPort->isOpen())//如果串口已经打开了 先给他关闭了
    {
        m_serialPort->clear();
        m_serialPort->close();
    }

    //设置串口名字 假设我们上面已经成功获取到了 并且使用第一个
    m_serialPort->setPortName(ui->portEdit->text());

    if(!m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
    {
        ui->statusbar->showMessage("Open Failed!");
        //qDebug()<<m_serialPortName[0]<<"打开失败!";
        return;
    }
    //打开成功

        m_serialPort->setBaudRate(QSerialPort::Baud57600,QSerialPort::AllDirections);//设置波特率和读写方向
        m_serialPort->setDataBits(QSerialPort::Data8);		//数据位为8位
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
        m_serialPort->setParity(QSerialPort::NoParity);	//无校验位
        m_serialPort->setStopBits(QSerialPort::OneStop); //一位停止位

        m_serialPort->waitForReadyRead(50);

        //连接信号槽 当下位机发送数据QSerialPortInfo 会发送个 readyRead 信号,我们定义个槽void receiveInfo()解析数据
        connect(m_serialPort,SIGNAL(readyRead()),this,SLOT(receiveInfo()));
}

//接收单片机的数据
    void MotionControl::receiveInfo()
    {
        QByteArray info = m_serialPort->readAll();
        ui->recvEdit->moveCursor(QTextCursor::End);
        ui->recvEdit->insertPlainText(info);
        if(info.at(2) == 0)
            ui->curSpeedLabel->setText(QString::number(info.at(3) << 8 | info.at(4)));


    }
    //向单片机发送数据


void MotionControl::on_moveButton_clicked()
{
//    QByteArray xfer;
//    int32_t x_cord = ui->xLineEdit->text().toFloat() * 1000;
//    int32_t y_cord = ui->yLineEdit->text().toFloat() * 1000;
//    int32_t z_cord = ui->zLineEdit->text().toFloat() * 1000;
//    uint16_t speed = ui->speedLineEdit->text().toUShort();
//    uint16_t crc16_val = 0;

//    xfer.resize(19);
//    xfer[0] = 0xf0;
//    xfer[1] = 0x5a;
//    xfer[2] = 'M';
//    xfer[3] = x_cord >> 24;
//    xfer[4] = x_cord >> 16;
//    xfer[5] = x_cord >> 8;
//    xfer[6] = x_cord;
//    xfer[7] = y_cord >> 24;
//    xfer[8] = y_cord >> 16;
//    xfer[9] = y_cord >> 8;
//    xfer[10] = y_cord;
//    xfer[11] = z_cord >> 24;
//    xfer[12] = z_cord >> 16;
//    xfer[13] = z_cord >> 8;
//    xfer[14] = z_cord;
//    xfer[15] = speed >> 8;
//    xfer[16] = speed;

//    crc16_val = crc16tablefast(xfer, 15);

//    ui->CRCHighLabel->setText(QString::number((quint8)(crc16_val >> 8), 16));
//    ui->CRCLowLabel->setText(QString::number((quint8)crc16_val, 16));

//    xfer[17] = crc16_val >> 8;
//    xfer[18] = crc16_val;
//    xfer[19] = 0xa5;
//    xfer[20] = 0x0f;

//    m_serialPort->write(xfer);
}

void MotionControl::on_speedSlider_sliderMoved(int position)
{
    ui->speedLineEdit->setText(QString::number(position));
}

void MotionControl::on_pushButton_clicked()
{
    QByteArray xfer;
    int32_t xc_cord = ui->cirX0LineEdit->text().toFloat() * 1000;
    int32_t yc_cord = ui->cirY0LineEdit->text().toFloat() * 1000;
    int32_t x0_cord = ui->cirXCLineEdit->text().toFloat() * 1000;
    int32_t y0_cord = ui->cirYCLineEdit->text().toFloat() * 1000;

    xfer.resize(8);
    xfer[0] = 0xf0;
    xfer[1] = 0x5a;
    xfer[2] = 'V';
    xfer[3] = 'A';
    xfer[4] = 0;
    xfer[5] = 0x01;
    xfer[6] = 0xa5;
    xfer[7] = 0x0f;

    m_serialPort->write(xfer);

    xfer.resize(16);
    xfer[0] = 0xf0;
    xfer[1] = 0x5a;
    xfer[2] = 'V';
    xfer[3] = 'O';
    xfer[4] = 0x10;
    xfer[5] = 0x00;
    xfer[6] = 0x02;
    xfer[7] = 0x06;
    xfer[8] = xc_cord >> 16;
    xfer[9] = xc_cord >> 8;
    xfer[10] = xc_cord;
    xfer[11] = yc_cord >> 16;
    xfer[12] = yc_cord >> 8;
    xfer[13] = yc_cord;
    xfer[14] = 0xa5;
    xfer[15] = 0x0f;

    m_serialPort->write(xfer);

    xfer.resize(16);
    xfer[0] = 0xf0;
    xfer[1] = 0x5a;
    xfer[2] = 'V';
    xfer[3] = 'P';
    xfer[4] = 0x10;
    xfer[5] = 0x00;
    xfer[6] = 0x09;
    xfer[7] = 0x06;
    xfer[8] = x0_cord >> 16;
    xfer[9] = x0_cord >> 8;
    xfer[10] = x0_cord;
    xfer[11] = y0_cord >> 16;
    xfer[12] = y0_cord >> 8;
    xfer[13] = y0_cord;
    xfer[14] = 0xa5;
    xfer[15] = 0x0f;

    m_serialPort->write(xfer);

}

void MotionControl::on_eraseButton_clicked()
{
    QByteArray xfer;
    uint32_t size = ui->eraseLineEdit->text().toUInt();

    xfer.resize(10);
    xfer[0] = 0xf0;
    xfer[1] = 0x5a;
    xfer[2] = 'V';
    xfer[3] = 'S';
    xfer[4] = size >> 16;
    xfer[5] = size >> 8;
    xfer[6] = size;
    xfer[7] = 0xa5;
    xfer[8] = 0x0f;

    m_serialPort->write(xfer);
}

uint16_t crc16tablefast(QByteArray ptr, uint16_t len)
{
    uint16_t crc = 0xffff;
    uint16_t i;
    uint8_t ch;

    for (i = 0; i < len; i++)
    {
        ch = ptr[i + 2];
        crc = crctalbeabs[(ch ^ crc) & 15] ^ (crc >> 4);
        crc = crctalbeabs[((ch >> 4) ^ crc) & 15] ^ (crc >> 4);
    }

    return crc;
}

