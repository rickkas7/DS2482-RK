#ifndef __DS2482_RK_H
#define __DS2482_RK_H

#include "Particle.h"

#include <list>

class DS2482;

/**
 * \defgroup Device Class for DS2482 devices
 * \defgroup Commands Commonly used commands
 * \defgroup Utility Utility classes
 * \defgroup Internal Internal and low-level classes
 */

/**
 * @brief Useful static constants, but the class is mostly just a base class for other commands.
 *
 * You won't need to instantiate this but will often use constants like RESULT_DONE in it.
 *
 * @ingroup Utility
 */
class DS2482Command {
public:
	/**
	 * @brief Construct a command object.
	 *
	 * You won't need to do this, as this class is only used as superclass of other command classes
	 */
	DS2482Command(DS2482 &parent);

	virtual ~DS2482Command(); //!< Destructor

	/**
	 * @brief Overridden by subclasses to do things; typically to run the state machine.
	 */
	virtual int loop();

	static const int RESULT_WORKING = 0; //!< Asynchronous call still in progress
	static const int RESULT_DONE = 1; //!< Asynchronous call completed successfully

	// Error codes are negative numbers
	// Wire.endTransmission errors (value * -1)
	static const int RESULT_I2C_ENDTRANSMISSION_BUSY_ERROR = -1; //!< Busy timeout upon entering endTransmission (I2C error)
	static const int RESULT_I2C_ENDTRANSMISSION_START_ERROR = -2; //!< Start bit generation timeout  (I2C error)
	static const int RESULT_I2C_ENDTRANSMISSION_ADDRESS_ERROR = -3; //!< End of address transmission timeout  (I2C error)
	static const int RESULT_I2C_ENDTRANSMISSION_TRANSFER_ERROR = -4; //!< Data byte transfer timeout  (I2C error)
	static const int RESULT_I2C_ENDTRANSMISSION_AFTER_TRANSFER_ERROR = -5; //!< Data byte transfer succeeded, busy timeout immediately after  (I2C error)

	// Misc errors
	static const int RESULT_I2C_READ_TIMEOUT = -100; //!< I2C requestFrom did not return bytes
	static const int RESULT_I2C_COMMAND_TIMEOUT = -101; //!< An I2C command timed out
	static const int RESULT_1WIRE_SHORT = -200; //!< On a 1-wire bus reset, a short was detected (SD flag set)
	static const int RESULT_1WIRE_BUSY = -201; //!< The previous 1-wire command had not completed yet
	static const int RESULT_1WIRE_NO_DEVICE = -202; //!< No device on the 1-wire bus when doing a 1-wire send address (no presence pulse detected)
	static const int RESULT_TOO_MANY_RETRIES = -300; //!< Specified number of retries exceeded without getting a response with a valid CRC
	static const int RESULT_SEARCH_FAILED = -301; //!< During a 1-wire search bus, got an invalid set of bits returned
	static const int RESULT_SET_CHANNEL_FAILED = -303; //!< A DS2482-800 set channel operation failed to set the channel

	static const uint8_t TRIPLET_CMD = 0x78; //!< DS2482 I2C command code
	static const uint8_t SINGLE_1WIRE_BIT_CMD = 0x87; //!< DS2482 I2C command code
	static const uint8_t WRITE_BYTE_CMD = 0xa5; //!< DS2482 I2C command code
	static const uint8_t READ_BYTE_CMD = 0x96; //!< DS2482 I2C command code
	static const uint8_t RESET_1WIRE_CMD = 0xb4; //!< DS2482 I2C command code
	static const uint8_t CHANNEL_SELECT_CMD = 0xc3; //!< DS2482 I2C command code
	static const uint8_t WRITE_CONFIG_CMD = 0xd2; //!< DS2482 I2C command code
	static const uint8_t SET_READ_PTR_CMD = 0xe1; //!< DS2482 I2C command code
	static const uint8_t DEVICE_RESET_CMD = 0xf0; //!< DS2482 I2C command code

	static const uint8_t CONFIG_REG = 0xc3; //!< DS2482 register code
	static const uint8_t READ_DATA_REG = 0xe1; //!< DS2482 register code
	static const uint8_t STATUS_REG = 0xf0; //!< DS2482 register code


	static const uint8_t STATUS_DIR_MASK = 0b10000000; //!< DS2482 status register bit
	static const uint8_t STATUS_TSB_MASK = 0b01000000; //!< DS2482 status register bit
	static const uint8_t STATUS_SBR_MASK = 0b00100000; //!< DS2482 status register bit
	static const uint8_t STATUS_RST_MASK = 0b00010000; //!< DS2482 status register bit
	static const uint8_t STATUS_LL_MASK  = 0b00001000; //!< DS2482 status register bit
	static const uint8_t STATUS_SD_MASK  = 0b00000100; //!< DS2482 status register bit
	static const uint8_t STATUS_PPD_MASK = 0b00000010; //!< DS2482 status register bit
	static const uint8_t STATUS_1WB_MASK = 0b00000001; //!< DS2482 status register bit

	static const uint8_t CONFIG_1WS_MASK = 0b1000; //!< DS2482 config register bit
	static const uint8_t CONFIG_SPU_MASK = 0b0100; //!< DS2482 config register bit
	static const uint8_t CONFIG_APU_MASK = 0b0001; //!< DS2482 config register bit

	static const uint8_t SEARCH_ROM = 0xf0; //!< 1-wire ROM select command
	static const uint8_t READ_ROM = 0x33; //!< 1-wire ROM select command
	static const uint8_t MATCH_ROM = 0x55; //!< 1-wire ROM select command
	static const uint8_t SKIP_ROM = 0xcc; //!< 1-wire ROM select command

	static const uint8_t ALARM_SEARCH = 0xec; //!< DS18B20 command
	static const uint8_t CONVERT_T = 0x44; //!< DS18B20 command
	static const uint8_t WRITE_SCRATCHPAD = 0x4e; //!< DS18B20 command
	static const uint8_t READ_SCRATCHPAD = 0xbe; //!< DS18B20 command
	static const uint8_t COPY_SCRATCHPAD = 0x48; //!< DS18B20 command
	static const uint8_t RECALL_E2 = 0xb8; //!< DS18B20 command
	static const uint8_t READ_POWER_SUPPLY = 0xb4; //!< DS18B20 command

	static const int CONVERSION_9BIT = 0; //!< 9-bit conversion size, resolution of 1/2 degree C, conversion time 94 ms
	static const int CONVERSION_10BIT = 1; //!< 10-bit conversion size, resolution of 1/4 degree C, conversion time 188 ms
	static const int CONVERSION_11BIT = 2; //!< 11-bit conversion size, resolution of 1/8 degree C, conversion time 375 ms
	static const int CONVERSION_12BIT = 3; //!< 12-bit conversion size, resolution of 1/16 degree C, conversion time 750 ms (default)

	static const int RETRIES_DEFAULT = 3; //!< Default numnber of retries if you don't override it with withRetries()


protected:
	/**
	 * @brief Add this command to the parent queue
	 */
	void pushCommand();


	// DS2482 low-level calls
	/**
	 * @brief Pushes a command list onto the command list stack. This is done when a command needs their own sequence of commands to run.
	 */
	void pushCommandList();

