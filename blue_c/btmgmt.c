#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>   
#include <unistd.h>   
#include <signal.h>   
#include <pthread.h>   
#include <sys/socket.h>   
#include <bluetooth/bluetooth.h>   
#include <bluetooth/rfcomm.h>   
#include <fcntl.h>
#include <ctype.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <getopt.h>
//#include <glib.h>

int s,client ;   
void ctrl_c_handler(int signal);   
void close_sockets();   
void *readMsg();   
void *sendMsg();   

int main(int argc,char **argv){   
    (void) signal(SIGINT,ctrl_c_handler);   

    pthread_t readT, writeT;   
    char *message1 = "Read thread\n";   
    char *message2 = "Write thread\n";   
    int iret1, iret2;   

    struct sockaddr_rc loc_addr={ 0 },client_addr={ 0 };   
    char buf[18] = { 0 };   

    unsigned int opt = sizeof(client_addr) ;   

test:
    //allocate socket   
    s = socket(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM) ;   
    //  
    //    
    //    //bind socket to port 1 of the first available   
    loc_addr.rc_family = AF_BLUETOOTH ;   
    str2ba("B8:27:EB:29:42:F3",&loc_addr.rc_bdaddr) ;//hci0; server device address is given   
    loc_addr.rc_channel = 1 ; //port (maximum should be 30 for RFCOMM)   

    bind(s,(struct sockaddr *)&loc_addr,sizeof(loc_addr));   
    printf("Binding success\n");   

            //put socket into listen mode   
    listen(s,1) ;   
    printf("socket in listen mode\n");   
           //accept one connection   

    client = accept(s,(struct sockaddr *)&client_addr,&opt);   
    ba2str(&client_addr.rc_bdaddr,buf);   
    fprintf(stdout,"Connection accepted from %s\n",buf);   

    char *dev_addr = "7C:2A:DB:88:76:6C"; // Replace with your Bluetooth address
    int dev_id = hci_devid(dev_addr);

    int dd = hci_open_dev(dev_id); // dev_id is up to you to get.
    int8_t rssi = 0;

    if (dd < 0) {
            printf("HCI device open failed");
    } else {

        if (hci_read_rssi(dd, client, &rssi, 1000) < 0) {
            printf("Read RSSI failed");
        } else {
            printf("RSSI return value: %d\n", rssi);
        }

    }

                /* Create independent threads each of which will execute function */   

    iret1 = pthread_create(&readT,NULL,readMsg,(void*) message1);   
    //iret2 = pthread_create(&writeT,NULL,sendMsg,(void*) message2);   

    pthread_join(readT,NULL);   
    //pthread_join(writeT,NULL);   

    close_sockets();
    goto test;
    return 0 ;   
    }   
    //                    
    void *sendMsg(){   
         char msg[25] ;   
         int status ;   

         do{   
            memset(msg,0,sizeof(msg));   
            fgets(msg,24,stdin);   
            if(strncmp("EXIT",msg,4)==0 || strncmp("exit",msg,4)==0)break;   
            status = send(client,msg,strlen(msg),0);   
            fprintf(stdout,"Status = %d\n",status);   
            }while(status > 0);   
         }   

    void *readMsg(){   
            int bytes_read;   
            char buf[1024] = { 0 };   
            do{   
               memset(buf,0,sizeof(buf));   
               //read data from the client   
               bytes_read = recv(client,buf,sizeof(buf),0) ;   
               //fprintf(stdout,"Bytes read = %d\n",bytes_read);   
               if(bytes_read <= 0)break;   
               fprintf(stdout,"Received Value = %s",buf);   
               }while(1);   
             }   

     void close_sockets(){   
                     //close connection   
               close(client);   
               close(s) ;   
               printf("sockets closed\n");   
              }   

     void ctrl_c_handler(int signal) {   
               printf("Catched signal: %d ... !!\n", signal);   
               close_sockets();   
               exit(0);   
               //(void) signal(SIGINT, SIG_DFL);   
               }  