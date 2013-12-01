#ifndef EXECUTORFACTORY_H
#define EXECUTORFACTORY_H

#include "IExecutor.h"
#include "OpenImageExecutor.h"

class ExecutorFactory
{
private:
  ExecutorFactory();
public:
  static IExecutor *getExecutor(RequestStruct &request);

};

#endif // EXECUTORFACTORY_H
