#include <avr/sleep.h>
#include <avr/wdt.h>

#define WDT16MS (0)
#define WDT32MS (1)
#define WDT64MS (2)
#define WDT125MS (3)
#define WDT250MS (4)
#define WDT500MS (5)
#define WDT1S (6)
#define WDT2S (7)
#define WDT4S (32)
#define WDT8S (33)

ISR (WDT_vect) {wdt_disable();}

//pin defines for accessories
#define PIRPIN (8)
#define LEDPIN (9)
#define LDRPOWER (11)
#define LDRPIN (A0)
#define LDRTHRESHOLD 500

void setup(){
  delay(1000);                          //delay to allow reprogramming
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN,LOW);             //LED off
  pinMode(LDRPOWER,OUTPUT);             
  digitalWrite(LDRPOWER,LOW);           //LDR off
}

void loop() {
  static int ledstate=0;                 //keep track of led state, and simple time delay
  digitalWrite(LDRPOWER,HIGH);           //LDR on
  analogRead(LDRPIN);                    //read LDR, needs to be done twice out of power down
  analogRead(LDRPIN);                    //read LDR again
  if(analogRead(LDRPIN)<LDRTHRESHOLD){   //if lights off
    if(digitalRead(PIRPIN)){             //and PIR triggered
      ledstate=5;                        //activate LED with timeout
    }
  }
  digitalWrite(LDRPOWER,LOW);            //LDR off
  
  if(ledstate){
    digitalWrite(LEDPIN,HIGH);            //LED on
    ledstate--;                           //count down timer
    if(ledstate<2){                       //if timer is almost finished, flicker light
      digitalWrite(LEDPIN,LOW);           //LED off    
      wdtsleep(WDT125MS);                 //wait a bit
      digitalWrite(LEDPIN,HIGH);          //LED on
    }    
  }else{
    digitalWrite(LEDPIN,LOW);             //LED off    
  }  
  wdtsleep(WDT4S);                        //sleep till next check
}

void wdtsleep(byte n){                    //use one of the constants above for n
  WDTCSR=_BV(WDCE)|_BV(WDE);
  WDTCSR=_BV(WDIE)| n;
  wdt_reset();
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();
  sleep_cpu ();    
}
