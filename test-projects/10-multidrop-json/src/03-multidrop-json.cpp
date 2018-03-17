#include "DS2482-RK.h"
#include "JsonParserGeneratorRK.h"

SerialLogHandler logHandler;

DS2482 ds(Wire, 3);

DS2482DeviceListStatic<10> deviceList;
JsonWriterStatic<256> jsonWriter;

const unsigned long CHECK_PERIOD = 30000;
unsigned long lastCheck = 10000 - CHECK_PERIOD;

void setup() {
	Serial.begin(9600);
	ds.setup();

	DS2482DeviceReset::run(ds, [](DS2482DeviceReset&, int status) {
		Serial.printlnf("deviceReset=%d", status);
		DS2482SearchBusCommand::run(ds, deviceList, [](DS2482SearchBusCommand &obj, int status) {

			if (status != DS2482Command::RESULT_DONE) {
				Serial.printlnf("DS2482SearchBusCommand status=%d", status);
				return;
			}

			Serial.printlnf("Found %u devices", deviceList.getDeviceCount());
		});
	});

	Serial.println("setup complete");
}


void loop() {

	ds.loop();

	if (millis() - lastCheck >= CHECK_PERIOD) {
		lastCheck = millis();

		if (deviceList.getDeviceCount() > 0) {

			DS2482GetTemperatureForListCommand::run(ds, deviceList, [](DS2482GetTemperatureForListCommand&, int status, DS2482DeviceList &deviceList) {
				if (status != DS2482Command::RESULT_DONE) {
					Serial.printlnf("DS2482GetTemperatureForListCommand status=%d", status);
					return;
				}

				Serial.printlnf("got temperatures!");

				// Initialize the JsonWriter object and sets it to send 2 decimal places
				jsonWriter.init();
				jsonWriter.setFloatPlaces(2);

				// startObject is for the outer object and must be balanced with finishObjectOrArray.
				jsonWriter.startObject();

				// Write the actual temperatures
				for(size_t ii = 0; ii < deviceList.getDeviceCount(); ii++) {
					if (deviceList.getDeviceByIndex(ii).getValid()) {
						// This creates a key of the form "t0" for the first, "t1" for the second, ...
						char key[4];
						key[0] = 't';
						key[1] = '0' + ii;
						key[2] = 0;

						// Inserts the float as a key value pair
						jsonWriter.insertKeyValue(key, deviceList.getDeviceByIndex(ii).getTemperatureC());
					}
				}

				jsonWriter.finishObjectOrArray();

				Serial.println(jsonWriter.getBuffer());
				Particle.publish("temperature", jsonWriter.getBuffer(), PRIVATE);

				// Example output:
				// {"t0":23.56,"t1":23.12}
			});
		}
		else {
			Serial.printlnf("no devices found");
		}
	}
}


