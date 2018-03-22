#include "stdafx.h"
#include "Characteristic2.h"


//void BluetoothGattEvent(BTH_LE_GATT_EVENT_TYPE EventType, PVOID EventOutParameter, PVOID Context);

Characteristic::Characteristic()
	:m_notificationCallbackContext(nullptr),
	m_notificationCallbackFunc(nullptr)
{
}


Characteristic::~Characteristic()
{
}

bool Characteristic::Initialize(PBTH_LE_GATT_CHARACTERISTIC a_pCharacteristic, HANDLE a_handleDevice)
{
	m_handleDevice = a_handleDevice;
	m_characteristic = a_pCharacteristic;
	PBTH_LE_GATT_CHARACTERISTIC_VALUE pCharValueBuffer;

	// Determine Descriptor buffer size.
	USHORT descriptorBufferSize;
	HRESULT hr = BluetoothGATTGetDescriptors(a_handleDevice, a_pCharacteristic, 0, nullptr, &descriptorBufferSize, BLUETOOTH_GATT_FLAG_NONE);

	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
		printf("BluetoothGATTGetDescriptors - Buffer Size %d\n", descriptorBufferSize);
	}

	PBTH_LE_GATT_DESCRIPTOR pDescriptorBuffer;
	if (descriptorBufferSize > 0) {
		pDescriptorBuffer = (PBTH_LE_GATT_DESCRIPTOR)
			malloc(descriptorBufferSize
				* sizeof(BTH_LE_GATT_DESCRIPTOR));

		if (NULL == pDescriptorBuffer) {
			printf("pDescriptorBuffer out of memory\r\n");
		}
		else {
			RtlZeroMemory(pDescriptorBuffer, descriptorBufferSize);
		}

		////////////////////////////////////////////////////////////////////////////
		// Retrieve Descriptors
		////////////////////////////////////////////////////////////////////////////

		USHORT numDescriptors;
		hr = BluetoothGATTGetDescriptors(
			a_handleDevice,
			m_characteristic,
			descriptorBufferSize,
			pDescriptorBuffer,
			&numDescriptors,
			BLUETOOTH_GATT_FLAG_NONE);

		if (S_OK != hr) {
			printf("BluetoothGATTGetDescriptors - Actual Data %d\n", hr);
		}

		if (numDescriptors != descriptorBufferSize) {
			printf("buffer size and buffer size actual size mismatch\r\n");
		}

		for (int kk = 0; kk<numDescriptors; kk++) {
			PBTH_LE_GATT_DESCRIPTOR  currGattDescriptor = &pDescriptorBuffer[kk];
			////////////////////////////////////////////////////////////////////////////
			// Determine Descriptor Value Buffer Size
			////////////////////////////////////////////////////////////////////////////
			USHORT descValueDataSize;
			hr = BluetoothGATTGetDescriptorValue(
				a_handleDevice,
				currGattDescriptor,
				0,
				NULL,
				&descValueDataSize,
				BLUETOOTH_GATT_FLAG_NONE);

			if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
				printf("BluetoothGATTGetDescriptorValue - Buffer Size %d\n", hr);
			}

			PBTH_LE_GATT_DESCRIPTOR_VALUE pDescValueBuffer = (PBTH_LE_GATT_DESCRIPTOR_VALUE)malloc(descValueDataSize);

			if (NULL == pDescValueBuffer) {
				printf("pDescValueBuffer out of memory\r\n");
			}
			else {
				RtlZeroMemory(pDescValueBuffer, descValueDataSize);
			}

			////////////////////////////////////////////////////////////////////////////
			// Retrieve the Descriptor Value
			////////////////////////////////////////////////////////////////////////////

			hr = BluetoothGATTGetDescriptorValue(
				a_handleDevice,
				currGattDescriptor,
				(ULONG)descValueDataSize,
				pDescValueBuffer,
				NULL,
				BLUETOOTH_GATT_FLAG_NONE);
			if (S_OK != hr) {
				printf("BluetoothGATTGetDescriptorValue - Actual Data %d\n", hr);
			}
			//you may also get a descriptor that is read (and not notify) andi am guessing the attribute handle is out of limits
			// we set all descriptors that are notifiable to notify us via IsSubstcibeToNotification
			if (currGattDescriptor->AttributeHandle < 255) {
				BTH_LE_GATT_DESCRIPTOR_VALUE newValue;

				RtlZeroMemory(&newValue, sizeof(newValue));

				newValue.DescriptorType = ClientCharacteristicConfiguration;
				newValue.ClientCharacteristicConfiguration.IsSubscribeToNotification = TRUE;

				hr = BluetoothGATTSetDescriptorValue(
					a_handleDevice,
					currGattDescriptor,
					&newValue,
					BLUETOOTH_GATT_FLAG_NONE);
				if (S_OK != hr) {
					printf("BluetoothGATTGetDescriptorValue - Actual Data %d \n", hr);
				}
				else {
					printf("setting notification for service handle %d \n", currGattDescriptor->ServiceHandle);
				}

			}

		}
	}

	//


	//set the appropriate callback function when the descriptor change value
	BLUETOOTH_GATT_EVENT_HANDLE EventHandle;

	if (m_characteristic->IsNotifiable) {
		printf("Setting Notification for ServiceHandle %d\n", m_characteristic->ServiceHandle);
		BTH_LE_GATT_EVENT_TYPE EventType = CharacteristicValueChangedEvent;

		BLUETOOTH_GATT_VALUE_CHANGED_EVENT_REGISTRATION EventParameterIn;
		EventParameterIn.Characteristics[0] = *m_characteristic;
		EventParameterIn.NumCharacteristics = 1;
		hr = BluetoothGATTRegisterEvent(
			a_handleDevice,
			EventType,
			&EventParameterIn,
			Characteristic::BluetoothGattEvent,
			this,
			&EventHandle,
			BLUETOOTH_GATT_FLAG_NONE);

		if (S_OK != hr) {
			printf("BluetoothGATTRegisterEvent - NOT OK %d", hr);
		}
	}





	if (m_characteristic->IsReadable) {//currGattChar->IsReadable
		////////////////////////////////////////////////////////////////////////////
		// Determine Characteristic Value Buffer Size
		////////////////////////////////////////////////////////////////////////////
		USHORT charValueDataSize;
		hr = BluetoothGATTGetCharacteristicValue(
			a_handleDevice,
			m_characteristic,
			0,
			NULL,
			&charValueDataSize,
			BLUETOOTH_GATT_FLAG_NONE);

		if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
			printf("BluetoothGATTGetCharacteristicValue - Buffer Size %d", hr);
		}

		pCharValueBuffer = (PBTH_LE_GATT_CHARACTERISTIC_VALUE)malloc(charValueDataSize);

		if (NULL == pCharValueBuffer) {
			printf("pCharValueBuffer out of memory\r\n");
		}
		else {
			RtlZeroMemory(pCharValueBuffer, charValueDataSize);
		}

		////////////////////////////////////////////////////////////////////////////
		// Retrieve the Characteristic Value
		////////////////////////////////////////////////////////////////////////////

		hr = BluetoothGATTGetCharacteristicValue(
			a_handleDevice,
			m_characteristic,
			(ULONG)charValueDataSize,
			pCharValueBuffer,
			NULL,
			BLUETOOTH_GATT_FLAG_NONE);

		if (S_OK != hr) {
			printf("BluetoothGATTGetCharacteristicValue - NOT OK %d\n", hr);
		}

		//print the characeteristic Value
		//for an HR monitor this might be the body sensor location
		printf("\n Printing a read (not notifiable) characterstic: \n");
		for (int iii = 0; iii< pCharValueBuffer->DataSize; iii++) {// ideally check ->DataSize before printing
			printf("%c", pCharValueBuffer->Data[iii]);
		}
		printf("\n");


		// Free before going to next iteration, or memory leak.
		free(pCharValueBuffer);
		pCharValueBuffer = NULL;
	}


	return true;
}

