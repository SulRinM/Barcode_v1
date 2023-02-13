#include "RStepper.h"

#if 0

void dump(uint8_t* p, int l)
{
    int i;

    for (i = 0; i < l; i++)
    {
	Serial.print(p[i], HEX);
	Serial.print(" ");
    }
    Serial.println("");
}
#endif

void RStepper::moveTo(long absolute)
{
    if (_targetPos != absolute)
    {
	_targetPos = absolute;
	computeNewSpeed();
    }
}

void RStepper::move(long relative)
{
    moveTo(_currentPos + relative);
}

boolean RStepper::runSpeed()
{
    if (!_stepInterval)
	return false;

    unsigned long time = micros();   
    if (time - _lastStepTime >= _stepInterval)
    {
	if (_direction == DIRECTION_CW)
	{
	    _currentPos += 1;
	}
	else
	{
	    _currentPos -= 1;
	}
	step(_currentPos);

	_lastStepTime = time; 

	return true;
    }
    else
    {
	return false;
    }
}

long RStepper::distanceToGo()
{
    return _targetPos - _currentPos;
}

long RStepper::targetPosition()
{
    return _targetPos;
}

long RStepper::currentPosition()
{
    return _currentPos;
}

void RStepper::setCurrentPosition(long position)
{
    _targetPos = _currentPos = position;
    _n = 0;
    _stepInterval = 0;
    _speed = 0.0;
}

void RStepper::computeNewSpeed()
{
    long distanceTo = distanceToGo(); 

    long stepsToStop = (long)((_speed * _speed) / (2.0 * _acceleration)); 

    if (distanceTo == 0 && stepsToStop <= 1)
    {
	_stepInterval = 0;
	_speed = 0.0;
	_n = 0;
	return;
    }

    if (distanceTo > 0)
    {
	if (_n > 0)
	{
	    if ((stepsToStop >= distanceTo) || _direction == DIRECTION_CCW)
		_n = -stepsToStop;
	}
	else if (_n < 0)
	{
	    
	    if ((stepsToStop < distanceTo) && _direction == DIRECTION_CW)
		_n = -_n; 
	}
    }
    else if (distanceTo < 0)
    {
	if (_n > 0)
	{   
	    if ((stepsToStop >= -distanceTo) || _direction == DIRECTION_CW)
		_n = -stepsToStop; 
	}
	else if (_n < 0)
	{
	    if ((stepsToStop < -distanceTo) && _direction == DIRECTION_CCW)
		_n = -_n; 
	}
    }

    if (_n == 0)
    {
	_cn = _c0;
	_direction = (distanceTo > 0) ? DIRECTION_CW : DIRECTION_CCW;
    }
    else
    {
	_cn = _cn - ((2.0 * _cn) / ((4.0 * _n) + 1)); 
	_cn = max(_cn, _cmin); 
    }
    _n++;
    _stepInterval = _cn;
    _speed = 1000000.0 / _cn;
    if (_direction == DIRECTION_CCW)
	_speed = -_speed;

#if 0
    Serial.println(_speed);
    Serial.println(_acceleration);
    Serial.println(_cn);
    Serial.println(_c0);
    Serial.println(_n);
    Serial.println(_stepInterval);
    Serial.println(distanceTo);
    Serial.println(stepsToStop);
    Serial.println("-----");
#endif
}

boolean RStepper::run()
{
    if (runSpeed())
	computeNewSpeed();
    return _speed != 0.0 || distanceToGo() != 0;
}

RStepper::RStepper(uint8_t interface, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, bool enable)
{
    _interface = interface;
    _currentPos = 0;
    _targetPos = 0;
    _speed = 0.0;
    _maxSpeed = 1.0;
    _acceleration = 0.0;
    _sqrt_twoa = 1.0;
    _stepInterval = 0;
    _minPulseWidth = 1;
    _enablePin = 0xff;
    _lastStepTime = 0;
    _pin[0] = pin1;
    _pin[1] = pin2;
    _pin[2] = pin3;
    _pin[3] = pin4;
    _enableInverted = false;
    
    _n = 0;
    _c0 = 0.0;
    _cn = 0.0;
    _cmin = 1.0;
    _direction = DIRECTION_CCW;

    int i;
    for (i = 0; i < 4; i++)
	_pinInverted[i] = 0;
    if (enable)
	enableOutputs();
    setAcceleration(1);
}

