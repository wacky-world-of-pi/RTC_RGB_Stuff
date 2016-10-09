// this example has the DeadOn RTC hooked up according to this: http://www.l8ter.com/?p=375
// The second half of the example that sets time based on compine time isn't working
// there's an LED on pin 10 that blinks if the number of seconds has changed, letting me know that the clock is running
// gotta add the login to this to light a "neopixel" led at certain times. or move th e
//SPI.h stuff to my other sync_lace sketches...

#include <SPI.h>
const int  cs=8; //chip select 
int hours;
int minutes;
int seconds;
const int ledPin =  13;
//all the RGB LED example stuff in here comes from "strandtestsingleled" sketch
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, 6, NEO_GRB + NEO_KHZ800);

int sameseconds = 0; //this might be used to determine whether the second has changed
int variable = 0;

void setup() {
  Serial.begin(9600);
  RTC_init();
  //day(1-31), month(1-12), year(0-99), hour(0-23), minute(0-59), second(0-59)
  SetTimeDate(8,10,16,23,59,0); 

  //some RGB LED stuff
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

void loop() {

  Serial.println(ReadTimeDate());

  Serial.print(hours);
  Serial.println(" hours");

  Serial.print(minutes);
  Serial.println(" minutes");
  Serial.print(seconds);
  Serial.println(" seconds");
if (variable == seconds){
    //strip.setPixelColor format is (pixel#, red, green, blue);
    strip.setPixelColor(0,100,0,0);  
    strip.show(); 
    delay(10);
    strip.setPixelColor(0,0,0,0);  
    strip.show(); // Initialize all pixels to 'off' 
  variable++;
  
  if (variable == 60){
  variable = 0;
  }
  
}

////  if (variable != seconds){
//    digitalWrite (13, LOW);
//    delay(10);
//    digitalWrite (13, LOW);
////  }
////  delay(900);
//sameseconds = variable;
//
//  if (sameseconds == seconds){
//    //strip.setPixelColor format is (pixel#, red, green, blue);
//    strip.setPixelColor(0,0,0,100);  
//    strip.show(); 
//    delay(10);
//    strip.setPixelColor(0,0,0,0);  
//    strip.show(); // Initialize all pixels to 'off' 
//  }
//  else {
//       //strip.setPixelColor format is (pixel#, red, green, blue);
//    strip.setPixelColor(0,0,0,0);  
//    strip.show(); // Initialize all pixels to 'off' 
//  }
  
}
//=====================================
int RTC_init(){ 
  pinMode(cs,OUTPUT); // chip select
  // start the SPI library:
  SPI.begin();
  SPI.setBitOrder(MSBFIRST); 
  SPI.setDataMode(SPI_MODE3); // both mode 1 & 3 should work 
  //set control register 
  digitalWrite(cs, LOW);  
  SPI.transfer(0x8E);
  SPI.transfer(0x60); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
  digitalWrite(cs, HIGH);
  delay(10);
}
//=====================================
int SetTimeDate(int d, int mo, int y, int h, int mi, int s){ 
  int TimeDate [7]={
    s,mi,h,0,d,mo,y    };
  for(int i=0; i<=6;i++){
    if(i==3)
      i++;
    int b= TimeDate[i]/10;
    int a= TimeDate[i]-b*10;
    if(i==2){
      if (b==2)
        b=B00000010;
      else if (b==1)
        b=B00000001;
    }	
    TimeDate[i]= a+(b<<4);

    digitalWrite(cs, LOW);
    SPI.transfer(i+0x80); 
    SPI.transfer(TimeDate[i]);        
    digitalWrite(cs, HIGH);
  }

}
//=====================================
String ReadTimeDate(){
  String temp;
  int TimeDate [7]; //second,minute,hour,null,day,month,year		
  for(int i=0; i<=6;i++){
    if(i==3)
      i++;
    digitalWrite(cs, LOW);
    SPI.transfer(i+0x00); 
    unsigned int n = SPI.transfer(0x00);        
    digitalWrite(cs, HIGH);
    int a=n & B00001111;    
    if(i==2){	
      int b=(n & B00110000)>>4; //24 hour mode
      if(b==B00000010)
        b=20;        
      else if(b==B00000001)
        b=10;
      TimeDate[i]=a+b;
    }
    else if(i==4){
      int b=(n & B00110000)>>4;
      TimeDate[i]=a+b*10;
    }
    else if(i==5){
      int b=(n & B00010000)>>4;
      TimeDate[i]=a+b*10;
    }
    else if(i==6){
      int b=(n & B11110000)>>4;
      TimeDate[i]=a+b*10;
    }
    else{	
      int b=(n & B01110000)>>4;
      TimeDate[i]=a+b*10;	
    }
  }
  temp.concat(TimeDate[4]);
  temp.concat("/") ;
  temp.concat(TimeDate[5]);
  temp.concat("/") ;
  temp.concat(TimeDate[6]);
  temp.concat("     ") ;
  temp.concat(TimeDate[2]);
  temp.concat(":") ;
  temp.concat(TimeDate[1]);
  temp.concat(":") ;
  temp.concat(TimeDate[0]);
  hours = TimeDate[2];
  minutes = TimeDate[1];
  seconds = TimeDate[0];
  return(temp);
}