void Characteristic::Destroy()
{
}

void* Characteristic::GetValue(uint8_t * bufferSizeOut)
{

	if (!m_characteristic->IsReadable)
	{
		return nullptr;
		*bufferSizeOut = 0;
	}

	USHORT charValueDataSize = 0;
	HRESULT hr = BluetoothGATTGetCharacteristicValue(
		m_handleDevice,
		m_characteristic,
		0,
		NULL,
		&charValueDataSize,
		BLUETOOTH_GATT_FLAG_NONE);

	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr)
	{
		printf("Characteristic::GetValue ERROR_MORE_DATA \n");
	}

	PBTH_LE_GATT_CHARACTERISTIC_VALUE pCharValueBuffer = (PBTH_LE_GATT_CHARACTERISTIC_VALUE)malloc(charValueDataSize);
	if (NULL == pCharValueBuffer) {
		printf("characteristic::GetValue out of memory\r\n");
		return nullptr;
		*bufferSizeOut = 0;
	}
	else {
		RtlZeroMemory(pCharValueBuffer, charValueDataSize);
	}

	hr = BluetoothGATTGetCharacteristicValue(
		m_handleDevice,
		m_characteristic,
		(ULONG)charValueDataSize,
		pCharValueBuffer,
		NULL,
		BLUETOOTH_GATT_FLAG_NONE);

	

	if (S_OK != hr) {
		printf("BluetoothGATTGetCharacteristicValue - failed %d", hr);
		return nullptr;
		*bufferSizeOut = 0;
	}

	
	//BTH_LE_GATT_CHARACTERISTIC_VALUE

	void* buff = malloc(pCharValueBuffer->DataSize);
	memcpy(buff, pCharValueBuffer->Data, pCharValueBuffer->DataSize);
	*bufferSizeOut = pCharValueBuffer->DataSize;

	free(pCharValueBuffer);

	return buff;
}

