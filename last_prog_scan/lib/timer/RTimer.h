#ifndef RTimer_h_
#define RTimer_h_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "known_16bit_timers.h"
#if defined (__AVR_ATtiny85__)
#define TIMER1_RESOLUTION 256UL  
#elif defined(__AVR__)
#define TIMER1_RESOLUTION 65536UL 
#else
#define TIMER1_RESOLUTION 65536UL 
#endif

class RTimer
{

#if defined (__AVR_ATtiny85__)
public:
	void initialize(unsigned long microseconds = 1000000) __attribute__((always_inline)) {
		TCCR1 = _BV(CTC1);             
		TIMSK |= _BV(OCIE1A);          
		setPeriod(microseconds);
	}
	void setPeriod(unsigned long microseconds) __attribute__((always_inline)) {
		const unsigned long cycles = microseconds * ratio;
		if (cycles < TIMER1_RESOLUTION) {
			clockSelectBits = _BV(CS10);
			pwmPeriod = cycles;
		}
		else
			if (cycles < TIMER1_RESOLUTION * 2UL) {
				clockSelectBits = _BV(CS11);
				pwmPeriod = cycles / 2;
			}
			else
				if (cycles < TIMER1_RESOLUTION * 4UL) {
					clockSelectBits = _BV(CS11) | _BV(CS10);
					pwmPeriod = cycles / 4;
				}
				else
					if (cycles < TIMER1_RESOLUTION * 8UL) {
						clockSelectBits = _BV(CS12);
						pwmPeriod = cycles / 8;
					}
					else
						if (cycles < TIMER1_RESOLUTION * 16UL) {
							clockSelectBits = _BV(CS12) | _BV(CS10);
							pwmPeriod = cycles / 16;
						}
						else
							if (cycles < TIMER1_RESOLUTION * 32UL) {
								clockSelectBits = _BV(CS12) | _BV(CS11);
								pwmPeriod = cycles / 32;
							}
							else
								if (cycles < TIMER1_RESOLUTION * 64UL) {
									clockSelectBits = _BV(CS12) | _BV(CS11) | _BV(CS10);
									pwmPeriod = cycles / 64UL;
								}
								else
									if (cycles < TIMER1_RESOLUTION * 128UL) {
										clockSelectBits = _BV(CS13);
										pwmPeriod = cycles / 128;
									}
									else
										if (cycles < TIMER1_RESOLUTION * 256UL) {
											clockSelectBits = _BV(CS13) | _BV(CS10);
											pwmPeriod = cycles / 256;
										}
										else
											if (cycles < TIMER1_RESOLUTION * 512UL) {
												clockSelectBits = _BV(CS13) | _BV(CS11);
												pwmPeriod = cycles / 512;
											}
											else
												if (cycles < TIMER1_RESOLUTION * 1024UL) {
													clockSelectBits = _BV(CS13) | _BV(CS11) | _BV(CS10);
													pwmPeriod = cycles / 1024;
												}
												else
													if (cycles < TIMER1_RESOLUTION * 2048UL) {
														clockSelectBits = _BV(CS13) | _BV(CS12);
														pwmPeriod = cycles / 2048;
													}
													else
														if (cycles < TIMER1_RESOLUTION * 4096UL) {
															clockSelectBits = _BV(CS13) | _BV(CS12) | _BV(CS10);
															pwmPeriod = cycles / 4096;
														}
														else
															if (cycles < TIMER1_RESOLUTION * 8192UL) {
																clockSelectBits = _BV(CS13) | _BV(CS12) | _BV(CS11);
																pwmPeriod = cycles / 8192;
															}
															else
																if (cycles < TIMER1_RESOLUTION * 16384UL) {
																	clockSelectBits = _BV(CS13) | _BV(CS12) | _BV(CS11) | _BV(CS10);
																	pwmPeriod = cycles / 16384;
																}
																else {
																	clockSelectBits = _BV(CS13) | _BV(CS12) | _BV(CS11) | _BV(CS10);
																	pwmPeriod = TIMER1_RESOLUTION - 1;
																}
		OCR1A = pwmPeriod;
		OCR1C = pwmPeriod;
		TCCR1 = _BV(CTC1) | clockSelectBits;
	}
	
