#include "Tester.h"

SerialLogHandler logHandler;

void setup() {
	Serial.begin(9600);
	tester.setup();
}

void loop() {
	tester.loop();
}



