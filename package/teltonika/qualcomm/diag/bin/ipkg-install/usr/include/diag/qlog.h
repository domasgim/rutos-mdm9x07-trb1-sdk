#ifndef __QUECTEL_QLOG_H
#define __QUECTEL_QLOG_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/ioctl.h>

#define __QUECTEL_MDLOG_FORWARD__

// #define DEBUG_HEARTBEAT  //Uncomment this line to enable debugging output
#ifdef DEBUG_HEARTBEAT
#define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) do { } while (0)
#endif




#define FTP_F  "ftp:"
#define safe_close_fd(_fd) do { if (_fd != -1) { int tmpfd = _fd; _fd = -1; close(tmpfd); }} while(0)
#define NUM_PROC 10
#define FILE_NAME_LEN 500
#define ADB_DEVID  "/etc/adb_devid"
#define QUEC_UART_NUM 8
enum {
    QDB_DATA = 0,
    DM_DATA,
    QDSS_DATA,
    ADPL_DATA,
    XML_DATA,
    MAX_DATA,
};

enum{
    QUEC_FTP_SEND_QDB_XML = 1,
    QUEC_FTP_READ_DIAG_CFG,
    QUEC_MAX,
};

typedef struct QuecFtpStruct
{
    int quec_qdb_xml;       //reserve
    int readDiagCfg;        //read diag cfg
    int mixFunction;        //gather all function
    char dirname[256];
    char dest[50];          //temporary use
    char qcfg_dir[50];
};

extern struct QuecFtpStruct quec_ftp_arguments;

extern const char *g_ftp_server_ip;
extern const char *g_ftp_server_usr;
extern const char *g_ftp_server_pass;
extern int logfile_fd;
extern unsigned qlog_exit_requested;
extern int second_logfile;
extern int ip_addr_write;
extern int quec_qdb_xml;
extern unsigned s_logfile_seq;
extern int qdb_logfile;
extern int adpl_logfile;
extern int xmlfile_fd;
extern char output_dir[NUM_PROC][FILE_NAME_LEN];
void parser_ftp(const char *str);
ssize_t qlog_poll_write(int fd, const void *buf, size_t size, unsigned timeout_mesc);
int qlog_logfile_create_fullname(int file_type, const char *fullname, long tftp_size, int is_dump);
const char * qlog_time_name(int type);
int qlog_logfile_create(const char *logfile_dir, const char *logfile_suffix, unsigned logfile_seq);

int ftp_write_request(int index, const char *ftp_server, const char *user, const char *pass, const char *filename);

unsigned qlog_msecs(void);
int qget_adb_devid(char *dirname);

#define qlog_raw_log(fmt, arg... ) do { unsigned msec = qlog_msecs(); printf("\r[%03u.%03u] " fmt,  msec/1000, msec%1000, ## arg); fflush(stdout);} while (0)
#define qlog_dbg(fmt, arg... ) do { unsigned msec = qlog_msecs(); printf("[%03u.%03u] " fmt,  msec/1000, msec%1000, ## arg); fflush(stdout);} while (0)



size_t kfifo_write(int idx, const void *buf, size_t size);
void kfifo_free(int idx);
int kfifo_alloc(int fd,int file_type);
int kfifo_idx(int fd);
void ftp_quit(void);
void quec_ftp_quit(int server_fd);
void qlog_rmfile(void);
int file_transfer(int idx,char* path,unsigned long int file_size);
int quec_ftp_login(const char *ftp_server, const char *user, const char *password);
int ftp_download_and_save(int c_sock,const char *server_ip,char *p);
void directory_file_concatenation(char *dest,char *dir, char *file);
#endif
