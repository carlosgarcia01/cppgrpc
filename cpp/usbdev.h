#ifndef USBDEV_H
#define USBDEV_H

#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <QThread>
#include <QStringList>
#include <QString>
#include <QDebug>



//NOTE: Create a file 70-imu-device.rules in /etc/dev/dules.d with this line:
//#SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", NAME="ttyUSB0", MODE="0666", OWNER="eritski"
//
//SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", SYMLINK+="ttyIMU0", MODE="0666", OWNER="eritski"



class usbdev  : public QObject{
    Q_OBJECT
private:
    QString data;
    int fd;
    QString deviceName;
    int speed;
    int Configure(int speed, int parity);
    void SetBlocking (int shouldBlock);
    bool oscannLightReady;
public:
    Q_INVOKABLE explicit usbdev(QObject *parent = 0)  : QObject(parent), deviceName("/dev/ttyOSCANN"), speed(57600){}
    ~usbdev();
    Q_INVOKABLE void setDeviceName(const QString &f);
    Q_INVOKABLE void setSpeed(const int speed);
    Q_INVOKABLE bool getOscannLightReady(){return oscannLightReady;}
    Q_INVOKABLE bool setOscannLightReady(const bool value){oscannLightReady = value;}

public slots:
    void writeData(const QString data);
    bool openDevice();
    void closeDevice();

signals:
    void deviceReady();
    void deviceError(const int errId);
};

#endif // USBDEV_H
