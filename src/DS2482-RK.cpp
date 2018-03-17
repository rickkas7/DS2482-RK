#include "Particle.h"

#include "DS2482-RK.h"

// Define the debug logging level here
// 0 = Off
// 1 = Normal
// 2 = High
#define DS2482_DEBUG_LEVEL 1

// Don't change these, just change the debugging level above
#if DS2482_DEBUG_LEVEL >= 1
#define DEBUG_NORMAL(x) Log.info x
#else
#define DEBUG_NORMAL(x)
#endif

#if DS2482_DEBUG_LEVEL >= 2
#define DEBUG_HIGH(x) Log.info x
#else
#define DEBUG_HIGH(x)
#endif


const uint8_t DS2482::crcTable[256] =
{
	0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
		157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
		35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
		190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
		70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
		219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
		101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
		248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
		140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
		17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
		175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
		50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
		202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
		87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
		233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
		116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

const uint16_t DS2482ConvertTCommand::convTime[4] =
{
	94, 188, 375, 750
};

const uint8_t DS2482ChannelSelect::channelCodes[8] =
{
	0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87
};

const uint8_t DS2482ChannelSelect::channelResponseCodes[8] =
{
	0xb8, 0xb1, 0xaa, 0xa3, 0x9c, 0x95, 0x8e, 0x87
};




DS2482::DS2482(TwoWire &wire, int addr) : wire(wire), addr(addr) {
}

DS2482::~DS2482() {

}

void DS2482::setup() {
	wire.begin();
}

void DS2482::loop() {
	commandListStack[commandListIndex].loop();
}

uint8_t DS2482::getI2CAddr() const {
	if (addr < 8) {
		return 0b0011000 | (addr & 0b111);
	}
	else {
		return addr;
	}
}

void DS2482::pushCommand(DS2482Command *cmd) {
	commandListStack[commandListIndex].push(cmd);
}

// [static]
bool DS2482::checkCRC(const uint8_t *buf, size_t bufSize) {
	uint8_t crc = 0;

	for(size_t ii = 0; ii < bufSize; ii++) {
		crc = crcTable[crc ^ buf[ii]];
	}

	return crc == buf[bufSize];
}



void DS2482::beginTransmission() {
	wire.beginTransmission(getI2CAddr());
}
uint8_t DS2482::endTransmission(bool stop /* = true */) {
	return wire.endTransmission(stop);
}
uint8_t DS2482::requestFrom(uint8_t numBytes, bool stop /* = true */) {
	return wire.requestFrom(getI2CAddr(), numBytes, (uint8_t) stop);
}
size_t DS2482::write(uint8_t val) {
	return wire.write(val);
}

size_t DS2482::write(const uint8_t *buf, size_t count) {
	return wire.write(buf, count);
}

int DS2482::available(void) {
	return wire.available();
}

int DS2482::read(void) {
	return wire.read();
}

void DS2482::pushCommandList() {
	commandListIndex++;
}
void DS2482::popCommandList() {
	commandListIndex--;
}


DS24821WireAddress::DS24821WireAddress() {
	clear();
}

DS24821WireAddress::~DS24821WireAddress() {

}

DS24821WireAddress::DS24821WireAddress(const DS24821WireAddress &other) {
	*this = other;
}

DS24821WireAddress &DS24821WireAddress::operator=(const DS24821WireAddress &other) {
	for(size_t ii = 0; ii < sizeof(addr); ii++) {
		addr[ii] = other.addr[ii];
	}
	return *this;
}

bool DS24821WireAddress::operator==(const DS24821WireAddress &other) const {
	for(size_t ii = 0; ii < sizeof(addr); ii++) {
		if (addr[ii] != other.addr[ii]) {
			return false;
		}
	}
	return true; // same
}


DS24821WireAddress &DS24821WireAddress::withBytes(const uint8_t *buf) {
	for(size_t ii = 0; ii < sizeof(addr); ii++) {
		addr[ii] = buf[ii];
	}
	return *this;
}

void DS24821WireAddress::clear() {
	for(size_t ii = 0; ii < sizeof(addr); ii++) {
		addr[ii] = 0;
	}
}
bool DS24821WireAddress::isValid() const {
	bool nonZero = false;

	for(size_t ii = 0; ii < sizeof(addr); ii++) {
		if (addr[ii] != 0) {
			nonZero = true;
			break;
		}
	}
	if (!nonZero) {
		return false;
	}

	return DS2482::checkCRC(addr, sizeof(addr) - 1);
}


String DS24821WireAddress::toString() const {
	String result;

	// Data is sent LSB first for a 64-bit (8 byte) value, so when printing it naturally,
	// the order of bytes is reversed
	for(size_t ii = 0; ii < sizeof(addr); ii++) {
		char buf[3];
		snprintf(buf, sizeof(buf), "%02x", addr[sizeof(addr) - ii - 1]);
		result += buf;
	}

	return result;
}

bool DS24821WireAddress::getBit(size_t bitNum) const {
	// The data is set LSB first and is stored in addr in that order!

	size_t byteIndex = bitNum / 8;
	size_t bitIndex = bitNum % 8;

	return (addr[byteIndex] & (1 << bitIndex)) != 0;
}

void DS24821WireAddress::setBit(size_t bitNum, bool value) {
	// The data is set LSB first and is stored in addr in that order!

	size_t byteIndex = bitNum / 8;
	size_t bitIndex = bitNum % 8;

	if (value) {
		addr[byteIndex] |= (1 << bitIndex);
	}
	else {
		addr[byteIndex] &= ~(1 << bitIndex);
	}
}

DS2482Device::DS2482Device() : temperatureC(0), valid(false) {
}

DS2482Device::~DS2482Device() {
}

DS2482Device::DS2482Device(const DS24821WireAddress &addr) : addr(addr), temperatureC(0), valid(false) {

}

DS2482Device::DS2482Device(const DS2482Device &other) : addr(other.addr), temperatureC(other.temperatureC), valid(other.valid) {

}

DS2482Device &DS2482Device::operator=(const DS2482Device &other) {
	addr = other.addr;
	temperatureC = other.temperatureC;
	valid = other.valid;
	return *this;
}

String DS2482Device::toString() const {
	String result;

	result.format("%s temperatureC=%f valid=%", addr.toString().c_str(), temperatureC, valid);

	return result;
}

void DS2482Device::clear() {
	addr.clear();
	temperatureC = 0.0;
	valid = false;
}

///


DS2482DeviceList::DS2482DeviceList(DS2482Device *devices, size_t deviceMax) : devices(devices), deviceCount(0), deviceMax(deviceMax) {
}

DS2482DeviceList::~DS2482DeviceList() {
}

bool DS2482DeviceList::addAddress(const DS24821WireAddress &addr) {
	if (deviceCount < deviceMax) {
		devices[deviceCount].clear();
		devices[deviceCount].setAddress(addr);
		deviceCount++;
		return true;
	}
	else {
		return false;
	}
}

void DS2482DeviceList::clear() {
	deviceCount = 0;
}

void DS2482DeviceList::clearValid() {
	for(size_t ii = 0; ii < deviceCount; ii++) {
		devices[ii].setValid(false);
	}
}


DS2482Command::DS2482Command(DS2482 &parent) : parent(parent) {
}

DS2482Command::~DS2482Command() {
}

int DS2482Command::loop() {
	return -1;
}

void DS2482Command::pushCommand() {
	parent.pushCommand(this);
}

void DS2482Command::pushCommandList() {
	parent.pushCommandList();
}

void DS2482Command::popCommandList() {
	parent.popCommandList();
}


int DS2482Command::readStatus(uint8_t &value, bool setReadPointerFirst) {
	int result;

	if (setReadPointerFirst) {
		result = setReadPointer(STATUS_REG);
		if (result) {
			return result;
		}
	}

	result = requestFrom(1, true);
	if (result != 1) {
		return RESULT_I2C_READ_TIMEOUT;
	}

	value = (uint8_t) read();

	DEBUG_HIGH(("readStatus value=0x%x %s%s%s%s%s%s%s%s", value,
			((value & STATUS_DIR_MASK) ? "DIR " : ""),
			((value & STATUS_TSB_MASK) ? "TSB " : ""),
			((value & STATUS_SBR_MASK) ? "SBR " : ""),
			((value & STATUS_RST_MASK) ? "RST " : ""),
			((value & STATUS_LL_MASK)  ? "LL "  : ""),
			((value & STATUS_SD_MASK)  ? "SD "  : ""),
			((value & STATUS_PPD_MASK) ? "PPD " : ""),
			((value & STATUS_1WB_MASK) ? "1WB " : "")
	));


	return 0; // success
}

int DS2482Command::readData(uint8_t &value, bool setReadPointerFirst) {
	int result;

	if (setReadPointerFirst) {
		result = setReadPointer(READ_DATA_REG);
		if (result) {
			return result;
		}
	}

	result = requestFrom(1, true);
	if (result != 1) {
		return RESULT_I2C_READ_TIMEOUT;
	}

	value = (uint8_t) read();

	DEBUG_HIGH(("readData value=0x%x", value));

	return 0; // success
}

int DS2482Command::setReadPointer(uint8_t reg) {
	DEBUG_HIGH(("setReadPointer %02x", reg));
	return writeReg1(SET_READ_PTR_CMD, reg);
}


int DS2482Command::readConfig(uint8_t &value) {
	int result;

	result = setReadPointer(CONFIG_REG);
	if (result) {
		return result;
	}

	result = requestFrom(1, true);
	if (result != 1) {
		return RESULT_I2C_READ_TIMEOUT;
	}

	value = (uint8_t) read();

	DEBUG_HIGH(("readConfig value=0x%x", value));

	return 0; // success
}


int DS2482Command::writeConfig(bool speed, bool spu, bool apu) {
	uint8_t value = 0;
	if (speed) {
		value |= CONFIG_1WS_MASK;
	}
	if (spu) {
		value |= CONFIG_SPU_MASK;
	}
	if (apu) {
		value |= CONFIG_APU_MASK;
	}
	value |= (~value & 0xf) << 4;

	return writeReg1(WRITE_CONFIG_CMD, value);
}

int DS2482Command::writeReg0(uint8_t reg) {
	beginTransmission();
	write(reg);
	uint8_t stat = endTransmission(true);
	if (stat != 0) {
		DEBUG_HIGH(("writeReg0 failed stat=%d", stat));
	}

	return - (int)stat;
}

int DS2482Command::writeReg1(uint8_t reg, uint8_t value) {
	beginTransmission();
	write(reg);
	write(value);
	uint8_t stat = endTransmission(true);
	if (stat != 0) {
		DEBUG_HIGH(("writeReg1 failed stat=%d", stat));
	}

	return - (int)stat;
}

int DS2482Command::strongPullUp(bool on) {
	// speed, spu, apu
	return writeConfig(false, on, true);
}



void DS2482Command::beginTransmission() {
	return parent.beginTransmission();
}

uint8_t DS2482Command::endTransmission(bool stop /* = true */) {
	return parent.endTransmission(stop);
}

uint8_t DS2482Command::requestFrom(uint8_t numBytes, bool stop /* = true */) {
	return parent.requestFrom(numBytes, stop);
}

size_t DS2482Command::write(uint8_t val) {
	return parent.write(val);
}

size_t DS2482Command::write(const uint8_t *buf, size_t count) {
	return parent.write(buf, count);
}
int DS2482Command::available(void) {
	return parent.available();
}
int DS2482Command::read(void) {
	return parent.read();
}




DS2482CommandList::DS2482CommandList() {

}
DS2482CommandList::~DS2482CommandList() {
}

int DS2482CommandList::loop() {
	int result = DS2482Command::RESULT_WORKING;

	if (!list.empty()) {
		DS2482Command *cmd = list.front();
		result = cmd->loop();
		if (result != DS2482Command::RESULT_WORKING) {
			delete cmd;
			list.pop_front();
		}
	}
	return result;
}

void DS2482CommandList::clear() {
	list.clear();
}

void DS2482CommandList::removeFront() {
	DS2482Command *cmd = list.front();
	delete cmd;
	list.pop_front();
}

void DS2482CommandList::push(DS2482Command *cmd) {
	list.push_back(cmd);
}

DS2482DeviceReset::DS2482DeviceReset(DS2482 &parent, std::function<void(DS2482DeviceReset&, int)> completion) :
		DS2482Command(parent), completion(completion), stateTime(0) {

}

DS2482DeviceReset::~DS2482DeviceReset() {
}

int DS2482DeviceReset::loop() {

	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_NORMAL(("DeviceReset status=%d", result));
		completion(*this, result);
	}
	return result;
}

