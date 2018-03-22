#pragma once
#include <vector>
#include "Characteristic2.h"


class Service
{
public:
	Service();
	~Service();

	bool Initialize(PBTH_LE_GATT_SERVICE service, HANDLE a_handleDevice);
	void Destroy();

	PBTH_LE_GATT_SERVICE m_service;

	std::vector<Characteristic*> m_characteristics;
	HANDLE m_handleDevice;
};

