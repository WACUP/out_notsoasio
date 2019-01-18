
class
SToolBar : public SControl
{
public:
	SToolBar(SWindow* parent, int id);
	~SToolBar(void);

	void	ButtonStructSize(UINT cb);
	int		AddBitmap(UINT nButtons, TBADDBITMAP* lptbab);
	int		AddString(HINSTANCE hInst, LPCTSTR idString);
	bool	AddButtons(UINT uNumButtons, LPTBBUTTON lpButtons);
	void	SetImageList(HIMAGELIST himl);
	void	SetImageList(SImageList* himl);
	bool	SetBitmapSize(int dxBitmap, int dyBitmap);
	bool	SetButtonSize(int dxButton, int dyButton);
	bool	SetState(int idButton, UINT fState);
	int		GetState(int idButton);
	void	SetExtendedStyle(int idButton, UINT fStyle);
	HWND	GetToolTips(void);
	int		AddString(LPCTSTR String);
	void	GetItemRect(int idButton, RECT* rect);
	int		CommandToIndex(int Command);
};