int DS2482DeviceReset::startState() {
	int result;
	uint8_t value;

	result = readStatus(value);
	if (result) {
		return result;
	}

	if (value & STATUS_RST_MASK) {
		// Already in reset (probably just powered up)
		DEBUG_HIGH(("DS2482DeviceReset already in reset"));
		stateHandler = &DS2482DeviceReset::setConfig;
		return RESULT_WORKING;
	}

	result = writeReg0(DEVICE_RESET_CMD);
	if (result != 0) {
		return result;
	}

	stateTime = millis();
	stateHandler = &DS2482DeviceReset::waitForResetState;

	return result;
}

int DS2482DeviceReset::waitForResetState() {
	int result;
	uint8_t value;

	result = readStatus(value);
	if (result) {
		return result;
	}

	if (value & STATUS_RST_MASK) {
		stateHandler = &DS2482DeviceReset::setConfig;
		return RESULT_WORKING;
	}

	if (millis() - stateTime > 10) {
		return RESULT_I2C_COMMAND_TIMEOUT;
	}

	return RESULT_WORKING;
}

int DS2482DeviceReset::setConfig() {
	int result = writeConfig(false, false, true);
	if (result != 0) {
		return result;
	}
	return RESULT_DONE;
}

// static
DS2482DeviceReset &DS2482DeviceReset::run(DS2482 &parent, std::function<void(DS2482DeviceReset&, int)> completion) {
	DS2482DeviceReset *result = new DS2482DeviceReset(parent, completion);
	result->pushCommand();
	return *result;
}


DS24821WireReset::DS24821WireReset(DS2482 &parent, std::function<void(DS24821WireReset&, int, bool)> completion)  :
	DS2482Command(parent), completion(completion), presenceDetected(false), stateTime(0) {

}

DS24821WireReset::~DS24821WireReset() {

}

int DS24821WireReset::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_NORMAL(("1WireReset status=%d", result));
		completion(*this, result, presenceDetected);
	}
	return result;
}

int DS24821WireReset::startState() {
	int result;
	uint8_t value;

	result = readStatus(value);
	if (result) {
		return result;
	}

	if (value & STATUS_1WB_MASK) {
		return RESULT_1WIRE_BUSY;
	}

	result = writeReg0(RESET_1WIRE_CMD);
	if (result) {
		return result;
	}

	stateHandler = &DS24821WireReset::waitState;
	stateTime = millis();

	return RESULT_WORKING;
}

int DS24821WireReset::waitState() {
	int result;
	uint8_t value;

	result = readStatus(value);
	if (result) {
		return result;
	}

	if (millis() - stateTime > 100) {
		return RESULT_I2C_COMMAND_TIMEOUT;
	}

	if (value & STATUS_1WB_MASK) {
		return RESULT_WORKING;
	}

	if (value & STATUS_SD_MASK) {
		DEBUG_NORMAL(("1-wire short detected"));
		return RESULT_1WIRE_SHORT;
	}

	presenceDetected = ((value & STATUS_PPD_MASK) != 0);

	DEBUG_HIGH(("DS24821WireReset done presenceDetected=%d", presenceDetected));

	return RESULT_DONE;

}

