
#include <sdk/config.h>
#include <stdio.h>
#include "gnss.h"

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int logger_main(int argc, char *argv[])
#endif
{
  printf("GNSS Logger\n");
  gnss_init();
  gnss_loop();
  return 0;
}
