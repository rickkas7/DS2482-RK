#include "DS2482-RK.h"

SerialLogHandler logHandler;

DS2482 ds(Wire, 0);

DS2482DeviceListStatic<10> deviceList;
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

				for(size_t ii = 0; ii < deviceList.getDeviceCount(); ii++) {
					Serial.printlnf("%s valid=%d C=%f F=%f",
							deviceList.getAddressByIndex(ii).toString().c_str(),
							deviceList.getDeviceByIndex(ii).getValid(),
							deviceList.getDeviceByIndex(ii).getTemperatureC(),
							deviceList.getDeviceByIndex(ii).getTemperatureF());
				}

			});
		}
		else {
			Serial.printlnf("no devices found");
		}
	}
}


