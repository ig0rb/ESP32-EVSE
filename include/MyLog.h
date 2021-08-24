
#ifndef _MY_LOG_H
#define _MY_LOG_H

#include <Syslog.h>
#include <WiFiUdp.h>


#define ll(fmt, ...) MyLog::write("[X][%s:%d] %s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#define ddnn(x) MyLog::dn(x)

#define l(fmt, ...) MyLog::write("[X][%s:%d] %s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#define l_debug(fmt, ...) MyLog::write("[D][%s:%d] %s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#define l_info(fmt, ...) MyLog::write("[I][%s:%d] %s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#define l_notice(fmt, ...) MyLog::write("[N][%s:%d] %s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#define l_warning(fmt, ...) MyLog::write("[W][%s:%d] %s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#define l_error(fmt, ...) MyLog::write("[E][%s:%d] %s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#define l_fatal(fmt, ...) MyLog::write("[F][%s:%d] %s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

class MyLog {

  public:
  static void server(const char* server);
  static void write(const char* format, ...);
  static void log(const char* file, int line, const char* method, const char* format, ...);
  
  static void dn(void *x = NULL);

  private:
    MyLog();
    static MyLog* instance;
    static MyLog* getInstance();
    WiFiUDP udp;
    Syslog *syslog;

};

#endif