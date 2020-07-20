#include "base.h"
#include "gitbin/gitbin.h"

void printHelp() {
  print("usage:");
  print("  gitools bin $file");
}

int main(int argc, const char* argv[]) {
  if (argc < 2) goto args_error;

  if (argv[1] == string("bin")) {
    if (argc != 3) goto args_error;
    gitbins::convert(argv[2]);
    return 0;
  }

args_error:
  printHelp();
  return 1;
}
