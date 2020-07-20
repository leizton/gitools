#include <iostream>

#define loginfo std::cout << "\033[37m[I" << "] "
#define logwarn std::cout << "\033[33m[W" << "] "
#define logerr  std::cout << "\033[31m[E" << "] "
#define logendl "\033[0m" << std::endl;
