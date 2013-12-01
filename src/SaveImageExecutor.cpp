#include "SaveImageExecutor.h"
#include "ImageOperation.h"
#include "BitmapTO.h"
#include "BitmapFile.h"
#include <stdexcept>
#include <sstream>

ResultStruct SaveImageExecutor::executeOperation(RequestStruct request) {
  BitmapTO* to = (BitmapTO*) request.getAttribute(1);
  std::string newBmpFileName((char*) request.getAttribute(2));
  BitmapFile* newBitmap = new BitmapFile(*to);

  newBitmap->copy_bmp(newBmpFileName);

  std::stringstream stream;

  stream << "Il file " << newBmpFileName << " è stato correttamente salvato";

  return ResultStruct(1,stream.str().c_str());
}
