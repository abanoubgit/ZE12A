#ifndef ZE12A_h
#define ZE12A_h

#include "Arduino.h"

class ZE12A
{

public:

   ZE12A();
   void begin(Stream *current_serial);
   void switchToActiveUploadMode();
   void switchToQuestionAnswerMode();
   void CalibartionMode();
   int readContinuousGasConcentration();
   int readManualGasConcentration();

private:

bool verifyChecksum(byte data[], int length);
Stream *_current_serial; 
};



#endif