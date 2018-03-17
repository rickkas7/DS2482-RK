

#include "Tester.h"

#include <math.h>


// Test board
//
// Pin (left to right)
// 1   5V or 3V3   Orange
// 2   GND         Black
// 3   Green       SDA to D0
// 4   Blue        SCL to D1
//
// DS2482_100 Address 1
// One DS18B20, always powered
//
// DS2482_800 Address 0
// Channel  Num DS18B20s  Powered by
// 0        4             GP0, GP1, GP2, GP3
// 1        2             GP4, GP5
// 2        1             GP6
// 3        1-wire screw terminals (always powered)
// 7        Empty (used to test having no sensors)

// MCP23008 Address 0
// GP0 - GP6 connected to DS18B20s

Tester tester;


//
// TesterBus
//

TesterBus::TesterBus(const char *name, DS2482 *ds, int channel, int powerPins, int numExpected, bool isParasiticPowered) :
		name(name), ds(ds), channel(channel), powerPins(powerPins), numExpected(numExpected), isParasiticPowered(isParasiticPowered) {

}

TesterBus::~TesterBus() {

}

void TesterBus::runTests(std::function<void()> completion) {

	tester.testsRun++;

	busSetup([this,completion](int status) {
		if (status != DS2482Command::RESULT_DONE) {
			Log.error("FAILURE unable to set up bus %s", name.c_str());
			completion();
			return;
		}

		// Bus has been set up. Now run tests.

		// Check bus, determines if this is single drop or multi-drop
		DS2482CheckBusCommand::run(*ds, [this,completion](DS2482CheckBusCommand &obj, int status, int busStatus) {

			if (status != DS2482Command::RESULT_DONE) {
				Log.error("FAILURE DS2482CheckBusCommand failed %d bus %s", status, name.c_str());
				completion();
				return;
			}

			if (numExpected == 0) {
				if (busStatus != DS2482CheckBusCommand::NO_DEVICES) {
					Log.error("FAILURE DS2482CheckBusCommand failed busStatus=%d expected NO_DEVICES bus %s", busStatus, name.c_str());
					completion();
					return;
				}
			}
			else
			if (numExpected == 1) {
				if (busStatus != DS2482CheckBusCommand::SINGLE_DROP) {
					Log.error("FAILURE DS2482CheckBusCommand failed busStatus=%d expected SINGLE_DROP bus %s", busStatus, name.c_str());
					completion();
					return;
				}
			}
			else {
				if (busStatus != DS2482CheckBusCommand::MULTI_DROP) {
					Log.error("FAILURE DS2482CheckBusCommand failed busStatus=%d expected MULTI_DROP bus %s", busStatus, name.c_str());
					completion();
					return;
				}
			}

			// Search bus, determine the device IDs
			DS2482SearchBusCommand::run(*ds, deviceList, [this,completion](DS2482SearchBusCommand &obj, int status) {

				if (status != DS2482Command::RESULT_DONE) {
					Log.error("FAILURE DS2482SearchBusCommand failed %d bus %s", status, name.c_str());
					completion();
					return;
				}

				if ((int)deviceList.getDeviceCount() != numExpected) {
					Log.error("FAILURE DS2482SearchBusCommand got %d devices, expected %d bus %s", deviceList.getDeviceCount(), numExpected, name.c_str());
					completion();
					return;
				}

				if (numExpected != 0) {
					checkTempAddressed(completion);
				}
				else {
					testsComplete(completion);
				}
			});

		});

	});


}

void TesterBus::checkTempAddressed(std::function<void()> completion) {
	deviceList.clearValid();

	DS2482GetTemperatureForListCommand::run(*ds, deviceList, [this,completion](DS2482GetTemperatureForListCommand&tempObj, int status, DS2482DeviceList &deviceList) {
		if (status != DS2482Command::RESULT_DONE) {
			Log.error("FAILURE DS2482GetTemperatureForListCommand failed %d bus %s", status, name.c_str());
			completion();
			return;
		}

		for(size_t ii = 0; ii < deviceList.getDeviceCount(); ii++) {
			DS2482Device d = deviceList.getDeviceByIndex(ii);
			if (!d.getValid()) {
				Log.error("FAILURE DS2482GetTemperatureForListCommand failed sensor %u not valid bus %s", ii, name.c_str());
				completion();
				return;
			}
			float t = d.getTemperatureC();
			if (t < 15.0 || t > 30.0) {
				Log.error("FAILURE DS2482GetTemperatureForListCommand failed sensor %u temp out of range %f bus %s", ii, t, name.c_str());
				completion();
				return;
			}
		}

		if (tempObj.getNumRetries() > 0) {
			Log.warn("WARNING DS2482GetTemperatureForListCommand required %u retries bus %s", tempObj.getNumRetries(), name.c_str());
		}


		checkTemp9bit(completion);

	}).withParasiticPower(isParasiticPowered);


}

