#include "log.h"


bool Logger::Init(const std::string& log_path) {
  file_path_ = log_path;
  auto su = true;
  if (!std::filesystem::exists(file_path_)) {
    su = std::filesystem::create_directory(file_path_);
  }
  return su;
}

std::ofstream Logger::file_;
std::mutex Logger::mutex_;
std::string Logger::file_path_;
std::ostream Logger::console_(std::cout.rdbuf());
