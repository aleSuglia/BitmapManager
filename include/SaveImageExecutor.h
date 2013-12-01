#ifndef SAVEIMAGEEXECUTOR_H
#define SAVEIMAGEEXECUTOR_H

#include "IExecutor.h"
#include "ResultStruct.h"
#include "RequestStruct.h"

class SaveImageExecutor : public IExecutor
{
public:
  ResultStruct executeOperation(RequestStruct request);
};

#endif // SAVEIMAGEEXECUTOR_H
