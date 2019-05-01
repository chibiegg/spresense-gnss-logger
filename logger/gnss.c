#include <sdk/config.h>
#include <stdio.h>
#include <arch/chip/gnss.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include "gpsutils/cxd56_gnss_nmea.h"
#include "gnss.h"

#define MY_GNSS_SIG 18

// GNSS
int gnss_fd;
static struct cxd56_gnss_positiondata_s posdat;
static int gnss_setparams(int fd);
static int gnss_read(int fd);


// NMEA
char nmea_buf[NMEA_SENTENCE_MAX_LEN];
FAR static char *reqbuf(uint16_t size);
static void freebuf(FAR char *buf);
static int outnmea(FAR char *buf);
static int outbin(FAR char *buf, uint32_t len);


void gnss_init(){
  NMEA_OUTPUT_CB funcs;
  struct cxd56_gnss_signal_setting_s setting;

  NMEA_InitMask();
  NMEA_SetMask(0x000040ff);
  funcs.bufReq  = reqbuf;
  funcs.out     = outnmea;
  funcs.outBin  = outbin;
  funcs.bufFree = freebuf;
  NMEA_RegistOutputFunc(&funcs);

  gnss_fd = open("/dev/gps", O_RDONLY);
  if (gnss_fd < 0)
  {
    printf("open error:%d,%d\n", gnss_fd, errno);
    return;
  }

  /* Set the signal to notify GNSS events. */
  setting.fd      = gnss_fd;
  setting.enable  = 1;
  setting.gnsssig = CXD56_GNSS_SIG_GNSS;
  setting.signo   = MY_GNSS_SIG;
  setting.data    = NULL;
  ioctl(gnss_fd, CXD56_GNSS_IOCTL_SIGNAL_SET, (unsigned long)&setting);

  gnss_setparams(gnss_fd);

  /* Start GNSS. */
  ioctl(gnss_fd, CXD56_GNSS_IOCTL_START, CXD56_GNSS_STMOD_HOT);
}

void gnss_loop(){
  int ret;
  sigset_t mask;

  sigemptyset(&mask);
  sigaddset(&mask, MY_GNSS_SIG);
  sigprocmask(SIG_BLOCK, &mask, NULL);

  while(1){
    ret = sigwaitinfo(&mask, NULL);
    if (ret != MY_GNSS_SIG)
    {
      printf("sigwaitinfo error %d\n", ret);
      break;
    }

    gnss_read(gnss_fd);
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
    printf("ioctl(CXD56_GNSS_IOCTL_SET_OPE_MODE) NG!!\n");
    return ret;
  }

  /* Set the type of satellite system used by GNSS. */

  set_satellite = CXD56_GNSS_SAT_GPS | CXD56_GNSS_SAT_SBAS | CXD56_GNSS_SAT_QZ_L1CA | CXD56_GNSS_SAT_QZ_L1S;

  ret = ioctl(fd, CXD56_GNSS_IOCTL_SELECT_SATELLITE_SYSTEM, set_satellite);
  if (ret < 0)
  {
    printf("ioctl(CXD56_GNSS_IOCTL_SELECT_SATELLITE_SYSTEM) NG!!\n");
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
    printf("read error\n");
    return ret;
  }
  else if (ret != sizeof(posdat))
  {
    printf("read size error\n");
    return -1;
  }

  NMEA_Output(&posdat);
  return 0;
}

/* output NMEA */


FAR static char *reqbuf(uint16_t size)
{
  if (size > sizeof(nmea_buf))
    {
      printf("reqbuf error: oversize %s\n", size);
      return NULL;
    }
  return nmea_buf;
}

static void freebuf(FAR char *buf)
{
}

static int outnmea(FAR char *buf)
{
  return printf("%s", buf);
}

static int outbin(FAR char *buf, uint32_t len)
{
  return len;
  //return write(WRITE_FD, buf, (size_t)len);
}
