#include "ExecutorFactory.h"
#include "IExecutor.h"
#include "ImageOperation.h"
#include "ModifyImageExecutor.h"
#include "SaveImageExecutor.h"
#include <stdexcept>

IExecutor *ExecutorFactory::getExecutor(RequestStruct& request){
  ImageOperation op = *((ImageOperation*)request.getAttribute(0));

  switch(op) {
    case OPEN_IMAGE:
      return new OpenImageExecutor();
      break;
    case SAVE_IMAGE:
      return new SaveImageExecutor();
      break;
    case MODIFY_IMAGE:
      return new ModifyImageExecutor();
      break;
    default: throw std::invalid_argument("Impossibile creare executor per l'opzione richiesta");


  }
    return NULL;
}
