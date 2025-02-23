#include "base.h"
#include "gpio.h"
#include "hw_procs.h"
#include "config_hw.h"
#include "config_file_names.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int s_iGPIOButtonsDirectionDetected = 1;
static int s_iGPIOButtonsPullDirection = 0;


int GPIOExport(int pin)
{
   if ( pin <= 0 )
      return 0;
   char buffer[6];
   ssize_t bytes_written;
   int fd;

   fd = open("/sys/class/gpio/export", O_WRONLY);
   if (-1 == fd)
   {
      //fprintf(stderr, "Failed to open export for writing!\n");
      return(-1);
   }

   bytes_written = snprintf(buffer, 6, "%d", pin);
   write(fd, buffer, bytes_written);
   close(fd);
   return 0;
}

int GPIOUnexport(int pin)
{
   if ( pin <= 0 )
      return 0;
   char buffer[6];
   ssize_t bytes_written;
   int fd;

   fd = open("/sys/class/gpio/unexport", O_WRONLY);
   if (-1 == fd) {
    //fprintf(stderr, "Failed to open unexport for writing!\n");
    return(-1);
   }

   bytes_written = snprintf(buffer, 6, "%d", pin);
   write(fd, buffer, bytes_written);
   close(fd);
   return 0;
}

int GPIODirection(int pin, int dir)
{
   if ( pin <= 0 )
      return 0;
   static const char s_directions_str[]  = "in\0out";
   char path[64];
   int fd;

   snprintf(path, 64, "/sys/class/gpio/gpio%d/direction", pin);
   fd = open(path, O_WRONLY);
   if (-1 == fd) {
    //fprintf(stderr, "Failed to open gpio direction for writing!\n");
    return(-1);
   }

   if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
    //fprintf(stderr, "Failed to set direction!\n");
    return(-1);
   }

   close(fd);
   return 0;
}

int GPIORead(int pin)
{
   if ( pin <= 0 )
      return -1;
   char path[64];
   char value_str[5];
   int fd;

   snprintf(path, 64, "/sys/class/gpio/gpio%d/value", pin);
   fd = open(path, O_RDONLY);
   if (-1 == fd) {
    //fprintf(stderr, "Failed to open gpio value for reading!\n");
    return(-1);
   }

   int ir = read(fd, value_str, 3);
          if ( ir == -1 ) {
    //fprintf(stderr, "Failed to read value!\n");
    return(-1);
   }
          value_str[ir] = 0;
   close(fd);
   return atoi(value_str);

   /*
   char szBuff[128];
   char szOutput[128];
   sprintf(szBuff, "gpioget gpiochip3 %d", pin);
   hw_execute_bash_command_silent(szBuff, szOutput);
   return atoi(szOutput);
   */
}

int GPIOWrite(int pin, int value)
{
    if (pin <= 0)
        return 0;

    static const char s_values_str[] = "01";

    char path[64];
    int fd;

    snprintf(path, 64, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        // fprintf(stderr, "Failed to open gpio value for writing!\n");
        return (-1);
    }

    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
        // fprintf(stderr, "Failed to write value!\n");
        return (-1);
    }

    close(fd);
    return 0;
}

int GPIOGetButtonsPullDirection()
{
   return s_iGPIOButtonsPullDirection;
}

int _GPIOTryPullUpDown(int iPin, int iPullDirection)
{
   return 0;
}


void _GPIO_PullAllDown()
{
}

void _GPIO_PullAllUp()
{
}

// Returns 0 on success, 1 on failure

int GPIOInitButtons()
{
   _gpio_load_custom_mapping();
   log_line("[GPIO] Export and initialize buttons (for Radxa)...");

   s_iGPIOButtonsDirectionDetected = 1;
   s_iGPIOButtonsPullDirection = 0;

   int failed = 0;
   if (-1 == GPIOExport(GPIOGetPinMenu()) || -1 == GPIOExport(GPIOGetPinBack()))
   {
      log_line("Failed to get GPIO access to pin Menu/Back.");
      failed = 1;
   }
   if (-1 == GPIOExport(GPIOGetPinPlus()) || -1 == GPIOExport(GPIOGetPinMinus()))
   {
      log_line("Failed to get GPIO access to pin Plus/Minus.");
      failed = 1;
   }

   if (-1 == GPIOExport(GPIOGetPinQA1()))
   {
      log_line("Failed to get GPIO access to pin QA1.");
      failed = 1;
   }
   if (-1 == GPIOExport(GPIOGetPinQA2()))
   {
      log_line("Failed to get GPIO access to pin QA2.");
      failed = 1;
   }
   
   if (-1 == GPIOExport(GPIOGetPinQA3()))
   {
      log_line("Failed to get GPIO access to pin QA3.");
      failed = 1;
   }

   /*
   if (-1 == GPIOExport(GPIOGetPinQAPlus()))
   {
      log_line("Failed to get GPIO access to pin QAPLUS.");
      failed = 1;
   }

   if (-1 == GPIOExport(GPIOGetPinQAMinus()))
   {
      log_line("Failed to get GPIO access to pin QAMINUS.");
      failed = 1;
   }
   */

   if (-1 == GPIODirection(GPIOGetPinMenu(), IN) || -1 == GPIODirection(GPIOGetPinBack(), IN))
   {
      log_line("Failed set GPIO configuration for pin Menu/Back.");
      failed = 1;
   }
   if (-1 == GPIODirection(GPIOGetPinPlus(), IN) || -1 == GPIODirection(GPIOGetPinMinus(), IN))
   {
      log_line("Failed set GPIO configuration for pin Plus/Minus.");
      failed = 1;
   }
   
   if (-1 == GPIODirection(GPIOGetPinQA1(), IN))
   {
      log_line("Failed set GPIO configuration for pin QA1.");
      failed = 1;
   }
   if (-1 == GPIODirection(GPIOGetPinQA2(), IN))
   {
      log_line("Failed set GPIO configuration for pin QA2.");
      failed = 1;
   }
   
   if (-1 == GPIODirection(GPIOGetPinQA3(), IN))
   {
      log_line("Failed set GPIO configuration for pin QA3.");
      failed = 1;
   }

   /*
   if (-1 == GPIODirection(GPIOGetPinQAPlus(), IN))
   {
      log_line("Failed set GPIO configuration for pin QAACTIONPLUS.");
      failed = 1;
   }

   if (-1 == GPIODirection(GPIOGetPinQAMinus(), IN))
   {
      log_line("Failed set GPIO configuration for pin QAACTIONMINUS.");
      failed = 1;
   }
   */

   return failed;
}
