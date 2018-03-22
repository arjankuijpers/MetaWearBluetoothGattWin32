#include "stdafx.h"
#include "Descriptor2.h"


Descriptor::Descriptor()
{
}


Descriptor::~Descriptor()
{
}

bool Descriptor::Initialize(PBTH_LE_GATT_DESCRIPTOR descriptor, HANDLE a_handleDevice)
{
	m_descriptor = descriptor;
	m_handleDevice = a_handleDevice;
	return false;
}

void Descriptor::Destroy()
{
}


void Descriptor::GetValue()
{

	USHORT descValueDataSize;
	HRESULT	hr = BluetoothGATTGetDescriptorValue(
		m_handleDevice,
		m_descriptor,
		0,
		NULL,
		&descValueDataSize,
		BLUETOOTH_GATT_FLAG_NONE);

	PBTH_LE_GATT_DESCRIPTOR_VALUE pDescValueBuffer = (PBTH_LE_GATT_DESCRIPTOR_VALUE)malloc(descValueDataSize);

	if (NULL == pDescValueBuffer) {
		printf("pDescValueBuffer out of memory\r\n");
	}
	else {
		RtlZeroMemory(pDescValueBuffer, descValueDataSize);
	}

	 hr = BluetoothGATTGetDescriptorValue(
		m_handleDevice,
		m_descriptor,
		(ULONG)descValueDataSize,
		pDescValueBuffer,
		NULL,
		BLUETOOTH_GATT_FLAG_NONE);
	if (S_OK != hr) {
		printf("BluetoothGATTGetDescriptorValue - Actual Data %d", hr);
	}
}

void Descriptor::SetValue()
{
	BTH_LE_GATT_DESCRIPTOR_VALUE newValue;
	RtlZeroMemory(&newValue, sizeof(newValue));

	

	HRESULT hr = BluetoothGATTSetDescriptorValue(
		m_handleDevice,
		m_descriptor,
		&newValue,
		BLUETOOTH_GATT_FLAG_NONE);
	if (S_OK != hr) {
		printf("BluetoothGATTGetDescriptorValue - Actual Data %d", hr);
	}
	else {
		printf("Setting value for serivice handle %d\n", m_descriptor->ServiceHandle);
	}
}
