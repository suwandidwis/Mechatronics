#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> // Header file for sleeping.
#include <math.h> 
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
//#include <sys/filio.h>
#include <fcntl.h>
#include <termios.h>
#include "serial.h"
#include "msg.h"

#define SERIAL_NONE      0  
#define	SERIAL_CONSUME   1  
#define	SERIAL_NONBLOCK  2  
#define SERIAL_DEBUG 0

char debug=0, buf[15];

int serial_read(int fd, char* buf, size_t numBytes, int options)
{
	char *ptr = buf;


	// Determine how many bytes there are to read.
	size_t bytesAvailable = 0;
	ioctl(fd, FIONREAD, &bytesAvailable);
	if(SERIAL_DEBUG)
		msg(DEBUG, "serial_read(): Avail to read: %d\n", (int)bytesAvailable);

	/* If SERIAL_NONBLOCK is set, and if there are not enough bytes
	 * available to read, return 0 so the caller can instead return a
	 * cached value. */
	if(bytesAvailable < numBytes &&
	   options & SERIAL_NONBLOCK)
	{
		if(SERIAL_DEBUG)
			msg(DEBUG, "serial_read(): Timeout\n");
		return 0;
	}
	
	/* If SERIAL_CONSUME is set and if there are more than numBytes*2
	   bytes available, repeatedly read numBytes. This will eventually
	   lead to having numBytes or slightly more than numBytes
	   available for us to actually read. */
	if(options & SERIAL_CONSUME)
	{
		while(bytesAvailable >= numBytes*2)
		{
			ssize_t r = read(fd, ptr, numBytes);
			if(r == 0)
			{
				// We can get here if the USB cord is disconnected
				// from the computer. Treat it as a read error.
				if(SERIAL_DEBUG)
					msg(DEBUG, "serial_read(): Did serial cable get disconnected?\n");
				return -1;
			}
			else if(r < 0)
			{
				if(SERIAL_DEBUG)
					msg(DEBUG, "serial_read(): read error %s\n",  strerror(errno));
				return -1;
			}
			else
				bytesAvailable -= r;
			
			if(SERIAL_DEBUG)
			{
				// Print the bytes we consumed
				msg(DEBUG, "serial_read(): consumed a total of %4d bytes: ", (int)r);
				char *str = (char*)malloc(r * 8);
				int index = 0;
				for(ssize_t i=0; i<r; i++)
					index += snprintf(str+index, r*8-index, "%02x ", (unsigned char) ptr[i]);
				msg(DEBUG, "%s", str);
				free(str);
			}
		} // end consume bytes loop

		if(SERIAL_DEBUG)
			msg(DEBUG, "serial_read(): Avail to read after consumption: %d\n", (int)bytesAvailable);
	} // end if consume bytes

	/* Actually read the data */
	size_t totalBytesRead = 0;
	while(totalBytesRead < numBytes)
	{
		/* If SERIAL_NONBLOCK was specified and if there are not
		 * enough bytes to read, we will have returned---so it is
		 * impossible for us to get here and potentially have read()
		 * block. */
		ssize_t bytesRead = read(fd, ptr, numBytes-totalBytesRead);
		if(bytesRead == 0)
		{
			/* This can happen if the USB cable gets disconnected from the computer. */
			if(SERIAL_DEBUG)
				msg(DEBUG, "serial_read(): Did serial cable get disconnected?\n");
			return -1;
		}
		else if(bytesRead < 0)
		{
			if(SERIAL_DEBUG)
				msg(DEBUG, "serial_read(): read error %s\n",  strerror(errno));
			return -1;
		}
		else
		{
			// read() either read all or some of the bytes we wanted to read.
			ptr += bytesRead;
			totalBytesRead += bytesRead;
		}
	}

	if(SERIAL_DEBUG)
	{
		msg(DEBUG, "serial_read(): Read a total of %4d bytes: ", (int)totalBytesRead);
		char *str = (char*)malloc(totalBytesRead * 8);
		int index = 0;
		for(size_t i=0; i<totalBytesRead; i++)
			index += snprintf(str+index, totalBytesRead*8-index, "i-%02x ", (unsigned char) buf[i]);
		msg(DEBUG, "%s", str);
		free(str);
	}

	/* If we didn't read the full numBytes, we would have exited. */
	return numBytes;
}

void serial_write(const int fd, const char* buf, size_t numBytes)
{
	while (numBytes > 0)
	{
		ssize_t result = write(fd, buf, numBytes);
		if(result < 0)
		{
			//msg(FATAL, "write error %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		// write() wrote none, some, or all of the bytes we wanted to write.
		buf += result;
		numBytes -= (size_t)result;
	}
}

int main()
{
	// Berikut jika ingin menggunakan serial port
  	fd = open_port();
   	init_port(fd);
	char head = 'X', head1='Y', head2 = 'Z';
	float dq1Ratio = 200 , dq2Ratio = 255;
	char chRatio1, chRatio2; 

	memset(&buf[0], 0, sizeof(buf));
	//while (buf[0] != 'X'){
	//	serial_read(fd, buf, 1, SERIAL_CONSUME);
	//}
	//serial_write (fd,&head,1);
	//serial_read(fd, buf, 1, SERIAL_CONSUME);
	//sudut1 = buf[0];
	//serial_read(fd, buf, 1, SERIAL_CONSUME);
	//sudut2 = buf[0];

	while(fd)
	{
		//kirim ratio
		//printf("posisi 1\n");
		chRatio1 = (char)(dq1Ratio*255/100);
		chRatio2 = (char)(dq2Ratio*255/100);
		//printf("posisi 2\n");
		buf[0]=0;
		while (buf[0] != 'Y'){
			printf("%c\n",buf[0]);
			serial_read(fd, buf, 1, SERIAL_CONSUME);
		}
		//printf("posisi 3\n");
		usleep(1000);
		serial_write(fd, &chRatio1, 1);
		//printf("posisi 4\n");
		while (buf[0] != 'Z'){
			printf("%c\n",buf[0]);
			serial_read(fd, buf, 1, SERIAL_CONSUME);
		}
		//printf("posisi 5\n");
		usleep(1000);
		serial_write(fd, &chRatio2, 1);
		//printf("posisi 6\n");
	}
}
