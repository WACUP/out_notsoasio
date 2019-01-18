
class
SImageList
{
public:
	SImageList(int cx, int cy, UINT flags, int cInitial, int cGrow);
	SImageList(HINSTANCE hi, LPCTSTR lpbmp, int cx, int cGrow, COLORREF crMask);
	SImageList(
			HINSTANCE hi,
			LPCTSTR lpbmp,
			int cx,
			int cGrow,
			COLORREF crMask,
			UINT uType,
			UINT uFlags);
	SImageList(HIMAGELIST hImage);
	~SImageList(void);

	HIMAGELIST	GetHandle(void);
	int		GetImageCount(void);
	int		Add(HICON hIcon);
	int		AddMasked(HBITMAP hbmImage, COLORREF crMask);
	bool	Remove(int i);
	bool	Draw(int i, HDC hdcDst, int x, int y, UINT fStyle);
	bool	BeginDrag(int iTrack, int dxHotspot, int dyHotspot);
	void	EndDrag(void);
	bool	DragEnter(HWND hwndLock, int x, int y);
	bool	DragLeave(HWND hwndLock);
	bool	DragMove(int x, int y);
	bool	DragShowNolock(bool fShow);
	bool	SetOverlayImage(int iImage, int iOverlay);
	COLORREF	SetBkColor(COLORREF clrBk);

protected:
	HIMAGELIST	Handle;
};