	/**
	 * @brief Pops a command list off the command list stack. This is done when a command needs their own sequence of commands to run.
	 */
	void popCommandList();

	/**
	 * @brief Reads the status DS2482 status register
	 */
	int readStatus(uint8_t &value, bool setReadPointerFirst = true);

	/**
	 * @brief Reads the status DS2482 data register
	 */
	int readData(uint8_t &value, bool setReadPointerFirst = true);

	/**
	 * @brief Sets the DS2482 read pointer to a specific register (CONFIG_REG, READ_DATA_REG, READ_DATA_REG)
	 */
	int setReadPointer(uint8_t reg);

	/**
	 * @brief Reads the DS2482 config register. Always sets the data pointer first.
	 */
	int readConfig(uint8_t &value);

	/**
	 * @brief Writes the DS2482 config register
	 */
	int writeConfig(bool speed, bool spu, bool apu);

	/**
	 * @brief Writes a DS2482 register
	 */
	int writeReg0(uint8_t reg);

	/**
	 * @brief Writes the DS2482 config register and a value
	 */
	int writeReg1(uint8_t reg, uint8_t value);

	/**
	 * @brief Enables or disables strong pull-up mode. Used by parasitic power mode.
	 */
	int strongPullUp(bool on);

	// Low-level I2C calls, wrap Wire interface calls but handle address setting

	void beginTransmission(); //!< Reflects to DS2482 parent to do an I2C operation
	uint8_t endTransmission(bool stop = true); //!< Reflects to DS2482 parent to do an I2C operation
	uint8_t requestFrom(uint8_t numBytes, bool stop = true);  //!< Reflects to DS2482 parent to do an I2C operation
	size_t write(uint8_t val);  //!< Reflects to DS2482 parent to do an I2C operation
	size_t write(const uint8_t *buf, size_t count);  //!< Reflects to DS2482 parent to do an I2C operation
	int available(void);  //!< Reflects to DS2482 parent to do an I2C operation
	int read(void); //!< Reflects to DS2482 parent to do an I2C operation

	DS2482 &parent; //!< The DS2482 that this command is being run on
};

/**
 * @brief Used internally to hold a list of command objects to run.
 *
 * @ingroup Internal
 */
class DS2482CommandList {
public:
	DS2482CommandList();
	virtual ~DS2482CommandList();

	/**
	 * @brief Called from DS2482::loop() when this command list is the currently executing command list
	 */
	virtual int loop();

	/**
	 * @brief Removes the DS2482Command at the front of the queue and deletes it.
	 */
	void removeFront();

	/**
	 * @brief Clears all of the DS2482Command objects from the queue and deletes them.
	 */
	void clear();

	/**
	 * @brief Adds a DS2482Command subclass to the end of this queue.
	 *
	 * Ownership of the object transfers and it will deleted when it's no longer used. Thus you
	 * cannot pass a stack-allocated object to this method.
	 */
	void push(DS2482Command *cmd);

private:
	std::list<DS2482Command *> list;
};

/**
 * @brief Resets the DS2482
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Commands
 */
class DS2482DeviceReset : public DS2482Command {
private:
	DS2482DeviceReset(DS2482 &parent, std::function<void(DS2482DeviceReset&, int status)> completion);
	virtual ~DS2482DeviceReset();

	virtual int loop();

public:
	/**
	 * @brief Resets the DS2482
	 *
	 * @param parent The DS2482 you want to send the command to.
	 *
	 * @param completion The completion handler lambda function. status is the result status of the call; if DS2482Command::RESULT_DONE then the call succeeded.
	 *
	 * This call executes asynchronously. The completion function is called when complete or an error occurs.
	 */
	static DS2482DeviceReset &run(DS2482 &parent, std::function<void(DS2482DeviceReset&, int status)> completion);

private:
	int startState();
	int waitForResetState();
	int setConfig();

	std::function<void(DS2482DeviceReset&, int)> completion;
	std::function<int(DS2482DeviceReset&)> stateHandler = &DS2482DeviceReset::startState;
	unsigned long stateTime;
	friend class DS2482;
};

/**
 * @brief Resets the 1-wire bus.
 *
 * You normally don't need to do this yourself, as it's done internally when necessary, such as before
 * a bus search.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS24821WireReset : public DS2482Command {
private:
	DS24821WireReset(DS2482 &parent, std::function<void(DS24821WireReset&, int status, bool presenceDetected)> completion);
	virtual ~DS24821WireReset();

	virtual int loop();

public:
	/**
	 * @brief Low-level call to reset the 1-wire bus
	 */
	static DS24821WireReset &run(DS2482 &parent, std::function<void(DS24821WireReset&, int status, bool presenceDetected)> completion);

	/**
	 * @brief Get whether a presence pulse was detected after 1-wire reset. Also passed to the completion function.
	 */
	inline bool getPresenceDetected() const { return presenceDetected; }

private:
	int startState();
	int waitState();

	std::function<void(DS24821WireReset&, int, bool)> completion;
	std::function<int(DS24821WireReset&)> stateHandler = &DS24821WireReset::startState;
	unsigned long stateTime;
	bool presenceDetected;
	friend class DS2482;
};

/**
 * @brief Low-level class to read a byte from the 1-wire bus.
 *
 * You will likely use the higher-level functions instead.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS24821WireReadByte : public DS2482Command {
private:
	DS24821WireReadByte(DS2482 &parent, std::function<void(DS24821WireReadByte&, int, uint8_t)> completion);
	virtual ~DS24821WireReadByte();

	virtual int loop();

public:
	/**
	 * @brief Low-level call to read a byte from the 1-wire bus
	 */
	static DS24821WireReadByte &run(DS2482 &parent, std::function<void(DS24821WireReadByte&, int, uint8_t)> completion);

	/**
	 * @brief Get the value that was read. Also is included as a parameter to the callback.
	 */
	inline uint8_t getValue() const { return value; }

private:
	int startState();
	int waitState();

	std::function<void(DS24821WireReadByte&, int, uint8_t)> completion;
	std::function<int(DS24821WireReadByte&)> stateHandler = &DS24821WireReadByte::startState;
	unsigned long stateTime;
	uint8_t value;
	friend class DS2482;
};

/**
 * @brief Low-level class to write a byte to the 1-wire bus.
 *
 * You will likely use the higher-level functions instead.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS24821WireWriteByte : public DS2482Command {
protected:
	/**
	 * @brief Used internally - you won't use this directly, but it is used by DS2482ReadRom, DS2482SearchRom, etc.
	 */
	DS24821WireWriteByte(DS2482 &parent, uint8_t value, std::function<void(DS24821WireWriteByte&, int)> completion);
	virtual ~DS24821WireWriteByte();

	virtual int loop();

public:
	/**
	 * @brief Low-level call to write a byte to the 1-wire bus
	 */
	static DS24821WireWriteByte &run(DS2482 &parent, uint8_t value, std::function<void(DS24821WireWriteByte&, int status)> completion);

private:
	int startState();
	int waitState();

	uint8_t value;
	std::function<void(DS24821WireWriteByte&, int)> completion;
	std::function<int(DS24821WireWriteByte&)> stateHandler = &DS24821WireWriteByte::startState;
	unsigned long stateTime;
	friend class DS2482;
};

