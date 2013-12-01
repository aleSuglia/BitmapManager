#ifndef REQUESTSTRUCT_H
#define REQUESTSTRUCT_H

#include <vector>

class RequestStruct
{
public:
  RequestStruct();
  RequestStruct(int num_args, ...);
  RequestStruct(const RequestStruct& request);
  RequestStruct& operator=(const RequestStruct& request);

  void *getAttribute(int index);
private:
  std::vector<void*> var_list;
};

#endif // REQUESTSTRUCT_H
