
class
SDialog : public SWindow
{
public:
	SDialog(SWindow* parent, LPCTSTR id);
	SDialog(SWindow* parent, int id);
	SDialog(HWND parent, LPCTSTR id);
	SDialog(HWND parent, int id);
	SDialog(void);
	~SDialog(void);

	HRESULT	EnableThemeDialogTexture(DWORD dwFlags);
	bool	_PropSheet_Changed(void);
	void	_PropSheet_SetTitle(WPARAM wStyle, LPCTSTR lpszText);
	bool	SetDlgItemText(int nIDDlgItem, LPCTSTR lpString);
	UINT	GetDlgItemText(int nIDDlgItem, LPTSTR lpString, int nMaxCount);
	bool	SetDlgItemInt(int nIDDlgItem, UINT uValue, bool bSigned = false);
	UINT	GetDlgItemInt(int nIDDlgItem, bool* lpTranslated = NULL, bool bSigned = false);

protected:
	virtual void	CmOk(void);
	virtual void	CmCancel(void);

	void	WmCommand(Org_Mes* OrgMes, int wNotifyCode, int wID, HWND hwndCtl);
};

