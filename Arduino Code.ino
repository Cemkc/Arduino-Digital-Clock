#include <LiquidCrystal.h>;

#define ONESEC 1000
#define BUTTONHOLDTIME 3000

const int rs=12, en=11, d5=6, d6=5, d7=4, d8=3;
LiquidCrystal lcd(rs, en, d5, d6, d7, d8);

int buzzerPin = 2;

int mode = 0;
int hh = 23, mm = 59, ss = 50; // clock values.
int ahh = 0, amm = 0; // alarm clock values.
unsigned long oneSecFlag;
unsigned long btn1StartTime;
unsigned long btn2StartTime;
bool button1PressedFlag;
bool stateActivaton = true;

bool resetButton2 = false;
bool isAlarmRinging = false;

int tempType = 0;
int tempSensPin = 0;

int btn1 = 10, btn2 = 9, btn3 = 8, btn4 = 7;
int btnPS1, btnPS2, btnPS3, btnPS4;  // Previous button states
int btnCS1 = false, btnCS2 = false, btnCS3 = false, btnCS4 = false; // current button states

int blinkOnOff = 0;

int time_mode = 0; // 0 is am-pm 1 is 24 format
int time_am_pm = 0;
int prev_time_mode;
int time_upper_bound = 13;

int alarm_time_am_pm = 0;
int alarm_time_mode = 0;
bool alarmOnOff = true;

int alarmBlinkState = 0;

const uint16_t t1_load = 0;
const uint16_t t1_comp = 62500;

void setup(){

    Serial.begin(9600);
    lcd.begin(16,2);
  	pinMode(btn1, INPUT);
    pinMode(btn2, INPUT);
    pinMode(btn3, INPUT);
    pinMode(btn4, INPUT);
    pinMode(tempSensPin, INPUT);
    button1PressedFlag = true;
    cli();

    TCCR1A = 0; //Reset timer 1 control reg A

    //Set prescaler to 256
    TCCR1B = 0;
  	TCCR1B |= (1 << CS12); 

    //Reset Timer1 and set compare value
    TCNT1 = t1_load;
    OCR1A = t1_comp;

    //Enable Timer1 compare interrupt
    TIMSK1 = (1 << OCIE1A);

    //Enable global interrupts
    sei();


}

ISR(TIMER1_COMPA_vect){
    TCNT1 = t1_load;
    timeCounter();
}

void loop(){
    isAlarmRinging = false;
    
    if(alarmOnOff == true){        
        alarmCheck();
    }

    if(mode == 0){mainScreen();}
    if(mode == 1){setTimeSetupState();}
    if(mode == 2){alarmSetup();}

    if(hh < 12){
        time_am_pm = 0;
    }
    else{
        time_am_pm = 1;
    }

    delay(200);
}

void mainScreen(){
    prev_time_mode = time_mode;    

    lcdPrintTime(0, 0);
    
    showTemperature(0, 1);

    displayAlarmStatus();

    btnPS1 = btnCS1;
    btnPS2 = btnCS2;
    btnPS3 = btnCS3;
    btnPS4 = btnCS4;

    btnCS1 = digitalRead(btn1);
    btnCS2 = digitalRead(btn2);
    btnCS3 = digitalRead(btn3);
    btnCS4 = digitalRead(btn4);

    if(stateActivaton){
        if(btnCS1 == LOW){
            btn1StartTime = millis();
            
            if(btnCS1 != btnPS1){
                time_mode = !time_mode;
            }
        }

        if(btnCS2 == LOW){
            btn2StartTime = millis();

            if(btnCS2 != btnPS2){
                alarmOnOff = !alarmOnOff;
                lcd.setCursor(7, 1);
                lcd.print("         ");
            }
        }
    
        if(millis() - btn1StartTime > (unsigned long) BUTTONHOLDTIME){
            mode = 1;
            stateActivaton = false;
            lcd.clear();
        }
        else if(millis() - btn2StartTime > (unsigned long) BUTTONHOLDTIME){
            mode = 2;
            stateActivaton = false;
            lcd.clear();
        }
    }
    else{
        btn1StartTime = millis();
        btn2StartTime = btn1StartTime;
    }
    
    if(btnCS1 != btnPS1){
        if(btnCS1 == HIGH){
            btn1StartTime = millis();
        }
        else{
            stateActivaton = true;
        }
    }

    if(btnCS2 != btnPS2){
        if(btnCS2 == HIGH){
            btn2StartTime = millis();
        }
        else{
            stateActivaton = true;
        }
    }

    if(btnCS3 != btnPS3){
        if(btnCS3 == LOW){
            tempType = !tempType;
            lcd.setCursor(0, 1);
            lcd.print("      ");
        }
    }

    if(btnCS4 != btnPS4){
        if(btnCS4 == LOW){
            if(isAlarmRinging){
                amm = amm + 5;
            }
        }
    }

    if(time_mode != prev_time_mode){
        lcd.clear();
    }
}

