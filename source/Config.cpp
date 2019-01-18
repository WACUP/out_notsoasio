
#define	STRICT

#include <windows.h>

#include "out_asio.h"
#include "Config.h"
#include "pcmasio.h"

extern AsioDrivers*	asioDrivers;

extern PARAM_GLOBAL	ParamGlobal;

extern PcmAsio*	pPcmAsio;

const char*	List_ThreadPriority[] =
{"Normal", "Above normal", "Highest", "Time critical"};

const int	List_Resampling_SampleRate[] =
{11025, 22050, 24000, 32000, 44100, 48000, 88200, 96000, 176400, 192000};

const char*	List_Resampling_Quality[] =
{"Low", "Normal", "High", "Top", "Ultra"};

DialogOption::DialogOption(const HWND hParentWnd) : SDialog(hParentWnd, IDD_CONFIG)
{
}

DialogOption::~DialogOption(void)
{
	delete Device;
	delete ThreadPriority;
	delete BufferSizeUpDown;
	delete ShiftChannelsUpDown;
	delete GaplessMode;
	delete Convert1chTo2ch;
	delete DirectInputMonitor;
	delete Volume_Control;
	delete Resampling_Enable;
	delete Resampling_ThreadPriority;
	delete Resampling_SampleRate;
	delete Resampling_Quality;
}

bool
DialogOption::WmInitDialog(Org_Mes* OrgMes, HWND hwnd, LONG lInitParam)
{
	OrgMes->ExecMessage = true;
	SDialog::WmInitDialog(OrgMes, hwnd, lInitParam);

	HWND	hParentParentWindow = ::GetParent(HParentWindow);

	MoveWindowCenter(hParentParentWindow ? hParentParentWindow : HParentWindow);

	Device = new SComboBox(this, IDC_DEVICE);
	ThreadPriority = new SComboBox(this, IDC_THREAD_PRIORITY);
	BufferSizeUpDown = new SUpDown(this, IDC_BUFFER_SIZE_UPDOWN);
	ShiftChannelsUpDown = new SUpDown(this, IDC_SHIFT_CHANNELS_UPDOWN);
	GaplessMode = new SButton(this, IDC_GAPLESS_MODE, BS_CHECKBOX);
	Convert1chTo2ch = new SButton(this, IDC_CONVERT_1CH_TO_2CH, BS_CHECKBOX);
	DirectInputMonitor = new SButton(this, IDC_DIRECT_INPUT_MONITOR, BS_CHECKBOX);
	Volume_Control = new SButton(this, IDC_VOLUME_CONTROL, BS_CHECKBOX);
	Resampling_Enable = new SButton(this, IDC_RESAMPLING_ENABLE);
	Resampling_ThreadPriority = new SComboBox(this, IDC_RESAMPLING_THREAD_PRIORITY);
	Resampling_SampleRate = new SComboBox(this, IDC_RESAMPLING_SAMPLE_RATE);
	Resampling_Quality = new SComboBox(this, IDC_RESAMPLING_QUALITY);

	const int	MaxDriver = asioDrivers->asioGetNumDev();

	for(int Idx = 0; Idx < MaxDriver; Idx++) {
		const int	DriverNameLen = 64;
		char	DriverName[DriverNameLen];

		asioDrivers->asioGetDriverName(Idx, DriverName, DriverNameLen);
		Device->AddString(DriverName);
	}

	Device->SetCurSel(ParamGlobal.Device);

	for(int Idx = 0; Idx < 4; Idx++) {
		ThreadPriority->AddString(List_ThreadPriority[Idx]);
	}

	ThreadPriority->SetCurSel(ParamGlobal.ThreadPriority);

	BufferSizeUpDown->SetRange(0, 63);
	BufferSizeUpDown->SetPos(ParamGlobal.BufferSize);

	ShiftChannelsUpDown->SetRange(0, 99);
	ShiftChannelsUpDown->SetPos(ParamGlobal.ShiftChannels);

	if(ParamGlobal.GaplessMode) {
		GaplessMode->Check();
	}

	if(ParamGlobal.Convert1chTo2ch) {
		Convert1chTo2ch->Check();
	}

	if(ParamGlobal.DirectInputMonitor) {
		DirectInputMonitor->Check();
	}

	if(ParamGlobal.Volume_Control) {
		Volume_Control->Check();
	}

	if(ParamGlobal.Resampling_Enable) {
		Resampling_Enable->Check();
	}

	for(int Idx = 0; Idx < 4; Idx++) {
		Resampling_ThreadPriority->AddString(List_ThreadPriority[Idx]);
	}

	Resampling_ThreadPriority->SetCurSel(ParamGlobal.Resampling_ThreadPriority);

	for(int Idx = 0; Idx < 10; Idx++) {
		char	StrSampleRate[8];

		_itoa_s(List_Resampling_SampleRate[Idx], StrSampleRate, sizeof StrSampleRate, 10);
		Resampling_SampleRate->AddString(StrSampleRate);

		if(List_Resampling_SampleRate[Idx] == ParamGlobal.Resampling_SampleRate) {
			Resampling_SampleRate->SetCurSel(Idx);
		}
	}

	for(int Idx = 0; Idx < 5; Idx++) {
		Resampling_Quality->AddString(List_Resampling_Quality[Idx]);
	}

	Resampling_Quality->SetCurSel(ParamGlobal.Resampling_Quality);

	return true;
}

