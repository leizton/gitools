#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "io_utils/io_utils.h"
#include "log.h"

namespace io_utils {

std::string getDirPath(const std::string& path, int upper) {
  size_t idx = path.length();
  for (; upper > 0; upper--) {
    idx = path.rfind('/', idx);
    if (idx == std::string::npos) return "/";
    if (idx == 0) return "/";
    idx--;
  }
  return path.substr(0, idx+1);
}

int mkdir(const std::string& path, bool _p) {
  int ret = ::mkdir(path.c_str(), S_IRWXU|S_IRWXG);
  if (ret == 0) return 0;
  if (::access(path.c_str(), F_OK|X_OK) == 0) return 1;
  if (!_p) {
    logerr << "mkdir error: " << path << ". ret=" << ret << logendl;
    return -1;
  }

  auto parent = getDirPath(path);
  if (parent.empty()) {
    logerr << "mkdir error: " << path << logendl;
    return -1;
  }
  if (mkdir(parent, true) < 0) {
    return -1;
  }
  return mkdir(path, false);
}

int getFileSize(const std::string& file) {
  struct stat st;
  if (::stat(file.c_str(), &st)) {
    logerr << "get file stat error: " << file << logendl;
    return -1;
  }
  return st.st_size;
}

int openr(const std::string& file) {
  int fd = ::open(file.c_str(), O_RDONLY);
  if (fd < 0) {
    logerr << "open to read error: " << file << logendl;
    return -1;
  }
  return fd;
}

int openw(const std::string& file) {
  int fd = ::open(file.c_str(), O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
  if (fd < 0) {
    logerr << "open to write error: " << file << logendl;
    return -1;
  }
  return fd;
}

File::File(const std::string& _path)
  : path(_path)
  , fd_(-1)
{}

File::~File() {
  if (fd_ > 0) {
    ::close(fd_);
  }
}

bool FileReader::open() {
  fd_ = openr(path);
  return fd_ > 0;
}

int FileReader::read(uint8_t* buf, int pos, int limit) {
  int p = pos;
  while (p < limit) {
    int n = ::read(fd_, buf + p, limit - p);
    if (n < 0) {
      logerr << "read from file fail: " << path << logendl;
      return -1;
    }
    if (n == 0) break;
    p += n;
  }
  return p - pos;
}

bool FileWriter::open() {
  fd_ = openw(path);
  return fd_ > 0;
}

int FileWriter::write(const uint8_t* buf, int pos, int limit) {
  while (pos < limit) {
    int n = ::write(fd_, buf + pos, limit - pos);
    if (n < 0) {
      logerr << "write to file fail: " << path << logendl;
      return -1;
    }
    pos += n;
  }
  return limit - pos;
}

}  // namespace io_utils