/**
 * @brief Holds the 64-bit address of a device on a 1-wire bus.
 *
 * The address is unique for every device, set at the factory.
 *
 * @ingroup Utility
 */
class DS24821WireAddress {
public:
	/**
	 * @brief Construct an empty 1-wire address (all 0s)
	 */
	DS24821WireAddress();

	/**
	 * @brief Destructor
	 */
	virtual ~DS24821WireAddress();

	/**
	 * @brief Construct as a copy of another (copy constructor)
	 */
	DS24821WireAddress(const DS24821WireAddress &other);

	/**
	 * @brief Set this address to be the same as another (copy operator)
	 */
	DS24821WireAddress &operator=(const DS24821WireAddress &other);

	/**
	 * @brief Test against another address for equality (comparing the 8-byte address values, not pointer)
	 */
	bool operator==(const DS24821WireAddress &other) const;

	/**
	 * @brief Set the 1-wire address from a buffer of 8 unsigned bytes.
	 */
	DS24821WireAddress &withBytes(const uint8_t *buf);

	/**
	 * @brief Get a specific bit from the address
	 *
	 * @param bitNum Bits are numbered 0 <= bitNum < 64 and are in wire order. Bit 0 is the least significant
	 * bit of the least significant byte.
	 */
	bool getBit(size_t bitNum) const;

	/**
	 * @brief Set a specific bit from the address
	 *
	 * @param bitNum Bits are numbered 0 <= bitNum < 64 and are in wire order. Bit 0 is the least significant
	 * bit of the least significant byte.
	 *
	 * @param value Value to set the bit to. true or false, default is true.
	 */
	void setBit(size_t bitNum, bool value = true);

	/**
	 * @brief Get a byte from the address by offset
	 *
	 * @param offset The byte to get 0 <= offset < 8
	 *
	 * Note that the bytes are in wire order, LSB first. When printed, they're printed in reverse order,
	 * MSB first like a normal 64-bit integer.
	 */
	inline uint8_t operator[](size_t offset) const { return addr[offset]; };

	/**
	 * @brief Clears the address to all 0s.
	 *
	 * This is done during construction.
	 */
	void clear();

	/**
	 * @brief Returns true if the 1-wire address is valid
	 *
	 * A 1-wire address contains a CRC, and this method validates the CRC.
	 */
	bool isValid() const;

	/**
	 * @brief Outputs the 1-wire address as a string, 8 hexadecimal bytes
	 *
	 * Note that the presentation of addresses is MSB first, so the hex bytes will appear in the
	 * opposite order they're in the addr buffer.
	 */
	String toString() const;

	/**
	 * @brief The size of a 1-wire address in bytes (8 bytes = 64 bits).
	 */
	static const size_t ADDR_SIZE = 8;

protected:
	/**
	 * @brief The 1-wire address in wire order.
	 *
	 * Note that addr is in wire order (LSB first). When addresses are displayed
	 * to the user at 8 hexadecimal bytes, they're presented in the opposite order!
	 */
	uint8_t addr[ADDR_SIZE];
};

/**
 * @brief Holds the address and temperature for a single DS18B20 the 1-wire bus
 *
 * This hold the address, the last temperature, and whether the last retrieval
 * was valid or not.
 *
 * @ingroup Utility
 */
class DS2482Device {
public:
	/**
	 * @brief Constructs an empty DS2482Device object.
	 */
	DS2482Device();

	/**
	 * @brief Destructor for DS2482Device
	 */
	virtual ~DS2482Device();

	/**
	 * Constructs a DS2482Device object from a 1-wire address DS24821WireAddress.
	 */
	DS2482Device(const DS24821WireAddress &addr);

	/**
	 * @brief Constructs a DS2482Device object from another DS2482Device (copy constructor).
	 */
	DS2482Device(const DS2482Device &other);

	/**
	 * @brief Copies another DS2482Device object values into this one.
	 */
	DS2482Device &operator=(const DS2482Device &other);

	/**
	 * Returns a string representation of this object including the address, temperature, and valid flag.
	 */
	String toString() const;

	/**
	 * @brief Clears the address, temperature, and valid flags
	 */
	void clear();

	/**
	 * @brief Gets the temperature in degrees Celsius
	 *
	 * Note that the resolution depends on the conversion bit settings. It ranges from 1/2 degree (9-bit) to
	 * 1/16 degree (12-bit)
	 */
	inline float getTemperatureC() const { return temperatureC; };

	/**
	 * @brief Gets the temperature in degrees Fahrenheit
	 *
	 * The DS18B20 uses Celsius internally, so the resolution is reflected in that. The degrees F is
	 * the actual converted value and may have more decimal places.
	 */
	inline float getTemperatureF() const { return temperatureC * 9 / 5 + 32; };

	/**
	 * @brief Sets the temperature in degrees Celsius
	 *
	 * @param value Temperature in degrees Celsius (as a float)
	 */
	inline void setTemperatureC(float value) { temperatureC = value; };

	/**
	 * @brief Gets the valid flag for this temperature reading
	 *
	 * When getting a list of temperatures using DS2482GetTemperatureForListCommand, it's possible that
	 * not every sensor will return a value. The getValid allows you to figure out which ones are valid.
	 */
	inline bool getValid() const { return valid; };

	/**
	 * @brief Sets the valid flag
	 */
	inline void setValid(bool value) { valid = value; };

	/**
	 * @brief Gets a copy of the DS24821WireAddress address
	 */
	inline DS24821WireAddress getAddress() const { return addr; };

	/**
	 * @brief Sets the 1-wire address
	 *
	 * @param addr The address to set.
	 *
	 * The addr is copied, so it does not need to be preserved after this call completes.
	 */
	inline void setAddress(const DS24821WireAddress &addr) { this->addr = addr; };

protected:
	DS24821WireAddress addr; //!< The 1-wire address (8 bytes)
	float temperatureC; //!< The last temperature in degrees Celsius
	bool valid; //!< True if the last temperature read was successful
};

/**
 * @brief Holds an array of DS2482Device objects
 *
 * Normally you will use DS2482DeviceListStatic, below, instead of instantiating this object directly.
 *
 * This class does not allocate memory; you pass in a buffer and length and it keeps track of the
 * number of devices in the list. Note that deviceMax is in number of DS2482Device objects; the
 * actual size in bytes is 16 times that.
 *
 * @ingroup Utility
 */
class DS2482DeviceList {
public:
	/**
	 * @brief Constructs a DS2482DeviceList object with an array of DS2482Device objects
	 */
	DS2482DeviceList(DS2482Device *devices, size_t deviceMax);

	/**
	 * @brief Destructor for DS2482DeviceList
	 */
	virtual ~DS2482DeviceList();

	/**
	 * @brief Adds a DS24821WireAddress to the device list
	 *
	 * @param addr The 1-wire address to add. addr is copied.
	 *
	 * @return true if the address was added or false if the device list is full.
	 */
	bool addAddress(const DS24821WireAddress &addr);

	/**
	 * @brief Clears add of the objects in the array
	 */
	void clear();

	/**
	 * @brief Clears all of the valid values in the array
	 *
	 * This is done before reading temperatures using DS24821GetTemperatureForListCommand.
	 */
	void clearValid();

