#include <fstream>
#include <vector>

#include "gitbin/gitbin.h"
#include "io_utils/io_utils.h"
#include "log.h"

using namespace io_utils;

namespace gitbins {

void encode(const std::string& in_file, const std::string& out_dir) {
  const int in_size = io_utils::getFileSize(in_file);
  if (in_size < 0) return;
  if (in_size == 0) {
    logwarn << "empty file: " << in_file << logendl;
    return;
  }
  if (io_utils::mkdir(out_dir) < 0) return;
  if (out_dir[out_dir.length()-1] != '/') {
    const_cast<std::string&>(out_dir) = out_dir + "/";
  }

  int rfd = io_utils::openr(in_file);
  if (rfd < 0) return;
  void* rbuf = ::mmap(NULL, in_size, PROT_READ, MAP_SHARED, rfd, 0);
  if (!rbuf) {
    logerr << "open mmap error: " << in_file << logendl;
    return;
  }
  ::close(rfd);

  vector<int> part_sizes;
  constexpr int MaxPartSize = 1024 * 1024;
  bool succ = true;
  int pos = 0;
  for (int part_i = 0; pos < in_size; part_i++) {
    const int part_size = std::min(MaxPartSize, in_size - pos);
    const std::string out_file = out_dir + std::to_string(part_i);
    int wfd = io_utils::openw(out_file);
    if (wfd < 0) {
      succ = false;
      break;
    }
    if (::write(wfd, (char*)rbuf + pos, part_size) != part_size) {
      logerr << "write error: " << out_file << logendl;
      succ = false;
      break;
    }
    ::close(wfd);
    part_sizes.push_back(part_size);
    pos += part_size;
  }

  ::munmap(rbuf, in_size);
  const std::string meta_file = out_dir + "meta";
  ::remove(meta_file.c_str());
  if (!succ) return;

  std::ofstream meta(meta_file);
  if (meta.fail()) {
    logerr << "open to write error: " << meta_file << logendl;
    return;
  }
  for (int part_size : part_sizes) {
    meta << part_size << std::endl;
  }
  meta.close();
}

void decode(const std::string& in_dir, const std::string& out_file) {
  if (in_dir[in_dir.length()-1] != '/') {
    const_cast<std::string&>(in_dir) = in_dir + "/";
  }

  vector<int> part_sizes;
  const std::string meta_file = in_dir + "meta";
  std::ifstream meta(meta_file);
  if (meta.fail()) {
    logerr << "open to read error: " << meta_file << logendl;
    return;
  }
  int part_size;
  while (meta >> part_size) {
    part_sizes.push_back(part_size);
  }
  meta.close();

  bool succ = true;
  int wfd = io_utils::openw(out_file);
  if (wfd < 0) return;
  for (size_t part_i = 0; part_i < part_sizes.size(); part_i++) {
    const int part_size = part_sizes[part_i];
    const std::string in_file = in_dir + std::to_string(part_i);
    int rfd = io_utils::openr(in_file);
    void* rbuf = ::mmap(NULL, part_size, PROT_READ, MAP_SHARED, rfd, 0);
    if (!rbuf) {
      logerr << "open mmap error: " << in_file << logendl;
      succ = false;
      break;
    }
    ::close(rfd);

    if (::write(wfd, rbuf, part_size) != part_size) {
      logerr << "write error: " << out_file << logendl;
      succ = false;
      break;
    }
    ::munmap(rbuf, part_size);
  }
  ::close(wfd);

  if (!succ) {
    ::remove(out_file.c_str());
  }
}

void convert(const std::string& file) {
  if (startsWith(file, GitbinsFilePrefix)) {
    if (file[file.length() - 1] == '/') {
      const_cast<std::string&>(file) = file.substr(0, file.length() - 1);
    }
    std::string srcfile = file.substr(GitbinsFilePrefix.length(),
        file.length() - GitbinsFilePrefix.length() - GitbinsFileSuffix.length());
    logwarn << "decode to \"" << srcfile << "\"" << logendl;
    decode(file, srcfile);
  } else {
    logwarn << "encode \"" << file << "\"" << logendl;
    encode(file, GitbinsFilePrefix + file + GitbinsFileSuffix);
  }
}

}