void
DialogOption::CmOk(void)
{
	bool	New = false;
	bool	bParam;
	int		iParam;

	iParam = Device->GetCurSel();
	if(ParamGlobal.Device != iParam) {
		ParamGlobal.Device = iParam;
		New = true;
	}

	iParam = ThreadPriority->GetCurSel();
	if(ParamGlobal.ThreadPriority != iParam) {
		ParamGlobal.ThreadPriority = iParam;
		New = true;
	}

	iParam = BufferSizeUpDown->GetPos();
	if(ParamGlobal.BufferSize != iParam) {
		ParamGlobal.BufferSize = iParam;
		New = true;
	}

	iParam = ShiftChannelsUpDown->GetPos();
	if(ParamGlobal.ShiftChannels != iParam) {
		ParamGlobal.ShiftChannels = iParam;
		New = true;
	}

	bParam = GaplessMode->GetCheck() == BF_CHECKED;
	if(ParamGlobal.GaplessMode != bParam) {
		ParamGlobal.GaplessMode = bParam;
		New = true;
	}

	bParam = Convert1chTo2ch->GetCheck() == BF_CHECKED;
	if(ParamGlobal.Convert1chTo2ch != bParam) {
		ParamGlobal.Convert1chTo2ch = bParam;
		New = true;
	}

	bParam = DirectInputMonitor->GetCheck() == BF_CHECKED;
	if(ParamGlobal.DirectInputMonitor != bParam) {
		ParamGlobal.DirectInputMonitor = bParam;
		New = true;
	}

	bParam = Volume_Control->GetCheck() == BF_CHECKED;
	if(ParamGlobal.Volume_Control != bParam) {
		ParamGlobal.Volume_Control = bParam;
		New = true;
	}

	bParam = Resampling_Enable->GetCheck() == BF_CHECKED;
	if(ParamGlobal.Resampling_Enable != bParam) {
		ParamGlobal.Resampling_Enable = bParam;
		New = true;
	}

	iParam = Resampling_ThreadPriority->GetCurSel();
	if(ParamGlobal.Resampling_ThreadPriority != iParam) {
		ParamGlobal.Resampling_ThreadPriority = iParam;
		New = true;
	}

	iParam = List_Resampling_SampleRate[Resampling_SampleRate->GetCurSel()];
	if(ParamGlobal.Resampling_SampleRate != iParam) {
		ParamGlobal.Resampling_SampleRate = iParam;
		New = true;
	}

	iParam = Resampling_Quality->GetCurSel();
	if(ParamGlobal.Resampling_Quality != iParam) {
		ParamGlobal.Resampling_Quality = iParam;
		New = true;
	}

	if(New) {
		// only save if needed
		WriteProfile();
		pPcmAsio->SetReOpen();
	}

	SDialog::CmOk();
}

