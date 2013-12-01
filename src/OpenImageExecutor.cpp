#include "OpenImageExecutor.h"

#include "BitmapFile.h"

ResultStruct OpenImageExecutor::executeOperation(RequestStruct request){
    std::string bmpFileName((char*) request.getAttribute(1));

    BitmapFile bmp(bmpFileName);
    return ResultStruct(1, bmp.getTO());
}