	/**
	 * @brief Gets the 1-wire address of a device in list by index
	 *
	 * @param ii The index to retrieve (0 is the first element)
	 *
	 * @return The address as a DS24821WireAddress (copied)
	 */
	inline DS24821WireAddress getAddressByIndex(size_t ii) { return devices[ii].getAddress(); };

	/**
	 * @brief Gets the DS2482Device of a device in list by index
	 *
	 * @param ii The index to retrieve (0 is the first element)
	 *
	 * @return The DS2482Device object (the actual object reference, not copied)
	 */
	inline DS2482Device &getDeviceByIndex(size_t ii) { return devices[ii]; };

	/**
	 * @brief Gets the DS2482Device of a device in list by index
	 *
	 * @param ii The index to retrieve (0 is the first element)
	 *
	 * @return The DS2482Device object (the actual object reference, not copied) as a const object that can't be modified.
	 *
	 * Note that DS2482Device supports the copy operator and copy constructor so it's easy to make your own copy of it.
	 */
	inline const DS2482Device &getDeviceByIndex(size_t ii) const { return devices[ii]; };

	/**
	 * @brief Returns the number of devices in the list (devices that have been added)
	 *
	 * @return The number of added devices (0 = none, 1 = one, ...)
	 */
	inline size_t getDeviceCount() { return deviceCount; };

	/**
	 * @brief Returns the maximum number of devices that can be added, based on the size of the array
	 *
	 * @return The maximum number of devices that can be added
	 *
	 * If getDeviceCount() == getDeviceMax() then the list is full and no more can be added.
	 */
	inline size_t getDeviceMax() { return deviceMax; };

protected:
	DS2482Device *devices; //!< The array of device objects
	size_t deviceCount; //!< The number of devices currently in the list (0 = none, 1 = one, ...)
	size_t deviceMax; //!< The maximum number of objects in the array
};

/**
 * @brief Holds an array of DS2482Device objects of a given size.
 *
 * You normally use this class to allocate the array on the heap, stack, or as a class member.
 * You need to know the maximum size in order to use this; the actual number of devices can
 * be less than the maximum size.
 *
 * The overhead is 16 bytes for each device. Note that size is in number of devices, not bytes.
 *
 * It's typically used like this:
 *
 * DS2482DeviceListStatic<10> deviceList;
 *
 * This allocates a variable deviceList that can hold up to 10 devices. You can include a device list
 * as a global variable, local variable (stack-allocated), a class member variable, or on the heap.
 *
 * @ingroup Utility
 */
template <size_t SIZE>
class DS2482DeviceListStatic : public DS2482DeviceList {
public:
	/**
	 * @brief Constructs a static list of devices
	 */
	explicit DS2482DeviceListStatic() : DS2482DeviceList(devices, SIZE) {};

private:
	DS2482Device devices[SIZE];
};

/**
 * @brief Low-level class to generate a 1-wire triplet, used for bus search. You won't need to call this directly.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS24821WireTriplet : public DS2482Command {
private:
	DS24821WireTriplet(DS2482 &parent, bool dirIn, std::function<void(DS24821WireTriplet&, int)> completion);
	virtual ~DS24821WireTriplet();

	virtual int loop();

public:
	/**
	 * @brief Runs a 1-wire triplet operation - used internally to do bus search
	 */
	static DS24821WireTriplet &run(DS2482 &parent, bool dirIn, std::function<void(DS24821WireTriplet&, int status)> completion);

	/**
	 * @brief Gets the status bit result from 1-wire triplet
	 */
	inline uint8_t getStatusBits() const { return statusBits; }

	/**
	 * @brief Returns the state of the SBR in the status register
	 */
	inline bool getSBR() const { return (statusBits & STATUS_SBR_MASK) != 0; }

	/**
	 * @brief Returns the state of the TSB in the status register
	 */
	inline bool getTSB() const { return (statusBits & STATUS_TSB_MASK) != 0; }

	/**
	 * @brief Returns the state of the DIR in the status register
	 */
	inline bool getDIR() const { return (statusBits & STATUS_DIR_MASK) != 0; }

	// SBR TSB
	// 0   0    Both bits represented on bus - writes DIR
	// 0   1    All devices have a 0 in this position - writes 0
	// 1   0    All devices have a 1 in this position - writes 1
	// 1   1    Error - writes 1

private:
	int startState();
	int waitState();

	bool dirIn;
	std::function<void(DS24821WireTriplet&, int)> completion;
	std::function<int(DS24821WireTriplet&)> stateHandler = &DS24821WireTriplet::startState;
	unsigned long stateTime;
	uint8_t statusBits;
	friend class DS2482;
};

/**
 * @brief When using the DS2482-800, this selects which of the 8 channels you want to work with.
 *
 * On power-up, channel 0 is selected. Valid channel numbers are 0 <= channel < 8.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Commands
 */
class DS2482ChannelSelect : public DS2482Command {
private:
	DS2482ChannelSelect(DS2482 &parent, int channel, std::function<void(DS2482ChannelSelect&, int)> completion);
	virtual ~DS2482ChannelSelect();

	virtual int loop();

public:
	/**
	 * @brief When using the DS2482-800, this selects which of the 8 channels you want to work with.
	 *
	 * @param parent The DS2482-800 you want to send the command to.
	 *
	 * @param channel The channel number to set to where 0 <= channel < 8. Default at power-up is 0.
	 *
	 * @param completion The completion handler lambda function. status is the result status of the call; if DS2482Command::RESULT_DONE then the call succeeded.
	 *
	 * This call executes asynchronously. The completion function is called when complete or an error occurs.
	 */
	static DS2482ChannelSelect &run(DS2482 &parent, int channel, std::function<void(DS2482ChannelSelect&, int status)> completion);

private:
	int startState();
	int waitState();

	static const uint8_t channelCodes[8];
	static const uint8_t channelResponseCodes[8];

	int channel;
	std::function<void(DS2482ChannelSelect&, int)> completion;
	std::function<int(DS2482ChannelSelect&)> stateHandler = &DS2482ChannelSelect::startState;
	friend class DS2482;
};

/**
 * @brief Low-level class to send a 1-wire command and read a number of bytes back. You won't need to call this directly.
 *
 * There is no run method in this class as it's only subclassed by specific commands.
 *
 * @ingroup Internal
 */
class DS24821WireCommand : public DS2482Command {
protected:
	/**
	 * @brief Sends a 1-wire command and reads bytes back. Used internally.
	 */
	DS24821WireCommand(DS2482 &parent, uint8_t cmd, size_t readSize, std::function<void(DS24821WireCommand&, int status)> completion);
	virtual ~DS24821WireCommand(); //!< Destructor

	virtual int loop(); //!< Used to provide allow the command to executed.

public:
	/**
	 * @brief Get the number of bytes that have been saved in the buffer so far
	 */
	inline size_t getSize() const { return offset; }

	/**
	 * @brief Gets a pointer to the buffer
	 */
	inline const uint8_t *getBuffer() const { return buffer; }

	/**
	 * @brief Gets the buffer as a 1-wire address
	 *
	 * A copy is returned as a DS24821WireAddress object.
	 */
	DS24821WireAddress getAddress() const;