//static
DS24821WireReset &DS24821WireReset::run(DS2482 &parent, std::function<void(DS24821WireReset&, int, bool)> completion) {
	DS24821WireReset *result = new DS24821WireReset(parent, completion);
	result->pushCommand();
	return *result;
}


// The implementation of DS24821WireReadBit has been removed.
// It's only used for ReadPowerSupply and ConvertT when polling for completion.
// I can't get this to work reliably, however, so I've removed support. You need to manually
// configure the power supply mode and conversion time, which is perfectly reasonable as
// you ought to know how the bus is wired!


DS24821WireReadByte::DS24821WireReadByte(DS2482 &parent, std::function<void(DS24821WireReadByte&, int, uint8_t)> completion)   :
			DS2482Command(parent), completion(completion), value(0), stateTime(0) {

}

DS24821WireReadByte::~DS24821WireReadByte() {

}

int DS24821WireReadByte::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_HIGH(("DS24821WireReadByte completed status=%d value=%02x", result, value));
		completion(*this, result, value);
	}
	return result;
}

int DS24821WireReadByte::startState() {
	int result;
	uint8_t statusValue;

	stateHandler = &DS24821WireReadByte::waitState;
	stateTime = millis();

	result = readStatus(statusValue);
	if (result) {
		return result;
	}

	if (statusValue & STATUS_1WB_MASK) {
		return RESULT_1WIRE_BUSY;
	}

	result = writeReg0(READ_BYTE_CMD);
	if (result) {
		return result;
	}

	return RESULT_WORKING;
}

int DS24821WireReadByte::waitState() {
	int result;
	uint8_t statusValue;

	result = readStatus(statusValue);
	if (result) {
		return result;
	}

	if (millis() - stateTime > 100) {
		return RESULT_I2C_COMMAND_TIMEOUT;
	}

	if (statusValue & STATUS_1WB_MASK) {
		return RESULT_WORKING;
	}

	result = readData(value, true);
	if (result) {
		return result;
	}

	return RESULT_DONE;

}

// static
DS24821WireReadByte &DS24821WireReadByte::run(DS2482 &parent, std::function<void(DS24821WireReadByte&, int, uint8_t)> completion) {
	DS24821WireReadByte *result = new DS24821WireReadByte(parent, completion);
	result->pushCommand();
	return *result;
}

DS24821WireWriteByte::DS24821WireWriteByte(DS2482 &parent, uint8_t value, std::function<void(DS24821WireWriteByte&, int)> completion)   :
			DS2482Command(parent), value(value), completion(completion), stateTime(0) {

}

DS24821WireWriteByte::~DS24821WireWriteByte() {

}

int DS24821WireWriteByte::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_HIGH(("DS24821WireWriteByte completed status=%d", result));
		completion(*this, result);
	}
	return result;
}

int DS24821WireWriteByte::startState() {
	int result;
	uint8_t statusValue;

	result = readStatus(statusValue);
	if (result) {
		return result;
	}

	if (statusValue & STATUS_1WB_MASK) {
		return RESULT_1WIRE_BUSY;
	}

	DEBUG_HIGH(("WRITE_BYTE_CMD %02x", value));

	result = writeReg1(WRITE_BYTE_CMD, value);
	if (result) {
		return result;
	}

	stateHandler = &DS24821WireWriteByte::waitState;
	stateTime = millis();

	return RESULT_WORKING;
}

int DS24821WireWriteByte::waitState() {
	int result;
	uint8_t statusValue;

	result = readStatus(statusValue, false);
	if (result) {
		return result;
	}

	if (millis() - stateTime > 500) {
		return RESULT_I2C_COMMAND_TIMEOUT;
	}

	if (statusValue & STATUS_1WB_MASK) {
		return RESULT_WORKING;
	}

	return RESULT_DONE;

}

// static
DS24821WireWriteByte &DS24821WireWriteByte::run(DS2482 &parent, uint8_t value, std::function<void(DS24821WireWriteByte&, int)> completion) {
	DS24821WireWriteByte *result = new DS24821WireWriteByte(parent, value, completion);
	result->pushCommand();
	return *result;
}


DS24821WireTriplet::DS24821WireTriplet(DS2482 &parent, bool dirIn, std::function<void(DS24821WireTriplet&, int)> completion) :
		DS2482Command(parent), dirIn(dirIn), completion(completion), statusBits(0), stateTime(0) {

}

DS24821WireTriplet::~DS24821WireTriplet() {

}

int DS24821WireTriplet::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_HIGH(("DS24821WireTriplet completed status=%d", result));
		completion(*this, result);
	}
	return result;
}

int DS24821WireTriplet::startState() {
	int result;
	uint8_t statusValue;

	result = readStatus(statusValue, false);
	if (result) {
		return result;
	}

	if (statusValue & STATUS_1WB_MASK) {
		return RESULT_1WIRE_BUSY;
	}

	DEBUG_HIGH(("DS24821WireTriplet dirIn=%d", dirIn));

	result = writeReg1(TRIPLET_CMD, dirIn ? 0x80 : 0x00);
	if (result) {
		return result;
	}

	stateHandler = &DS24821WireTriplet::waitState;
	stateTime = millis();

	return RESULT_WORKING;
}

int DS24821WireTriplet::waitState() {
	int result;
	uint8_t statusValue;

	result = readStatus(statusValue, false);
	if (result) {
		return result;
	}

	if (millis() - stateTime > 500) {
		return RESULT_I2C_COMMAND_TIMEOUT;
	}

	if (statusValue & STATUS_1WB_MASK) {
		return RESULT_WORKING;
	}

	statusBits = statusValue;

	return RESULT_DONE;

}

// static
DS24821WireTriplet &DS24821WireTriplet::run(DS2482 &parent, bool dirIn, std::function<void(DS24821WireTriplet&, int)> completion) {
	DS24821WireTriplet *result = new DS24821WireTriplet(parent, dirIn, completion);
	result->pushCommand();
	return *result;
}


DS2482ChannelSelect::DS2482ChannelSelect(DS2482 &parent, int channel, std::function<void(DS2482ChannelSelect&, int)> completion)  :
		DS2482Command(parent), channel(channel), completion(completion) {

}

DS2482ChannelSelect::~DS2482ChannelSelect() {

}

int DS2482ChannelSelect::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_NORMAL(("ChannelSelect completed channel=%d status=%d", channel, result));
		completion(*this, result);
	}
	return result;
}

int DS2482ChannelSelect::startState() {
	int result;
	uint8_t statusValue;

	// On channel select, need to set the read pointer, as channel select sets it to the channel selection register
	result = readStatus(statusValue, true);
	if (result) {
		return result;
	}

	if (statusValue & STATUS_1WB_MASK) {
		return RESULT_1WIRE_BUSY;
	}

	DEBUG_HIGH(("DS2482ChannelSelect channel=%d", channel));

	result = writeReg1(CHANNEL_SELECT_CMD, channelCodes[channel & 0x7]);
	if (result) {
		return result;
	}

	stateHandler = &DS2482ChannelSelect::waitState;

	return RESULT_WORKING;
}

int DS2482ChannelSelect::waitState() {
	int result;
	uint8_t statusValue;

	result = readStatus(statusValue, false);
	if (result) {
		return result;
	}

	if (statusValue != channelResponseCodes[channel & 0x7]) {
		return RESULT_SET_CHANNEL_FAILED;
	}

	return RESULT_DONE;

}


