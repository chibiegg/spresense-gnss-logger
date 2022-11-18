#ifdef __cplusplus
extern "C" {
#endif

void log_init(void);
void log_write(struct cxd56_gnss_positiondata_s *posdatp);
void log_dcreport_write(struct cxd56_gnss_dcreport_data_s *dcreportp);


#ifdef __cplusplus
}
#endif
