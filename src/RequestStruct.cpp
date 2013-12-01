#include "RequestStruct.h"
#include <cstdarg>

RequestStruct::RequestStruct() : var_list(std::vector<void*>()) {}

RequestStruct::RequestStruct(int num_args, ...) {
  va_list var;
  va_start(var, num_args);
  for(int i = 0; i < num_args; i++)
      var_list.push_back(va_arg(var, void*));
  va_end(var);
}

RequestStruct::RequestStruct(const RequestStruct& request) {
  *this = request;
}

RequestStruct& RequestStruct::operator=(const RequestStruct& request) {
  if(this != &request) {
      this->var_list = std::vector<void*>(request.var_list);
  }
  return *this;
}

void* RequestStruct::getAttribute(int index) {
  return var_list.at(index);
}