int timeBlinkState = 0;

void setTimeSetupState(){
    timeSetupDisplay(timeBlinkState);

    btnPS1 = btnCS1;
    btnPS2 = btnCS2;
    btnPS3 = btnCS3;
    btnPS4 = btnCS4;

    btnCS1 = digitalRead(btn1);
    btnCS2 = digitalRead(btn2);
    btnCS3 = digitalRead(btn3);
    btnCS4 = digitalRead(btn4);

    if(btnCS1 != btnPS1){
        if(btnCS1 == HIGH){
            lcd.clear();
            mode = 0;
        }
    }
    if(btnCS2 != btnPS2){
        if(btnCS2 == HIGH){
            if(timeBlinkState == 0){
                timeBlinkState = 1;
            }
            else if(timeBlinkState == 1){
                timeBlinkState = 2;
            }
            else{
                timeBlinkState = £0;
            }
        }
    }
    if(btnCS3 != btnPS3){
        if(btnCS3 == HIGH){
            if(timeBlinkState == 0) hh++;
            else if(timeBlinkState == 1) mm++;
            else{
                lcd.clear();
                hh = hh + 12;
            }
        }
    }
}

void lcdPrintTime(int column, int row){
    lcd.setCursor(column, row);
    if(time_mode == 0){
        if(hh == 0 || hh == 12){
            printTime(12);
        }
        else{
            if(time_am_pm == 0){
                printTime(hh);
            }
            else{
                printTime(hh - 12);
            }
        }
    }
    else{
        printTime(hh);
    }
    
    lcd.print(":");
    printTime(mm); lcd.print(":");
    printTime(ss); 

    if(time_mode == 0){
        if(time_am_pm == 0){
            lcd.print("AM");
        }else{
            lcd.print("PM");
        }
    }
}

void alarmSetup(){
    
    alarmClockDisplay();

    btnPS1 = btnCS1;
    btnPS2 = btnCS2;
    btnPS3 = btnCS3;
    btnPS4 = btnCS4;

    btnCS1 = digitalRead(btn1);
    btnCS2 = digitalRead(btn2);
    btnCS3 = digitalRead(btn3);
    btnCS4 = digitalRead(btn4);

    if(btnCS1 != btnPS1 && btnCS1 == LOW){
        lcd.clear();
        resetButton2 = false;
        alarm_time_mode = time_mode;
        mode = 0;
    }

    if(resetButton2){
        if((btnCS2 != btnPS2) && (btnCS2 == HIGH)){
            if(alarmBlinkState == 0){
                alarmBlinkState = 1;
            }
            else if (alarmBlinkState == 1)
            {
                if(time_mode == 0){
                    alarmBlinkState = 2;
                }
                else{
                    alarmBlinkState = 0;
                }
            }
            else{
                alarmBlinkState = 0;
            }            
        }
        if(btnCS3 != btnPS3 && btnCS3 == HIGH){
            if(alarmBlinkState == 0){
                amm++;
                if(amm >= 60){
                    amm = 0;
                }
            }
            else if(alarmBlinkState == 1){
                Serial.println(ahh);
                ahh++;
                if(ahh >= 24){
                    ahh = 0;
                }
            }
            else{
                alarm_time_am_pm = !alarm_time_am_pm;
            }
        }
    }

    if(btnCS2 != btnPS2 && btnCS2 == HIGH){
        resetButton2 = true;
    }

}

int alarmBlinkSwitch = 0;