	/**
	 * @brief Checks the CRC of the buffer
	 *
	 * The assumption is that the CRC value will be at (offset - 1). It will calculate the CRC of the bytes
	 * from 0 to (offset - 2) inclusive.
	 */
	bool checkCRC() const;

private:
	int startState();
	int waitState();
	int nextReadState();

	uint8_t cmd;
	size_t readSize;
	std::function<void(DS24821WireCommand&, int)> completion;
	size_t offset;
	std::function<int(DS24821WireCommand&)> stateHandler = &DS24821WireCommand::startState;
	unsigned long stateTime;
	uint8_t buffer[32];
	int finalResult;
	friend class DS2482;
};

/**
 * @brief Low-level class to send a 1-wire address. You won't need to call this directly.
 *
 * This is used after sending a MATCH_ROM to send the actual ROM address to match.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS24821WireSendAddress : public DS2482Command {
private:
	DS24821WireSendAddress(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS24821WireSendAddress&, int)> completion);
	virtual ~DS24821WireSendAddress();

	virtual int loop();

public:
	/**
	 * @brief Runs a 1-wire send address to send an address over the wire - used internally.
	 */
	static DS24821WireSendAddress &run(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS24821WireSendAddress&, int status)> completion);

private:
	int startState();
	int waitState();
	int sendAddrState();

	DS24821WireAddress addr;
	std::function<void(DS24821WireSendAddress&, int)> completion;
	std::function<int(DS24821WireSendAddress&)> stateHandler = &DS24821WireSendAddress::startState;
	unsigned long stateTime;
	int finalResult;
	size_t offset;
	friend class DS2482;
};


/**
 * @brief Low-level class to send a READ_ROM command.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS2482ReadRom : public DS24821WireCommand {
private:
	DS2482ReadRom(DS2482 &parent, std::function<void(DS24821WireCommand&, int)> completion);
	virtual ~DS2482ReadRom();

public:
	/**
	 * @brief Sends a READ_ROM command - used internally
	 */
	static DS2482ReadRom &run(DS2482 &parent, std::function<void(DS24821WireCommand&, int status)> completion);
	friend class DS2482;
};

/**
 * @brief Low-level class to send a SKIP_ROM command. This is used internally by DS2482ReadScratchpad and others.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS2482SkipRom : public DS24821WireWriteByte {
private:
	DS2482SkipRom(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int)> completion);
	virtual ~DS2482SkipRom();

public:
	/**
	 * @brief Sends a SKIP_ROM command - used internally
	 */
	static DS2482SkipRom &run(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int status)> completion);
	friend class DS2482;
};

/**
 * @brief Low-level class to send a MATCH_ROM command. This is used internally by DS2482ReadScratchpad and others.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS2482MatchRom : public DS24821WireWriteByte {
private:
	DS2482MatchRom(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int)> completion);
	virtual ~DS2482MatchRom();

public:
	/**
	 * @brief Sends a MATCH_ROM command - used internally
	 */
	static DS2482MatchRom &run(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int status)> completion);
	friend class DS2482;
};

/**
 * @brief Low-level class to send a SEARCH_ROM command. Normally you'd use DS2482SearchBusCommand instead.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS2482SearchRom : public DS24821WireWriteByte {
private:
	DS2482SearchRom(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int)> completion);
	virtual ~DS2482SearchRom();

public:
	/**
	 * @brief Sends a SEARCH_ROM command - used internally
	 */
	static DS2482SearchRom &run(DS2482 &parent, std::function<void(DS24821WireWriteByte&, int status)> completion);
	friend class DS2482;
};

/**
 * @brief Low-level class to send a CONVERT_T command.
 *
 * This is a low-level command to send a ConvertT command to the DS18B20s that only does the
 * conversion but does not read the result. You'll typically want to use DS2482GetTemperatureCommand or
 * DS2482GetTemperatureForListCommand instead.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS2482ConvertTCommand : public DS2482Command {
private:
	DS2482ConvertTCommand(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482ConvertTCommand&, int)> completion);
	virtual ~DS2482ConvertTCommand();

	virtual int loop();

public:
	/**
	 * @brief Sets the conversion size in bits. Default is CONVERSION_12BIT.
	 *
	 * @param value The number of bits the DS18B20s are configured for. The constants are defined in the
	 * DS2482Command class and are one of: CONVERSION_9BIT, CONVERSION_10BIT, CONVERSION_11BIT, or CONVERSION_12BIT.
	 * At 9 bits, the resolution is 1/2 degrees C. At 12 bits, the resolution is 1/16 degrees C. The factory default
	 * hardware setting is 12 bits. Note that you must have use DS2482SetConfig to set the conversion size first;
	 * just reducing the value in this call without changing the DS18B20 configuration will result in invalid
	 * readings.
	 *
	 * In the unusual situation that you have sensors of different bit settings, select the largest bit setting.
	 */
	inline DS2482ConvertTCommand &withConversionSize(int value) { conversionSize = value; return *this; };

	/**
	 * @brief Sets whether to use parasitic power or not. The default is false.
	 */
	inline DS2482ConvertTCommand &withParasiticPower(bool value = true) { parasiticPower = value; return *this; };

	/**
	 * @brief Sends a CONVERT_T command - used internally
	 *
	 * Normally you use DS2482GetTemperatureCommand instead of this low-level function.
	 */
	static DS2482ConvertTCommand &run(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482ConvertTCommand&, int status)> completion);


private:
	int startState();
	int waitState();
	int timeWaitState();

	static const uint16_t convTime[4];

	std::function<void(DS2482ConvertTCommand&, int)> completion;
	std::function<int(DS2482ConvertTCommand&)> stateHandler = &DS2482ConvertTCommand::startState;
	DS24821WireAddress addr;
	int conversionSize;
	bool parasiticPower;
	unsigned long stateTime;
	int finalResult;
	unsigned long tconv;
	friend class DS2482;
};

/**
 * @brief Low-level call to read the scratchpad
 *
 * The most common use for reading the scratchpad is to get the previously converted temperature value.
 *
 * You'll typically want to use DS2482GetTemperatureCommand or DS2482GetTemperatureForListCommand instead
 * which uses DS2482ConvertTCommand and DS2482ReadScratchpadCommand internally.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS2482ReadScratchpadCommand : public DS2482Command {
private:
	DS2482ReadScratchpadCommand(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482ReadScratchpadCommand&, int, uint8_t *)> completion);
	virtual ~DS2482ReadScratchpadCommand();

	virtual int loop();

public:
	/**
	 * @brief Sets the number of times to retry reading the temperature. Default is 3.
	 */
	inline DS2482ReadScratchpadCommand &withMaxRetries(size_t value) { maxRetries = value; return *this; };

	/**
	 * @brief Gets the total number of retries.
	 *
	 * This should be 0 most of the time when the scratchpad read successfully the first time.
	 */
	inline size_t getNumRetries() const { return numRetries; };

	/**
	 * @brief Sends a READ_SCRATCHPAD command - used internally
	 */
	static DS2482ReadScratchpadCommand &run(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482ReadScratchpadCommand&, int status, uint8_t *scratchpad)> completion);

