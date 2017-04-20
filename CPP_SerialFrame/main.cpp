#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include"frame.h"
#include"serialcom.h"

using namespace std;

Frame2 fra;
SerialCom com1;
int eFlag=0;
void *t1_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);

     //test preparing tramsmitting frame
     fra.setTxData(4,6,8,0.9,0.3);
     fra.printTxFrame();
     //printf("TxN %i \n", fra.getTxN());
     printf("\n\n");
     char *mesg1 = fra.getTxBuf();
     com1.Write(mesg1,fra.getTxN());

     for(int i=0;i<5;i++){
         com1.SetRTS(i & 1);
         if(com1.GetCTS()) printf("CTS=1\n");
         else printf("CTS=0\n");
         sleep(1);
     }

     getchar();
     eFlag=1;

}
void *t2_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);

     char c;
     int d;
     while (eFlag!=1)
     {
            if(com1.Read(&c)>0) {
                d=(int)c & 0xFF;
                printf("%02x ",d);
                if(fra.receiveRxFrame(c)>1){
                    fra.printRxData();
                }
            }
     }
}

int main(int argc, char *argv[])
{
    int j;
    //The followings are some external tools to check
    //https://www.lammertbies.nl/comm/info/crc-calculation.html
    //https://gregstoll.dyndns.org/~gregstoll/floattohex/

    /*
    //test extracting receiving frame
    //unsigned char d[]={0x02,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x40,0xCD,0xCC,0xCC,0x3D,0xCD,0xCC,0x4C,0x3E,0x03,0x8D,0x16};
    unsigned char d[]={0x02,0x00,0x00,0x10,0x03,0x3F,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x40,0xCD,0xCC,0xCC,0x3D,0xCD,0xCC,0x4C,0x10,0x10,0x03,0x4C,0x34};
    */

    //testing serial port
    //printf("Opening serial port...");
    com1.Open();

    printf("\n\n");
    //create threads
    pthread_t thread1, thread2;
    char *message1 = "Thread 1";
    char *message2 = "Thread 2";
    int  iret1, iret2;

    iret1 = pthread_create( &thread1, NULL, t1_function, (void*) message1);
    iret2 = pthread_create( &thread2, NULL, t2_function, (void*) message2);

    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);

    printf("Thread 1 returns: %d\n",iret1);
    printf("Thread 2 returns: %d\n",iret2);
    com1.Close();
    return 0;
}

