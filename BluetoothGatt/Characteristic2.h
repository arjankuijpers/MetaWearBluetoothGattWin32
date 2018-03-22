#pragma once

#include <vector>
#include "Descriptor2.h"

class Characteristic;

typedef void(*CHARACTERISTIC_NOTIFICATION_CALLBACK)(
	_In_ size_t changedCharValueDataSize,
	_In_ void* bufferCharValue,
	_In_ void* Context,
	Characteristic* characteristicCalledUpon
	);


class Characteristic
{
public:
	Characteristic();
	~Characteristic();

	bool Initialize(PBTH_LE_GATT_CHARACTERISTIC a_pCharacteristic, HANDLE a_handleDevice);
	void Destroy();


	// buffer should be freed by user, function does not.
	void* GetValue(uint8_t* bufferSizeOut);
	bool SetValue(const uint8_t* value, const uint8_t length);
	void SetNotificationCallback(CHARACTERISTIC_NOTIFICATION_CALLBACK callbackFunction, void* CallbackContext);
	static void BluetoothGattEvent(BTH_LE_GATT_EVENT_TYPE EventType, PVOID EventOutParameter, PVOID Context);


	HANDLE m_handleDevice;
	PBTH_LE_GATT_CHARACTERISTIC m_characteristic;

	std::vector<Descriptor> m_descriptors;
	CHARACTERISTIC_NOTIFICATION_CALLBACK m_notificationCallbackFunc;
	void* m_notificationCallbackContext;


	
};

