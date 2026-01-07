#ifndef MSC_H
#define MSC_H

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>

int msleep(long msec);
// Function to disable terminal echoing
void disableEcho();

// Function to enable terminal echoing
void enableEcho();

int kbhit(void);
unsigned int termWidth();
unsigned int termHeight();

#endif
