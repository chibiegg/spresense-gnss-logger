
#include <sdk/config.h>
#include <stdio.h>

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int logger_main(int argc, char *argv[])
#endif
{
  return 0;
}
