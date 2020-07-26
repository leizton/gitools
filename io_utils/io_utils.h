#pragma once

#include <string>

#include <sys/mman.h>

namespace io_utils {

std::string getDirPath(const std::string& path, int upper = 1);

/**
 * @param
 *   _p  create intermediate directories as required
 * @return
 *    0  succ;
 *    1  already exist;
 *   -1  fail;
 */
int mkdir(const std::string& path, bool _p = false);

inline int mkdirp(const std::string& path) {
  return mkdir(path, true);
}

int getFileSize(const std::string& file);

int openr(const std::string& file);
int openw(const std::string& file);

struct File {
public:
  const std::string path;

protected:
  int fd_;

public:
  File(const std::string& _path);
  virtual ~File();
};

class FileReader : public File {
public:
  FileReader(const std::string& path) : File(path) {}
  ~FileReader() {}

  bool open();

  int read(uint8_t* buf, int pos, int limit);
};

class FileWriter : public File {
public:
  FileWriter(const std::string& path) : File(path) {}
  ~FileWriter() {}

  bool open();

  int write(const uint8_t* buf, int pos, int limit);
};

}  // namespace io_utils
