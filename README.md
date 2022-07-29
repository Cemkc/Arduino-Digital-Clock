# Digital Clock Project

## Introduction

This project is an implementation of a digital clock.

## Features:

1. Adjustable to display clock with both 24 and am-pm format.
2. Displays temperature in celsius and fahrenheit.
3. Alarm system that is configurable in both time formats.
4. Alarm snooze for 5 minutes.

## Code:

1. Written in C++, used library is Liquid Crystal (for lcd display).
2. Total of 553 lines of code.
3. Used TCNT1 and OCR1A registers to achieve precise timing by generating
internal interrupts at each second.
4. The program consist of 3 main modes in each iteration either one of them is
executed. The modes are; Main Screen (Time display screen), Set Time
Setup State (Time Configuration Screen), Alarm Setup (Alarm
Configuration).

## Circuit:

1. Components: Arduino UNO R3, LCD 16x2, 4x Button, Temperature Sensor,
Piezo Buzzer, Potentiometer, 5x Resistor.
2. Button pins: 10, 9, 8, 7, 6; Temperature pin: A0; Piezo Buzzer: 2.

## User Manual:

**Button 1**

- Press to change the format between am-pm and 24-hour format.

- Hold 3 seconds to set the time; in setup menu press button 2 to iterate
through hour, minute and am-pm. Use button 3 to change the value.

**Button 2**

- Press to set the alarm on and off.

- Hold 3 seconds to set the alarm to a specified time. Use button 2 to
iterate through hour, minute and am-pm. Use button 3 to change the
value

**Button 3**

- Press to switch temperature between Fahrenheit and Celsius.
  
**Button 4**

- Snooze the alarm for 5 minutes when the alarm is triggered.
