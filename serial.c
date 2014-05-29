#if 0
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

/* we need a termios structure to clear the HUPCL bit */
struct termios tio;

int main(int argc, char *argv[])
{
  int fd;
  int status;

  if (argc != 4)
  {
    printf("Usage: setSerialSignal port                  DTR RTS\n");
    printf("Usage: setSerialSignal /dev/ttyS0|/dev/ttyS1 0|1 0|1\n");
    exit( 1 );
  }

  if ((fd = open(argv[1],O_RDWR)) < 0)
  {
    printf("Couldn't open %s\n",argv[1]);
    exit(1);
  }
  tcgetattr(fd, &tio);          /* get the termio information */
  tio.c_cflag &= ~HUPCL;        /* clear the HUPCL bit */
  tcsetattr(fd, TCSANOW, &tio); /* set the termio information */

  ioctl(fd, TIOCMGET, &status); /* get the serial port status */

  if ( argv[2][0] == '1' )      /* set the DTR line */
    status &= ~TIOCM_DTR;
  else
    status |= TIOCM_DTR;

  if ( argv[3][0] == '1' )      /* set the RTS line */
    status &= ~TIOCM_RTS;
  else
    status |= TIOCM_RTS;

  ioctl(fd, TIOCMSET, &status); /* set the serial port status */

  close(fd);                    /* close the device file */
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <curses.h>

int main (int argc, const char * argv[]) 
{
// /dev/tty.usbserial-142

    char buf[255];
    struct termios theTermios;
    int res;

    if(--argc != 1)
    {
      printf("Say which device to talk to\n");
      return(1);
    }
    
    int fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    if(fd == -1)
    {
        printf("Unable to open %s\n", argv[1]);
        return(2);
    }

    memset(&theTermios, 0, sizeof(struct termios));
    cfmakeraw(&theTermios);
    cfsetspeed(&theTermios, 115200);

    theTermios.c_cflag = CREAD | CLOCAL | IGNPAR | CS8 | IXON;
    theTermios.c_cflag &= ~PARENB;
    theTermios.c_cflag &= ~CSTOPB;
    
    theTermios.c_cc[VMIN] = 0;
    theTermios.c_cc[VTIME] = 10;     // 1 sec timeout

    ioctl(fd, TIOCSETA, &theTermios);

    initscr();
    while(1)
    {
      puts("> ");
      refresh();
      char* ptr = buf;
      while((*ptr++ = getch()) != '\n');
      *ptr++ = '\n';
      *ptr = 0;
      if (buf[0] == 'q')
      {
        break;
      }
      int bytes = write(fd, buf, strlen(buf));
  
      while((res = read(fd, buf, sizeof(buf)-1)) == 0);
      if(res > 0)
      {
          buf[res-1]=0;
          printf("%s\n", buf);
      }

    }
    endwin();
    close(fd);
    return 0;
}
