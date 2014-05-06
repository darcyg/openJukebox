#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h> // needed for memset

//MPD relevant includes
#include <mpd/client.h>
//#include <mpd/status.h>
//#include <mpd/connection.h>


#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1


void signal_handler_IO(int status);   /* definition of signal handler */
void writeOnSerial();
int wait_flag=TRUE;

        struct termios tio, oldtio, newtio;
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

	volatile int STOP=FALSE;
	char buf[255];
	int res;
	
int main(int argc,char** argv)
{
	printf("serialMPDClient...\n");
	////Establishing the mpd connection
	//conn = mpd_connection_new("localhost", 6600, 0);
	conn = mpd_connection_new("192.168.1.136", 6600, 0);
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

	/* open the device to be non−blocking (read will return immediatly) */
	tty_fd=open(argv[1], O_RDWR | O_NOCTTY |O_NONBLOCK);
	if(tty_fd <0) {
		perror(argv[1]);
		exit (-1);
	}

		/* install the signal handler before making the device asynchronous */
	 saio.sa_handler = signal_handler_IO;
	//saio.sa_mask = 0;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);


		/* allow the process to receive SIGIO */
	fcntl(tty_fd, F_SETOWN, getpid());

		 /* Make the file descriptor asynchronous (the manual page says only O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	fcntl(tty_fd, F_SETFL, FASYNC);



	tcgetattr(tty_fd,&oldtio); /* save current port settings */

	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
		 /*
		BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
		CRTSCTS : output hardware flow control (only used if the cable has
		all necessary lines. See sect. 7 of Serial−HOWTO)
		CS8 : 8n1 (8bit,no parity,1 stopbit)
		CLOCAL : local connection, no modem contol
		CREAD : enable receiving characters
		*/
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
		 /*
		IGNPAR : ignore bytes with parity errors
		ICRNL : map CR to NL (otherwise a CR input on the other computer
		will not terminate input)
		otherwise make device raw (no other input processing)
		IGNCR: Ignore carriage return on input
		*/
	newtio.c_iflag = IGNPAR | IGNCR | ICRNL;
		 /*
		Raw output.
		*/
	newtio.c_oflag = 0;

		 /*
		ICANON : enable canonical input
		disable all echo functionality, and don't send signals to calling program
		*/
	newtio.c_lflag = ICANON;

		 /*
		initialize all control characters
		default values can be found in /usr/include/termios.h, and are given
		in the comments, but we don't need them here
		*/
	
	//newtio.c_cc[VINTR] = 0; /* Ctrl−c */
	//newtio.c_cc[VQUIT] = 0; /* Ctrl−\ */
	//newtio.c_cc[VERASE] = 0; /* del */
	//newtio.c_cc[VKILL] = 0; /* @ */
	//newtio.c_cc[VEOF] = 4; /* Ctrl−d */
	newtio.c_cc[VTIME] = 0; /* inter−character timer unused */
	newtio.c_cc[VMIN] = 1; /* blocking read until 1 character arrives */
	//newtio.c_cc[VSWTC] = 0; /* '\0' */
	//newtio.c_cc[VSTART] = 0; /* Ctrl−q */
	//newtio.c_cc[VSTOP] = 0; /* Ctrl−s */
	//newtio.c_cc[VSUSP] = 0; /* Ctrl−z */
	//newtio.c_cc[VEOL] = 0; /* '\0' */
	//newtio.c_cc[VREPRINT] = 0; /* Ctrl−r */
	//newtio.c_cc[VDISCARD] = 0; /* Ctrl−u */
	//newtio.c_cc[VWERASE] = 0; /* Ctrl−w */
	//newtio.c_cc[VLNEXT] = 0; /* Ctrl−v */
	//newtio.c_cc[VEOL2] = 0; /* '\0' */

		 /*
		now clean the modem line and activate the settings for the port
		*/
	tcflush(tty_fd, TCIFLUSH);
	tcsetattr(tty_fd,TCSANOW,&newtio);

		 /*
		terminal settings done, now handle input
		In this example, inputting a 'z' at the beginning of a line will
		exit the program.
		*/
	while (STOP==FALSE) { /* loop until we have a terminating condition */
		/* read blocks program execution until a line terminating character is
		input, even if more than 255 chars are input. If the number
		of characters read is smaller than the number of chars available,
		subsequent reads will return the remaining chars. res will be set
		to the actual number of characters actually read */
	writeOnSerial();
	sleep(1);
	//printf(".\n");usleep(1000000);
	if(wait_flag==FALSE){
		res = read(tty_fd,buf,255);
		buf[res]=0; /* set end of string, so we can printf */
		char c=buf[0];

		if(c=='2') {
				system("mpc -h 192.168.1.136 volume -2");//printf("v-\n");
			}
			else if(c=='1') {
				system("mpc -h 192.168.1.136 volume +2"); //("v+\n");
			}
			else if(c=='3') {
				//mpd_send_toggle_pause(conn);
				system("mpc -h 192.168.1.136 toggle");
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

		printf("%s\n", buf);
		//STOP=TRUE;
		wait_flag = TRUE;
		}
	}	
	//if (buf[0]=='z') STOP=TRUE;
	
	mpd_connection_free(conn);
        close(tty_fd);
}

void signal_handler_IO(int status)
{
	//printf("recieved SIGIO signal.\n");
	wait_flag=FALSE;
}


void writeOnSerial() {
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
				strcat(transmitter,"2;");
			} else if (state == 1) //stop
			{
				strcat(transmitter,"1;");
			} else if(state == 2) //play
			{
				strcat(transmitter,"0;");
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
			//strcat(str,"\n");
			strcat(transmitter,str);
		} else {
			//strcat(transmitter,"\n");
		}

		printf("%s",transmitter);
		for(int i = 0; i<strlen(transmitter);i++)
		{	
			//printf("writing character\n");
			char tmp = transmitter[i];
			write(tty_fd, &tmp, 1);
		}
		if(write(tty_fd,"\n",1) <1) {
			printf("writing on serial was not posibel\n");
		} else {
			printf("writing on serial complete\n");
		}
		mpd_status_free(status);
		}
}
