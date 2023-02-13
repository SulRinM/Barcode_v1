#include "RTimer.h"

RTimer Timer1;              

unsigned short RTimer::pwmPeriod = 0;
unsigned char RTimer::clockSelectBits = 0;
void (*RTimer::isrCallback)() = RTimer::isrDefaultUnused;

#if defined (__AVR_ATtiny85__)
ISR(TIMER1_COMPA_vect)
{
  Timer1.isrCallback();
}
#elif defined(__AVR__)
ISR(TIMER1_OVF_vect)
{
  Timer1.isrCallback();
}
#elif defined(__arm__) && defined(TEENSYDUINO) && (defined(KINETISK) || defined(KINETISL))
void ftm1_isr(void)
{
  uint32_t sc = FTM1_SC;
  #ifdef KINETISL
  if (sc & 0x80) FTM1_SC = sc;
  #else
  if (sc & 0x80) FTM1_SC = sc & 0x7F;
  #endif
  Timer1.isrCallback();
}
#elif defined(__arm__) && defined(TEENSYDUINO) && defined(__IMXRT1062__)
void RTimer::isr(void)
{
  FLEXPWM1_SM3STS = FLEXPWM_SMSTS_RF;
  Timer1.isrCallback();
}

#endif

void RTimer::isrDefaultUnused()
{
}