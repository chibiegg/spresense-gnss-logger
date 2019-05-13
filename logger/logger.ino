#include "gnss.h"

void setup()
{
  /* Wait HW initialization done. */
  sleep(2);

  gnss_init();
}

void loop()
{
  gnss_loop();
}
