#include "ook-json.h"

OOKJSON::OOKJSON() 
{
  this->oTransmitPin = -1;
  this->oReceiveInterrupt = -1;
  this->setPulseLength(350);
  this->setSendAttempt(10);
  OOKJSON::oReceiveValue = NULL;
  OOKJSON::oReceiveBitlength = 0;
}

// sets the transmitter pin.
void OOKJSON::setTransmitPin(int oTransmitPin)
{
  this->oTransmitPin = oTransmitPin;
  pinMode(this->oTransmitPin, OUTPUT);
}

// sets the pulse length/speed
void OOKJSON::setPulseLength(int oPulseLength)
{
  this->oPulseLength = oPulseLength;
}

// sets the amount of attempts to make to send
void OOKJSON::setSendAttempt(int oSendAttempt)
{
  this->oSendAttempt = oSendAttempt;
}

// sets the tolerance of receiving
void OOKJSON::setReceiveTolerance(int oReceiveTolerance)
{
  OOKJSON::oReceiveTolerance = oReceiveTolerance;
}

// determines what send function to call based on the string
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
    this->sendSync();
  }
}

// transmit a 0 bit
void OOKJSON::send0()
{
  this->transmit(1,3);
}

// transmit a 1 bit
void OOKJSON::send1()
{
  this->transmit(3,1);
}

// transmit the bits. writes to pin
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

    // arduino specific functions
    // write high bit, wait, write low bit, wait
    digitalWrite(this->oTransmitPin, HIGH);
    delayMicroseconds(this->oPulseLength * highPulse);
    digitalWrite(this->oTransmitPin, LOW);
    delayMicroseconds(this->oPulseLength * lowPulse);
    
    if(receiveDisabled)
    {
      this->enableReceive(oReceiveInterruptBackup);
    }
  }
}

// detaches interrupt
void OOKJSON::disableReceive()
{
  detachInterrupt(this->oReceiveInterrupt); // Arduino function
  this->oReceiveInterrupt = -1;
}

// attaches interrupt
void OOKJSON::enableReceive(int interrupt)
{
  this->oReceiveInterrupt = interrupt;
  this->enableReceive();
}

// attaches interrupt (helper function)
void OOKJSON::enableReceive()
{
  if(this->oReceiveInterrupt != -1)
  {
    OOKJSON::oReceiveValue = NULL;
    OOKJSON::oReceiveBitlength = NULL;
    attachInterrupt(this->oReceiveInterrupt, handleInterrupt, CHANGE); // CHANGE is an Arduino interrupt paramater
  }
}

void OOKJSON::handleInterrupt()
{
  static unsigned int currentTime;
  static unsigned int changeCount;
  static unsigned long lastTime;
  static unsigned int repeatCount;
  
  // gets current time
  long now = micros();
  currentTime = now - lastTime;
 
  if(currentTime > 5000 && currentTime > OOKJSON::sync[0] - 200 && currentTime < OOKJSON::sync[0] + 200)
  {
    repeatCount++;
    changeCount--;
    if(repeatCount == 2) {
      if(receiveProtocol1(changeCount) == false)
      {
        // nothing
      }
      repeatCount = 0;
    }
    changeCount = 0;
  }
  else if(currentTime > 5000)
  {
    changeCount = 0;
  }
 
  if(changeCount >= MAX_CHANGES)
  {
    changeCount = 0;
    repeatCount = 0;
  }

  OOKJSON::sync[changeCount++] = currentTime;
  lastTime = now;
}

bool OOKJSON::receiveProtocol1(unsigned int changeCount){
    
  unsigned long code = 0;
  unsigned long delay = OOKJSON::sync[0] / 31;
  unsigned long delayTolerance = delay * OOKJSON::oReceiveTolerance * 0.01;

  for(int i = 1; i<changeCount ; i=i+2)
  {
    if(OOKJSON::sync[i] > delay-delayTolerance && OOKJSON::sync[i] < delay+delayTolerance && OOKJSON::sync[i+1] > delay*3-delayTolerance && OOKJSON::sync[i+1] < delay*3+delayTolerance)
    {
      code = code << 1;
    }
    else if(OOKJSON::sync[i] > delay*3-delayTolerance && OOKJSON::sync[i] < delay*3+delayTolerance && OOKJSON::sync[i+1] > delay - delayTolerance && OOKJSON::sync[i+1] < delay+delayTolerance)
    {
      code += 1;
      code = code << 1;
    } 
    else
    {
      // Failed
      i = changeCount;
      code = 0;
    }
  }
  code = code >> 1;
  if(changeCount > 6)
  { 
    // ignore < 4bit values as there are no devices sending 4bit values => noise
    OOKJSON::oReceiveValue = code;
    OOKJSON::oReceiveBitlength = changeCount / 2;
    OOKJSON::oReceiveDelay = delay;
    OOKJSON::oReceiveProtocol = 1;
  }

  if(code == 0)
  {
    return false;
  }
  else if(code != 0)
  {
    return true;
  }
}

// sends a "sync" bit
void OOKJSON::sendSync()
{
  this->transmit(1,31);
}
