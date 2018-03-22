#include "stdafx.h"
#include "Profile.h"
#include "Config.h"

Profile::Profile()
	:m_hLEDevice(INVALID_HANDLE_VALUE)
{
}


Profile::~Profile()
{
}

bool Profile::Initialize(const char* a_deviceUUID)
{
	GUID deviceGUID;
	CLSIDFromString(TEXT(""), &deviceGUID);

	GetServices();
	return true;
}

bool Profile::Initialize(GUID a_deviceGUID)
{
	GUID deviceGUID = a_deviceGUID;
	m_hLEDevice = GetBLEHandle(deviceGUID);

	GetServices();


	return true;
}

void Profile::Destroy()
{
}

HANDLE Profile::GetBLEHandle(__in GUID AGuid)
{
	HDEVINFO hDI;
	SP_DEVICE_INTERFACE_DATA did;
	SP_DEVINFO_DATA dd;
	GUID BluetoothInterfaceGUID = AGuid;
	HANDLE hComm = NULL;

	hDI = SetupDiGetClassDevs(&BluetoothInterfaceGUID, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

	if (hDI == INVALID_HANDLE_VALUE) return NULL;

	did.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	dd.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDI, NULL, &BluetoothInterfaceGUID, i, &did); i++)
	{
		SP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData;

		DeviceInterfaceDetailData.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		DWORD size = 0;

		if (!SetupDiGetDeviceInterfaceDetail(hDI, &did, NULL, 0, &size, 0))
		{
			int err = GetLastError();

			if (err == ERROR_NO_MORE_ITEMS) break;

			PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)GlobalAlloc(GPTR, size);

			pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			if (!SetupDiGetDeviceInterfaceDetail(hDI, &did, pInterfaceDetailData, size, &size, &dd))
				break;

			hComm = CreateFile(
				pInterfaceDetailData->DevicePath,
				GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);

			GlobalFree(pInterfaceDetailData);
		}
	}

	SetupDiDestroyDeviceInfoList(hDI);
	return hComm;
}

void Profile::GetServices()
{
	USHORT serviceBufferCount;
	HRESULT hr = BluetoothGATTGetServices(
		m_hLEDevice, 0, nullptr, &serviceBufferCount, BLUETOOTH_GATT_FLAG_NONE);
	
	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr)
	{
		printf("BluetoothGATTGetServices - Error: %d\n", hr);
	}

	PBTH_LE_GATT_SERVICE pServiceBuffer = (PBTH_LE_GATT_SERVICE)malloc(sizeof(BTH_LE_GATT_SERVICE) * serviceBufferCount);

	if (nullptr == pServiceBuffer)
	{
		printf("services, out of memory.\n");
	}
	else
	{
		RtlZeroMemory(pServiceBuffer, sizeof(BTH_LE_GATT_SERVICE) * serviceBufferCount);
	}

	// retreive services.
	USHORT numServices;
	hr = BluetoothGATTGetServices(
		m_hLEDevice,
		serviceBufferCount,
		pServiceBuffer,
		&numServices,
		BLUETOOTH_GATT_FLAG_NONE);

	if (S_OK != hr) {
		printf("BluetoothGATTGetServices - NOT OK: %d\n", hr);
	}

	for (size_t i = 0; i < numServices; i++)
	{
		Service s;
		s.Initialize(&pServiceBuffer[i], m_hLEDevice);
		m_services.push_back(s);
	}
	

}