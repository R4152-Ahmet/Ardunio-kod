#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "math.h"

#define ENA   3
#define IN1   7
#define IN2   8
#define IN3   9
#define IN4   10
#define ENB   11

#define Kp  15//40
#define Kd  0.05//0.05
#define Ki  10//40
#define sampleTime  0.005
#define targetAngle -2.5//-2.5

MPU6050 mpu;

int16_t accY, accZ, gyroX;
volatile int motorPower, gyroRate;
volatile float accAngle, gyroAngle, currentAngle, prevAngle=0, error, prevError=0, errorSum=0;
volatile byte count=0;
int distanceCm;

void setMotors(int leftMotor, int rightMotor) {
  if(leftMotor >= 0) {
    analogWrite(ENA, leftMotor);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  else {
    analogWrite(ENA, abs(leftMotor));
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  if(rightMotor >= 0) {
    analogWrite(ENB, rightMotor);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  else {
    analogWrite(ENB, abs(rightMotor));
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
}

void init_PID() {  
  // initialize Timer1
  cli();          // disable global interrupts
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B    
  // set compare match register to set sample time 5ms
  OCR1A = 9999;    
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for prescaling by 8
  TCCR1B |= (1 << CS11);
  // leftMotorble timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();          // leftMotorble global interrupts
}

void setup() {
  // set the motor control and PWM pins to output mode
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  // set the status LED to output mode 
  pinMode(13, OUTPUT);
  // initialize the MPU6050 and set offset values
  mpu.initialize();
  mpu.setYAccelOffset(-2503);//1593
  mpu.setZAccelOffset(9900);//963
  mpu.setXGyroOffset(40);//40
  // initialize PID sampling loop
  init_PID();
//Serial.begin(9600);
}

void loop() {
  // Serial.println(motorPower);
   // read acceleration and gyroscope values
  accY = mpu.getAccelerationY();
  accZ = mpu.getAccelerationZ();  
  gyroX = mpu.getRotationX();
  // set motor power after constraining it
  motorPower = constrain(motorPower, -255, 255);
  setMotors(motorPower, motorPower);
  // measure distance every 100 milliseconds

}
// The ISR will be called every 5 milliseconds
ISR(TIMER1_COMPA_vect)
{
  // calculate the angle of inclination
  accAngle = atan2(accY, accZ)*RAD_TO_DEG;
  gyroRate = map(gyroX, -32768, 32767, -250, 250);
  gyroAngle = (float)gyroRate*sampleTime;  
  currentAngle = 0.9934*(prevAngle + gyroAngle) + 0.0066*(accAngle);
  
  error = currentAngle - targetAngle;
  errorSum = errorSum + error;  
  errorSum = constrain(errorSum, -300, 300);//-300 300
  //calculate output from P, I and D values
  motorPower = Kp*(error) + Ki*(errorSum)*sampleTime - Kd*(currentAngle-prevAngle)/sampleTime;
  prevAngle = currentAngle;
  // toggle the led on pin13 every second
  count++;
  if(count == 200)  {
    count = 0;
    digitalWrite(13, !digitalRead(13));
  }
  
}
