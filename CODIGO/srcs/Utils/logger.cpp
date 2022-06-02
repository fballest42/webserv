#include "logger.hpp"

Logger::Logger() {
};

void Logger::setLogLevel(LogLevel level) {
  _level = level;
}

LogLevel Logger::getLogLevel() {
  return _level;
}

void Logger::print(LogLevel level, std::string str, std::string color, bool error) {
  (void)error;
 // if (level <= _level) {
    struct tm	*tm;
    time_t rawtime;
    char buf[32];

    time(&rawtime);
    tm = localtime (&rawtime);
    int ret = strftime(buf, 32, "%T", tm);
    buf[ret] = '\0';

    //pthread_mutex_lock(&g_write);
    
    if (error) {
      std::cerr << CYAN << "[" << buf << "] " << RESET;
      std::cerr << color << str << RESET;
      std::cerr << "\n";
    } else {
      std::cout << CYAN << "[" << buf << "] " << RESET;
      std::cout << color << str << RESET;
      std::cout << "\n";
    }
    //pthread_mutex_unlock(&g_write);
  //}
};