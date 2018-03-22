#include "stdafx.h"
#include "Application.h"

#include "metawear\peripheral\led.h"
#include "metawear\sensor\switch.h"
#include "metawear\core\datasignal.h"

Application::Application()
{
}


Application::~Application()
{
}

bool Application::Initialize()
{
	m_profile.Initialize(GUID_METAWEAR_SERVICE);
	m_profileDeviceInfo.Initialize(GUID_DEVICE_INFO_SERVICE);

	m_mwDevice.Initialize(&m_profile, &m_profileDeviceInfo);


	MblMwLedPattern pattern;
	memset(&pattern, 0, sizeof(MblMwLedPattern));
	mbl_mw_led_stop_and_clear(m_mwDevice.GetMetaWearBoard());
	mbl_mw_led_load_preset_pattern(&pattern, MBL_MW_LED_PRESET_PULSE);
	mbl_mw_led_write_pattern(m_mwDevice.GetMetaWearBoard(), &pattern, MBL_MW_LED_COLOR_BLUE);
	mbl_mw_led_write_pattern(m_mwDevice.GetMetaWearBoard(), &pattern, MBL_MW_LED_COLOR_GREEN);
	mbl_mw_led_play(m_mwDevice.GetMetaWearBoard());



	static auto data_handler = [](void* context, const MblMwData* data) {
		if (*((uint32_t*)data->value)) {
			std::cout << "Switch Pressed" << std::endl;
		}
		else {
			std::cout << "Switch Released" << std::endl;
		}
	};

	auto switch_signal = mbl_mw_switch_get_state_data_signal(m_mwDevice.GetMetaWearBoard());
	mbl_mw_datasignal_subscribe(switch_signal, nullptr, data_handler);

	while (1) {
		Sleep(1000);
		//printf("look for notification");
	}

	return false;
}

void Application::Destroy()
{
}
