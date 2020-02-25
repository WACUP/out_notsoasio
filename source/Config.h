
#include "WSL/WSL.h"
#include "WSL/Window.h"
//#include "WSL/Dialog.h"
#include "WSL/Control.h"
#include "WSL/Button.h"
#include "WSL/Static.h"
#include "WSL/Edit.h"
#include "WSL/UpDown.h"
#include "WSL/ComboBox.h"

#include "resource.h"

class
DialogOption : public SWindow
{
public:
	DialogOption(const HWND hParentWnd);
	~DialogOption(void);

private:
	SComboBox*	Device;
	SComboBox*	ThreadPriority;
	SUpDown*	BufferSizeUpDown;
	SUpDown*	ShiftChannelsUpDown;
	SButton*	GaplessMode;
	SButton*	Convert1chTo2ch;
	SButton*	DirectInputMonitor;
	SButton*	Volume_Control;
	SButton*	Resampling_Enable;
	SComboBox*	Resampling_ThreadPriority;
	SComboBox*	Resampling_SampleRate;
	SComboBox*	Resampling_Quality;

protected:
	bool	WmInitDialog(Org_Mes* OrgMes, HWND hwnd, LONG lInitParam);
};
