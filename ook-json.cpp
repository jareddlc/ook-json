#include "ook-json.h"

OOKJSON::OOKJSON() 
{
  this->oTransmitPin = -1;
  this->oReceiverInterrupt = -1;
  this->setPulseLength(350);
  this->setSendAttempt(10);
  OOKJSON::oReceiveValue = NULL;
  OOKJSON::oReceiveBitlength = 0;
}

void OOKJSON::setTransmitPin(int oTransmitPin)
{
  this->oTransmitPin = oTransmitPin;
  pinMode(this->oTransmitPin, OUTPUT);
}

void OOKJSON::setPulseLength(int oPulseLength)
{
  this->oPulseLength = oPulseLength;
}

void OOKJSON::setSendAttempt(int oSendAttempt)
{
  this->oSendAttempt = oSendAttempt;
}

void OOKJSON::setReceiveTolerance(int oReceiveTolerance)
{
  OOKJSON::oReceiveTolerance = oReceiveTolerance;
}

void OOKJSON::send(char* msg)
{
  for(int i = 0; i < oSendAttempt; i++)
  {
    int k = 0;
    while(msg[k] != '\0')
    {
      switch(msg[k])
      {
        case '0':
          this->send0();
        break;
        case '1':
          this->send1();
        break;
      }
      k++;
    }
    //this->sendSync();
  }
}

void OOKJSON::send0()
{
  this->transmit(1,3);
}

void OOKJSON::send1()
{
  this->transmit(3,1);
}

void OOKJSON::transmit(int highPulse, int lowPulse)
{
  boolean receiveDisabled = false;
  int oReceiveInterruptBackup = this->oReceiveInterrupt;
  if(this->oTransmitPin != -1)
  {
    if(this->oReceiveInterrupt != -1)
    {
      this->disableReceive();
      receiveDisabled = true;
    }

    digitalWrite(this->oTransmitPin, HIGH);
    delayMicroseconds(this->oPulseLength * highPulse);
    digitalWrite(this->oTransmitPin, LOW);
    delayMicroseconds(this->oPulseLength * lowPulse);
    
    /*
    #if not defined( RCSwitchDisableReceiving )
    if(receiveDisabled){
        this->enableReceive(oReceiveInterruptBackup);
    }
    #endif
    */
  }
}

void OOKJSON::disableReceive()
{
  detachInterrupt(this->oReceiveInterrupt); // Arduino function
  this->oReceiveInterrupt = -1;
}

void OOKJSON::enableReceive(int interrupt)
{
  this->oReceiveInterrupt = interrupt;
  this->enableReceive();
}

void OOKJSON::enableReceive()
{
  if(this->oReceiveInterrupt != -1)
  {
    OOKJSON::oReceiveValue = NULL;
    OOKJSON::oReceiveBitlength = NULL;
    //attachInterrupt(this->oReceiveInterrupt, handleInterrupt, CHANGE); // CHANGE is an Arduino interrupt paramater
  }
}
