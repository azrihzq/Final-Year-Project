#include <NewPing.h>
#define trigPin_1 5
#define echoPin_1 4
#define trigPin_2 7
#define echoPin_2 6
#define MAX_DISTANCE 400
unsigned long counter_1, counter_2, counter_3, counter_4, counter_5, current_count;
byte last_CH1_state, last_CH2_state, last_CH3_state, last_CH4_state, last_CH5_state;
unsigned long current_time, esc_loop_timer, timer_now, myTime;
unsigned long timer_channel_1, timer_channel_2, timer_channel_3, timer_channel_4, timer_channel_5;
float distance1, distance2;
int ROLL;
int PITCH;
int THROTTLE;
int YAW;
int FLIGHTMODE;

NewPing sonar1(trigPin_1, echoPin_1, MAX_DISTANCE);
NewPing sonar2(trigPin_2, echoPin_2, MAX_DISTANCE);

void setup()
{    
  Serial.begin(9600);
  SREG |= 0b10000000; //enable global interrupt
  DDRC &= 0b00011111; //port C as input
  PCICR |= 0b00000011; //PCIE0 & PCIE1
  PCMSK1 |= 0b00011111; //PCINT 8-12 ENABLE
  DDRB |= 0b00101111; //PORT B 0,1,2,3,5 AS OUTPUT
}

void loop()
{
  myTime = micros();
  
  //OUTPUT SIGNAL
 PORTB |= 0b00101111;
 timer_now = micros();
 timer_channel_1 = ROLL + timer_now;
 timer_channel_2 = PITCH + timer_now;
 timer_channel_3 = THROTTLE + timer_now;
 timer_channel_4 = YAW + timer_now; 
 timer_channel_5 = FLIGHTMODE + timer_now;
 
 while (PORTB > 0)
 {
   esc_loop_timer =
    micros();
   if (timer_channel_1 <= esc_loop_timer) PORTB &= 0b11111110;
   if (timer_channel_2 <= esc_loop_timer) PORTB &= 0b11111101;
   if (timer_channel_3 <= esc_loop_timer) PORTB &= 0b11111011;
   if (timer_channel_4 <= esc_loop_timer) PORTB &= 0b11110111;
   if (timer_channel_5 <= esc_loop_timer) PORTB &= 0b11011111;
 }
  Serial.print('\t');
  Serial.print(distance1);
  Serial.print('\t');
  Serial.print(distance2);
  Serial.print('\t');
  Serial.print(ROLL);
  Serial.print('\t');
  Serial.print(PITCH);
  Serial.print('\t');
  Serial.print(THROTTLE);
  Serial.print('\t');
  Serial.print(YAW);
  Serial.print('\t');
  Serial.println(FLIGHTMODE);

  if (FLIGHTMODE > 1500)
  {
   distance1 = sonar1.ping_cm();
   distance2 = sonar2.ping_cm();

   if (distance1 <= 100 && (distance2 > 100 || distance2 == 0))
   {
    THROTTLE += 150;
   }
   else if (distance2 <= 100 && (distance1 > 100 || distance1 == 0))
   {
    THROTTLE -= 150;
   }
   else
   {
    THROTTLE;
   }
  }
  else
  {
    distance1 = 0;
    distance2 = 0;
  }


  while (myTime + 32000 > micros());
}

//   The Below Codes Reads the PWM value of Receiver
ISR(PCINT1_vect)
{
  current_count = micros();

//CHANNEL 1 : ROLL//
  
  if(PINC & B00000001)
  {                              
    if(last_CH1_state == 0)
    {                         
      last_CH1_state = 1;                           
      counter_1 = current_count;                     
    }
  }
  else if(last_CH1_state == 1)
  {                        
    last_CH1_state = 0;                             
    ROLL = current_count - counter_1;  
  }


//CHANNEL 2 : PITCH//
  
  if(PINC & B00000010 )
  {                                                                          
    if(last_CH2_state == 0)
    {                                               
      last_CH2_state = 1;                                                   
      counter_2 = current_count;                                             
    }
  }
  else if(last_CH2_state == 1)
  {                                           
     last_CH2_state = 0;                                                     
     PITCH = current_count - counter_2;                             
  } 


//CHANNEL 3 : THROTTLE//
  
  if(PINC & B00000100 )
  {                                                                   
    if(last_CH3_state == 0)
   {                                             
      last_CH3_state = 1;                                                  
      counter_3 = current_count;                                               
   }
  }
  else if(last_CH3_state == 1)
  {                                             
    last_CH3_state = 0;                                                    
    THROTTLE = current_count - counter_3;                            
  } 


//CHANNEL 4 : YAW//
  
  if(PINC & B00001000 )
  {                                                                   
   if(last_CH4_state == 0)
   {                                             
      last_CH4_state = 1;                                                  
      counter_4 = current_count;                                               
    }
  }
  else if(last_CH4_state == 1)
  {                                             
    last_CH4_state = 0;                                                    
    YAW = current_count - counter_4;                            
  }


//CHANNEL 5 : FLIGHTMODE//
  
  if(PINC & B00010000 )
  {                                                                   
   if(last_CH5_state == 0)
   {                                             
      last_CH5_state = 1;                                                  
      counter_5 = current_count;                                               
    }
  }
  else if(last_CH5_state == 1)
  {                                             
    last_CH5_state = 0;                                                    
    FLIGHTMODE = current_count - counter_5;                            
  }
}