// static
DS2482ChannelSelect &DS2482ChannelSelect::run(DS2482 &parent, int channel, std::function<void(DS2482ChannelSelect&, int)> completion) {
	DS2482ChannelSelect *result = new DS2482ChannelSelect(parent, channel, completion);
	result->pushCommand();
	return *result;
}


DS24821WireCommand::DS24821WireCommand(DS2482 &parent, uint8_t cmd, size_t readSize, std::function<void(DS24821WireCommand&, int)> completion)   :
					DS2482Command(parent), cmd(cmd), readSize(readSize), completion(completion), offset(0),
					finalResult(RESULT_WORKING), stateTime(0) {
}

DS24821WireCommand::~DS24821WireCommand() {
}

int DS24821WireCommand::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_HIGH(("DS24821WireWCommand cmd=0x%x readSize=%u completed status=%d", cmd, readSize, result));
		completion(*this, result);
	}
	return result;
}



int DS24821WireCommand::startState() {
	DS24821WireCommand *outer = this;


	stateHandler = &DS24821WireCommand::waitState;
	stateTime = millis();

	DEBUG_HIGH(("DS24821WireCommand 0x%02x", cmd));

	pushCommandList();

	DS24821WireWriteByte::run(parent, cmd, [outer](DS24821WireWriteByte &, int status) {
		DEBUG_HIGH(("oneWireWriteByte completion status=%d", status));
		outer->popCommandList();

		if (status != RESULT_DONE) {
			outer->finalResult = status;
			return;
		}

		// Set read pointer to the read data register
		outer->setReadPointer(DS2482Command::READ_DATA_REG);

		outer->stateHandler = &DS24821WireCommand::nextReadState;
	});

	return RESULT_WORKING;
}
int DS24821WireCommand::waitState() {
	if (millis() - stateTime > 1000) {
		finalResult = RESULT_I2C_COMMAND_TIMEOUT;
	}

	return finalResult;
}

int DS24821WireCommand::nextReadState() {
	DS24821WireCommand *outer = this;

	DEBUG_HIGH(("nextReadState offset = %d", offset));

	if (offset < readSize) {
		// Read another byte
		stateHandler = &DS24821WireCommand::waitState;

		pushCommandList();

		DS24821WireReadByte::run(parent, [outer](DS24821WireReadByte &, int status, uint8_t value) {
			outer->popCommandList();
			if (status != RESULT_DONE) {
				outer->finalResult = status;
				return;
			}
			outer->buffer[outer->offset++] = value;
			outer->stateHandler = &DS24821WireCommand::nextReadState;
		});
	}
	else {
		// Done!
		return RESULT_DONE;
	}

	return finalResult;
}

DS24821WireAddress DS24821WireCommand::getAddress() const {
	DS24821WireAddress addr;

	if (offset == DS24821WireAddress::ADDR_SIZE) {
		addr.withBytes(buffer);
	}

	return addr;
}

bool DS24821WireCommand::checkCRC() const {
	if (offset == readSize) {
		return DS2482::checkCRC(buffer, offset - 1);
	}
	else {
		return false;
	}
}


DS24821WireSendAddress::DS24821WireSendAddress(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS24821WireSendAddress&, int)> completion) :
		DS2482Command(parent), addr(addr), completion(completion), finalResult(RESULT_WORKING), offset(0), stateTime(0) {

}

DS24821WireSendAddress::~DS24821WireSendAddress() {

}

int DS24821WireSendAddress::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_HIGH(("DS24821WireSendAddress completed addr=%s status=%d", addr.toString().c_str(), result));
		completion(*this, result);
	}
	return result;
}

int DS24821WireSendAddress::startState() {

	DS24821WireSendAddress *outer = this;

	stateHandler = &DS24821WireSendAddress::waitState;
	stateTime = millis();
	offset = 0;

	DEBUG_HIGH(("DS24821WireSendAddress %s", addr.toString().c_str()));

	pushCommandList();

	// Reset the bus
	DS24821WireReset::run(parent, [outer](DS24821WireReset&, int status, bool presenceDetected) {
		DEBUG_HIGH(("oneWireReset=%d ppd=%d", status, presenceDetected));
		outer->popCommandList();

		if (status != DS2482Command::RESULT_DONE) {
			outer->finalResult = status;
			return;
		}

		if (!presenceDetected) {
			outer->finalResult = RESULT_1WIRE_NO_DEVICE;
			return;
		}

		// Send either MATCH_ROM if we have an address, or SKIP_ROM (typically if there's only one device)
		if (outer->addr.isValid()) {
			DEBUG_HIGH(("MATCH_ROM %s", outer->addr.toString().c_str()));
			outer->pushCommandList();

			DS2482MatchRom::run(outer->parent, [outer](DS24821WireWriteByte &, int status) {
				DEBUG_HIGH(("oneWireWriteByte MATCH_ROM completion status=%d", status));
				outer->popCommandList();
				if (status != RESULT_DONE) {
					outer->finalResult = status;
					return;
				}

				outer->stateHandler = &DS24821WireSendAddress::sendAddrState;

			});
		}
		else {
			DEBUG_HIGH(("SKIP_ROM"));
			outer->pushCommandList();

			DS2482SkipRom::run(outer->parent, [outer](DS24821WireWriteByte &, int status) {
				DEBUG_HIGH(("oneWireWriteByte SKIP_ROM completion status=%d", status));
				outer->popCommandList();
				if (status != RESULT_DONE) {
					outer->finalResult = status;
					return;
				}
				// Done!
				outer->finalResult = RESULT_DONE;
			});

		}

	});



	return RESULT_WORKING;
}

int DS24821WireSendAddress::waitState() {
	if (millis() - stateTime > 1000) {
		finalResult = RESULT_I2C_COMMAND_TIMEOUT;
	}

	return finalResult;
}

int DS24821WireSendAddress::sendAddrState() {

	DS24821WireSendAddress *outer = this;

	DEBUG_HIGH(("sendAddrState offset = %d", offset));

	if (offset < DS24821WireAddress::ADDR_SIZE) {
		// Write another byte
		stateHandler = &DS24821WireSendAddress::waitState;

		pushCommandList();

		DS24821WireWriteByte::run(parent, addr[offset++], [outer](DS24821WireWriteByte &, int status) {

			outer->popCommandList();

			if (status != RESULT_DONE) {
				outer->finalResult = status;
				return;
			}
			outer->stateHandler = &DS24821WireSendAddress::sendAddrState;
		});

		return RESULT_WORKING;
	}
	else {
		// Done!
		return RESULT_DONE;
	}
}

// static
DS24821WireSendAddress &DS24821WireSendAddress::run(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS24821WireSendAddress&, int)> completion) {
	DS24821WireSendAddress *result = new DS24821WireSendAddress(parent, addr, completion);
	result->pushCommand();
	return *result;
}



DS2482ReadRom::DS2482ReadRom(DS2482 &parent, std::function<void(DS24821WireCommand&, int)> completion) :
	DS24821WireCommand(parent, DS2482Command::READ_ROM, 8, completion){

}

DS2482ReadRom::~DS2482ReadRom() {
}

// static
DS2482ReadRom &DS2482ReadRom::run(DS2482 &parent, std::function<void(DS24821WireCommand&, int)> completion) {
	DS2482ReadRom *result = new DS2482ReadRom(parent, completion);
	result->pushCommand();
	return *result;
}

DS2482SkipRom::DS2482SkipRom(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int)> completion) :
		DS24821WireWriteByte(parent, SKIP_ROM, completion) {
}

DS2482SkipRom::~DS2482SkipRom() {

}

// static
DS2482SkipRom &DS2482SkipRom::run(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int)> completion) {
	DS2482SkipRom *result = new DS2482SkipRom(parent, completion);
	result->pushCommand();
	return *result;
}


