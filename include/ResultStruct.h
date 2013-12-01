#ifndef RESULTSTRUCT_H
#define RESULTSTRUCT_H
#include <vector>
#include <cstdarg>

class ResultStruct
{
public:
  ResultStruct(int num_args, ...);
  ResultStruct();
  ResultStruct(const ResultStruct& result);
  ResultStruct& operator=(const ResultStruct& result);

  void* getAttribute(int index);
private:
  std::vector<void*> var_list;
};

#endif // RESULTSTRUCT_H
