#pragma once
#include "Profile.h"
#include <iomanip>
#include <iostream>
#include <unordered_map>

#include "metawear/core/metawearboard.h"
#include "metawear/platform/btle_connection.h"
#include "metawear\core\status.h"

class MWDevice
{
public:
	MWDevice();
	~MWDevice();

	bool Initialize(Profile * a_profile, Profile* a_profileDevInfo);
	void Destroy();

	Characteristic* GetCharacteristicFromGUID(GUID a_guid);

	MblMwMetaWearBoard* GetMetaWearBoard();

	Profile* m_profile;
	Profile* m_profileDevInfo;

	static void read_gatt_char_v2(void* context, const void* caller, const MblMwGattChar* characteristic,
		MblMwFnIntVoidPtrArray handler);

	static void write_gatt_char_v2(void* context, const void* caller, MblMwGattCharWriteType write_type,
		const MblMwGattChar* characteristic, const uint8_t* value, uint8_t length);


	static std::unordered_map<const void*, MblMwFnIntVoidPtrArray> notify_handlers;

	static void enable_char_notify(void* context, const void* caller, const MblMwGattChar* characteristic,
		MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready);

	static void on_disconnect(void* context, const void* caller, MblMwFnVoidVoidPtrInt handler);

	GUID HighLow2Uuid(const uint64_t high, const uint64_t low);
	void PrintAllCharacteristics();

	static void NotifyCharChange(size_t changedCharValueDataSize, void * bufferCharValue, void * Context, Characteristic * characteristicCalledUpon);

	static std::unordered_map<const void*, MblMwFnVoidVoidPtrInt> dc_handlers;

	MblMwBtleConnection m_btle_conn;
	MblMwMetaWearBoard* m_mwBoard;

};



