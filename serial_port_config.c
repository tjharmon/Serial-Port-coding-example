#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    /* Check args, 3rd arg is for test code */
    if (argc < 3) {
        fprintf(stderr, "syntax: %s <port_name/(tty*)> <'R|W'>", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* Port name must be in format of 'tty*' */
    char *port_name = argv[1];
    int serial_port = open(port_name, O_RDWR);

    /* Check for errors */
    if (serial_port < 0) {
        fprintf(stderr, "Error %i when port: %s tried to open: %s\n",
                errno, port_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* config port */
    struct termios tty;
   
    if (tcgetattr(serial_port, &tty) != 0) {
        fprintf(stderr, "Error %i from tcgetattr: %s\n",
                errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    /* config port to cfmakeraw defaults or add custom flags here */
    cfmakeraw(&tty);
    /* config speed */
    cfsetspeed(&tty, B115200);
    /* set attributes/config from termios struct to serial port */
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        fprintf(stderr, "Error %i from tcsetattr: %s\n",
                 errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /*********************************************************
     * TEST CODE TO BE REMOVED                               *
     * Terminal 1: socat -d -d pty,raw,echo=0 pty,raw,echo=0 *
     * Terminal 2: ./exif /dev/pts/<given number> R          *
     * Terminal 3: ./exif /dev/pts/<given number> W          *
     * Result: 'Received message: Oxcart' on Terminal 2      *
      ********************************************************/
    if (strcmp(argv[2], "R") == 0) {
        char buf[512];
        memset(&buf, '\0', sizeof(buf));
        int bytes = read(serial_port, &buf, sizeof(buf));
        if (bytes < 0) {
            fprintf(stderr, "Error reading message: %s",
                    strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("Received message: %s", buf);
    } else if (strcmp(argv[2], "W") == 0) {
        char msg[] = {'I', 't', ' ', 'w', 'o', 'r', 'k', 's', '\n'};
        write(serial_port, msg, sizeof(msg));
    } else {
        fprintf(stderr, "Error with read/write argument. %s",
                strerror(errno));
        exit(EXIT_FAILURE);
    } // ** end of test code **    
}
