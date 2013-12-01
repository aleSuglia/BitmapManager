#include "ResultStruct.h"

ResultStruct::ResultStruct() : var_list(std::vector<void*>()) {}

ResultStruct::ResultStruct(int num_args, ...) {
    va_list var;

    va_start(var, num_args);
    for(int i = 0; i < num_args; i++)
        var_list.push_back(va_arg(var, void*));
    va_end(var);
}

ResultStruct::ResultStruct(const ResultStruct& result) {
  *this = result;
}

ResultStruct& ResultStruct::operator=(const ResultStruct& result) {
  if(this != &result) {
    this->var_list = std::vector<void*>(result.var_list);
  }
  return *this;
}


void* ResultStruct::getAttribute(int index) {
  return var_list.at(index);
}
