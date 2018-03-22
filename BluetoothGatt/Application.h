#pragma once

#include "Profile.h"
#include "Config.h"

// MWDevice, .. temp
#include "MWDevice.h"

class Application
{
public:
	Application();
	~Application();


	bool Initialize();
	void Destroy();

	Profile m_profile;
	Profile m_profileDeviceInfo;

	MWDevice m_mwDevice;
};