void TesterBus::checkTemp9bit(std::function<void()> completion) {
	deviceList.clearValid();


	DS2482SetConfigCommand::run(*ds, deviceList, false, DS2482Command::CONVERSION_9BIT, [this,completion](DS2482SetConfigCommand&, int status) {

		if (status != DS2482Command::RESULT_DONE) {
			Log.error("FAILURE DS2482SetConfigCommand (9BIT) failed %d bus %s", status, name.c_str());
			completion();
			return;
		}

		DS2482GetTemperatureForListCommand::run(*ds, deviceList, [this,completion](DS2482GetTemperatureForListCommand &tempObj, int status, DS2482DeviceList &) {

			int numRetries = tempObj.getNumRetries();
			int getTempStatus = status;

			DS2482SetConfigCommand::run(*ds, deviceList, false, DS2482Command::CONVERSION_12BIT, [this,completion,getTempStatus,numRetries](DS2482SetConfigCommand&, int status) {

				if (status != DS2482Command::RESULT_DONE) {
					Log.error("FAILURE DS2482SetConfigCommand (12BIT) failed %d bus %s", status, name.c_str());
					completion();
					return;
				}
				if (getTempStatus != DS2482Command::RESULT_DONE) {
					Log.error("FAILURE DS2482GetTemperatureForListCommand (9BIT) failed %d bus %s", status, name.c_str());
					completion();
					return;
				}

				for(size_t ii = 0; ii < deviceList.getDeviceCount(); ii++) {
					DS2482Device d = deviceList.getDeviceByIndex(ii);
					if (!d.getValid()) {
						Log.error("FAILURE DS2482GetTemperatureForListCommand failed sensor %u not valid bus %s", ii, name.c_str());
						completion();
						return;
					}
					float t = d.getTemperatureC();
					if (t < 15.0 || t > 30.0) {
						Log.error("FAILURE DS2482GetTemperatureForListCommand failed sensor %u temp out of range %f bus %s", ii, t, name.c_str());
						completion();
						return;
					}
					float remainder = t - floor(t);
					if (remainder != 0.0 && remainder != 0.5) {
						Log.error("FAILURE DS2482GetTemperatureForListCommand failed sensor %u bad remainder %f bus %s", ii, t, name.c_str());
						completion();
						return;
					}
				}
				if (numRetries > 0) {
					Log.warn("WARNING DS2482GetTemperatureForListCommand required %u retries bus %s", numRetries, name.c_str());
					tester.totalRetries += numRetries;
				}

				if (numExpected == 1) {
					// This is a single-drop bus so check the SKIP_ROM method
					checkTempSingleDrop(completion);
				}
				else {
					// Otherwise we're done
					testsComplete(completion);
				}
			}).withParasiticPower(isParasiticPowered);

		}).withParasiticPower(isParasiticPowered);
	}).withParasiticPower(isParasiticPowered);

}


void TesterBus::checkTempSingleDrop(std::function<void()> completion) {
	deviceList.clearValid();

	// Empty address uses SKIP_ROM
	DS24821WireAddress addr;

	DS2482GetTemperatureCommand::run(*ds, addr, [this,completion](DS2482GetTemperatureCommand&tempObj, int status, float t) {
		if (status != DS2482Command::RESULT_DONE) {
			Log.error("FAILURE DS2482GetTemperatureCommand failed %d bus %s", status, name.c_str());
			completion();
			return;
		}

		if (t < 15.0 || t > 30.0) {
			Log.error("FAILURE DS2482GetTemperatureCommand failed temp out of range %f bus %s", t, name.c_str());
			completion();
			return;
		}

		if (tempObj.getNumRetries() > 0) {
			Log.warn("WARNING DS2482GetTemperatureForListCommand required %u retries bus %s", tempObj.getNumRetries(), name.c_str());
			tester.totalRetries += tempObj.getNumRetries();
		}

		// Test reading an invalid address on single drop busses as well
		readInvalidAddrTest(completion);


	}).withParasiticPower(isParasiticPowered);


}

