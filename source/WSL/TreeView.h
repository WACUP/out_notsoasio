
class	STreeView;

class
STreeItem : public TVITEM
{
public:
	STreeItem(void);
	STreeItem(TVITEM item);
	STreeItem(HTREEITEM item);
	STreeItem(LPCTSTR text, int len = 0);
	STreeItem(LPCTSTR text, int index, int selIndex, int len = 0);
	STreeItem(LPCTSTR text, int index, int selIndex, UINT data, int len = 0);

	void	Init(void);
	void	SetMask(UINT newMask);
	void	SetHTreeItem(HTREEITEM hItem);
	HTREEITEM	GetHTreeitem(void) const;
	void	SetState(UINT newState, UINT newStateMask);
	void	SetText(LPCTSTR text, int len = 0);
	void	SetImageIndex(int index);
	void	SetSelectedImageIndex(int index);
	void	SetChildren(int index);
	void	SetItemData(UINT data);
};

class
STreeNode
{
public:
	STreeNode(STreeView& tree, TVITEM item);
	STreeNode(STreeView& tree, HTREEITEM hItem = TVI_ROOT);
	STreeNode(const STreeNode& tree);

	STreeNode&	CopyNode(const STreeNode& node);
	STreeNode&	operator = (const STreeNode& node);

	STreeNode	GetNextItem(UINT flag) const;
	STreeNode	GetParent(void) const;
	STreeNode	GetChild(void) const;
	STreeNode	GetNextSibling(void) const;
	STreeNode	GetPrevSibling(void) const;
	STreeNode	GetNextVisible(void) const;
	STreeNode	GetPrevVisible(void) const;

	STreeNode	InsertChild(const STreeItem&, HTREEITEM how) const;
	STreeNode	InsertItem(const STreeItem&) const;
	STreeNode	AddChild(const STreeItem& item) const;
	STreeNode	AddSibling(const STreeItem& item) const;

	bool	SetItem(void);
	bool	SetItem(STreeItem* item);
	bool	GetItem(STreeItem* item);

	bool	Delete(void);
	HIMAGELIST	CreateDragImage(void);
	bool	EnsureVisible(void);
	bool	ExpandItem(UINT flag);
	bool	SelectItem(UINT flag);
	bool	SortChildren(bool recurse = false);
	bool	GetItemRect(RECT& rect, bool textOnly = true) const;

	void	SetHTreeItem(HTREEITEM hItem);
	HTREEITEM	GetHTreeItem(void) const;
	operator	HTREEITEM(void) const;

	bool	SetState(UINT newState, UINT newStateMask, bool Send = true);
	bool	GetState(UINT& state, UINT mask);
	bool	SetImageIndex(int index, bool Send = true);
	bool	GetImageIndex(int& index);
	bool	SetSelectedImageIndex(int index, bool Send = true);
	bool	SetChildren(int index, bool Send = true);
	bool	SetItemData(UINT data, bool Send = true);
	bool	GetItemData(UINT& data);
	bool	SetText(LPCTSTR text, bool Send = true);
	bool	GetText(LPTSTR text, UINT length);

protected:
	STreeItem	ItemStruct;
	STreeView*	TreeView;
};

class
STreeView : public SControl
{
public:
	STreeView(SWindow* parent, int id);
	~STreeView(void);

	STreeNode	GetRoot(void);
	STreeNode	GetSelection(void);
	STreeNode	GetDropHilite(void);
	STreeNode	GetFirstVisible(void);

	HIMAGELIST	SetImageList(HIMAGELIST newList, UINT type);
	HIMAGELIST	SetImageList(SImageList* newList, UINT type);
	HIMAGELIST	GetImageList(UINT type);

	bool	DeleteAllItems(void);

	bool	SortChildren(HTREEITEM item, bool recurse = false);
	bool	Delete(HTREEITEM hItem);
	bool	EnsureVisible(HTREEITEM hItem);
	bool	ExpandItem(UINT flag, HTREEITEM hItem);
	bool	SelectItem(UINT flag, HTREEITEM hItem);
	HTREEITEM	GetNextItem(UINT flag, HTREEITEM hItem);
	HTREEITEM	InsertItem(TV_INSERTSTRUCT* tvis);
	HTREEITEM	HitTest(POINT& Pnt, UINT flags = TVHT_ONITEM);
	HIMAGELIST	CreateDragImage(HTREEITEM item);
};