DS2482MatchRom::DS2482MatchRom(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int)> completion) :
		DS24821WireWriteByte(parent, MATCH_ROM, completion) {
}

DS2482MatchRom::~DS2482MatchRom() {

}

// static
DS2482MatchRom &DS2482MatchRom::run(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int)> completion) {
	DS2482MatchRom *result = new DS2482MatchRom(parent, completion);
	result->pushCommand();
	return *result;
}


DS2482SearchRom::DS2482SearchRom(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int)> completion) :
		DS24821WireWriteByte(parent, SEARCH_ROM, completion) {
}

DS2482SearchRom::~DS2482SearchRom() {

}

// static
DS2482SearchRom &DS2482SearchRom::run(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int)> completion) {
	DS2482SearchRom *result = new DS2482SearchRom(parent, completion);
	result->pushCommand();
	return *result;
}


// The implementation of DS2482ReadPowerSupply has been removed.
// I can't get this to work reliably, however, so I've removed support. You need to manually
// configure the power supply mode to enable parasitic power mode, which seems reasonable to me
// as you ought to know if you need parasitic power supply mode or not!


DS2482ConvertTCommand::DS2482ConvertTCommand(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482ConvertTCommand&, int)> completion) :
		DS2482Command(parent), completion(completion), addr(addr), conversionSize(CONVERSION_12BIT),
		parasiticPower(false), finalResult(RESULT_WORKING), tconv(0), stateTime(0) {

}

DS2482ConvertTCommand::~DS2482ConvertTCommand() {

}

int DS2482ConvertTCommand::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_NORMAL(("ConvertT status=%d", result));
		completion(*this, result);
	}
	return result;
}

int DS2482ConvertTCommand::startState() {

	DS2482ConvertTCommand *outer = this;

	stateHandler = &DS2482ConvertTCommand::waitState;
	stateTime = millis();

	DEBUG_HIGH(("DS2482ConvertTCommand 0x44"));

	pushCommandList();
	DS24821WireSendAddress::run(parent, addr, [outer](DS24821WireSendAddress &, int status) {
		DEBUG_HIGH(("ConvertT SendAddress completion status=%d", status));
		outer->popCommandList();

		if (status != RESULT_DONE) {
			outer->finalResult = status;
			return;
		}

		if (outer->parasiticPower) {
			// Enable strong pull-up mode
			status = outer->strongPullUp(true);
			if (status) {
				outer->finalResult = status;
				return;
			}
		}


		outer->pushCommandList();

		DS24821WireWriteByte::run(outer->parent, CONVERT_T, [outer](DS24821WireWriteByte &, int status) {
			outer->popCommandList();

			if (status != RESULT_DONE) {
				outer->finalResult = status;
				return;
			}

			// In theory we can use the bit test by reading a single bit to find out when conversion is done
			// when not using parasitic power mode.
			// I can't get this to work reliably, however, so I've disabled it to use timed-conversion instead.
			// You should know how many bits you're reading, and leaving it as 12 bits works regardless of
			// how many bits you've configured (it's just slower).

			outer->tconv = (unsigned long) convTime[outer->conversionSize];
			outer->stateHandler = &DS2482ConvertTCommand::timeWaitState;
			outer->stateTime = millis();
			DEBUG_HIGH(("delaying of %lu ms for conversion", outer->tconv));

		});

	});

	return RESULT_WORKING;
}

int DS2482ConvertTCommand::waitState() {
	if (millis() - stateTime > 1500) {
		finalResult = RESULT_I2C_COMMAND_TIMEOUT;
	}

	return finalResult;
}

int DS2482ConvertTCommand::timeWaitState() {
	if (millis() - stateTime > tconv) {
		if (parasiticPower) {
			// Turn off strong pull-ups for parasitic power mode
			int status = strongPullUp(false);
			if (status) {
				return status;
			}
		}
		return RESULT_DONE;
	}

	return RESULT_WORKING;
}

// static
DS2482ConvertTCommand &DS2482ConvertTCommand::run(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482ConvertTCommand&, int)> completion) {
	DS2482ConvertTCommand *result = new DS2482ConvertTCommand(parent, addr, completion);
	result->pushCommand();
	return *result;
}

DS2482ReadScratchpadCommand::DS2482ReadScratchpadCommand(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482ReadScratchpadCommand&, int, uint8_t *)> completion) :
		DS2482Command(parent), completion(completion), addr(addr), maxRetries(RETRIES_DEFAULT), numRetries(0),
		finalResult(RESULT_WORKING), stateTime(0) {

}

DS2482ReadScratchpadCommand::~DS2482ReadScratchpadCommand() {

}

int DS2482ReadScratchpadCommand::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_NORMAL(("ReadScratchpad completed status=%d", result));
		completion(*this, result, scratchpad);
	}
	return result;
}

int DS2482ReadScratchpadCommand::startState() {

	DS2482ReadScratchpadCommand *outer = this;

	stateHandler = &DS2482ReadScratchpadCommand::waitState;
	stateTime = millis();

	DEBUG_HIGH(("DS2482ReadScratchpadCommand %s", addr.toString().c_str()));

	pushCommandList();
	DS24821WireSendAddress::run(parent, addr, [outer](DS24821WireSendAddress &, int status) {
		DEBUG_HIGH(("DS24821WireSendAddress completion status=%d", status));
		outer->popCommandList();

		if (status != RESULT_DONE) {
			outer->finalResult = status;
			return;
		}

		outer->stateHandler = &DS2482ReadScratchpadCommand::readScratchpadState;

	});

	return RESULT_WORKING;
}

int DS2482ReadScratchpadCommand::waitState() {
	if (millis() - stateTime > 1500) {
		finalResult = RESULT_I2C_COMMAND_TIMEOUT;
	}

	return finalResult;
}

int DS2482ReadScratchpadCommand::readScratchpadState() {

	DS2482ReadScratchpadCommand *outer = this;

	stateHandler = &DS2482ReadScratchpadCommand::waitState;
	stateTime = millis();

	outer->pushCommandList();

	DS2482ReadScratchpadInternal::run(outer->parent, [outer](DS24821WireCommand &obj, int status) {
		outer->popCommandList();

		DEBUG_HIGH(("readScratchpad completion status=%d", status));

		if (status != RESULT_DONE) {
			outer->finalResult = status;
			return;
		}

		if (obj.checkCRC()) {
			// CRC is good!
			memcpy(outer->scratchpad, obj.getBuffer(), 9);

			DEBUG_HIGH(("scratchpad CRC good TH=%d TL=%d config=0x%02x", (int8_t)outer->scratchpad[2], (int8_t)outer->scratchpad[3], outer->scratchpad[4]));

			outer->finalResult = RESULT_DONE;
			return;
		}

		outer->numRetries++;
		DEBUG_HIGH(("readScratchpad crc did not match, retrying numRetries=%d", outer->numRetries));

		if (outer->numRetries >= outer->maxRetries) {
			DEBUG_HIGH(("readScratchpad too many retries"));
			outer->finalResult = RESULT_TOO_MANY_RETRIES;
			return;
		}

		outer->stateHandler = &DS2482ReadScratchpadCommand::startState;
	});

	return RESULT_WORKING;
}
// static
DS2482ReadScratchpadCommand &DS2482ReadScratchpadCommand::run(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482ReadScratchpadCommand&, int, uint8_t *)> completion) {
	DS2482ReadScratchpadCommand *result = new DS2482ReadScratchpadCommand(parent, addr, completion);
	result->pushCommand();
	return *result;
}


DS2482ReadScratchpadInternal::DS2482ReadScratchpadInternal(DS2482 &parent, std::function<void(DS24821WireCommand&, int)> completion) :
		DS24821WireCommand(parent, READ_SCRATCHPAD, 9, completion) {

}