	void start() __attribute__((always_inline)) {
		TCCR1 = 0;
		TCNT1 = 0;
		resume();
	}
	void stop() __attribute__((always_inline)) {
		TCCR1 = _BV(CTC1);
	}
	void restart() __attribute__((always_inline)) {
		start();
	}
	void resume() __attribute__((always_inline)) {
		TCCR1 = _BV(CTC1) | clockSelectBits;
	}

	void attachInterrupt(void (*isr)()) __attribute__((always_inline)) {
		isrCallback = isr;
		TIMSK |= _BV(OCIE1A);
	}
	void attachInterrupt(void (*isr)(), unsigned long microseconds) __attribute__((always_inline)) {
		if (microseconds > 0) setPeriod(microseconds);
		attachInterrupt(isr);
	}
	void detachInterrupt() __attribute__((always_inline)) {
		
		TIMSK &= ~_BV(OCIE1A);
	}
	static void (*isrCallback)();
	static void isrDefaultUnused();

private:
	static unsigned short pwmPeriod;
	static unsigned char clockSelectBits;
	static const byte ratio = (F_CPU) / (1000000);

#elif defined(__AVR__)

#if defined (__AVR_ATmega8__)
	
#define TIMSK1 TIMSK
#endif

public:
	void initialize(unsigned long microseconds = 1000000) __attribute__((always_inline)) {
		TCCR1B = _BV(WGM13);        
		TCCR1A = 0;                
		setPeriod(microseconds);
	}
	void setPeriod(unsigned long microseconds) __attribute__((always_inline)) {
		const unsigned long cycles = ((F_CPU / 100000 * microseconds) / 20);
		if (cycles < TIMER1_RESOLUTION) {
			clockSelectBits = _BV(CS10);
			pwmPeriod = cycles;
		}
		else
			if (cycles < TIMER1_RESOLUTION * 8) {
				clockSelectBits = _BV(CS11);
				pwmPeriod = cycles / 8;
			}
			else
				if (cycles < TIMER1_RESOLUTION * 64) {
					clockSelectBits = _BV(CS11) | _BV(CS10);
					pwmPeriod = cycles / 64;
				}
				else
					if (cycles < TIMER1_RESOLUTION * 256) {
						clockSelectBits = _BV(CS12);
						pwmPeriod = cycles / 256;
					}
					else
						if (cycles < TIMER1_RESOLUTION * 1024) {
							clockSelectBits = _BV(CS12) | _BV(CS10);
							pwmPeriod = cycles / 1024;
						}
						else {
							clockSelectBits = _BV(CS12) | _BV(CS10);
							pwmPeriod = TIMER1_RESOLUTION - 1;
						}
		ICR1 = pwmPeriod;
		TCCR1B = _BV(WGM13) | clockSelectBits;
	}

	void start() __attribute__((always_inline)) {
		TCCR1B = 0;
		TCNT1 = 0;		
		resume();
	}
	void stop() __attribute__((always_inline)) {
		TCCR1B = _BV(WGM13);
	}
	void restart() __attribute__((always_inline)) {
		start();
	}
	void resume() __attribute__((always_inline)) {
		TCCR1B = _BV(WGM13) | clockSelectBits;
	}

