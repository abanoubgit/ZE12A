#include "Arduino.h"
#include "ZE12A.h"


ZE12A::ZE12A(){
  _current_serial = NULL;


}

void ZE12A::begin(Stream *current_serial){
  _current_serial = current_serial;
}


void ZE12A::switchToActiveUploadMode() {
  // Command to switch to active upload mode
  byte command[] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
 _current_serial->write(command, sizeof(command));
   delay(1500);

}

void ZE12A::switchToQuestionAnswerMode() {
  // Command to switch from active upload mode to question and answer mode
  byte command[] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46};
 _current_serial->write(command, sizeof(command));
   delay(1500);

}


void ZE12A::CalibartionMode() {
  // Command to switch from active upload mode to question and answer mode
  byte command[] = {0xFF, 0x01, 0x88, 0x00, 0x07, 0x00, 0x00, 0x00, 0x70};
 _current_serial->write(command, sizeof(command));
   delay(1500);

}


int ZE12A::readContinuousGasConcentration() {
  if (_current_serial->available() >= 9) {
    byte response[9];
    _current_serial->readBytes(response, 9);

    // Print response bytes for debugging
    Serial.print("Response Bytes: ");
    for (int i = 0; i < 9; i++) {
      Serial.print(response[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    if (verifyChecksum(response, sizeof(response))) {
      unsigned int highByte = response[4];
      unsigned int lowByte = response[5];
      unsigned int concentration_ppb = (highByte << 8) + lowByte;

      return concentration_ppb;
    } else {
      // Handle checksum verification failure
      return -1;
    }
  } else {
    // Handle insufficient bytes available
    return -1;
  }
}

int ZE12A::readManualGasConcentration() {
  switchToQuestionAnswerMode();
  int ppb;
  byte petition[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; // Petition to get a single result
  byte measure[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};      // Space for the response

  _current_serial->write(petition, sizeof(petition));
  delay(2000);

  // read
  if (_current_serial->available() > 0) {
    _current_serial->readBytes(measure, 9);

    // Print received bytes for debugging
    Serial.print("Received Bytes: ");
    for (int i = 0; i < 9; i++) {
      Serial.print(measure[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  // calculate
    if (verifyChecksum(measure, sizeof(measure))) {
    ppb = measure[4] * 256 + measure[5]; // this formula depends on your sensor dataSheet


    return ppb;
  } else {
    // Handle checksum verification failure
    Serial.println("Checksum verification failed");
    return -1;
  }
}

bool ZE12A::verifyChecksum(byte data[], int length) {
  byte checksum = 0;
  for (int i = 1; i < length - 1; i++) {
    checksum += data[i];
  }
  checksum = (~checksum) + 1;
  return checksum == data[length - 1];
}