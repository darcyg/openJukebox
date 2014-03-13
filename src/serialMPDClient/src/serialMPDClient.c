#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/signal.h>
#include <string.h> // needed for memset

//MPD relevant includes
#include <mpd/client.h>
//#include <mpd/status.h>
//#include <mpd/connection.h>


void signal_handler_IO (int status);   /* definition of signal handler */

        struct termios tio;
        struct termios stdio;
	struct sigaction saio;
        int tty_fd, connected;
        fd_set rdset;
 	char compareString[] = "";

        unsigned char c='D';

	struct mpd_connection * conn;
	struct mpd_status *status;
	enum mpd_state  state;
	struct mpd_entity * entity;
	struct mpd_song * song;
	int elapsed_time, total_time, kbit_rate, currentPositionInPlaylist, totalCountOfSongsInPlaylist, volume, song_id = 0;
	bool consume, random_mode, single;
	char transmitter[255], artist[100], title[100];
	
int main(int argc,char** argv)
{
	printf("serialMPDClient...\n");
	////Establishing the mpd connection
	conn = mpd_connection_new("localhost", 6600, 0);
	if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
		printf("error!!!\n");
		fprintf(stderr,"%s\n", mpd_connection_get_error_message(conn));
		mpd_connection_free(conn);
		return -1;
	} else {
	printf("MPD connection established\n");
	mpd_send_status(conn);
	status = mpd_recv_status(conn);
	
	printf("Volume:%i\n",mpd_status_get_volume(status));

	}

        printf("Please start with %s /dev/ttyS1 (for example)\n",argv[0]);
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
	printf("UART configured....\n");
	connected =1;
        while (connected == 1)
        {	
		//Format for serial line. Artist;Title;elapsedTime;totalTime;consume;random_mode;single;state;kbit_rate;currentPositionsInPlaylist;totalCountOfSongsInPlaylist;volume
		//each value which is noch changed since last transmit is not retransmittet
		//entity = mpd_entity_begin(conn)
		sleep(1);
		status = mpd_run_status(conn);
		if(status != NULL){
		strcpy(transmitter,"");


		if(mpd_status_get_song_id(status) != song_id)
		{
			song_id=mpd_status_get_song_id(status);
			song = mpd_run_current_song(conn);
			strncpy(artist,mpd_song_get_tag(song,MPD_TAG_ARTIST,0),99);
			strcat(artist, ";");
			strcat(transmitter,artist);

			strncpy(title,mpd_song_get_tag(song,MPD_TAG_TITLE,0),99);
			strcat(title,";");
			strcat(transmitter,title);
		} 
		else
		{
			strcat(transmitter,";;");
		}
		//ToDo Title


		if(mpd_status_get_elapsed_time(status) != elapsed_time)
		{
			elapsed_time = mpd_status_get_elapsed_time(status);
			char str[4];
			sprintf(str, "%d",elapsed_time);
			strcat(str,";");
			strcat(transmitter,str);
		} else {
			strcat(transmitter,";");
		}

		if(mpd_status_get_total_time(status) != total_time)
		{
			total_time = mpd_status_get_total_time(status);
			char str[4];
			sprintf(str,"%d",total_time);
			strcat(str,";");
			strcat(transmitter,str);
		} else {
			strcat(transmitter,";");
		}
			
		if(mpd_status_get_consume(status) != consume)
		{
			consume = mpd_status_get_consume(status);
			if(consume) 
			{
				strcat(transmitter,"1;"); 
			}
			else 
			{
				strcat(transmitter,"0;");
			}
		} else
			{
			strcat(transmitter,";");
			}

		if(mpd_status_get_random(status) != random_mode)
		{
			random_mode = mpd_status_get_random(status);
			if(random)
			{
				strcat(transmitter,"1;"); 
			}
			else 
			{
				strcat(transmitter,"0;");
			}
		} else
			{
			strcat(transmitter,";");
			}

		if(mpd_status_get_single(status) != single)
		{
			single = mpd_status_get_single(status);
			if(single)
			{
				strcat(transmitter,"1;");
			}
			else
			{
				strcat(transmitter,"0;");
			}
		} else
			{
			strcat(transmitter,";");
		}

		if(mpd_status_get_state(status) != state)
		{
			state=mpd_status_get_state(status);
			if(state == 0)  //unknown
			{ 
				strcat(transmitter,"0;");
			} else if (state == 1) //stop
			{
				strcat(transmitter,"1;");
			} else if(state == 2) //play
			{
				strcat(transmitter,"2;");
			} else if(state == 3) //pause
			{
				strcat(transmitter,"3;"); 
			}
		}
		else 
		{
			strcat(transmitter,";");
		}

		if(mpd_status_get_kbit_rate(status) != kbit_rate)
		{
			kbit_rate = mpd_status_get_kbit_rate(status);
			char str[10];
			sprintf(str, "%d",kbit_rate);
			strcat(str,";");
			strcat(transmitter,str);
		} else {
			strcat(transmitter,";");
		}

		if(mpd_status_get_song_pos(status) != currentPositionInPlaylist)
		{
			currentPositionInPlaylist = mpd_status_get_song_pos(status);
			char str[6];
			sprintf(str, "%d",currentPositionInPlaylist);
			strcat(str,";");
			strcat(transmitter,str);
		} else {
			strcat(transmitter,";");
		}

		if(mpd_status_get_queue_length(status) != totalCountOfSongsInPlaylist)
		{
			totalCountOfSongsInPlaylist = mpd_status_get_queue_length(status);
			char str[6];
			sprintf(str, "%d",totalCountOfSongsInPlaylist);
			strcat(str,";");
			strcat(transmitter,str);
		} else {
			strcat(transmitter,";");
		}

		if(mpd_status_get_volume(status) != volume)
		{
			volume = mpd_status_get_volume(status);
			char str[4];
			sprintf(str,"%d",volume);
			strcat(str,"\n");
			strcat(transmitter,str);
		} else {
			strcat(transmitter,"\n");
		}

		printf("%s",transmitter);
		for(int i = 0; i<strlen(transmitter);i++)
		{	
			//printf("writing character\n");
			char tmp = transmitter[i];
			write(tty_fd, &tmp, 1);
		}
		write(tty_fd,"\n",1);
		printf("writing on serial complete\n");
		mpd_status_free(status);
		}
		//usleep(50000);
		sleep(1);
		/*
                */
		//connected =0;
	

        }
	mpd_connection_free(conn);
        close(tty_fd);
}



void signal_handler_IO (int status)
{
	
    printf("received data from UART.\n");
/*
	if (read(tty_fd,&c,1)>0)     {//   write(STDOUT_FILENO,&c,1);              // if new data is available on the serial port, print it out
			//write(STDOUT_FILENO,&c,1);


			if(c=='1') {
				//system("mpc volume -2");//printf("v-\n");
			}
			else if(c=='2') {
				//system("mpc volume +2"); //("v+\n");
			}
			else if(c=='3') {
				//mpd_send_toggle_pause(conn);
				//system("mpc toggle");
			}
			else if(c=='4') {
				mpd_send_previous(conn);
			}
			else if(c=='5') {
				mpd_send_next(conn);
			}
			else if(c=='6') {
				mpd_send_toggle_pause(conn);
			}

		}
*/
	printf("done\n");

}
