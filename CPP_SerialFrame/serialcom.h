// Serial communication
// Author: Yan Naing Aye
// Reference: https://en.wikibooks.org/wiki/Serial_Programming/termios
#ifndef SERIALCOM_H
#define SERIALCOM_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>//read() write() close()
#include <fcntl.h>
#include <termios.h>//the terminal I/O interfaces
#include <string.h> // needed for memset
#include <sys/ioctl.h> //ioctl() call defenitions

#define S_BAUD B115200
#define S_PORT "/dev/ttyUSB0"
//#define S_PORT "/dev/ttyS0"
#define TX_BUF_SIZE 128
#define RX_BUF_SIZE 128

class SerialCom {
    int fd;//serial_fd
    char tb[TX_BUF_SIZE];//transmit buffer
    char rb[RX_BUF_SIZE];//receive buffer
public:
    SerialCom();
    int Open(void);
    void Close();
    int Read(char* c);
    void Write(char *b,int n);
    int SetRTS(int d);
    int GetCTS();
};
//-----------------------------------------------------------------------------
SerialCom::SerialCom(): tb("Test\a\n"),rb(""){

}
//-----------------------------------------------------------------------------
int SerialCom::Open(void){

    struct termios settings;
    memset(&settings,0,sizeof(settings));
    settings.c_iflag=0;
    settings.c_oflag=0;
    settings.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    settings.c_lflag=0;
    settings.c_cc[VMIN]=1;
    settings.c_cc[VTIME]=0;


    printf("Opening serial port...");
    fd=open(S_PORT, O_RDWR | O_NONBLOCK);
    if(fd == -1) { printf(" ...failed.\n"); return 0; }
    else printf(" ...success.\n");
    cfsetospeed(&settings,B115200);            // 115200 baud
    cfsetispeed(&settings,B115200);            // 115200 baud

    tcsetattr(fd,TCSANOW,&settings);
    return 1;
}
//-----------------------------------------------------------------------------
void SerialCom::Close(){
    close(fd);
    printf("Serial is closed.\n");
}
//------------------------------------------------------------------------------
int SerialCom::Read(char* c){
    return read(fd,c,1); // if new data is available on the serial port, print it out
}
//------------------------------------------------------------------------------
void SerialCom::Write(char *b,int n){
    write(fd,b,n);
}
//------------------------------------------------------------------------------
int SerialCom::SetRTS(int d){
   int RTS_flag;
   RTS_flag = TIOCM_RTS;
   if(d){
       if(ioctl(fd,TIOCMBIS,&RTS_flag)==-1) return 0;//Set RTS pin
   }
   else{
       if(ioctl(fd,TIOCMBIC,&RTS_flag)==-1) return 0;//Clear RTS pin
   }
   return 1;
}
//------------------------------------------------------------------------------
int SerialCom::GetCTS(){
   int sflag;
   ioctl(fd,TIOCMGET, & sflag );
   return ((sflag & TIOCM_CTS)!=0);
}
//------------------------------------------------------------------------------
#endif // SERIALCOM_H
