//gcc -Wall -O2 main.c -o main -lwiringPi
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>
#include <sys/time.h>

//HC-SR04
#define	HC_SR04_TRIG_PIN		8
#define	HC_SR04_ECHO_PIN		7
//TA7291P
#define TA7291P_MOTOR_FORWARD_PIN 	14
#define TA7291P_MOTOR_REVERSE_PIN 	15
#define TA7291P_MOTOR_PWM_PIN 	18
//#define TA7291P_OUT_POWER 		512
#define TA7291P_STOP			0
#define TA7291P_FORWARD			1
#define TA7291P_REVERSE			2

//HC-SR04
void HC_SR04_init(void);
long HC_SR04_get_distance(void);
int HC_SR04_pulseIn(int pin, int level, int timeout);
//TA7291P
int m_nTA7291P_mode;
void TA7291P_init(void);
void TA7291P_forward(int nPower);
void TA7291P_reverse(int nPower);
void TA7291P_stop(void);
/*****************************************************************
*
*****************************************************************/
int main (int argc, char **argv)
{
	int i;
	long distance;

	if(wiringPiSetupGpio() == -1){
		printf("wiringPiSetupGpio() error\n");
		return -1;
	}

	HC_SR04_init();	//HC-SR04 初期化
	TA7291P_init();	//TA7291P 初期化


	for(i=0;i<200;i++){
		delayMicroseconds(100000); //wait
		distance = HC_SR04_get_distance();	//HC-SR04 距離取得


		if(distance >= 20){
			if(m_nTA7291P_mode != TA7291P_FORWARD){
				TA7291P_forward(512);	//TA7291P 正回転開始
			}
		}


		if(distance < 10){
			TA7291P_stop();	//TA7291P 停止
		}
	

		printf("distance=%ld cm\n",distance);
	}

	TA7291P_stop();	//TA7291P 停止

	return 0;
}

/*****************************************************************
*	HC-SR04 距離取得
*****************************************************************/
long HC_SR04_get_distance(void)
{
	long duration, distance;

	digitalWrite(HC_SR04_TRIG_PIN, LOW);
	delayMicroseconds(2);

	digitalWrite(HC_SR04_TRIG_PIN, HIGH);
	delayMicroseconds(10);

	digitalWrite(HC_SR04_TRIG_PIN, LOW);
	duration = HC_SR04_pulseIn(HC_SR04_ECHO_PIN, HIGH, 1000000);
//	printf("duration=%d\n",duration);
	distance = (duration/2) / 29.1;
//	printf("distance=%d cm\n",distance);

	return distance;
}
/*****************************************************************
*	HC-SR04 初期化
*****************************************************************/
void HC_SR04_init(void)
{
	pinMode(HC_SR04_TRIG_PIN,OUTPUT);
	pinMode(HC_SR04_ECHO_PIN,INPUT);
}
/*****************************************************************
*	HC-SR04 パルス波時間を取得する
*****************************************************************/
int HC_SR04_pulseIn(int pin, int level, int timeout)
{
	struct timeval tn, t0, t1;
	long micros;

	gettimeofday(&t0, NULL);
	micros = 0;
	while (digitalRead(pin) != level){
		gettimeofday(&tn, NULL);
		if (tn.tv_sec > t0.tv_sec){
			micros = 1000000L;
		}
		else{
			micros = 0;
		}
		micros += (tn.tv_usec - t0.tv_usec);
		if (micros > timeout) return 0;
	}

	gettimeofday(&t1, NULL);
	while (digitalRead(pin) == level){
		gettimeofday(&tn, NULL);
		if (tn.tv_sec > t0.tv_sec){
			micros = 1000000L;
		}
		else{
			micros = 0;
		}
		micros = micros + (tn.tv_usec - t0.tv_usec);
		if (micros > timeout) return 0;
	}

	if (tn.tv_sec > t1.tv_sec) micros = 1000000L; else micros = 0;
	micros = micros + (tn.tv_usec - t1.tv_usec);

	return micros;
}
/*****************************************************************
*	TA7291P 初期化
*****************************************************************/
void TA7291P_init(void)
{
	m_nTA7291P_mode = TA7291P_STOP;

	pinMode(TA7291P_MOTOR_FORWARD_PIN, OUTPUT);
	pinMode(TA7291P_MOTOR_REVERSE_PIN, OUTPUT);
	pinMode(TA7291P_MOTOR_PWM_PIN, PWM_OUTPUT);

	TA7291P_stop();	//TA7291P 停止
}
/*****************************************************************
*	TA7291P 正回転
*   nPower:0..　回転速度(0が停止)
*****************************************************************/
void TA7291P_forward(int nPower)
{
	if(m_nTA7291P_mode == TA7291P_REVERSE){
		TA7291P_stop();	//TA7291P 停止
		delayMicroseconds(50000); //wait(50ms)
	}

	m_nTA7291P_mode = TA7291P_FORWARD;
	digitalWrite(TA7291P_MOTOR_FORWARD_PIN, HIGH);
	pwmWrite(TA7291P_MOTOR_PWM_PIN, nPower);
}
/*****************************************************************
*	TA7291P 逆回転
*   nPower:0..　回転速度(0が停止)
*****************************************************************/
void TA7291P_reverse(int nPower)
{
	if(m_nTA7291P_mode == TA7291P_FORWARD){
		TA7291P_stop();	//TA7291P 停止
		delayMicroseconds(50000); //wait(50ms)
	}

	m_nTA7291P_mode = TA7291P_REVERSE;
	digitalWrite(TA7291P_MOTOR_REVERSE_PIN, HIGH);
	pwmWrite(TA7291P_MOTOR_PWM_PIN, nPower);
}
/*****************************************************************
*	TA7291P 停止
*****************************************************************/
void TA7291P_stop(void)
{
	m_nTA7291P_mode = TA7291P_STOP;
	pwmWrite(TA7291P_MOTOR_PWM_PIN, LOW);
	digitalWrite(TA7291P_MOTOR_FORWARD_PIN, LOW);
	digitalWrite(TA7291P_MOTOR_REVERSE_PIN, LOW);
}


