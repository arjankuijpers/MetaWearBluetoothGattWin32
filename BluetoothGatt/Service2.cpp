#include "stdafx.h"
#include "Service2.h"


Service::Service()
{
}


Service::~Service()
{
}

bool Service::Initialize(PBTH_LE_GATT_SERVICE service, HANDLE a_handleDevice)
{
	m_service = service;
	m_handleDevice = a_handleDevice;


	USHORT charBufferSize;
	HRESULT hr = BluetoothGATTGetCharacteristics(m_handleDevice, m_service, 0, nullptr, &charBufferSize, BLUETOOTH_GATT_FLAG_NONE);

	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
		printf("BluetoothGATTGetCharacteristics - error %d", hr);
	}

	PBTH_LE_GATT_CHARACTERISTIC pCharBuffer;
	if (charBufferSize > 0) {
		pCharBuffer = (PBTH_LE_GATT_CHARACTERISTIC)
			malloc(charBufferSize * sizeof(BTH_LE_GATT_CHARACTERISTIC));

		if (NULL == pCharBuffer) {
			printf("pCharBuffer out of memory\r\n");
		}
		else {
			RtlZeroMemory(pCharBuffer,
				charBufferSize * sizeof(BTH_LE_GATT_CHARACTERISTIC));
		}

		////////////////////////////////////////////////////////////////////////////
		// Retrieve Characteristics
		////////////////////////////////////////////////////////////////////////////
		USHORT numChars;
		hr = BluetoothGATTGetCharacteristics(
			m_handleDevice,
			m_service,
			charBufferSize,
			pCharBuffer,
			&numChars,
			BLUETOOTH_GATT_FLAG_NONE);

		if (S_OK != hr) {
			printf("BluetoothGATTGetCharacteristics - Actual Data %d\n", hr);
		}

		if (numChars != charBufferSize) {
			printf("buffer size and buffer size actual size mismatch\r\n");
		}

		for (size_t i = 0; i < numChars; i++)
		{
			Characteristic* c = new Characteristic;
			c->Initialize(&pCharBuffer[i], m_handleDevice);
			m_characteristics.push_back(c);
		}

		

	}

	return true;
}

void Service::Destroy()
{
}
