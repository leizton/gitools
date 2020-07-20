#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "io_utils/io_utils.h"
#include "log.h"

namespace io_utils {

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
  fd_ = ::open(path.c_str(), O_RDONLY);
  if (fd_ < 0) {
    logerr << "open file to read fail: " << path << logendl;
    return false;
  }
  return true;
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
  fd_ = ::open(path.c_str(), O_RDONLY);
  if (fd_ < 0) {
    logerr << "open file to write fail: " << path << logendl;
    return false;
  }
  return true;
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