DS2482ReadScratchpadInternal::~DS2482ReadScratchpadInternal() {

}

// static
DS2482ReadScratchpadInternal &DS2482ReadScratchpadInternal::run(DS2482 &parent, std::function<void(DS24821WireCommand&, int)> completion) {
	DS2482ReadScratchpadInternal *result = new DS2482ReadScratchpadInternal(parent, completion);
	result->pushCommand();
	return *result;
}


DS2482GetTemperatureCommand::DS2482GetTemperatureCommand(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482GetTemperatureCommand&, int, float)> completion) :
		DS2482Command(parent), addr(addr), completion(completion),
		maxRetries(RETRIES_DEFAULT), conversionSize(CONVERSION_12BIT), parasiticPower(false),
		finalResult(RESULT_WORKING), temperature(0), stateTime(0) {

}

DS2482GetTemperatureCommand::~DS2482GetTemperatureCommand() {

}

int DS2482GetTemperatureCommand::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_NORMAL(("GetTemperature status=%d temperatureC=%f", result, temperature));
		completion(*this, result, temperature);
	}
	return result;
}

int DS2482GetTemperatureCommand::startState() {

	DS2482GetTemperatureCommand *outer = this;

	stateHandler = &DS2482GetTemperatureCommand::waitState;
	stateTime = millis();

	DEBUG_HIGH(("DS2482GetTemperatureCommand"));

	pushCommandList();
	DS2482ConvertTCommand::run(parent, addr, [outer](DS2482ConvertTCommand &obj, int status) {
		DEBUG_HIGH(("GetTemperature ConvertT completion status=%d", status));
		outer->popCommandList();

		if (status != RESULT_DONE) {
			outer->finalResult = status;
			return;
		}

		outer->pushCommandList();

		DS2482ReadScratchpadCommand::run(outer->parent, outer->addr, [outer](DS2482ReadScratchpadCommand &obj, int status, uint8_t *scratchpad) {
			outer->popCommandList();

			DEBUG_HIGH(("getTemperature scratchpadRead status=%d", status));

			outer->numRetries = obj.getNumRetries();

			if (status != RESULT_DONE) {
				outer->finalResult = status;
				return;
			}

			outer->temperature = convertTemp(scratchpad, outer->conversionSize);
			outer->finalResult = RESULT_DONE;
		}).withMaxRetries(outer->maxRetries);

	}).withConversionSize(conversionSize).withParasiticPower(parasiticPower);

	return RESULT_WORKING;
}

int DS2482GetTemperatureCommand::waitState() {
	if (millis() - stateTime > 3000) {
		finalResult = RESULT_I2C_COMMAND_TIMEOUT;
	}

	return finalResult;
}

// static
float DS2482GetTemperatureCommand::convertTemp(const uint8_t *scratchpad, int conversionSize) {
	uint16_t t = scratchpad[0] | (scratchpad[1] << 8);

	// When using smaller configuration sizes, the lower bits are not defined - not 0!
	// Zero them out so the numbers are consistent
	switch(conversionSize) {
	case CONVERSION_9BIT:
		t &= ~0b111;
		break;
	case CONVERSION_10BIT:
		t &= ~0b011;
		break;
	case CONVERSION_11BIT:
		t &= ~0b001;
		break;

	}

	return ((float) (int16_t) t) / 16.0;
}


// static
DS2482GetTemperatureCommand &DS2482GetTemperatureCommand::run(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482GetTemperatureCommand&, int, float)> completion) {
	DS2482GetTemperatureCommand *result = new DS2482GetTemperatureCommand(parent, addr, completion);
	result->pushCommand();
	return *result;
}



DS2482GetTemperatureForListCommand::DS2482GetTemperatureForListCommand(DS2482 &parent, DS2482DeviceList &deviceList, std::function<void(DS2482GetTemperatureForListCommand&, int, DS2482DeviceList &deviceList)> completion) :
		DS2482Command(parent), deviceList(deviceList), completion(completion),
		maxRetries(RETRIES_DEFAULT), conversionSize(CONVERSION_12BIT), parasiticPower(false),
		finalResult(RESULT_WORKING), index(0), stateTime(0) {

}

DS2482GetTemperatureForListCommand::~DS2482GetTemperatureForListCommand() {

}

int DS2482GetTemperatureForListCommand::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_NORMAL(("GetTemperatureForList status=%d", result));
		completion(*this, result, deviceList);
	}
	return result;
}

int DS2482GetTemperatureForListCommand::startState() {

	DS2482GetTemperatureForListCommand *outer = this;

	stateHandler = &DS2482GetTemperatureForListCommand::waitState;
	stateTime = millis();

	DEBUG_HIGH(("DS2482GetTemperatureForListCommand"));


	// Clear the valid valid on deviceList
	deviceList.clearValid();
	index = 0;

	// Empty addr means send to all devices
	DS24821WireAddress addr;

	pushCommandList();
	DS2482ConvertTCommand::run(parent, addr, [outer](DS2482ConvertTCommand &obj, int status) {
		DEBUG_HIGH(("GetTemperatureForList ConvertT completion status=%d", status));
		outer->popCommandList();

		if (status != RESULT_DONE) {
			outer->finalResult = status;
			return;
		}

		outer->stateHandler = &DS2482GetTemperatureForListCommand::nextSensorState;

	}).withConversionSize(conversionSize).withParasiticPower(parasiticPower);

	return RESULT_WORKING;
}

int DS2482GetTemperatureForListCommand::waitState() {
	if (millis() - stateTime > 3000) {
		finalResult = RESULT_I2C_COMMAND_TIMEOUT;
	}

	return finalResult;
}

int DS2482GetTemperatureForListCommand::nextSensorState() {

	DS2482GetTemperatureForListCommand *outer = this;

	stateHandler = &DS2482GetTemperatureForListCommand::waitState;

	if (index >= deviceList.getDeviceCount()) {
		return RESULT_DONE;
	}

	DS24821WireAddress addr = deviceList.getAddressByIndex(index);

	DEBUG_HIGH(("getting scratchpad for %s", addr.toString().c_str()));

	pushCommandList();
	DS2482ReadScratchpadCommand::run(outer->parent, addr, [outer](DS2482ReadScratchpadCommand &obj, int status, uint8_t *scratchpad) {
		outer->popCommandList();

		if (status == RESULT_DONE) {
			outer->deviceList.getDeviceByIndex(outer->index).setTemperatureC(DS2482GetTemperatureCommand::convertTemp(scratchpad, outer->conversionSize));
			outer->deviceList.getDeviceByIndex(outer->index).setValid(true);
		}
		outer->numRetries += obj.getNumRetries();

		// Get next sensor
		outer->index++;
		outer->stateHandler = &DS2482GetTemperatureForListCommand::nextSensorState;

	}).withMaxRetries(outer->maxRetries);


	return RESULT_WORKING;

}



// static
DS2482GetTemperatureForListCommand &DS2482GetTemperatureForListCommand::run(DS2482 &parent, DS2482DeviceList &deviceList, std::function<void(DS2482GetTemperatureForListCommand&, int, DS2482DeviceList &deviceList)> completion) {
	DS2482GetTemperatureForListCommand *result = new DS2482GetTemperatureForListCommand(parent, deviceList, completion);
	result->pushCommand();
	return *result;
}





DS2482CheckBusCommand::DS2482CheckBusCommand(DS2482 &parent, std::function<void(DS2482CheckBusCommand&, int, int)> completion) :
		DS2482Command(parent), completion(completion), finalResult(RESULT_WORKING), busStatus(NO_DEVICES), stateTime(0) {

}

DS2482CheckBusCommand::~DS2482CheckBusCommand() {

}