void alarmClockDisplay(){
    lcd.setCursor(0, 0);
    lcd.print("Set Alarm:");
    lcd.setCursor(0, 1);

    if(time_mode == 0){
        if(ahh == 0 || ahh == 12){
            printTime(12);
        }
        else{
            if(ahh < 12){
                printTime(ahh);
            }
            else{
                printTime(ahh - 12);
            }
        }
    }
    else{
        printTime(ahh);
    }
    
    lcd.print(":");
    printTime(amm); 

    if(time_mode == 0){
        if(alarm_time_am_pm == 0){
            lcd.print("AM");
        }
        else{
            lcd.print("PM");
        }
    }

    if(alarmBlinkState == 0){
        lcd.setCursor(3, 1);
        if(alarmBlinkSwitch == 1){
            lcd.print("  ");
        }
    }
    else if(alarmBlinkState == 1){
        lcd.setCursor(0, 1);
        if(alarmBlinkSwitch == 1){
            lcd.print("  ");
        }
    }
    else{
        lcd.setCursor(5, 1);
        if(alarmBlinkSwitch == 1){
            lcd.print("  ");
        }
    }

    delay(200);

    alarmBlinkSwitch = (alarmBlinkSwitch + 1) % 2;
    
}

void displayAlarmStatus(){
    if(alarmOnOff == true){
        lcd.setCursor(8, 1);
        lcd.print("ALARM:ON");
    }
    else{
        lcd.setCursor(7, 1);
        lcd.print("ALARM:OFF");
    }
}


int celcius = 0;
int fahrenheit = 0; 
int prevTemp = 0;

void showTemperature(int column, int row){
    prevTemp = celcius;
    celcius = map(((analogRead(tempSensPin)-20)*3.04),0,1023,-40,125);
    fahrenheit = ((celcius*9) / 5 + 32);

    lcd.setCursor(column, row);
    if(prevTemp != celcius) lcd.print("      ");
    
    lcd.setCursor(column, row);
    if (tempType == 0) lcd.print(celcius);	// 0=C - PRINT LCD CELSIUS 
    if (tempType == 1) lcd.print(fahrenheit);	// 1=F - PRINT LCD FAHRENHEIT 

    lcd.print((char)0xB0);	// ° symbol, (char)223
    
    if (tempType == 0) lcd.print("C");
    if (tempType == 1) lcd.print("F");
}

void timeCounter(){
    ss++;

    if(ss == 60){
        ss = 0;
        mm++;
    }
    if(mm == 60){
        mm = 0;
        hh++;
    }
    if(hh >= 24){
        hh = 0;
    }
    
}

void printTime(int x){
    if(x <= 9){
        lcd.print(0);
        lcd.print(x);
    }
    else{
        lcd.print(x);
    }
}



void timeSetupDisplay(int tbs){
   lcdPrintTime(3, 1);

    if(tbs == 0){
        lcd.setCursor(3, 1);
        if(blinkOnOff == 0){
        }
        else{
            lcd.print("  ");
        }
    }
    else if(tbs == 1){
        lcd.setCursor(6, 1);
        if(blinkOnOff == 0){
            if(mm <= 9){
                lcd.print("0");
                lcd.print(mm);
            }
            else{
                lcd.print(mm);
            }
        }
        else{
            lcd.print("  ");
        }
    }
    else if(tbs == 2){
        lcd.setCursor(11, 1);
        if(blinkOnOff){
            if(time_am_pm == 0) lcd.print("AM");
            else lcd.print("PM");
        }
        else{
            lcd.print("  ");
        }
    }

    delay(200);

    blinkOnOff = (blinkOnOff+1) % 2;

    lcd.setCursor(0,0);
    lcd.print("Set Time:");
}

void alarmCheck(){
    if(alarmOnOff){
        if(alarm_time_mode == 0){
            if(alarm_time_am_pm == 0){
                if(ahh == hh && mm == amm){
                    tone(buzzerPin, 1000, 5);
                    isAlarmRinging = true;
                }
            }
            else{
                if((ahh + 12) == hh && mm == amm){
                    tone(buzzerPin, 1000, 5);
                    isAlarmRinging = true;
                }
            }
        }
        if(alarm_time_mode == 1){
            if(hh == ahh && mm == amm){
                tone(buzzerPin, 1000, 5);
                isAlarmRinging = true;
            }
        }
    }
}