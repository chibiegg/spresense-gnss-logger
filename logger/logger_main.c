
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/boardctl.h>
#include "gnss.h"

static void *gnss_loop_task(void *arg);

int main(int argc, FAR char *argv[])
{
  pthread_t thread_gnss;

  printf("$GPLOG,GNSS Logger\n");

#if CONFIG_GNSSLOGGER_LOGGER_OUTPUT_SERIAL
  printf("$GPLOG,Output serial: Enabled\n");
#else
  printf("$GPLOG,Output serial: Disabled\n");
#endif
#if CONFIG_GNSSLOGGER_LOGGER_SAVEFILE
  printf("$GPLOG,Save file: Enabled\n");
#else
  printf("$GPLOG,Save file: Disabled\n");
#endif
  printf("$GPLOG,Backup interval: %d sec\n", CONFIG_GNSSLOGGER_LOGGER_SAVE_BACKUP_DATA_INTERVAL);


  gnss_init();

  pthread_create(&thread_gnss, NULL, gnss_loop_task, NULL);

  pthread_join(thread_gnss, NULL);
  return 0;
}

static void *gnss_loop_task(void *arg){
  gnss_loop();
  return NULL;
}
