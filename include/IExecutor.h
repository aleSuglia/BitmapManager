#ifndef IEXECUTOR_H
#define IEXECUTOR_H

#include "ResultStruct.h"
#include "RequestStruct.h"

class IExecutor {
public:
  virtual ResultStruct executeOperation(RequestStruct request) = 0;
};

#endif // IEXECUTOR_H
