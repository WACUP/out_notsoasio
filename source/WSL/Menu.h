
class
SMenu
{
public:
	enum
	{
		MenuMode_Normal, MenuMode_Popup,
	};

	SMenu(
		int MenuMode,
		bool ADestroy = false,
		SToolBar* bar1 = NULL,
		SToolBar* bar2 = NULL,
		SToolBar* bar3 = NULL,
		SToolBar* bar4 = NULL);
	SMenu(
		HMENU menu,
		bool ADestroy = false,
		SToolBar* bar1 = NULL,
		SToolBar* bar2 = NULL,
		SToolBar* bar3 = NULL,
		SToolBar* bar4 = NULL);
	~SMenu(void);

	HMENU	GetHandle(void);
	void	AssignToolBar1(SToolBar* bar);
	void	AssignToolBar2(SToolBar* bar);
	void	AssignToolBar3(SToolBar* bar);
	void	AssignToolBar4(SToolBar* bar);
	bool	AppendMenu(UINT fuFlags, UINT idNewItem = 0, LPCTSTR lpszNewItem = NULL);
	bool	AppendMenu(UINT fuFlags, SMenu* Menu, LPCTSTR lpszNewItem);
	DWORD	CheckMenuItem(UINT idCheckItem, UINT fuFlags);
	DWORD	Check(UINT idCheckItem, bool Check, UINT fuFlags = MF_BYCOMMAND);
	bool	EnableMenuItem(UINT uItem, UINT fuFlags);
	bool	DeleteMenu(UINT uItem, UINT fuFlags = MF_BYCOMMAND);
	bool	SetMenuItemBitmaps(
							UINT uItem,
							UINT fuFlags = MF_BYCOMMAND,
							HBITMAP hbmUnChecked = NULL,
							HBITMAP hbmChecked = NULL);
	BOOL	TrackPopupMenu(
						UINT uFlags,
						int x,
						int y,
						int nReserved,
						HWND hWnd,
						CONST RECT* prcRect = NULL);

private:
	HMENU	hMenu;
	SToolBar*	Toolbar1;
	SToolBar*	Toolbar2;
	SToolBar*	Toolbar3;
	SToolBar*	Toolbar4;
	bool	AutoDestroy;

	void	ToolBarCheckMenuItem(SToolBar* Toolbar, UINT idCheckItem, UINT fuFlags);
	void	ToolBarEnableMenuItem(SToolBar* Toolbar, UINT uItem, UINT fuFlags);
};

