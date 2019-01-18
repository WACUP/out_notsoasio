
class
SStatusBar : public SControl
{
public:
	SStatusBar(SWindow* parent, int id);
	~SStatusBar(void);

	void	MenuHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, HMENU hMainMenu, HINSTANCE hInst, UINT* lpwIDs);
	bool	SetParts(int nParts, LPINT aWidths);
	bool	SetText(int iPart, UINT uType, LPCTSTR szText);
};

