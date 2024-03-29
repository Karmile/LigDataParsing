#pragma once
#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
class Logger {
 public:
  static Logger& GetInstance() {
    static Logger logger;
    return logger;
  }
  static bool Init(const std::string& file_name);
  template <typename T>
  static void TryLog(const T& value);

  template <typename T>
  friend Logger& operator<<(Logger& logger, const T& value);

 private:
  static std::ofstream file_;
  static std::ostream console_;
  static std::mutex mutex_;
  static std::string log_dir_;
  static std::string file_path_;
  static int log_backup_index_;
  static int file_counts_;
  Logger() {}
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
};

#define MAX_LOG_FILE_SIZE (20 * 1024 * 1024)
// 设定日志文件的最大备份数量为 5
#define MAX_LOG_BACKUPS (5)

template <typename T>
void Logger::TryLog(const T& value) {
  std::unique_lock<std::mutex> lock(mutex_);
  // 获取日志文件的当前大小
  int64_t log_file_size = 0;
  if (file_.is_open()) {
    file_.seekp(0, std::ios::end);
    log_file_size = file_.tellp();
  } else {
    log_file_size = 0;
  }
  // 如果日志文件的大小超过了最大大小，则进行日志文件的滚动和备份
  if (log_file_size >= MAX_LOG_FILE_SIZE) {
    file_.close();
    // 重命名当前的日志文件为备份文件
    std::string backup_log_file_name = file_path_ + std::to_string(log_backup_index_) + ".log";
    std::rename(file_path_.c_str(), backup_log_file_name.c_str());

    file_.open(file_path_, std::ios::trunc);

    // 删除最老的日志文件备份
    if (file_counts_ >= MAX_LOG_BACKUPS) {
      log_backup_index_ = (log_backup_index_) % MAX_LOG_BACKUPS + 1;
      std::string oldest_log_file_name = file_path_ + std::to_string(log_backup_index_) + ".log";
      remove(oldest_log_file_name.c_str());
      file_counts_--;
      log_backup_index_--;
    }

    // 更新日志文件备份索引
    log_backup_index_++;
    file_counts_++;
  } else {
    file_ << value;
  }
}

template <typename T>
Logger& operator<<(Logger& logger, const T& value) {
  logger.console_ << value;
  logger.TryLog(value);
  return logger;
}
#define RAW_LOG(level, message)                                                              \
  do {                                                                                       \
    char file_name[_MAX_FNAME];                                                              \
    _splitpath(__FILE__, nullptr, nullptr, file_name, nullptr);                              \
    auto current_time = std::time(nullptr);                                                  \
    std::ostringstream oss;                                                                  \
    oss << "[" << level << "][" << std::put_time(std::gmtime(&current_time), "%F %T") << "]" \
        << file_name << " (" << __LINE__ << "): " << message;                                \
    Logger& logger = Logger::GetInstance();                                                  \
    logger << oss.str();                                                                     \
  } while (0)

#define LOG_INFO(message) RAW_LOG("LOG_INFO", message)
#define LOG_WARN(message) RAW_LOG("LOG_WARN", message)
#define LOG_ERROR(message) RAW_LOG("LOG_ERROR", message)
#define LOG_FATAL(message) RAW_LOG("LOG_FATAL", message)

#endif  // LOG_H