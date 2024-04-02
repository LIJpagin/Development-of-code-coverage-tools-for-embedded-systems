#include "ccamera_base.h"
//#include "../../drivers/cvideo.h"

const CCameraBase::VideoInputConfig CCameraBase::palSecamConfig  =
{
		0x00180018,
		0x01300130,
		0x01190119,
		0x06b006b0,
		0x0000027b,
		0x0006000a
};

bool CCameraBase::IsPilotMenuAvailable() const
{
	return _state == stDisabled;
}

void CCameraBase::WriteIinitSequence()
{
	const int INIT_SEQUENCE_LENGTH = 40;
	struct SEQUENCE_CONFIG
	{
		BYTE address;
		BYTE data;
	};
	SEQUENCE_CONFIG initSequence[INIT_SEQUENCE_LENGTH] =
	{
			{ 0x00, 0x0E },
			{ 0x03, 0x0C },
			{ 0x04, 0x77 },
			{ 0x13, 0x04 },
			{ 0x17, 0x41 },
			{ 0x1d, 0x47 },
			{ 0x37, 0x01 },
			{ 0x3a, 0x17 },
			{ 0x3b, 0x80 },
			{ 0x3d, 0xa2 },
			{ 0x3e, 0x6a },
			{ 0x3f, 0xa0 },
			{ 0x86, 0x0b },
			{ 0xf3, 0x01 },
			{ 0xf9, 0x03 },
			{ 0x0e, 0x80 },
			{ 0x52, 0x46 },
			{ 0x54, 0x00 },
			{ 0x7f, 0xff },
			{ 0x81, 0x30 },
			{ 0x90, 0xc9 },
			{ 0x91, 0x40 },
			{ 0x92, 0x3c },
			{ 0x93, 0xca },
			{ 0x94, 0xd5 },
			{ 0xb1, 0xff },
			{ 0xb6, 0x08 },
			{ 0xc0, 0x9a },
			{ 0xcf, 0x50 },
			{ 0xd0, 0x4e },
			{ 0xd1, 0xb9 },
			{ 0xd6, 0xdd },
			{ 0xd7, 0xe2 },
			{ 0xe5, 0x51 },
			{ 0x32, 0xC1 },
			{ 0x33, 0xC4 },
			{ 0x34, 0x10 },
			{ 0x35, 0x1C },
			{ 0xE9, 0x10,},
			{ 0x00, 0x0E },
	};

	for(int i = 0; i < INIT_SEQUENCE_LENGTH; i++)
		_i2cDriver.Write(initSequence[i].address, &(initSequence[i].data), 1);
	_videoDriver->Wait(10000000);
}

CVideo::STANDARD CCameraBase::GetStandard()
{
	BYTE data = 0;
	_i2cDriver.Read(0x10, &data, 1);
	data &= 0x70;
	if ((data == 0x60) || (data == 0x40) || (data == 0x30) || (data == 0x20))
		return CVideo::pal;
	else if ((data == 0x10) || (data == 0x00))
		return CVideo::ntsc;
	else if (data == 0x50)
		return CVideo::secam;
	else
		return CVideo::pal;//заглушка, попадать сюда не должен.
}

void CCameraBase::ManualConfigSynchro()
{
	_videoDriver->StartManualConfig();
	_videoDriver->SetStartV(_manualConfig.startV);
	_videoDriver->SetEndV(_manualConfig.endV);
	_videoDriver->SetStartH(_manualConfig.startH);
	_videoDriver->SetEndH(_manualConfig.endH);
	_videoDriver->SetQuant(_manualConfig.quant );
	_videoDriver->SetReduce(_manualConfig.reduce);
	_videoDriver->StopManualConfig();
}

void CCameraBase::LoadConfig()
{
	_videoDriver->Reset();
	WriteIinitSequence();
	_videoDriver->SetLayer();
	CVideo::STANDARD standard = GetStandard();
	_videoDriver->SetStandard(standard);
	if( _loadingConfigType == lcManual)
		ManualConfigSynchro();
	_videoDriver->TurnOn();
}

void CCameraBase::RefreshConfig()
{
	_manualConfig.startV = _videoDriver->GetStartV();
	_manualConfig.endV = _videoDriver->GetEndV();
	_manualConfig.startH = _videoDriver->GetStartH();
	_manualConfig.endH = _videoDriver->GetEndH();
	_manualConfig.quant = _videoDriver->GetQuant();
	_manualConfig.reduce = _videoDriver->GetReduce();
}
