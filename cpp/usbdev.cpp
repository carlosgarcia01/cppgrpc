#include "usbdev.h"

//INFO: Version 98 of subversion includes the code to handle the IMU
void usbdev::setDeviceName(const QString &deviceName){
	this->deviceName = deviceName;
}

void usbdev::setSpeed(const int speed){
    this->speed = speed;

}


bool usbdev::openDevice(){
    qDebug()<<Q_FUNC_INFO<<" deviceName: "<<deviceName;
    fd=open(deviceName.toLatin1().data(), O_RDWR);
    if(fd<0){
        qDebug("Opening %s ERROR(%d): %s ", deviceName.toLatin1().data(), errno, strerror(errno));
        emit deviceError(errno);
        return false;
    }
    Configure(speed,0);
    SetBlocking(0);
    emit deviceReady();
    qDebug("Device %s is configured", deviceName.toLatin1().data());
    return true;
}

//TODO: This method it is not used
void usbdev::closeDevice(){
    //qDebug("%s fd: %d", __PRETTY_FUNCTION__, fd);
    if(close(fd) < 0){
        qDebug("Closing %s ERROR(%d): %s ", deviceName.toLatin1().data(), errno, strerror(errno));
    }
}

void usbdev::writeData(const QString data){
    qDebug("%s data: %s, lenght: %d", __PRETTY_FUNCTION__, data.toLatin1().data(), strlen (data.toLatin1().data()));
    char dataWith[8];
    sprintf(dataWith, "%s\n", data.toLatin1().data());
    //int res = write(fd, data.toLatin1().data(),strlen (data.toLatin1().data()));
    int res = write(fd, dataWith,strlen (dataWith));
    if(res < 0){
      //qDebug("%s %s data: %s", __PRETTY_FUNCTION__, strerror(errno), data.toLatin1().data());
      return;
    }else{
      //qDebug("%s %d bytes written to device %s", __PRETTY_FUNCTION__, res, deviceName.toLatin1().data());
    }
}


usbdev::~usbdev(){}

int usbdev::Configure(int speed, int parity){
    struct termios tty;
    memset (&tty, 0, sizeof(tty));
    if(tcgetattr(fd,&tty)!=0){
        qDebug("error: %d from tcgetattr", errno);
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag=(tty.c_cflag & ~CSIZE) | CS8;

    tty.c_iflag &= ~IGNBRK; //disable break processing
    tty.c_iflag &= HUPCL; //Si se pone esta linea el arduino se resetea al arrancar el programa
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); //shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if(tcsetattr(fd, TCSANOW, &tty) != 0){
        qDebug("error: %d from tcgetattr", errno);
        return -1;
    }
    sleep(1);
    tcsetattr(fd, TCSAFLUSH, &tty);
    return 0;
}

void usbdev::SetBlocking (int shouldBlock){
    struct termios tty;
    memset (&tty, 0, sizeof(tty));
    if(tcgetattr(fd, &tty)!=0){
        qDebug("error: %d from tggetattr", errno);
        return;
    }

    tty.c_cc[VMIN] = shouldBlock ? 1 : 0;
    tty.c_cc[VTIME] = 10;//n*100milisegundos que bloquea la llamada a read 5;

    if(tcsetattr(fd,TCSANOW, &tty)!=0){
        qDebug("error: %d setting term attributes", errno);
    }
}




