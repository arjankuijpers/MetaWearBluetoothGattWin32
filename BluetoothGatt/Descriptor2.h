#pragma once
class Descriptor
{
public:
	Descriptor();
	~Descriptor();

	bool Initialize(PBTH_LE_GATT_DESCRIPTOR descriptor, HANDLE a_handleDevice);
	void Destroy();

	PBTH_LE_GATT_DESCRIPTOR m_descriptor;
	HANDLE m_handleDevice;

	void GetValue();
	void SetValue();
};