	void setPwmDuty(char pin, unsigned int duty) __attribute__((always_inline)) {
		unsigned long dutyCycle = pwmPeriod;
		dutyCycle *= duty;
		dutyCycle >>= 10;
		if (pin == TIMER1_A_PIN) OCR1A = dutyCycle;
#ifdef TIMER1_B_PIN
		else if (pin == TIMER1_B_PIN) OCR1B = dutyCycle;
#endif
#ifdef TIMER1_C_PIN
		else if (pin == TIMER1_C_PIN) OCR1C = dutyCycle;
#endif
	}
	void pwm(char pin, unsigned int duty) __attribute__((always_inline)) {
		if (pin == TIMER1_A_PIN) { pinMode(TIMER1_A_PIN, OUTPUT); TCCR1A |= _BV(COM1A1); }
#ifdef TIMER1_B_PIN
		else if (pin == TIMER1_B_PIN) { pinMode(TIMER1_B_PIN, OUTPUT); TCCR1A |= _BV(COM1B1); }
#endif
#ifdef TIMER1_C_PIN
		else if (pin == TIMER1_C_PIN) { pinMode(TIMER1_C_PIN, OUTPUT); TCCR1A |= _BV(COM1C1); }
#endif
		setPwmDuty(pin, duty);
		TCCR1B = _BV(WGM13) | clockSelectBits;
	}
	void pwm(char pin, unsigned int duty, unsigned long microseconds) __attribute__((always_inline)) {
		if (microseconds > 0) setPeriod(microseconds);
		pwm(pin, duty);
	}
	void disablePwm(char pin) __attribute__((always_inline)) {
		if (pin == TIMER1_A_PIN) TCCR1A &= ~_BV(COM1A1);
#ifdef TIMER1_B_PIN
		else if (pin == TIMER1_B_PIN) TCCR1A &= ~_BV(COM1B1);
#endif
#ifdef TIMER1_C_PIN
		else if (pin == TIMER1_C_PIN) TCCR1A &= ~_BV(COM1C1);
#endif
	}

	void attachInterrupt(void (*isr)()) __attribute__((always_inline)) {
		isrCallback = isr;
		TIMSK1 = _BV(TOIE1);
	}
	void attachInterrupt(void (*isr)(), unsigned long microseconds) __attribute__((always_inline)) {
		if (microseconds > 0) setPeriod(microseconds);
		attachInterrupt(isr);
	}
	void detachInterrupt() __attribute__((always_inline)) {
		TIMSK1 = 0;
	}
	static void (*isrCallback)();
	static void isrDefaultUnused();

private:
	static unsigned short pwmPeriod;
	static unsigned char clockSelectBits;

#elif defined(__arm__) && defined(TEENSYDUINO) && (defined(KINETISK) || defined(KINETISL))

#if defined(KINETISK)
#define F_TIMER F_BUS
#elif defined(KINETISL)
#define F_TIMER (F_PLL/2)
#endif


#undef TIMER1_RESOLUTION
#define TIMER1_RESOLUTION 32768

public:

	void initialize(unsigned long microseconds = 1000000) __attribute__((always_inline)) {
		setPeriod(microseconds);
	}
	void setPeriod(unsigned long microseconds) __attribute__((always_inline)) {
		const unsigned long cycles = (F_TIMER / 2000000) * microseconds;

		if (cycles < TIMER1_RESOLUTION) {
			clockSelectBits = 0;
			pwmPeriod = cycles;
		}
		else
			if (cycles < TIMER1_RESOLUTION * 2) {
				clockSelectBits = 1;
				pwmPeriod = cycles >> 1;
			}
			else
				if (cycles < TIMER1_RESOLUTION * 4) {
					clockSelectBits = 2;
					pwmPeriod = cycles >> 2;
				}
				else
					if (cycles < TIMER1_RESOLUTION * 8) {
						clockSelectBits = 3;
						pwmPeriod = cycles >> 3;
					}
					else
						if (cycles < TIMER1_RESOLUTION * 16) {
							clockSelectBits = 4;
							pwmPeriod = cycles >> 4;
						}
						else
							if (cycles < TIMER1_RESOLUTION * 32) {
								clockSelectBits = 5;
								pwmPeriod = cycles >> 5;
							}
							else
								if (cycles < TIMER1_RESOLUTION * 64) {
									clockSelectBits = 6;
									pwmPeriod = cycles >> 6;
								}
								else
									if (cycles < TIMER1_RESOLUTION * 128) {
										clockSelectBits = 7;
										pwmPeriod = cycles >> 7;
									}
									else {
										clockSelectBits = 7;
										pwmPeriod = TIMER1_RESOLUTION - 1;
									}

		uint32_t sc = FTM1_SC;
		FTM1_SC = 0;
		FTM1_MOD = pwmPeriod;
		FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_CPWMS | clockSelectBits | (sc & FTM_SC_TOIE);
	}

