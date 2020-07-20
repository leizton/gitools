#pragma once

#include <string>

namespace io_utils {

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
