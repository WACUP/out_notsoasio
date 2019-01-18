
class
SControl : public SWindow
{
public:
	SControl(SWindow* parent, LPCTSTR classname, int id);
	SControl(SWindow* parent, HWND hwnd);
	SControl(void);
	~SControl(void);
};

