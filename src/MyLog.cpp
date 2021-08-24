#include "MyLog.h"

/*       syslog = new Syslog(udp, config->syslog, 514, hostname, "EVSE", LOG_INFO);
      syslog->log("EVSE start!!!"); */


MyLog* MyLog::instance = NULL;


void MyLog::log(const char* file, int line, const char* method, const char* format, ...) {
  MyLog* me = getInstance();
  if (me->syslog == NULL) {
    log_e("syslog not initialized");
    return;
  }

  char buffer[1024];
  snprintf(buffer, sizeof(buffer), "[X][%s:%i] %s: ", file, line, method);
  char *p = buffer+strlen(buffer);
  va_list arg;
  va_start(arg, format);
  vsprintf(p, format, arg);
  va_end(arg);
  me->syslog->log(buffer);

}

void MyLog::dn(void *x) {

}

void MyLog::write(const char* format, ...) {
  MyLog* me = getInstance();
  if (me->syslog == NULL) {
    log_e("syslog not initialized");
    return;
  }
  char buffer[1024];
  va_list arg;
  va_start(arg, format);
  vsprintf(buffer, format, arg);
  va_end(arg);
  me->syslog->log(buffer);

}

void MyLog::server(const char* server) {
  MyLog* me = getInstance();
  if (me->syslog != NULL) {
    free(me->syslog);
  }
  me->syslog = new Syslog(me->udp, server, 514);
  //me->syslog->log("ciaone server settato");
}

MyLog* MyLog::getInstance() {
  if (instance == NULL) {
    instance = new MyLog();
  }
  return instance;
}

MyLog::MyLog() {
  syslog = NULL;
}