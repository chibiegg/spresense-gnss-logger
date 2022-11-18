#include <stdio.h>
#include <arch/chip/gnss.h>
#include "gpsutils/cxd56_gnss_nmea.h"
#include "log.h"

NMEA_OUTPUT_CB funcs;
FILE *fp = NULL;
char current_filename[100];


// NMEA
char nmea_buf[NMEA_SENTENCE_MAX_LEN];
FAR static char *reqbuf(uint16_t size);
static void freebuf(FAR char *buf);
static int outnmea(FAR char *buf);
static int outbin(FAR char *buf, uint32_t len);


void log_init(){
  NMEA_InitMask();
  NMEA_SetMask(0x000040ff);
  funcs.bufReq  = reqbuf;
  funcs.out     = outnmea;
  funcs.outBin  = outbin;
  funcs.bufFree = freebuf;
  NMEA_RegistOutputFunc(&funcs);
}

void log_open(char *filename){
  printf("$GPLOG,Open %s\n", filename);
  fp = fopen(filename, "a");
  if (fp == NULL) {
    printf("$GPLOG,Open error %s\n", filename);
  }
}

void log_dcreport_write(struct cxd56_gnss_dcreport_data_s *dcreportp) {
  if (dcreportp != NULL) {
    NMEA_DcReport_Output(dcreportp);
  }
}


void log_write(struct cxd56_gnss_positiondata_s *posdatp){

  printf(
    "$GPLOG,%04d/%02d/%02d %02d:%02d:%02d\n",
    posdatp->receiver.date.year, posdatp->receiver.date.month, posdatp->receiver.date.day,
    posdatp->receiver.time.hour, posdatp->receiver.time.minute, posdatp->receiver.time.sec
  );


#if CONFIG_GNSSLOGGER_LOGGER_SAVEFILE
  if (fp==NULL && posdatp->receiver.date.year>2000) {
    sprintf(
      current_filename, "/mnt/sd0/%04d%02d%02d-%02d%02d%02d.nmea",
      posdatp->receiver.date.year, posdatp->receiver.date.month, posdatp->receiver.date.day,
      posdatp->receiver.time.hour, posdatp->receiver.time.minute, posdatp->receiver.time.sec
    );
    log_open(current_filename);
  }
#endif // CONFIG_GNSSLOGGER_LOGGER_SAVEFILE

  NMEA_Output(posdatp);

#if CONFIG_GNSSLOGGER_LOGGER_SAVEFILE
  if (fp!=NULL) {
    if(posdatp->receiver.time.sec == 59){
      fflush(fp);
      int fd = fileno(fp);
      fsync(fd);
    }
    if(posdatp->receiver.time.minute == 59 && posdatp->receiver.time.sec == 59){
      fclose(fp);
      fp = NULL;
    }
  }
#endif // CONFIG_GNSSLOGGER_LOGGER_SAVEFILE

}

/* output NMEA */
FAR static char *reqbuf(uint16_t size)
{
  if (size > sizeof(nmea_buf))
    {
      printf("$GPLOG,reqbuf error: oversize %d bytes\n", size);
      return NULL;
    }
  return nmea_buf;
}

static void freebuf(FAR char *buf)
{
}

static int outnmea(FAR char *buf)
{
  int ret = 0;
  if (fp != NULL){
    ret = fputs(buf, fp);
  }

#if CONFIG_GNSSLOGGER_LOGGER_OUTPUT_SERIAL
  printf("%s", buf);
#endif

  return ret;
}

static int outbin(FAR char *buf, uint32_t len)
{
  return len;
  //return write(WRITE_FD, buf, (size_t)len);
}
