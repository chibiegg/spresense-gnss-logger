#include <sdk/config.h>
#include <stdio.h>
#include "gnss.h"

static void *gnss_loop_task(void *arg);

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int logger_main(int argc, char *argv[])
#endif
{
  pthread_t thread_gnss;


  printf("GNSS Logger\n");
  gnss_init();

  pthread_create(&thread_gnss, NULL, gnss_loop_task, NULL);

  pthread_join(thread_gnss, NULL);
  return 0;
}

static void *gnss_loop_task(void *arg){
  gnss_loop();
  return NULL;
}