private:
	int startState();
	int waitState();
	int readScratchpadState();

	std::function<void(DS2482ReadScratchpadCommand&, int, uint8_t *)> completion;
	std::function<int(DS2482ReadScratchpadCommand&)> stateHandler = &DS2482ReadScratchpadCommand::startState;
	DS24821WireAddress addr;
	size_t maxRetries;
	size_t numRetries;
	unsigned long stateTime;
	int finalResult;
	uint8_t scratchpad[9];
	friend class DS2482;
};

/**
 * @brief Used internally to do the actual read scratchpad operation.
 *
 * Normally you'd use DS2482ReadScratchpadCommand that handles setting the address and handling retries.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Internal
 */
class DS2482ReadScratchpadInternal : public DS24821WireCommand {
private:
	DS2482ReadScratchpadInternal(DS2482 &parent, std::function<void(DS24821WireCommand&, int)> completion);
	virtual ~DS2482ReadScratchpadInternal();

public:
	/**
	 * @brief Sends a READ_SCRATCHPAD command - used internally
	 */
	static DS2482ReadScratchpadInternal &run(DS2482 &parent, std::function<void(DS24821WireCommand&, int status)> completion);
	friend class DS2482;
};

/**
 * @brief Gets the temperature of a single sensor.
 *
 * See also DS2482GetTemperatureForListCommand for an efficient way to read multiple sensors on a single 1-wire bus.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Commands
 */
class DS2482GetTemperatureCommand : public DS2482Command {
private:
	DS2482GetTemperatureCommand(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482GetTemperatureCommand&, int, float)> completion);
	virtual ~DS2482GetTemperatureCommand();

	virtual int loop();

public:
	/**
	 * @brief Sets the number of times to retry reading the temperature. Default is 3.
	 */
	inline DS2482GetTemperatureCommand &withMaxRetries(size_t value) { maxRetries = value; return *this; };

	/**
	 * @brief Sets the conversion size in bits. Default is CONVERSION_12BIT.
	 *
	 * @param value The number of bits the DS18B20s are configured for. The constants are defined in the
	 * DS2482Command class and are one of: CONVERSION_9BIT, CONVERSION_10BIT, CONVERSION_11BIT, or CONVERSION_12BIT.
	 * At 9 bits, the resolution is 1/2 degrees C. At 12 bits, the resolution is 1/16 degrees C. The factory default
	 * hardware setting is 12 bits. Note that you must have use DS2482SetConfig to set the conversion size first;
	 * just reducing the value in this call without changing the DS18B20 configuration will result in invalid
	 * readings.
	 *
	 * In the unusual situation that you have sensors of different bit settings, select the largest bit setting.
	 */
	inline DS2482GetTemperatureCommand &withConversionSize(int value) { conversionSize = value; return *this; };

	/**
	 * @brief Sets whether to use parasitic power or not. The default is false.
	 */
	inline DS2482GetTemperatureCommand &withParasiticPower(bool value = true) { parasiticPower = value; return *this; };

	/**
	 * @brief Gets the total number of retries.
	 *
	 * This should be 0 most of the time when the sensor was read successfully the first time.
	 */
	inline size_t getNumRetries() const { return numRetries; };

	/**
	 * @brief Converts the 16-bit scratchpad temperature into a float (degrees C)
	 *
	 * It takes a conversion size because the least signficant bits in 9-11 bit operation are not zereoed -
	 * they contain random values. The conversionSize is used to mask them off to 0.
	 */
	static float convertTemp(const uint8_t *scratchpad, int conversionSize);

	/**
	 * @brief Gets the temperatures for a DS18B20 sensor on the 1-wire bus
	 *
	 * @param parent The DS2482 you want to send the command to. If it's a DS2482_800, make sure you set the channel as well.
	 *
	 * @param addr The address you want to read. For a single drop 1-wire bus you can pass a newly constructed empty DS24821WireAddress
	 * object which causes SKIP_ROM to be used instead of MATCH_ROM this eliminates the need to search the bus and more efficiently
	 * reads when there is only one DS18B20 on the 1-wire bus.
	 *
	 * @param completion The completion handler lambda function. status is the result status of the call; if DS2482Command::RESULT_DONE then the call succeeded.
	 *
	 * This call executes asynchronously. The completion function is called when complete or an error occurs.
	 * The execution time depends on the number of bits.
	 */
	static DS2482GetTemperatureCommand &run(DS2482 &parent, const DS24821WireAddress &addr, std::function<void(DS2482GetTemperatureCommand&, int status, float tempC)> completion);

private:
	int startState();
	int waitState();

	DS24821WireAddress addr;
	std::function<void(DS2482GetTemperatureCommand&, int, float)> completion;
	std::function<int(DS2482GetTemperatureCommand&)> stateHandler = &DS2482GetTemperatureCommand::startState;
	size_t maxRetries;
	int conversionSize;
	bool parasiticPower;
	unsigned long stateTime;
	int finalResult;
	float temperature;
	size_t numRetries = 0;
	friend class DS2482;
};

/**
 * @brief Gets the temperatures for multiple DS18B20 sensors on a single bus
 *
 * This is faster than calling DS2482GetTemperatureCommand for each sensor separately. The reason is that the
 * longest operation, ConvertT, takes 750 milliseconds (for 12-bit readings). When you use DS2482GetTemperatureForListCommand
 * ConvertT for all of the sensors on the bus is done at the same time, so you only incur the 750 millisecond time
 * once, regardless of how many sensors are on the bus. The individual readings need to be made for each DS18B20, but that's fast.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Commands
 */
class DS2482GetTemperatureForListCommand : public DS2482Command {
private:
	DS2482GetTemperatureForListCommand(DS2482 &parent, DS2482DeviceList &deviceList, std::function<void(DS2482GetTemperatureForListCommand&, int, DS2482DeviceList &deviceList)> completion);
	virtual ~DS2482GetTemperatureForListCommand();

	virtual int loop();

public:
	/**
	 * @brief Sets the number of times to retry reading the temperature. Default is 3.
	 */
	inline DS2482GetTemperatureForListCommand &withMaxRetries(size_t value) { maxRetries = value; return *this; };

	/**
	 * @brief Sets the conversion size in bits. Default is CONVERSION_12BIT.
	 *
	 * @param value The number of bits the DS18B20s are configured for. The constants are defined in the
	 * DS2482Command class and are one of: CONVERSION_9BIT, CONVERSION_10BIT, CONVERSION_11BIT, or CONVERSION_12BIT.
	 * At 9 bits, the resolution is 1/2 degrees C. At 12 bits, the resolution is 1/16 degrees C. The factory default
	 * hardware setting is 12 bits. Note that you must have use DS2482SetConfig to set the conversion size first;
	 * just reducing the value in this call without changing the DS18B20 configuration will result in invalid
	 * readings.
	 *
	 * In the unusual situation that you have sensors of different bit settings, select the largest bit setting.
	 */
	inline DS2482GetTemperatureForListCommand &withConversionSize(int value) { conversionSize = value; return *this; };

	/**
	 * @brief Sets whether to use parasitic power or not. The default is false.
	 */
	inline DS2482GetTemperatureForListCommand &withParasiticPower(bool value = true) { parasiticPower = value; return *this; };

