from machine import Pin, PWM
from time import sleep

# Initialize GPIO pins for IN1 and IN2 for direction control
in1 = Pin(14, Pin.OUT)
in2 = Pin(15, Pin.OUT)

# Initialize PWM for speed control
speed = PWM(Pin(16))
speed.freq(1000)

def motor_forward():
    in1.high()
    in2.low()
    speed.duty_u16(32768)  # Adjust this value for speed (0-65535)

def motor_backward():
    in1.low()
    in2.high()
    speed.duty_u16(32768)  # Adjust this value for speed

def motor_stop():
    in1.low()
    in2.low()
    speed.duty_u16(0)

# Test the motor control
try:
    while True:
        motor_forward()
        sleep(2)
        motor_stop()
        sleep(1)
        motor_backward()
        sleep(2)
        motor_stop()
        sleep(1)
except KeyboardInterrupt:
    motor_stop()