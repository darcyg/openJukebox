#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/signal.h>
#include <string.h> // needed for memset

//MPD relevant includes
//#include <mpd/client.h>
//#include <mpd/connection.h>

 
void signal_handler_IO (int status);   /* definition of signal handler */

        struct termios tio;
        struct termios stdio;
	struct sigaction saio;
        int tty_fd, connected;
        fd_set rdset;
 	char compareString[] = "";
	
        unsigned char c='D';
int main(int argc,char** argv)
{
	

	////Establishing the mpd connection
	//struct mpd_connection *conn;
	//conn = mpd_connection_new(NULL, 0, 30000);
	//if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
	//	fprintf(stderr,"%s\n", mpd_connection_get_error_message(conn));
	//	mpd_connection_free(conn);
	//	return -1;
	//}
 
        //printf("Please start with %s /dev/ttyS1 (for example)\n",argv[0]);
        memset(&stdio,0,sizeof(stdio));
        stdio.c_iflag=0;
        stdio.c_oflag=0;
        stdio.c_cflag=0;
        stdio.c_lflag=0;
        stdio.c_cc[VMIN]=1;
        stdio.c_cc[VTIME]=0;
        tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
        tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking
 
 
 
 
        memset(&tio,0,sizeof(tio));
        tio.c_iflag=0;
        tio.c_oflag=0;
        tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
        tio.c_lflag=0;
        tio.c_cc[VMIN]=1;
        tio.c_cc[VTIME]=5;
 
        tty_fd=open(argv[1], O_RDWR | O_NONBLOCK);      

	saio.sa_handler = signal_handler_IO;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL; 
	sigaction(SIGIO,&saio,NULL);

     fcntl(tty_fd, F_SETFL, FNDELAY);
     fcntl(tty_fd, F_SETOWN, getpid());
     fcntl(tty_fd, F_SETFL,  O_ASYNC );


        cfsetospeed(&tio,B9600);            // 115200 baud
        cfsetispeed(&tio,B9600);            // 115200 baud
 
        tcsetattr(tty_fd,TCSANOW,&tio);
	//printf("UART configured....\n");
	connected =1;
        while (connected == 1)
        {
                if (read(tty_fd,&c,1)>0)     {//   write(STDOUT_FILENO,&c,1);              // if new data is available on the serial port, print it out
			//write(STDOUT_FILENO,&c,1);


			if(c=='1') {
				system("mpc volume -2");//printf("v-\n");
			}
			else if(c=='2') {
				system("mpc volume +1"); //("v+\n");
			}
			else if(c=='3') {
				//mpd_send_toggle_pause(conn);
				//system("mpc toggle");
			}
			else if(c=='4') {
				system("mpc prev");
			}
			else if(c=='5') {
				system("mpc next");
			}
			else if(c=='6') {
				system("mpc toggle");
			}
		}
	//sleep(0.5);
	
        }
	//mpd_connection_free(conn); 
        close(tty_fd);
}

void signal_handler_IO (int status)
{
    // printf("received data from UART.\n");
}
