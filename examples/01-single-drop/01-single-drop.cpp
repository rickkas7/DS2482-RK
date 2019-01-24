#include "DS2482-RK.h"

SerialLogHandler logHandler;

DS2482 ds(Wire, 0);

const unsigned long CHECK_PERIOD = 30000;
unsigned long lastCheck = 5000 - CHECK_PERIOD;

void setup() {
	Serial.begin(9600);
	ds.setup();

	DS2482DeviceReset::run(ds, [](DS2482DeviceReset&, int status) {
		Serial.printlnf("deviceReset=%d", status);
	});

	Serial.println("setup complete");
}



void loop() {

	ds.loop();

	if (millis() - lastCheck >= CHECK_PERIOD) {
		lastCheck = millis();

		// For single-drop you can pass an empty address to get the temperature of the only
		// sensor on the 1-wire bus
		DS24821WireAddress addr;

		DS2482GetTemperatureCommand::run(ds, addr, [](DS2482GetTemperatureCommand&, int status, float tempC) {
			if (status == DS2482Command::RESULT_DONE) {
				char buf[32];
				snprintf(buf, sizeof(buf), "%.4f", tempC);

				Serial.printlnf("temperature=%s deg C", buf);
				Particle.publish("temperature", buf, PRIVATE);
			}
			else {
				Serial.printlnf("DS2482GetTemperatureCommand failed status=%d", status);
			}
		});
	}
}