	/**
	 * @brief Gets the total number of retries across all sensor reads.
	 *
	 * This should be 0 most of the time when all sensors were read successfully the first time.
	 */
	inline size_t getNumRetries() const { return numRetries; };

	/**
	 * @brief Gets the temperatures for a number of DS18B20 sensors on the 1-wire bus
	 *
	 * @param parent The DS2482 you want to send the command to. If it's a DS2482_800, make sure you set the channel as well.
	 *
	 * @param deviceList the DS2482DeviceList for the devices you want to get the temperatures for
	 *
	 * @param completion The completion handler lambda function. status is the result status of the call; if DS2482Command::RESULT_DONE then the call succeeded.
	 *
	 * This is faster than calling DS2482GetTemperatureCommand for each sensor separately. The reason is that the
	 * longest operation, ConvertT, takes 750 milliseconds (for 12-bit readings). When you use DS2482GetTemperatureForListCommand
	 * ConvertT for all of the sensors on the bus is done at the same time, so you only incur the 750 millisecond time
	 * once, regardless of how many sensors are on the bus. The individual readings need to be made for each DS18B20, but that's fast.
	 *
	 * Note that every DS18B20 on the 1-wire bus is commanded to convert their temperature, not just the ones in the deviceList.
	 * Only the temperatures for the sensors in the deviceList are retrieved, however.
	 *
	 * This call executes asynchronously. The completion function is called when complete or an error occurs.
	 */
	static DS2482GetTemperatureForListCommand &run(DS2482 &parent, DS2482DeviceList &deviceList, std::function<void(DS2482GetTemperatureForListCommand&, int status, DS2482DeviceList &deviceList)> completion);

private:
	int startState();
	int waitState();
	int nextSensorState();

	DS2482DeviceList &deviceList;
	std::function<void(DS2482GetTemperatureForListCommand&, int, DS2482DeviceList &deviceList)> completion;
	std::function<int(DS2482GetTemperatureForListCommand&)> stateHandler = &DS2482GetTemperatureForListCommand::startState;
	size_t maxRetries;
	int conversionSize;
	bool parasiticPower;
	unsigned long stateTime;
	int finalResult;
	size_t index;
	size_t numRetries = 0;
	friend class DS2482;
};

/**
 * @brief Checks the 1-wire bus to determine if it's single drop or multi-drop.
 *
 * Completion is called with NO_DEVICES, SINGLE_DROP, or MULTI_DROP indicating 0, 1, or more sensors on the
 * 1-wire bus.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Commands
 */
class DS2482CheckBusCommand : public DS2482Command {
private:
	DS2482CheckBusCommand(DS2482 &parent, std::function<void(DS2482CheckBusCommand&, int, int)> completion);
	virtual ~DS2482CheckBusCommand();

	virtual int loop();

public:
	/**
	 * @brief Check the 1-wire bus to see if there are 0, 1, or more devices on it
	 *
	 * @param parent The DS2482 you want to send the command to. If it's a DS2482_800, make sure you set the channel as well.
	 *
	 * @param completion The completion handler lambda function. status is the result status of the call; if DS2482Command::RESULT_DONE then the call succeeded. If it's any other value
	 * an error occurred. The busStatus is one of NO_DEVICES, SINGLE_DROP, or MULTI_DROP indicating 0, 1, or more sensors on the 1-wire bus.
	 *
	 * This call executes asynchronously. The completion function is called when complete or an error occurs.
	 */
	static DS2482CheckBusCommand &run(DS2482 &parent, std::function<void(DS2482CheckBusCommand&, int status, int busStatus)> completion);

	static const int NO_DEVICES = 0; //!< There are 0 devices on the 1-wire bus.
	static const int SINGLE_DROP = 1; //!< There is 1 device on the 1-wire bus. An empty address can be used to identify the single device.
	static const int MULTI_DROP = 2;  //!< There are 2 or more devices on the 1-wire bus. Addressing is required.

	/**
	 * @brief Gets the bus status: NO_DEVICES, NO_DEVICES, or MULTI_DROP
	 *
	 * You normally don't need this because it's passed as a parameter to the completion lambda.
	 */
	inline int getBusStatus() const { return busStatus; };

	/**
	 * @brief If SINGLE_DROP, gets the address of the single device on the bus
	 *
	 * Note that for DS2482GetTemperature you don't need the address for single drop; you can just pass
	 * an empty DS24821WireAddress and it will automatally use the only device on the bus.
	 */
	inline const DS24821WireAddress &getAddress() const { return addr; };

private:
	int startState();
	int waitState();

	std::function<void(DS2482CheckBusCommand&, int, int)> completion;
	std::function<int(DS2482CheckBusCommand&)> stateHandler = &DS2482CheckBusCommand::startState;
	DS24821WireAddress addr;
	unsigned long stateTime;
	int finalResult;
	int busStatus;
	friend class DS2482;
};

/**
 * @brief Internal struct used during 1-wire bus searches.
 *
 * @ingroup Internal
 */
typedef struct { // 12 bytes
	DS24821WireAddress addr; //!< The 1-wire address being searched
	size_t decisionBit; //!< The bit to swap on the next iteration
} DS2482SearchBranchPoint;

/**
 * @brief Finds all of the devices on the 1-wire bus
 *
 * If you have a multi-drop DS18B20 setup, this will find the 64-bit device addresses for every sensor on the
 * 1-wire bus.
 *
 * Searching the 1-wire bus temporarily requires about 800 bytes of heap space for the duration of the search.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Commands
 */
class DS2482SearchBusCommand : public DS2482Command {
private:
	DS2482SearchBusCommand(DS2482 &parent, DS2482DeviceList &deviceList, std::function<void(DS2482SearchBusCommand&, int)> completion);
	virtual ~DS2482SearchBusCommand();

	virtual int loop();

public:
	/**
	 * @brief Run a DS18B20 search bus command to find all of the DS18B20s on the 1-wire bus
	 *
	 * @param parent The DS2482 you want to send the command to. If it's a DS2482_800, make sure you set the channel as well.
	 *
	 * @param deviceList The DS2482DeviceList object you want to save the list of devices to. The object is cleared before use.
	 *
	 * @param completion The completion handler lambda function. status is the result status of the call; if DS2482Command::RESULT_DONE then the call succeeded. If it's any other value
	 * an error occurred.
	 *
	 * This call executes asynchronously. The completion function is called when complete or an error occurs.
	 */
	static DS2482SearchBusCommand &run(DS2482 &parent, DS2482DeviceList &deviceList, std::function<void(DS2482SearchBusCommand&, int status)> completion);

	/**
	 * @brief Get the device list that was passed into run
	 *
	 * You can use this from the completion, however it's not usually necessary since in most cases you'll
	 * be able to access the variable you originally passed to run from your capture. But if for some reason
	 * you don't want to capture it, you can use this.
	 */
	inline DS2482DeviceList &getDeviceList() { return devices; }

	/**
	 * @brief Get the device list that was passed into run a const object
	 */
	inline const DS2482DeviceList &getDeviceList() const { return devices; }

private:
	int startState();
	int searchState();
	int sendAddressTriplets();
	int waitState();

