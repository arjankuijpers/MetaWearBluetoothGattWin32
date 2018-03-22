#include "stdafx.h"
#include "MWDevice.h"
#include "metawear\platform\memory.h"
#include <sstream>
#include <string>
#include <array>
#include <cassert>

#include "metawear\core\data.h"
#include "metawear\core\datasignal.h"
#include "metawear/peripheral/led.h"
#include "metawear\sensor\switch.h"

MWDevice::MWDevice()
{
}


MWDevice::~MWDevice()
{
}

std::unordered_map<const void*, MblMwFnIntVoidPtrArray> MWDevice::notify_handlers;
std::unordered_map<const void*, MblMwFnVoidVoidPtrInt> MWDevice::dc_handlers;

bool MWDevice::Initialize(Profile * a_profile, Profile* a_profileDevInfo)
{
	m_profile = a_profile;
	m_profileDevInfo = a_profileDevInfo;

	//PrintAllCharacteristics();

	m_btle_conn = { this, MWDevice::write_gatt_char_v2, MWDevice::read_gatt_char_v2, MWDevice::enable_char_notify, MWDevice::on_disconnect };
	m_mwBoard = mbl_mw_metawearboard_create(&m_btle_conn);

	mbl_mw_metawearboard_initialize(m_mwBoard, this, [](void* context, MblMwMetaWearBoard* board, int32_t status) -> void {
		if (!status == MBL_MW_STATUS_OK) {
			printf("Error initializing board: %d\n", status);
		}
		else {
			printf("Board initialized\n");
		}
	});


	/*auto dev_info = mbl_mw_metawearboard_get_device_information(m_mwBoard);
	std::cout << "firmware = " << dev_info->firmware_revision << std::endl;
	mbl_mw_memory_free((void*)dev_info);
*/

	Sleep(10000);


	return true;
}

void MWDevice::Destroy()
{
}

Characteristic * MWDevice::GetCharacteristicFromGUID(GUID a_guid)
{
	GUID convertedGUID = {};
	convertedGUID.Data1 = a_guid.Data1;
	convertedGUID.Data1 = convertedGUID.Data1 >> 16;
	/*std::array<char, 40> output;
	snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", a_guid.Data1, a_guid.Data2, a_guid.Data3, a_guid.Data4[0], a_guid.Data4[1], a_guid.Data4[2], a_guid.Data4[3], a_guid.Data4[4], a_guid.Data4[5], a_guid.Data4[6], a_guid.Data4[7]);
	std::cout << std::endl << "searching for GUID: " << std::string(output.data()) << std::endl;*/


	for (size_t i = 0; i < m_profile->m_services.size(); i++)
	{
		for (size_t j = 0; j < m_profile->m_services[i].m_characteristics.size(); j++)
		{
			if (!m_profile->m_services[i].m_characteristics[j]->m_characteristic->CharacteristicUuid.IsShortUuid)
			{
				if (m_profile->m_services[i].m_characteristics[j]->m_characteristic->CharacteristicUuid.Value.LongUuid == a_guid)
				{
					return m_profile->m_services[i].m_characteristics[j];
				}
			}
			else
			{
				if (m_profile->m_services[i].m_characteristics[j]->m_characteristic->CharacteristicUuid.Value.LongUuid.Data1 == convertedGUID.Data1)
				{
					return m_profile->m_services[i].m_characteristics[j];
				}
			}
		}
	}

	for (size_t i = 0; i < m_profileDevInfo->m_services.size(); i++)
	{
		for (size_t j = 0; j < m_profileDevInfo->m_services[i].m_characteristics.size(); j++)
		{
			if (!m_profileDevInfo->m_services[i].m_characteristics[j]->m_characteristic->CharacteristicUuid.IsShortUuid)
			{
				if (m_profileDevInfo->m_services[i].m_characteristics[j]->m_characteristic->CharacteristicUuid.Value.LongUuid == a_guid)
				{
					return m_profileDevInfo->m_services[i].m_characteristics[j];
				}
			}
			else
			{
				if (m_profileDevInfo->m_services[i].m_characteristics[j]->m_characteristic->CharacteristicUuid.Value.LongUuid.Data1 == convertedGUID.Data1)
				{
					return m_profileDevInfo->m_services[i].m_characteristics[j];
				}
			}
		}
	}
	return nullptr;
}

MblMwMetaWearBoard * MWDevice::GetMetaWearBoard()
{
	assert(m_mwBoard);
	return m_mwBoard;
}

void MWDevice::read_gatt_char_v2(void * context, const void * caller, const MblMwGattChar * characteristic, MblMwFnIntVoidPtrArray handler)
{
	printf("read_gatt_char \n");

	MWDevice* mwdevice = static_cast<MWDevice*>(context);

	GUID g = mwdevice->HighLow2Uuid(characteristic->uuid_high, characteristic->uuid_low);
	Characteristic* charGATT = mwdevice->GetCharacteristicFromGUID(g);

	uint8_t buffSize = 0;

	uint8_t* buffer = (uint8_t*)charGATT->GetValue(&buffSize);

	handler(caller, buffer, buffSize);
	free(buffer);

}

