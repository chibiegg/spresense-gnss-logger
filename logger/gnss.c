#include <sdk/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <arch/chip/gnss.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>
#include <arch/board/board.h>
#include <arch/chip/pin.h>
#include "gpsutils/cxd56_gnss_nmea.h"
#include "gnss.h"
#include "log.h"

#define MY_GNSS_SIG 18
#define LED_0 PIN_I2S1_DATA_OUT
#define LED_1 PIN_I2S1_DATA_IN
#define LED_2 PIN_I2S1_LRCK
#define LED_3 PIN_I2S1_BCK

// GNSS
int gnss_fd;
static struct cxd56_gnss_positiondata_s posdat;
int64_t last_backup = -1;
static int gnss_setparams(int fd);
static int gnss_read(int fd);

void gnss_init(){
  gnss_fd = open("/dev/gps", O_RDONLY);
  if (gnss_fd < 0)
  {
    printf("$GPLOG,open error:%d,%d\n", gnss_fd, errno);
    return;
  }

  ioctl(gnss_fd, CXD56_GNSS_IOCTL_STOP, 0);

  board_gpio_config(LED_0, 0, false, true, 0);
  board_gpio_config(LED_1, 0, false, true, 0);
  board_gpio_config(LED_2, 0, false, true, 0);
  board_gpio_config(LED_3, 0, false, true, 0);

  log_init();
}

void gnss_loop(){
  int ret;
  sigset_t mask;
  struct cxd56_gnss_signal_setting_s setting;


  ret = gnss_setparams(gnss_fd);
  if (ret<0) {
    return;
  }

  /* Set the signal to notify GNSS events. */
  setting.fd      = gnss_fd;
  setting.enable  = 1;
  setting.gnsssig = CXD56_GNSS_SIG_GNSS;
  setting.signo   = MY_GNSS_SIG;
  setting.data    = NULL;
  ret = ioctl(gnss_fd, CXD56_GNSS_IOCTL_SIGNAL_SET, (unsigned long)&setting);
  if (ret < 0)
  {
    printf("$GPLOG,ioctl(CXD56_GNSS_IOCTL_SIGNAL_SET) NG!! %d\n", ret);
    return;
  }

  sigemptyset(&mask);
  sigaddset(&mask, MY_GNSS_SIG);
  sigprocmask(SIG_BLOCK, &mask, NULL);

  while(1){
    ret = sigwaitinfo(&mask, NULL);
    if (ret != MY_GNSS_SIG)
    {
      printf("$GPLOG,sigwaitinfo error %d\n", ret);
      break;
    }

    if (gnss_read(gnss_fd) == 0){
      log_write(&posdat);
      board_gpio_write(LED_0, 1);
      usleep(1000*20);
      board_gpio_write(LED_0, 0);
      if (posdat.receiver.pos_fixmode > 1){
        board_gpio_write(LED_1, 1);

        // save backup
        if (last_backup<0 || (posdat.data_timestamp-last_backup) > CONFIG_GNSSLOGGER_LOGGER_SAVE_BACKUP_DATA_INTERVAL*1000) {
          ret = ioctl(gnss_fd, CXD56_GNSS_IOCTL_SAVE_BACKUP_DATA, 0);
          if (ret < OK)
          {
            printf("$GPLOG,Failed to save BackupData\n");
          }else{
            printf("$GPLOG,BackupData saved\n");
          }
          last_backup = posdat.data_timestamp;
        }

      }else{
        board_gpio_write(LED_1, 0);
      }
    }else{

    }
  }
}


/* GNSS */
static int gnss_setparams(int fd)
{
  int      ret = 0;
  uint32_t set_satellite;
  struct cxd56_gnss_ope_mode_param_s set_opemode;

  /* Set the GNSS operation interval. */

  set_opemode.mode     = 1;     /* Operation mode:Normal(default). */
  set_opemode.cycle    = 1000;  /* Position notify cycle(msec step). */

  ret = ioctl(fd, CXD56_GNSS_IOCTL_SET_OPE_MODE, (uint32_t)&set_opemode);
  if (ret < 0)
  {
    printf("ioctl(CXD56_GNSS_IOCTL_SET_OPE_MODE) NG!! %d\n", ret);
    return ret;
  }

  /* Set the type of satellite system used by GNSS. */
  set_satellite = CXD56_GNSS_SAT_GPS | CXD56_GNSS_SAT_SBAS | CXD56_GNSS_SAT_QZ_L1CA | CXD56_GNSS_SAT_QZ_L1S;

  ret = ioctl(fd, CXD56_GNSS_IOCTL_SELECT_SATELLITE_SYSTEM, set_satellite);
  if (ret < 0)
  {
    printf("$GPLOG,ioctl(CXD56_GNSS_IOCTL_SELECT_SATELLITE_SYSTEM) NG!! %d\n", ret);
    return ret;
  }

  /* Start GNSS. */
  ret = ioctl(gnss_fd, CXD56_GNSS_IOCTL_START, CXD56_GNSS_STMOD_WARM);
  if (ret < 0)
  {
    printf("$GPLOG,ioctl(CXD56_GNSS_IOCTL_START) NG!! %d\n", ret);
    return ret;
  }

  return ret;
}

static int gnss_read(int fd){
  int ret;

  /* Read POS data. */
  ret = read(fd, &posdat, sizeof(posdat));
  if (ret < 0)
  {
    printf("$GPLOG,read error\n");
    return ret;
  }
  else if (ret != sizeof(posdat))
  {
    printf("$GPLOG,read size error\n");
    return -1;
  }

  return 0;
}