RStepper::RStepper(void (*forward)(), void (*backward)())
{
    _interface = 0;
    _currentPos = 0;
    _targetPos = 0;
    _speed = 0.0;
    _maxSpeed = 1.0;
    _acceleration = 0.0;
    _sqrt_twoa = 1.0;
    _stepInterval = 0;
    _minPulseWidth = 1;
    _enablePin = 0xff;
    _lastStepTime = 0;
    _pin[0] = 0;
    _pin[1] = 0;
    _pin[2] = 0;
    _pin[3] = 0;
    _forward = forward;
    _backward = backward;

    _n = 0;
    _c0 = 0.0;
    _cn = 0.0;
    _cmin = 1.0;
    _direction = DIRECTION_CCW;

    int i;
    for (i = 0; i < 4; i++)
	_pinInverted[i] = 0;
    setAcceleration(1);
}

void RStepper::setMaxSpeed(float speed)
{
    if (speed < 0.0)
       speed = -speed;
    if (_maxSpeed != speed)
    {
	_maxSpeed = speed;
	_cmin = 1000000.0 / speed;
	if (_n > 0)
	{
	    _n = (long)((_speed * _speed) / (2.0 * _acceleration)); 
	    computeNewSpeed();
	}
    }
}

float   RStepper::maxSpeed()
{
    return _maxSpeed;
}

void RStepper::setAcceleration(float acceleration)
{
    if (acceleration == 0.0)
	return;
    if (acceleration < 0.0)
      acceleration = -acceleration;
    if (_acceleration != acceleration)
    {
	_n = _n * (_acceleration / acceleration);
	_c0 = 0.676 * sqrt(2.0 / acceleration) * 1000000.0; 
	_acceleration = acceleration;
	computeNewSpeed();
    }
}

void RStepper::setSpeed(float speed)
{
    if (speed == _speed)
        return;
    speed = constrain(speed, -_maxSpeed, _maxSpeed);
    if (speed == 0.0)
	_stepInterval = 0;
    else
    {
	_stepInterval = fabs(1000000.0 / speed);
	_direction = (speed > 0.0) ? DIRECTION_CW : DIRECTION_CCW;
    }
    _speed = speed;
}

float RStepper::speed()
{
    return _speed;
}

void RStepper::step(long step)
{
    switch (_interface)
    {
        case FUNCTION:
            step0(step);
            break;

	case DRIVER:
	    step1(step);
	    break;
    
	case FULL2WIRE:
	    step2(step);
	    break;
    
	case FULL3WIRE:
	    step3(step);
	    break;  

	case FULL4WIRE:
	    step4(step);
	    break;  

	case HALF3WIRE:
	    step6(step);
	    break;  
		
	case HALF4WIRE:
	    step8(step);
	    break;  
    }
}

void RStepper::setOutputPins(uint8_t mask)
{
    uint8_t numpins = 2;
    if (_interface == FULL4WIRE || _interface == HALF4WIRE)
	numpins = 4;
    else if (_interface == FULL3WIRE || _interface == HALF3WIRE)
	numpins = 3;
    uint8_t i;
    for (i = 0; i < numpins; i++)
	digitalWrite(_pin[i], (mask & (1 << i)) ? (HIGH ^ _pinInverted[i]) : (LOW ^ _pinInverted[i]));
}

void RStepper::step0(long step)
{
    (void)(step); // Unused
    if (_speed > 0)
	_forward();
    else
	_backward();
}

void RStepper::step1(long step)
{
    (void)(step); 

    setOutputPins(_direction ? 0b10 : 0b00); 
    setOutputPins(_direction ? 0b11 : 0b01); 
    delayMicroseconds(_minPulseWidth);
    setOutputPins(_direction ? 0b10 : 0b00); 
}

void RStepper::step2(long step)
{
    switch (step & 0x3)
    {
	case 0: 
	    setOutputPins(0b10);
	    break;

	case 1:
	    setOutputPins(0b11);
	    break;

	case 2: 
	    setOutputPins(0b01);
	    break;

	case 3: 
	    setOutputPins(0b00);
	    break;
    }
}

void RStepper::step3(long step)
{
    switch (step % 3)
    {
	case 0:  
	    setOutputPins(0b100);
	    break;

	case 1:   
	    setOutputPins(0b001);
	    break;

	case 2:  
	    setOutputPins(0b010);
	    break;
	    
    }
}