int DS2482CheckBusCommand::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_NORMAL(("CheckBus status=%d busStatus=%d", result, busStatus));
		completion(*this, result, busStatus);
	}
	return result;
}

int DS2482CheckBusCommand::startState() {

	DS2482CheckBusCommand *outer = this;

	stateHandler = &DS2482CheckBusCommand::waitState;
	stateTime = millis();

	DEBUG_HIGH(("DS2482CheckBusCommand 0x44"));

	pushCommandList();

	DS24821WireReset::run(parent, [outer](DS24821WireReset&, int status, bool presenceDetected) {
		DEBUG_HIGH(("DS2482CheckBusCommand reset status=%d ppd=%d", status, presenceDetected));
		outer->popCommandList();

		if (status != RESULT_DONE) {
			outer->finalResult = status;
			return;
		}

		if (!presenceDetected) {
			outer->busStatus = NO_DEVICES;
			outer->finalResult = RESULT_DONE;
			DEBUG_HIGH(("DS2482CheckBusCommand NO_DEVICES"));
			return;
		}

		outer->pushCommandList();

		DS2482ReadRom::run(outer->parent, [outer](DS24821WireCommand &obj, int status) {
			outer->popCommandList();

			bool isValid = obj.getAddress().isValid();

			DEBUG_HIGH(("DS2482CheckBusCommand readRom=%d %s crcCheck=%d", status, obj.getAddress().toString().c_str(), isValid));

			if (status != DS2482Command::RESULT_DONE) {
				return;
			}

			if (isValid) {
				outer->addr = obj.getAddress();
				outer->busStatus = SINGLE_DROP;
				outer->finalResult = RESULT_DONE;
				DEBUG_HIGH(("DS2482CheckBusCommand SINGLE_DROP %s", outer->addr.toString().c_str()));
			}
			else {
				outer->busStatus = MULTI_DROP;
				outer->finalResult = RESULT_DONE;
				DEBUG_HIGH(("DS2482CheckBusCommand MULTI_DROP"));
			}
		});

	});

	return RESULT_WORKING;
}

int DS2482CheckBusCommand::waitState() {
	if (millis() - stateTime > 1500) {
		finalResult = RESULT_I2C_COMMAND_TIMEOUT;
	}

	return finalResult;
}


DS2482CheckBusCommand &DS2482CheckBusCommand::run(DS2482 &parent, std::function<void(DS2482CheckBusCommand&, int, int)> completion) {
	DS2482CheckBusCommand *result = new DS2482CheckBusCommand(parent, completion);
	result->pushCommand();
	return *result;
}


DS2482SearchBusCommand::DS2482SearchBusCommand(DS2482 &parent, DS2482DeviceList &deviceList, std::function<void(DS2482SearchBusCommand&, int)> completion) :
		DS2482Command(parent), devices(deviceList), completion(completion), finalResult(RESULT_WORKING),
		branchStackIndex(0), failures(0), decisionBit(0), currentBit(0), stateTime(0), dir(false) {

}

DS2482SearchBusCommand::~DS2482SearchBusCommand() {
}

int DS2482SearchBusCommand::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_NORMAL(("SearchBus status=%d deviceCount=%u", result, devices.getDeviceCount()));
		completion(*this, result);
	}
	return result;
}

int DS2482SearchBusCommand::startState() {

	DEBUG_HIGH(("DS2482SearchBusCommand"));

	stateHandler = &DS2482SearchBusCommand::searchState;
	stateTime = millis();

	devices.clear();

	return RESULT_WORKING;

}

int DS2482SearchBusCommand::searchState() {

	DS2482SearchBusCommand *outer = this;

	stateHandler = &DS2482SearchBusCommand::waitState;
	stateTime = millis();
	currentBit = 0;

	if (branchStackIndex == 0) {
		addr.clear();
		decisionBit = 0;
		dir = false;
	}
	else {
		addr = branchStack[branchStackIndex - 1].addr;
		decisionBit = branchStack[branchStackIndex - 1].decisionBit;
		branchStackIndex--;
		dir = true;
	}

	DEBUG_HIGH(("Starting new search addr=%s decisionBit=%u stackSize=%u", addr.toString().c_str(), decisionBit, branchStackIndex));

	pushCommandList();

	DS24821WireReset::run(parent, [outer](DS24821WireReset&, int status, bool presenceDetected) {
		DEBUG_HIGH(("DS2482SearchBusCommand reset status=%d ppd=%d", status, presenceDetected));
		outer->popCommandList();

		if (status != RESULT_DONE) {
			outer->finalResult = status;
			return;
		}

		if (!presenceDetected) {
			outer->finalResult = RESULT_DONE;
			DEBUG_HIGH(("DS2482SearchBusCommand NO_DEVICES"));
			return;
		}

		outer->pushCommandList();

		DS2482SearchRom::run(outer->parent, [outer](DS24821WireWriteByte &obj, int status) {
			outer->popCommandList();

			if (status != RESULT_DONE) {
				outer->finalResult = status;
				return;
			}

			outer->stateHandler = &DS2482SearchBusCommand::sendAddressTriplets;
		});

	});

	return RESULT_WORKING;
}




int DS2482SearchBusCommand::sendAddressTriplets() {

	DS2482SearchBusCommand *outer = this;

	stateHandler = &DS2482SearchBusCommand::waitState;


	DEBUG_HIGH(("sendAddressTriplets currentBit=%u decisionBit=%u stackIndex=%u", currentBit, decisionBit, branchStackIndex));

	if (currentBit < decisionBit) {
		//
		pushCommandList();

		DS24821WireTriplet::run(outer->parent, addr.getBit(currentBit), [outer](DS24821WireTriplet &obj, int status) {

			outer->popCommandList();
			DEBUG_HIGH(("sbr=%d tsb=%d dir=%d", obj.getSBR(), obj.getTSB(), obj.getDIR()));
			outer->currentBit++;

			// SBR TSB
			// 0   0    Both bits represented on bus - writes DIR
			// 0   1    All devices have a 0 in this position - writes 0
			// 1   0    All devices have a 1 in this position - writes 1
			// 1   1    Error - writes 1

			if (status != RESULT_DONE) {
				outer->finalResult = status;
				return;
			}

			outer->stateHandler = &DS2482SearchBusCommand::sendAddressTriplets;


		});

	}
	else {
		// Now we're at the point where we may or may not have a decision to make
		if (currentBit < 64) {

			pushCommandList();

			DS24821WireTriplet::run(outer->parent, dir, [outer](DS24821WireTriplet &obj, int status) {

				outer->popCommandList();

				bool sbr = obj.getSBR();
				bool tsb = obj.getTSB();

				DEBUG_HIGH(("sbr=%d tsb=%d dir=%d", sbr, tsb, obj.getDIR()));


				if (status != RESULT_DONE) {
					outer->finalResult = status;
					return;
				}

				if (sbr && tsb) {
					// SBR TSB
					// 1   1    Error - writes 1
					DEBUG_HIGH(("got sbr=1 tbr=1, failed"));
					outer->finalResult = RESULT_SEARCH_FAILED;
					return;
				}

				if (!sbr && !tsb) {
					// SBR TSB
					// 0   0    Both bits represented on bus - writes DIR. Leave the branch point on the stack

					if (obj.getDIR() == false) {
						outer->branchStack[outer->branchStackIndex].addr = outer->addr;
						outer->branchStack[outer->branchStackIndex].decisionBit = outer->currentBit;
						outer->branchStackIndex++;

						DEBUG_HIGH(("decision point bit=%u stackSize=%u taking 0 branch", outer->currentBit, outer->branchStackIndex));
					}
					else {
						DEBUG_HIGH(("decision point bit=%u stackSize=%u taking 1 branch", outer->currentBit, outer->branchStackIndex));
						outer->addr.setBit(outer->currentBit, true);
					}
				}
				else
				if (!sbr & tsb) {
					// SBR TSB
					// 0   1    All devices have a 0 in this position - writes 0

					// Decision unnecessary
					DEBUG_HIGH(("all devices have 0 at position %u, addr now=%s", outer->currentBit, outer->addr.toString().c_str()));
				}
				else
				if (sbr & !tsb) {
					// SBR TSB
					// 1   0    All devices have a 1 in this position - writes 1

					// Decision unnecessary.
					DEBUG_HIGH(("all devices have 1 at position %u, addr now=%s", outer->currentBit, outer->addr.toString().c_str()));

					outer->addr.setBit(outer->currentBit, true);
				}

				outer->currentBit++;
				outer->stateHandler = &DS2482SearchBusCommand::sendAddressTriplets;

			});

			dir = false;
		}
		else {
			if (addr.isValid()) {
				DEBUG_HIGH(("addr=%s is valid, added to list", addr.toString().c_str()));

				devices.addAddress(addr);
			}
			else {
				failures++;
				DEBUG_HIGH(("addr=%s is not valid, failures=%d", addr.toString().c_str(), failures));
			}

			if (outer->branchStackIndex == 0) {
				DEBUG_HIGH(("search done!"));
				return RESULT_DONE;
			}
			else {
				stateHandler = &DS2482SearchBusCommand::searchState;
			}


		}
	}



	return RESULT_WORKING;
}


