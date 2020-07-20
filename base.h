#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <limits>
#include <memory>
#include <cstring>
#include <string>
#include <string_view>
#include <algorithm>
#include <functional>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <shared_mutex>

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

using std::string;
using std::vector;
using std::list;
using std::queue;
using std::stack;
using std::set;
using std::unordered_set;
using std::map;
using std::unordered_map;
using std::shared_ptr;

inline void __print(std::ostringstream& out, std::string sep, bool is_begin) {
}

template<class First, class... Args>
void __print(std::ostringstream& out, std::string sep, bool is_begin, First first, Args... left) {
  if (!is_begin) out << sep;
  out << first;
  __print(out, sep, false, left...);
}

template<class... Args>
void print(Args... args) {
  std::ostringstream out;
  __print(out, ", ", true, args...);
  out << "\n";
  std::cout << out.str();
}

template<class... Args>
void println(Args... args) {
  std::ostringstream out;
  __print(out, "", true, args...);
  out << "\n";
  std::cout << out.str();
}

inline bool startsWith(const std::string& src, const std::string& st) {
  return src.length() >= st.length() && !::strncmp(src.c_str(), st.c_str(), st.length());
}