void TesterBus::readInvalidAddrTest(std::function<void()> completion) {

	uint8_t fakeAddr[9] = { 0x28, 0xa7, 0xf1, 0xd9, 0x2a, 0x00, 0x00, 0xc0};
	DS24821WireAddress addr;
	addr.withBytes(fakeAddr);


	DS2482GetTemperatureCommand::run(*ds, addr, [this,completion](DS2482GetTemperatureCommand&, int status, float t) {
		if (status != DS2482Command::RESULT_TOO_MANY_RETRIES) {
			Log.error("FAILURE DS2482GetTemperatureCommand invalid addr failed %d bus %s", status, name.c_str());
			completion();
			return;
		}

		testsComplete(completion);
	}).withParasiticPower(isParasiticPowered).withMaxRetries(1);

}


void TesterBus::testsComplete(std::function<void()> completion) {

	tester.testsComplete++;

	// Success
	Log.info("tests completed bus %s", name.c_str());
	completion();

}


// Bus scan - powered and parasitic
// Read temps - powered, parasitic, and mixed
// Non-addressed mode
// Change bit size

void TesterBus::busSetup(std::function<void(int status)> completion) {
	if (powerPins >= 0) {
		for(uint16_t pin = 0; pin < 7; pin++) {
			tester.gpio.pinMode(pin, OUTPUT);
			tester.gpio.digitalWrite(pin, (powerPins & (1 << pin)) != 0);
		}
	}
	if (channel >= 0) {
		DS2482ChannelSelect::run(*ds, channel, [this, completion](DS2482ChannelSelect &, int status) {
			completion(status);
		});
	}
	else {
		completion(DS2482Command::RESULT_DONE);
	}
}



Tester::Tester() :
		gpio(Wire, 0), ds1(Wire, 1), ds8(Wire, 0),
		bus1("bus1", &ds1, -1, -1, 1, false),
		bus8_4("bus8_4", &ds8, 0, 0b0001111, 4, false),
		bus8_4_parasitic("bus8_4_parasitic", &ds8, 0, 0b0000000, 4, true),
		bus8_4_half_parasitic("bus8_4_half_parasitic",&ds8, 0, 0b0001010, 4, true),
		bus8_2("bus8_2", &ds8, 1, 0b0110000, 2, false),
		bus8_2_parasitic("bus8_2_parasitic", &ds8, 1, 0b0000000, 2, true),
		bus8_1("bus8_1", &ds8, 2, 0b1000000, 1, false),
		bus8_1_parasitic("bus8_1_parasitic", &ds8, 2, 0b0000000, 1, true),
		bus8_screw("bus8_screw", &ds8, 3, 0b0000000, 1, true),
		bus8_empty("bus8_empty", &ds8, 7, -1, 0, false) {

	allBusses.push_back(&bus1);
	allBusses.push_back(&bus8_4);
	allBusses.push_back(&bus8_4_parasitic);
	allBusses.push_back(&bus8_4_half_parasitic);
	allBusses.push_back(&bus8_2);
	allBusses.push_back(&bus8_2_parasitic);
	allBusses.push_back(&bus8_1);
	allBusses.push_back(&bus8_1_parasitic);
	//allBusses.push_back(&bus8_screw);
	allBusses.push_back(&bus8_empty);

}

Tester::~Tester() {
}

void Tester::setup() {
	gpio.begin();
	ds1.setup();
	ds8.setup();

	lastFreeMemory = System.freeMemory();
}

void Tester::loop() {
	ds1.loop();
	ds8.loop();

	stateHandler(*this);
}

void Tester::startState() {
	curBusIter = allBusses.begin();
	stateHandler = &Tester::nextBusState;
}

void Tester::nextBusState() {
	if (curBusIter == allBusses.end()) {
		Log.info("all tests completed");
		Log.info("success=%d failure=%d total=%d", testsComplete, testsRun - testsComplete, testsRun);
		Log.info("totalRetries=%d", totalRetries);
		Log.info("freeMemory=%lu lastFreeMemory=%u delta=%d", System.freeMemory(), lastFreeMemory, (int)(System.freeMemory() - lastFreeMemory));
		lastFreeMemory = System.freeMemory();
		stateHandler = &Tester::retryWaitState;
		stateTime = millis();
		return;
	}
	curBus = *curBusIter++;
	stateHandler = &Tester::waitState;
	stateTime = millis();

	curBus->runTests([this]() {
		stateHandler = &Tester::nextBusState;
		return;
	});
}

void Tester::waitState() {
	if (millis() - stateTime >= MAX_TEST_TIME_MS) {
		Log.error("FAILURE test on bus %s timed out", curBus->name.c_str());
		stateHandler = &Tester::retryWaitState;
		stateTime = millis();
		return;
	}

}


void Tester::retryWaitState() {
	if (millis() - stateTime >= RETRY_WAIT_TIME_MS) {
		stateHandler = &Tester::startState;
		stateTime = millis();
		return;
	}
}