	DS2482DeviceList &devices;
	std::function<void(DS2482SearchBusCommand&, int)> completion;
	std::function<int(DS2482SearchBusCommand&)> stateHandler = &DS2482SearchBusCommand::startState;
	unsigned long stateTime;
	int finalResult;
	size_t currentBit;
	DS24821WireAddress addr;
	size_t decisionBit;
	bool dir;
	size_t branchStackIndex;
	DS2482SearchBranchPoint branchStack[64]; // 768 bytes
	int failures;
	friend class DS2482;
};

/**
 * @brief Sets the configuration for a device.
 *
 * Normally this is done to change the number of bits of conversion
 * precision. It can be used for the alarm feature as well, but alarm is not supported by this library
 * because the operation is a little strange and I'm not really convinced that it's that useful.
 *
 * As with all command objects, you do not typically construct one of these objects. Instead, use the
 * static run method to handle allocating, initializing, and queueing the command.
 *
 * @ingroup Commands
 */
class DS2482SetConfigCommand : public DS2482Command {
private:
	DS2482SetConfigCommand(DS2482 &parent, DS2482DeviceList &deviceList, bool saveToEEPROM, int conversionSize, std::function<void(DS2482SetConfigCommand&, int)> completion);
	virtual ~DS2482SetConfigCommand();

	virtual int loop();

public:
	/**
	 * Sets whether to use parasitic power or not. The default is false.
	 */
	inline DS2482SetConfigCommand &withParasiticPower(bool value = true) { parasiticPower = value; return *this; };

	/**
	 * Set the TL value, the low temperature alarm, which can also be used as an arbitrarary 8-bit value stored in EEPROM.
	 */
	inline DS2482SetConfigCommand &withTL(int8_t value) { tl = value; return *this; };

	/**
	 * Set the TH value, the hugh temperature alarm, which can also be used as an arbitrarary 8-bit value stored in EEPROM.
	 */
	inline DS2482SetConfigCommand &withTH(int8_t value) { th = value; return *this; };

	/**
	 * @brief Run a DS18B20 set configuration command to set DS18B20 configuration
	 *
	 * @param parent The DS2482 you want to send the command to. If it's a DS2482_800, make sure you set the channel as well.
	 *
	 * @param deviceList The list of devices you want to set the configuration on.
	 *
	 * @param saveToEEPROM Saves the configuration settings to EEPROM so they will be reused when power is reset.
	 *
	 * @param conversionSize The number of bits of conversion to do. The constants are defined in the DS2482Command class and
	 * are one of: CONVERSION_9BIT, CONVERSION_10BIT, CONVERSION_11BIT, or CONVERSION_12BIT. At 9 bits, the resolution is
	 * 1/2 degrees C. At 12 bits, the resolution is 1/16 degrees C. The factory default hardware setting is 12 bits.
	 *
	 * @param completion The completion handler lambda function. status is the result status of the call; if DS2482Command::RESULT_DONE then the call succeeded. If it's any other value
	 * an error occurred.
	 *
	 * @return The new configuration object so you can use the optional settings like withParasiticPower, fluent-style.
	 *
	 * Reducing the resolution reduces the amount of time to capture the current temperature (ConvertT):
	 *
	 * Bits  | Time (milliseconds)
	 * ----- | -------------------
	 *  9    |  94
	 * 10    | 188
	 * 11    | 375
	 * 12    | 750
	 *
	 * It doesn't affect the amount of data transmitted by the sensors; that's always 9 bytes, regardless of resolution.
	 *
	 * This call executes asynchronously. The completion function is called when complete or an error occurs.
	 */
	static DS2482SetConfigCommand &run(DS2482 &parent, DS2482DeviceList &deviceList, bool saveToEEPROM, int conversionSize, std::function<void(DS2482SetConfigCommand&, int status)> completion);

private:
	int startState();
	int waitState();
	int nextDeviceState();
	int sendConfigState();
	int copyWaitState();

	DS2482DeviceList &deviceList;
	bool saveToEEPROM;
	int conversionSize;
	std::function<void(DS2482SetConfigCommand&, int)> completion;
	bool parasiticPower;
	int8_t tl;
	int8_t th;
	std::function<int(DS2482SetConfigCommand&)> stateHandler = &DS2482SetConfigCommand::startState;
	unsigned long stateTime;
	int finalResult;
	size_t index;
	DS24821WireAddress addr;
	uint8_t config[3];
	size_t configIndex;
	friend class DS2482;
};

/**
 * @brief Class for DS2482 devices
 *
 * You need one instance of this class for each DS2482-100 or DS2482-800 you have on your I2C bus.
 *
 * You typically only ever need to call the setup() and loop() methods.
 *
 * @ingroup Device
 */
class DS2482 {
public:
	/**
	 * @brief Constructor for DS2482 objects. Instantiate one for each  DS2482-100 or DS2482-800 you have.
	 *
	 * @param wire The I2C interface to use, typically Wire, though it could be Wire1.
	 *
	 * @param addr The address. This is typically 0 <= addr < 8 for a DS2482-800 or 0 <= addr < 4 for a DS2482-100.
	 * If you specify a value >= 8 it's assumed to be an actual I2C addres 8 <= addr < 128.
	 */
	DS2482(TwoWire &wire, int addr);
	virtual ~DS2482();

	/**
	 * @brief setup call, call during setup()
	 */
	void setup();

	/**
	 * @brief loop call, call on every time through loop()
	 */
	void loop();


	/**
	 * Checks the CRC of a scratchpad buffer or an I2C address
	 *
	 * @return True if valid
	 */
	static bool checkCRC(const uint8_t *buf, size_t bufSize);


private:
	/**
	 * @brief Adds a DS2482Command object to the current command list
	 *
	 * You won't normally call this; run methods of command subclasses do.
	 */
	void pushCommand(DS2482Command *cmd);

	/**
	 * Gets the I2C address (0-127) of this DS2482
	 */
	uint8_t getI2CAddr() const;

	void beginTransmission(); //!< Reflects to Wire object to do an I2C operation with the correct address
	uint8_t endTransmission(bool stop = true); //!< Reflects to Wire object to do an I2C operation with the correct address
	uint8_t requestFrom(uint8_t numBytes, bool stop = true);  //!< Reflects to Wire object to do an I2C operation with the correct address
	size_t write(uint8_t val);  //!< Reflects to Wire object to do an I2C operation with the correct address
	size_t write(const uint8_t *buf, size_t count);  //!< Reflects to Wire object to do an I2C operation with the correct address
	int available(void);  //!< Reflects to Wire object to do an I2C operation with the correct address
	int read(void);  //!< Reflects to Wire object to do an I2C operation with the correct address

	/**
	 * Pushes a command list onto the command list stack. This is done when a command needs their own sequence of commands to run.
	 */
	void pushCommandList();

	/**
	 * Pops a command list off the command list stack. This is done when a command needs their own sequence of commands to run.
	 */
	void popCommandList();


	static const size_t COMMAND_LIST_STACK_SIZE = 4;

	TwoWire &wire;
	int addr;
	size_t commandListIndex = 0;
	DS2482CommandList commandListStack[COMMAND_LIST_STACK_SIZE];
	static const uint8_t crcTable[256];

	friend class DS2482Command;
};

#endif /* __DS2482_RK_H */
