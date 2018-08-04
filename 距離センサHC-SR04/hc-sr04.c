/*

 hc-sr04.c

 Raspberry Pi driving the HC-SR04

 to compile : gcc hc-sr04.c -o hc-sr04 -lwiringPi

*/
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define    trigPin    8
#define    echoPin    7

int pulseIn(int pin, int level, int timeout)
{
   struct timeval tn, t0, t1;
   long micros;

   gettimeofday(&t0, NULL);
   micros = 0;
   while (digitalRead(pin) != level)
   {
      gettimeofday(&tn, NULL);
      if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
      micros += (tn.tv_usec - t0.tv_usec);
      if (micros > timeout) return 0;
   }

   gettimeofday(&t1, NULL);
   while (digitalRead(pin) == level)
   {
      gettimeofday(&tn, NULL);
      if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
      micros = micros + (tn.tv_usec - t0.tv_usec);
      if (micros > timeout) return 0;
   }

   if (tn.tv_sec > t1.tv_sec) micros = 1000000L; else micros = 0;
   micros = micros + (tn.tv_usec - t1.tv_usec);
   return micros;
}

int main (int argc, char **argv) {
  int i,j,k,l,m;
  long duration, distance;

  if(wiringPiSetupGpio() == -1) return 1;
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

for(i=0;i<200;i++){
  delayMicroseconds(100000); // Added this line
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 1000000);
//  printf("duration=%d\n",duration);
  distance = (duration/2) / 29.1;
//  if(distance < 10){
      printf("distance=%d cm\n",distance);
//  }

//sleep(1);
}

}