void RStepper::step4(long step)
{
    switch (step & 0x3)
    {
	case 0:   
	    setOutputPins(0b0101);
	    break;

	case 1:   
	    setOutputPins(0b0110);
	    break;

	case 2:   
	    setOutputPins(0b1010);
	    break;

	case 3:    
	    setOutputPins(0b1001);
	    break;
    }
}

void RStepper::step6(long step)
{
    switch (step % 6)
    {
	case 0:   
	    setOutputPins(0b100);
            break;
	    
        case 1:  
	    setOutputPins(0b101);
            break;
	    
	case 2:   
	    setOutputPins(0b001);
            break;
	    
        case 3:   
	    setOutputPins(0b011);
            break;
	    
	case 4:    
	    setOutputPins(0b010);
            break;
	    
	case 5:    
	    setOutputPins(0b110);
            break;
	    
    }
}

void RStepper::step8(long step)
{
    switch (step & 0x7)
    {
	case 0:   
	    setOutputPins(0b0001);
            break;
	    
        case 1:  
	    setOutputPins(0b0101);
            break;
	    
	case 2:   
	    setOutputPins(0b0100);
            break;
	    
        case 3:   
	    setOutputPins(0b0110);
            break;
	    
	case 4:   
	    setOutputPins(0b0010);
            break;
	    
        case 5:   
	    setOutputPins(0b1010);
            break;
	    
	case 6:    
	    setOutputPins(0b1000);
            break;
	    
        case 7:   
	    setOutputPins(0b1001);
            break;
    }
}
    
void    RStepper::disableOutputs()
{   
    if (! _interface) return;

    setOutputPins(0);
    if (_enablePin != 0xff)
    {
        pinMode(_enablePin, OUTPUT);
        digitalWrite(_enablePin, LOW ^ _enableInverted);
    }
}

void    RStepper::enableOutputs()
{
    if (! _interface) 
	return;

    pinMode(_pin[0], OUTPUT);
    pinMode(_pin[1], OUTPUT);
    if (_interface == FULL4WIRE || _interface == HALF4WIRE)
    {
        pinMode(_pin[2], OUTPUT);
        pinMode(_pin[3], OUTPUT);
    }
    else if (_interface == FULL3WIRE || _interface == HALF3WIRE)
    {
        pinMode(_pin[2], OUTPUT);
    }

    if (_enablePin != 0xff)
    {
        pinMode(_enablePin, OUTPUT);
        digitalWrite(_enablePin, HIGH ^ _enableInverted);
    }
}

void RStepper::setMinPulseWidth(unsigned int minWidth)
{
    _minPulseWidth = minWidth;
}

void RStepper::setEnablePin(uint8_t enablePin)
{
    _enablePin = enablePin;
    if (_enablePin != 0xff)
    {
        pinMode(_enablePin, OUTPUT);
        digitalWrite(_enablePin, HIGH ^ _enableInverted);
    }
}

void RStepper::setPinsInverted(bool directionInvert, bool stepInvert, bool enableInvert)
{
    _pinInverted[0] = stepInvert;
    _pinInverted[1] = directionInvert;
    _enableInverted = enableInvert;
}

void RStepper::setPinsInverted(bool pin1Invert, bool pin2Invert, bool pin3Invert, bool pin4Invert, bool enableInvert)
{    
    _pinInverted[0] = pin1Invert;
    _pinInverted[1] = pin2Invert;
    _pinInverted[2] = pin3Invert;
    _pinInverted[3] = pin4Invert;
    _enableInverted = enableInvert;
}

void RStepper::runToPosition()
{
    while (run())
	;
}

boolean RStepper::runSpeedToPosition()
{
    if (_targetPos == _currentPos)
	return false;
    if (_targetPos >_currentPos)
	_direction = DIRECTION_CW;
    else
	_direction = DIRECTION_CCW;
    return runSpeed();
}

void RStepper::runToNewPosition(long position)
{
    moveTo(position);
    runToPosition();
}

void RStepper::stop()
{
    if (_speed != 0.0)
    {    
	long stepsToStop = (long)((_speed * _speed) / (2.0 * _acceleration)) + 1; 
	if (_speed > 0)
	    move(stepsToStop);
	else
	    move(-stepsToStop);
    }
}

bool RStepper::isRunning()
{
    return !(_speed == 0.0 && _targetPos == _currentPos);
}