#ifndef OPENIMAGEEXECUTOR_H
#define OPENIMAGEEXECUTOR_H

#include "IExecutor.h"

class OpenImageExecutor : public IExecutor
{
public:
    ResultStruct executeOperation(RequestStruct request);
};

#endif // OPENIMAGEEXECUTOR_H
