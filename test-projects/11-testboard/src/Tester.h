#ifndef __TESTER_H
#define __TESTER_H

#include "Particle.h"

#include <list>

#include "DS2482-RK.h"
#include "MCP23008-RK.h"



class TesterBus {
public:
	TesterBus(const char *name, DS2482 *ds, int channel, int powerPins, int numExpected, bool isParasiticPowered);
	virtual ~TesterBus();

	void runTests(std::function<void()> completion);

	void checkTempAddressed(std::function<void()> completion);

	void checkTemp9bit(std::function<void()> completion);

	void checkTempSingleDrop(std::function<void()> completion);

	void readInvalidAddrTest(std::function<void()> completion);

	void testsComplete(std::function<void()> completion);

	void busSetup(std::function<void(int)> completion);

public:
	String name;
	DS2482 *ds;
	int channel;
	int powerPins;
	int numExpected;
	bool isParasiticPowered;
	DS2482DeviceListStatic<10> deviceList;
};

class Tester {
public:
	Tester();
	virtual ~Tester();

	void setup();
	void loop();

protected:
	static const int RETRY_WAIT_TIME_MS = 30000; // 30 seconds
	static const int MAX_TEST_TIME_MS = 300000; // 5 minutes


	void startState();
	void nextBusState();
	void waitState();
	void retryWaitState();

	MCP23008 gpio;
	DS2482 ds1;
	DS2482 ds8;
	TesterBus bus1;
	TesterBus bus8_4;
	TesterBus bus8_4_parasitic;
	TesterBus bus8_4_half_parasitic;
	TesterBus bus8_2;
	TesterBus bus8_2_parasitic;
	TesterBus bus8_1;
	TesterBus bus8_1_parasitic;
	TesterBus bus8_screw;
	TesterBus bus8_empty;

	std::list<TesterBus *> allBusses;
	std::function<void(Tester&)> stateHandler = &Tester::startState;
	unsigned long stateTime = 0;

	TesterBus *curBus = NULL;
	std::list<TesterBus *>::iterator curBusIter;

	friend class TesterBus;

	int testsRun = 0;
	int testsComplete = 0;
	int totalRetries = 0;
	size_t lastFreeMemory = 0;
};

extern Tester tester;



#endif /* __TESTER_H */
