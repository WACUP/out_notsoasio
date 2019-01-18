
class
SLinkStatic : public SStatic
{
public:
	SLinkStatic(SWindow* parent, int id);
	~SLinkStatic(void);

private:
	HFONT	hFont;

	void	SetFontHdc(HDC hdc, LPRECT Rect, LPTSTR Text, int* TextLen);

protected:
	void	Paint(HDC hdc);
	void	WmPaint(Org_Mes* OrgMes);
	void	WmSetCursor(Org_Mes* OrgMes, HWND hwnd, UINT nHittest, UINT wMouseMsg);
};

