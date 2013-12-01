#ifndef MODIFYIMAGEEXECUTOR_H
#define MODIFYIMAGEEXECUTOR_H

#include "IExecutor.h"
#include "ResultStruct.h"
#include "RequestStruct.h"

class ModifyImageExecutor : public IExecutor
{
public:
    ResultStruct executeOperation(RequestStruct request);
};

std::vector<unsigned char> imageToSepia(const std::vector<unsigned char>& pixels, int sepiaDepth);
std::vector<unsigned char> imageToNegative(const std::vector<unsigned char>& pixels);


#endif // MODIFYIMAGEEXECUTOR_H