int DS2482SearchBusCommand::waitState() {
	if (millis() - stateTime > 5000) {
		finalResult = RESULT_I2C_COMMAND_TIMEOUT;
	}

	return finalResult;
}



DS2482SearchBusCommand &DS2482SearchBusCommand::run(DS2482 &parent, DS2482DeviceList &deviceList, std::function<void(DS2482SearchBusCommand&, int)> completion) {
	DS2482SearchBusCommand *result = new DS2482SearchBusCommand(parent, deviceList, completion);
	result->pushCommand();
	return *result;
}


DS2482SetConfigCommand::DS2482SetConfigCommand(DS2482 &parent, DS2482DeviceList &deviceList, bool saveToEEPROM, int conversionSize, std::function<void(DS2482SetConfigCommand&, int)> completion) :
		DS2482Command(parent), deviceList(deviceList), saveToEEPROM(saveToEEPROM), conversionSize(conversionSize), completion(completion),
		parasiticPower(false), tl(0), th(0), finalResult(RESULT_WORKING), index(0), configIndex(0), stateTime(0) {
}

DS2482SetConfigCommand::~DS2482SetConfigCommand() {
}

int DS2482SetConfigCommand::loop() {
	int result = stateHandler(*this);
	if (result != RESULT_WORKING && completion) {
		DEBUG_NORMAL(("SetConfig status=%d %02x %02x %02x", result, config[0], config[1], config[2]));
		completion(*this, result);
	}
	return result;
}


int DS2482SetConfigCommand::startState() {
	index = 0;
	stateHandler = &DS2482SetConfigCommand::nextDeviceState;

	return RESULT_WORKING;
}

int DS2482SetConfigCommand::nextDeviceState() {

	if (index >= deviceList.getDeviceCount()) {
		return RESULT_DONE;
	}

	addr = deviceList.getAddressByIndex(index++);

	if (!addr.isValid()) {
		// DS24821WireSendAddress will do a SKIP_ROM if the address is invalid, but we don't
		// want to do that here so skip this address.
		return RESULT_WORKING;
	}


	config[0] = (uint8_t)th;
	config[1] = (uint8_t)tl;
	config[2] = ((conversionSize & 0b11) << 5) | 0b11111;


	DS2482SetConfigCommand *outer = this;
	stateHandler = &DS2482SetConfigCommand::waitState;

	// DS24821WireSendAddress does a 1-wire bus reset before sending the address
	pushCommandList();
	DS24821WireSendAddress::run(parent, addr, [outer](DS24821WireSendAddress &, int status) {
		DEBUG_HIGH(("DS2482SetConfigCommand SendAddress completion status=%d", status));
		outer->popCommandList();

		if (status != RESULT_DONE) {
			outer->finalResult = status;
			return;
		}

		// Successfully match ROMed the device

		outer->pushCommandList();

		DS24821WireWriteByte::run(outer->parent, WRITE_SCRATCHPAD, [outer](DS24821WireWriteByte &, int status) {
			DEBUG_HIGH(("DS2482SetConfigCommand WRITE_SCRATCHPAD completion status=%d", status));
			outer->popCommandList();

			if (status != RESULT_DONE) {
				outer->finalResult = status;
				return;
			}
			outer->configIndex = 0;
			outer->stateHandler = &DS2482SetConfigCommand::sendConfigState;
		});

	});


	return RESULT_WORKING;
}

int DS2482SetConfigCommand::waitState() {
	return RESULT_WORKING;
}

int DS2482SetConfigCommand::sendConfigState() {

	DS2482SetConfigCommand *outer = this;
	stateHandler = &DS2482SetConfigCommand::waitState;

	if (configIndex < sizeof(config)) {
		DEBUG_HIGH(("configIndex=%u value=%02x", configIndex, config[configIndex]));

		pushCommandList();
		DS24821WireWriteByte::run(parent, config[configIndex++], [outer](DS24821WireWriteByte &, int status) {
			DEBUG_HIGH(("DS2482SetConfigCommand write byte status=%d", status));
			outer->popCommandList();

			if (status != RESULT_DONE) {
				outer->finalResult = status;
				return;
			}

			outer->stateHandler = &DS2482SetConfigCommand::sendConfigState;
		});
	}
	else {
		// Done sending config bytes. Now do copy scratchpad
		DEBUG_HIGH(("saving config"));

		if (parasiticPower) {
			// Enable strong pull-up mode
			int status = strongPullUp(true);
			if (status) {
				return status;
			}
		}

		pushCommandList();

		// DS24821WireSendAddress does a 1-wire bus reset before sending the address
		pushCommandList();
		DS24821WireSendAddress::run(parent, addr, [outer](DS24821WireSendAddress &, int status) {
			DEBUG_HIGH(("DS2482SetConfigCommand SendAddress for copy scratchpad completion status=%d", status));
			outer->popCommandList();

			if (status != RESULT_DONE) {
				outer->finalResult = status;
				return;
			}

			// Successfully match ROMed the device

			DS24821WireWriteByte::run(outer->parent, COPY_SCRATCHPAD, [outer](DS24821WireWriteByte &, int status) {
				DEBUG_HIGH(("DS2482SetConfigCommand COPY_SCRATCHPAD completion status=%d", status));
				outer->popCommandList();

				if (status != RESULT_DONE) {
					outer->finalResult = status;
					return;
				}

				// Wait for 10 ms longer.
				outer->stateTime = millis();
				outer->stateHandler = &DS2482SetConfigCommand::copyWaitState;
			});
		});
	}
	return RESULT_WORKING;
}

int DS2482SetConfigCommand::copyWaitState() {
	if (millis() - stateTime > 10) {
		// Done copying

		if (parasiticPower) {
			// Disable strong pull-up mode
			int status = strongPullUp(false);
			if (status) {
				return status;
			}
		}


		// Move to next item
		stateHandler = &DS2482SetConfigCommand::nextDeviceState;
	}
	return RESULT_WORKING;
}


// static
DS2482SetConfigCommand &DS2482SetConfigCommand::run(DS2482 &parent, DS2482DeviceList &deviceList, bool saveToEEPROM, int conversionSize, std::function<void(DS2482SetConfigCommand&, int)> completion) {

	DS2482SetConfigCommand *result = new DS2482SetConfigCommand(parent, deviceList, saveToEEPROM, conversionSize, completion);
	result->pushCommand();
	return *result;
}


