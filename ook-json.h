/*
  OOK-JSON:   Arduino libary sending and receiving JSON.
  Author:     Jared De La Cruz

  Based on:   RCSwitch - Arduino libary for remote control outlet switches
              Copyright (c) 2011 Suat Özgür.  All right reserved.

  Contributors:
  - Andre Koehler / info(at)tomate-online(dot)de
  - Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com
  - Skineffect / http://forum.ardumote.com/viewtopic.php?f=2&t=46
  - Dominik Fischer / dom_fischer(at)web(dot)de
  - Frank Oltmanns / <first name>.<last name>(at)gmail(dot)com
  
  Project home: http://code.google.com/p/rc-switch/
*/

#ifndef _OOKJSON_h
#define _OOKJSON_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad
  #include "Energia.h"  
#else
  #include "WProgram.h"
#endif

// Number of maximum High/Low changes per packet.
// Can handle (unsigned long) => 32 bit * 2 changes per bit + 2 for sync
#define MAX_CHANGES 67

class OOKJSON {
  int oTransmitPin;
  int oPulseLength;
  int oSendAttempt;
  int oReceiveInterrupt;

  static int oReceiveTolerance;
  static unsigned long oReceiveValue;
  static unsigned int oReceiveBitlength;
  static unsigned int oReceiveDelay;
  static unsigned int oReceiveProtocol;

  //sync[0] contains sync timing, followed by a number of bits
  static unsigned int sync[MAX_CHANGES];

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

    static void handleInterrupt();
    static bool receiveProtocol1(unsigned int changeCount);
};

#endif
