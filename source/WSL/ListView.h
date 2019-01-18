
class
SListWindItem : public LVITEM
{
public:
	SListWindItem(int subitem);
	SListWindItem(LPCTSTR text, int subitem = 0, int len = 0);

	void	SetIndex(int index);
	int		GetIndex(void) const;

	void	SetSubItem(int subItem);
	int		GetSubItem(void) const;

	void	SetText(LPCTSTR text);
	void	SetText(LPCTSTR text, int len);

	void	SetImageIndex(int index);
	int		GetImageIndex(void) const;

	void	SetItemData(UINT data);
	UINT	GetItemData(void) const;

protected:
	void	Init(void);
};

class
SListWindColumn : public LVCOLUMN
{
public:
	SListWindColumn(LPCTSTR txt, int width, UINT how = LVCFMT_LEFT, int subItem = 0);

	void	SetFormat(UINT how);
	void	SetWidth(int pixels);
	void	SetText(LPCTSTR txt, int = 0);
	void	SetSubItem(int subItem);

protected:
	void	Init(void);
};

class
SListView : public SControl
{
public:
	SListView(SWindow* parent, int id);
	~SListView(void);

	void	SetExtendedListViewStyle(UINT Style);
	UINT	GetExtendedListViewStyle(void);

	HWND	GetHeader(void);

	int		InsertColumn(int iCol, const SListWindColumn&);
	bool	GetColumn(int iCol, LV_COLUMN* column);
	bool	SetColumnWidth(int iCol, int cx);
	int		GetColumnWidth(int iCol) const;
	void	SetSelectedColumn(int iCol);

	int		InsertItem(SListWindItem& newItem, int index = -1);

	bool	SetItem(SListWindItem& item, int index = -1, int subitem = -1);
	bool	GetItem(SListWindItem& item, int index = -1, int subitem = -1) const;

	void	SetItemCountEx(int cItems, DWORD dwFlags = 0);
	int		GetItemCount(void) const;
	UINT	GetSelectedCount(void) const;
	bool	GetItemPosition(int index, POINT* p);

	bool	SetItemState(int index, UINT state, UINT mask);
	UINT	GetItemState(int index, UINT mask) const;
	int		GetItemText(int index, int subItem, LPTSTR text, int size) const;

	HIMAGELIST	CreateDragImage(int itemIndex, POINT* upLeft);
	bool	SetImageList(HIMAGELIST list, UINT type);
	bool	SetImageList(SImageList* list, UINT type);

	bool	DeleteAllItems(void);
	int		GetTopIndex(void) const;
	int		HitTest(POINT& Pnt, UINT flags = LVHT_ONITEM);
	bool	EnsureVisible(int index, bool partialOk);
	int		GetCountPerPage(void) const;
	HWND	EditLabel(int index);
	bool	Update(int index);

	int		SetHotItem(int index);
	int		GetHotItem(void);

	HCURSOR	SetHotCursor(HCURSOR hcur);
};