bool Characteristic::SetValue(const uint8_t * value, const uint8_t length)
{

	PBTH_LE_GATT_CHARACTERISTIC_VALUE newValue = (PBTH_LE_GATT_CHARACTERISTIC_VALUE)malloc(
		sizeof(BTH_LE_GATT_CHARACTERISTIC_VALUE) + length);


	RtlZeroMemory(newValue, sizeof(BTH_LE_GATT_CHARACTERISTIC_VALUE) + length);

	newValue->DataSize = length;
	memcpy(newValue->Data, value, length);

	HRESULT hr = BluetoothGATTSetCharacteristicValue(m_handleDevice, m_characteristic, newValue, NULL, BLUETOOTH_GATT_FLAG_NONE);
	if (hr != S_OK)
	{
		printf("BluetoothGATTSetCharacteristicValue - failed %d", hr);
		free(newValue);
		return false;
	}

	free(newValue);
	return true;
}

void Characteristic::SetNotificationCallback(CHARACTERISTIC_NOTIFICATION_CALLBACK callbackFunction, void * CallbackContext)
{
	m_notificationCallbackFunc = callbackFunction;
	m_notificationCallbackContext = CallbackContext;
}



void Characteristic::BluetoothGattEvent(BTH_LE_GATT_EVENT_TYPE EventType, PVOID EventOutParameter, PVOID Context)
{
	printf("Bluetooth Gatt Event \n");

	


	Characteristic* ctx = reinterpret_cast<Characteristic*>(Context);
	PBLUETOOTH_GATT_VALUE_CHANGED_EVENT pGattValueChangedEvent = reinterpret_cast<PBLUETOOTH_GATT_VALUE_CHANGED_EVENT>(EventOutParameter);

	if (!ctx->m_notificationCallbackContext || !ctx->m_notificationCallbackFunc)
	{
		printf("No event callback set. \n");
		return;
	}

	void* retBuffer = malloc(pGattValueChangedEvent->CharacteristicValue->DataSize);
	memcpy(retBuffer, pGattValueChangedEvent->CharacteristicValue->Data, pGattValueChangedEvent->CharacteristicValue->DataSize);

	ctx->m_notificationCallbackFunc(pGattValueChangedEvent->CharacteristicValueDataSize, retBuffer, ctx->m_notificationCallbackContext, ctx);


}