void MWDevice::write_gatt_char_v2(void * context, const void * caller, MblMwGattCharWriteType write_type, const MblMwGattChar * characteristic, const uint8_t * value, uint8_t length)
{
	printf("write_gatt_char \n");

	MWDevice* mwdevice = static_cast<MWDevice*>(context);

	GUID g = mwdevice->HighLow2Uuid(characteristic->uuid_high, characteristic->uuid_low);
	Characteristic* charGATT = mwdevice->GetCharacteristicFromGUID(g);

	bool success = false;
	success = charGATT->SetValue(value, length);
	assert(success);


	/*switch (write_type)
	{
	case MBL_MW_GATT_CHAR_WRITE_WITH_RESPONSE:
		assert(true);
		break;
	case MBL_MW_GATT_CHAR_WRITE_WITHOUT_RESPONSE:
		
		break;
	default:
		assert(true);
		break;
	}*/
//
	

}

void MWDevice::enable_char_notify(void * context, const void * caller, const MblMwGattChar * characteristic, MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready)
{
	// replace with platform specific BluetoothGatt code

	MWDevice* mwdevice = static_cast<MWDevice*>(context);
	GUID g = mwdevice->HighLow2Uuid(characteristic->uuid_high, characteristic->uuid_low);
	Characteristic* charGATT = mwdevice->GetCharacteristicFromGUID(g);
	charGATT->SetNotificationCallback(MWDevice::NotifyCharChange, mwdevice);

	notify_handlers.insert({ caller, handler });
	// call the 'ready' function pointer when the enable notification requeset has finished
	ready(caller, MBL_MW_STATUS_OK);
}

void MWDevice::on_disconnect(void* context, const void* caller, MblMwFnVoidVoidPtrInt handler) {
	// call this handler everytime connection is lost, use 0 for 'value' parameter
	// handler(context, caller, 0)
	dc_handlers.insert({ caller, handler });
}

GUID MWDevice::HighLow2Uuid(const uint64_t high, const uint64_t low)
{
		GUID guid;
		std::stringstream sstream;
		sstream << std::hex << high;
		sstream << std::hex << low;
		std::string s = sstream.str();
	
	
		unsigned long p0;
		int p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
	
		int err = sscanf_s(s.c_str(), "%08lX%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
			&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
	
		guid.Data1 = p0;
		guid.Data2 = p1;
		guid.Data3 = p2;
		guid.Data4[0] = p3;
		guid.Data4[1] = p4;
		guid.Data4[2] = p5;
		guid.Data4[3] = p6;
		guid.Data4[4] = p7;
		guid.Data4[5] = p8;
		guid.Data4[6] = p9;
		guid.Data4[7] = p10;
	
		return guid;
}

void MWDevice::PrintAllCharacteristics()
{
	for (size_t i = 0; i < m_profile->m_services.size(); i++)
	{
		for (size_t j = 0; j < m_profile->m_services[i].m_characteristics.size(); j++)
		{
			//if (!m_profile->m_services[i].m_characteristics[j].m_characteristic->CharacteristicUuid.IsShortUuid)
			{
				GUID & guid = m_profile->m_services[i].m_characteristics[j]->m_characteristic->CharacteristicUuid.Value.LongUuid;
				std::array<char, 40> output;
				snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
				std::cout << std::endl << "GUID: " << std::string(output.data());

			}
		}
	}

	for (size_t i = 0; i < m_profileDevInfo->m_services.size(); i++)
	{
		for (size_t j = 0; j < m_profileDevInfo->m_services[i].m_characteristics.size(); j++)
		{
			//if (!m_profileDevInfo->m_services[i].m_characteristics[j].m_characteristic->CharacteristicUuid.IsShortUuid)
			{
				GUID& guid = m_profileDevInfo->m_services[i].m_characteristics[j]->m_characteristic->CharacteristicUuid.Value.LongUuid;

				std::array<char, 40> output;
				snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
				std::cout << std::endl << "GUID: " << std::string(output.data());
			}
		}
	}
	std::cout << std::endl;
}

void MWDevice::NotifyCharChange(
	_In_ size_t changedCharValueDataSize,
	_In_ void* bufferCharValue,
	_In_ void* Context,
	Characteristic* characteristicCalledUpon)
{

	MWDevice* mwdevice = reinterpret_cast<MWDevice*>(Context);

	
	/*for (size_t i = 0; i < notify_handlers.size(); i++)
	{
		notify_handlers.at((mwdevice->m_mwBoard, (uint8_t*)bufferCharValue, changedCharValueDataSize);

	}*/
	notify_handlers.at(mwdevice->m_mwBoard)(mwdevice->m_mwBoard, (uint8_t*)bufferCharValue, changedCharValueDataSize);
	//char_changed_handler(mwdevice->m_mwBoard, (uint8_t*)bufferCharValue, changedCharValueDataSize);

	/*mbl_mw_metaboard_char
	MblMwMetaWearBoard::char_changed_handler

	mwdevice->char_changed_handler*/
//	mbl_mw
}