	void start() __attribute__((always_inline)) {
		stop();
		FTM1_CNT = 0;
		resume();
	}
	void stop() __attribute__((always_inline)) {
		FTM1_SC = FTM1_SC & (FTM_SC_TOIE | FTM_SC_CPWMS | FTM_SC_PS(7));
	}
	void restart() __attribute__((always_inline)) {
		start();
	}
	void resume() __attribute__((always_inline)) {
		FTM1_SC = (FTM1_SC & (FTM_SC_TOIE | FTM_SC_PS(7))) | FTM_SC_CPWMS | FTM_SC_CLKS(1);
	}

	void setPwmDuty(char pin, unsigned int duty) __attribute__((always_inline)) {
		unsigned long dutyCycle = pwmPeriod;
		dutyCycle *= duty;
		dutyCycle >>= 10;
		if (pin == TIMER1_A_PIN) {
			FTM1_C0V = dutyCycle;
		}
		else if (pin == TIMER1_B_PIN) {
			FTM1_C1V = dutyCycle;
		}
	}
	void pwm(char pin, unsigned int duty) __attribute__((always_inline)) {
		setPwmDuty(pin, duty);
		if (pin == TIMER1_A_PIN) {
			*portConfigRegister(TIMER1_A_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		}
		else if (pin == TIMER1_B_PIN) {
			*portConfigRegister(TIMER1_B_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		}
	}
	void pwm(char pin, unsigned int duty, unsigned long microseconds) __attribute__((always_inline)) {
		if (microseconds > 0) setPeriod(microseconds);
		pwm(pin, duty);
	}
	void disablePwm(char pin) __attribute__((always_inline)) {
		if (pin == TIMER1_A_PIN) {
			*portConfigRegister(TIMER1_A_PIN) = 0;
		}
		else if (pin == TIMER1_B_PIN) {
			*portConfigRegister(TIMER1_B_PIN) = 0;
		}
	}

	void attachInterrupt(void (*isr)()) __attribute__((always_inline)) {
		isrCallback = isr;
		FTM1_SC |= FTM_SC_TOIE;
		NVIC_ENABLE_IRQ(IRQ_FTM1);
	}
	void attachInterrupt(void (*isr)(), unsigned long microseconds) __attribute__((always_inline)) {
		if (microseconds > 0) setPeriod(microseconds);
		attachInterrupt(isr);
	}
	void detachInterrupt() __attribute__((always_inline)) {
		FTM1_SC &= ~FTM_SC_TOIE;
		NVIC_DISABLE_IRQ(IRQ_FTM1);
	}
	static void (*isrCallback)();
	static void isrDefaultUnused();

private:
	static unsigned short pwmPeriod;
	static unsigned char clockSelectBits;

#undef F_TIMER

#elif defined(__arm__) && defined(TEENSYDUINO) && defined(__IMXRT1062__)

public:
	void initialize(unsigned long microseconds = 1000000) __attribute__((always_inline)) {
		setPeriod(microseconds);
	}
	void setPeriod(unsigned long microseconds) __attribute__((always_inline)) {
		uint32_t period = (float)F_BUS_ACTUAL * (float)microseconds * 0.0000005f;
		uint32_t prescale = 0;
		while (period > 32767) {
			period = period >> 1;
			if (++prescale > 7) {
				prescale = 7;	
				period = 32767; 
				break;
			}
		}
		
		FLEXPWM1_FCTRL0 |= FLEXPWM_FCTRL0_FLVL(8); 
		FLEXPWM1_FSTS0 = 0x0008; 
		FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_CLDOK(8);
		FLEXPWM1_SM3CTRL2 = FLEXPWM_SMCTRL2_INDEP;
		FLEXPWM1_SM3CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale);
		FLEXPWM1_SM3INIT = -period;
		FLEXPWM1_SM3VAL0 = 0;
		FLEXPWM1_SM3VAL1 = period;
		FLEXPWM1_SM3VAL2 = 0;
		FLEXPWM1_SM3VAL3 = 0;
		FLEXPWM1_SM3VAL4 = 0;
		FLEXPWM1_SM3VAL5 = 0;
		FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_LDOK(8) | FLEXPWM_MCTRL_RUN(8);
		pwmPeriod = period;
	}

	void start() __attribute__((always_inline)) {
		stop();
		resume();
	}
	void stop() __attribute__((always_inline)) {
		FLEXPWM1_MCTRL &= ~FLEXPWM_MCTRL_RUN(8);
	}
	void restart() __attribute__((always_inline)) {
		start();
	}
	void resume() __attribute__((always_inline)) {
		FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_RUN(8);
	}

	void setPwmDuty(char pin, unsigned int duty) __attribute__((always_inline)) {
		if (duty > 1023) duty = 1023;
		int dutyCycle = (pwmPeriod * duty) >> 10;
		if (pin == TIMER1_A_PIN) {
			FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_CLDOK(8);
			FLEXPWM1_SM3VAL5 = dutyCycle;
			FLEXPWM1_SM3VAL4 = -dutyCycle;
			FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_LDOK(8);
		}
		else if (pin == TIMER1_B_PIN) {
			FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_CLDOK(8);
			FLEXPWM1_SM3VAL3 = dutyCycle;
			FLEXPWM1_SM3VAL2 = -dutyCycle;
			FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_LDOK(8);
		}
	}
	void pwm(char pin, unsigned int duty) __attribute__((always_inline)) {
		setPwmDuty(pin, duty);
		if (pin == TIMER1_A_PIN) {
			FLEXPWM1_OUTEN |= FLEXPWM_OUTEN_PWMB_EN(8);
			IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_01 = 6; 
		}
		else if (pin == TIMER1_B_PIN) {
			FLEXPWM1_OUTEN |= FLEXPWM_OUTEN_PWMA_EN(8);
			IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_00 = 6; 
		}
	}
	void pwm(char pin, unsigned int duty, unsigned long microseconds) __attribute__((always_inline)) {
		if (microseconds > 0) setPeriod(microseconds);
		pwm(pin, duty);
	}
	void disablePwm(char pin) __attribute__((always_inline)) {
		if (pin == TIMER1_A_PIN) {
			IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_01 = 5; 
			FLEXPWM1_OUTEN &= ~FLEXPWM_OUTEN_PWMB_EN(8);
		}
		else if (pin == TIMER1_B_PIN) {
			IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_00 = 5; 
			FLEXPWM1_OUTEN &= ~FLEXPWM_OUTEN_PWMA_EN(8);
		}
	}

	void attachInterrupt(void (*f)()) __attribute__((always_inline)) {
		isrCallback = f;
		attachInterruptVector(IRQ_FLEXPWM1_3, &isr);
		FLEXPWM1_SM3STS = FLEXPWM_SMSTS_RF;
		FLEXPWM1_SM3INTEN = FLEXPWM_SMINTEN_RIE;
		NVIC_ENABLE_IRQ(IRQ_FLEXPWM1_3);
	}
	void attachInterrupt(void (*f)(), unsigned long microseconds) __attribute__((always_inline)) {
		if (microseconds > 0) setPeriod(microseconds);
		attachInterrupt(f);
	}
	void detachInterrupt() __attribute__((always_inline)) {
		NVIC_DISABLE_IRQ(IRQ_FLEXPWM1_3);
		FLEXPWM1_SM3INTEN = 0;
	}
	static void isr(void);
	static void (*isrCallback)();
	static void isrDefaultUnused();

private:
	static unsigned short pwmPeriod;
	static unsigned char clockSelectBits;

#endif
};

extern RTimer Timer1;

#endif