#include "converter.hpp"

//
//  tconverter cli by HCPP20334
//
void error_argv() {
  std::cout << "error: arguments falled\n";
  std::cout << "usage:\n      tconverter --throne \"throne://add/..\" "
               "\nexample:\n      "
               "\n      tconverter --raw \"json \" "
               "tconverter --throne \"throne://add/..\" \n"
               "tconverter --raw \"json \" \n      ";
  return;
}
int main(int argc, char **argv) {
  if (argc < 3) {
    error_argv();
    return 1;
  }
    if (std::strcmp(argv[1], "--throne") == 0) {
      std::cout << convertAll(argv[2],false);
    } else if (std::strcmp(argv[1], "--raw") == 0) {
      std::cout << convertAll(argv[2],true);
    }
   else{
     error_argv();
     return 1;
   }


  return 0;
}
