#include "log.h"

bool Logger::Init(const std::string& file_name) {
  file_path_ = file_name;
  size_t last_slash_pos = file_path_.find_last_of("/\\");
  log_dir_ = file_path_.substr(0, last_slash_pos + 1);
  std::unique_lock<std::mutex> lock(mutex_);
  file_.open(file_path_, std::ios::app);
  log_backup_index_ = 1;
  file_counts_ = 1;
  return file_.is_open();
}

std::ofstream Logger::file_;
std::mutex Logger::mutex_;
std::string Logger::log_dir_;
std::string Logger::file_path_;
std::ostream Logger::console_(std::cout.rdbuf());
int Logger::log_backup_index_ = 1;
int Logger::file_counts_ = 1;
