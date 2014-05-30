#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <curses.h>

// /dev/tty.usbserial-142
int main (int argc, const char * argv[]) 
{
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
