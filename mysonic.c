#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define PA 2
#define PB 4
#define PC 1
#define PD 16
#define PE 15
#define PF 8
#define PG 9
#define PDP 0

char nums[10] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99,
				0x92, 0x82, 0xf8, 0x80, 0x90};

char pins[8] = {PA, PB, PC, PD, PE, PF, PG, PDP};

int TRIG = 29;
int ECHO = 28;

static int echo_distance() {

		unsigned long TX_time = 0;
		unsigned long RX_time = 0;
		int distance = 0;
		unsigned long timeout = 50000000;
		unsigned long Wait_time = micros();

		pinMode(TRIG, OUTPUT);
		pinMode(ECHO, INPUT);

		digitalWrite(TRIG, LOW);
		delay(50);

		digitalWrite(TRIG, HIGH);
		delayMicroseconds(10);
		digitalWrite(TRIG, LOW);

		while ( (digitalRead(ECHO) == LOW && (micros()-Wait_time) < timeout) ) {
				if( digitalRead(ECHO) == HIGH ) break;
		}

		if ( (micros() - Wait_time) > timeout ) {
				printf("0 Out of range.micros = %d, wait-time = %d \n", micros(), Wait_time);
		}

		TX_time = micros();
		
		while ( (digitalRead(ECHO) == HIGH && (micros() - Wait_time) ) < timeout) {
				if(digitalRead(ECHO) == LOW) break;
		}

		if ( (micros() - Wait_time) > timeout) {
				printf("1. Out of range.\n");
		}

		RX_time = micros();
		distance = (int) ((float) (RX_time - TX_time) * 0.017);

		return distance;
}

void clear_pin(void) {
		for (int i = 0; i < 8; i++) digitalWrite(pins[i], (0xbf >> i) & 0x1);
}

void set_pin(int n) {
		for (int i = 0; i < 8; i++) digitalWrite(pins[i], (nums[n] >> i) & 0x1);
}

void init_pin(void) {
		for (int i = 0; i < 8; i++) pinMode(pins[i], OUTPUT);
}

void CC_handler(int signal) {
		clear_pin();
		exit(0);
}

int main() {

		signal(SIGINT, CC_handler);

		printf ("HC-SR04 Ultra-sonic distance measure program \n");
		if(wiringPiSetup() == -1) {
			exit(EXIT_FAILURE);
		}

		if(setuid(getuid()) < 0) {
				perror("Dropping privileges failed.\n");
				exit(EXIT_FAILURE);
		}

		init_pin();
		int output = 0, distance;
		while(1) {
				distance = echo_distance();
				clear_pin();
				if (distance < 10 || distance > 99) output = 0;
				else output = distance / 10;
				set_pin(output);
				delay(500);
		}

		return 0;
}

