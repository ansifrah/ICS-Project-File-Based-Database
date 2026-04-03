#include "interpreter/interpreter.h"

void string_extractor(string &value){
      value=value.substr(1,value.size()-2);
}