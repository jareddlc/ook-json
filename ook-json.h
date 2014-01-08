/*
  OOK-JSON:   Arduino libary sending and receiving JSON.
  Author:     Jared De La Cruz
*/

#ifndef _OOKJSON_h
#define _OOKJSON_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
  #include "Energia.h"  
#else
  #include "WProgram.h"
#endif

class OOKJSON {
  int oTransmitPin;
  int oPulseLength;
  int oSendAttempt;
  int oReceiveInterrupt;

  static int oReceiveTolerance;
  static unsigned long oReceiveValue;
  static unsigned int oReceiveBitlength;

  public:
    OOKJSON();
    void setTransmitPin(int oTransmitPin);
    void setPulseLength(int oPulseLength);
    void setSendAttempt(int oSendAttempt);
    void setReceiveTolerance(int oReceiveTolerance);

    void send(char* msg);
    void send0();
    void send1();
    void sendSync();
    void transmit(int highPulse, int lowPulse);

    void disableReceive();
    void enableReceive(int interrupt);
    void enableReceive();
};