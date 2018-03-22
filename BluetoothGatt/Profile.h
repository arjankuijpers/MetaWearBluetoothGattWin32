#pragma once
#include <vector>
#include "Service2.h"


class Profile
{
public:
	Profile();
	~Profile();

	bool Initialize(const char * a_deviceUUID);

	bool Initialize(GUID a_deviceGUID);

	void Destroy();
	HANDLE GetBLEHandle(GUID AGuid);

	void GetServices();

	HANDLE m_hLEDevice;

	std::vector<Service> m_services;
};

