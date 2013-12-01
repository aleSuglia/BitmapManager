#include "BitmapController.h"
#include "ExecutorFactory.h"
#include "IExecutor.h"

ResultStruct BitmapController::executeOperation(RequestStruct request){
  ResultStruct res_struct;
  IExecutor* exec = ExecutorFactory::getExecutor(request);
  res_struct = exec->executeOperation(request);
  return res_struct;
}
