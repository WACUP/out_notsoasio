
#define	STRICT

#include <windows.h>
#include <commctrl.h>

#include <memory.h>

#include "WSL.h"
#include "ImageLst.h"
#include "Window.h"
#include "Control.h"
#include "TreeView.h"

// STreeItem

STreeItem::STreeItem(void)
{
	Init();
}

STreeItem::STreeItem(TVITEM item)
{
	Init();
	*static_cast<TVITEM*>(this) = item;
}

STreeItem::STreeItem(HTREEITEM item)
{
	Init();
	SetHTreeItem(item);
}

STreeItem::STreeItem(LPCTSTR text, int len)
{
	Init();
	SetText(text, len);
}

STreeItem::STreeItem(LPCTSTR text, int index, int selIndex, int len)
{
	Init();
	SetText(text, len);
	SetImageIndex(index);
	SetSelectedImageIndex(selIndex);
}

STreeItem::STreeItem(LPCTSTR text, int index, int selIndex, UINT data, int len)
{
	Init();
	SetText(text, len);
	SetImageIndex(index);
	SetSelectedImageIndex(selIndex);
	SetItemData(data);
}

void
STreeItem::Init(void)
{
	memset(static_cast<TVITEM*>(this), 0, sizeof TVITEM);
}

void
STreeItem::SetMask(UINT newMask)
{
	mask = (mask & TVIF_HANDLE) | newMask;
}

void
STreeItem::SetHTreeItem(HTREEITEM item)
{
	mask |= TVIF_HANDLE;
	hItem = item;
}

HTREEITEM
STreeItem::GetHTreeitem(void) const
{
	return (mask & TVIF_HANDLE) != 0 ? hItem : NULL;
}

void
STreeItem::SetState(UINT newState, UINT newStateMask)
{
	mask |= TVIF_STATE;
	state = newState;
	stateMask = newStateMask;
}

void
STreeItem::SetText(LPCTSTR text, int len)
{
	mask |= TVIF_TEXT;
	pszText = const_cast<LPTSTR>(text);
	cchTextMax = len;
}

void
STreeItem::SetImageIndex(int index)
{
	mask |= TVIF_IMAGE;
	iImage = index;
}

void
STreeItem::SetSelectedImageIndex(int index)
{
	mask |= TVIF_SELECTEDIMAGE;
	iSelectedImage = index;
}

void
STreeItem::SetChildren(int index)
{
	mask |= TVIF_CHILDREN;
	cChildren = index;
}

void
STreeItem::SetItemData(UINT data)
{
	mask |= TVIF_PARAM;
	lParam = data;
}

// STreeNode

STreeNode::STreeNode(STreeView& tree, TVITEM item) : ItemStruct(item), TreeView(&tree)
{
}

STreeNode::STreeNode(STreeView& tree, HTREEITEM hItem) : ItemStruct(hItem), TreeView(&tree)
{
}

STreeNode::STreeNode(const STreeNode& tree)
{
	CopyNode(tree);
}

STreeNode&
STreeNode::CopyNode(const STreeNode& node)
{
	TreeView = node.TreeView;
	ItemStruct = node.ItemStruct;

	return *this;
}

STreeNode&
STreeNode::operator = (const STreeNode& node)
{
	CopyNode(node);

	return *this;
}

STreeNode
STreeNode::GetNextItem(UINT flag) const
{
	return STreeNode(*TreeView, TreeView->GetNextItem(flag, *this));
}

STreeNode
STreeNode::GetParent(void) const
{
	return GetNextItem(TVGN_PARENT);
}

STreeNode
STreeNode::GetChild(void) const
{
	return GetNextItem(TVGN_CHILD);
}

STreeNode
STreeNode::GetNextSibling(void) const
{
	return GetNextItem(TVGN_NEXT);
}

STreeNode
STreeNode::GetPrevSibling(void) const
{
	return GetNextItem(TVGN_PREVIOUS);
}

STreeNode
STreeNode::GetNextVisible(void) const
{
	return GetNextItem(TVGN_NEXTVISIBLE);
}

STreeNode
STreeNode::GetPrevVisible() const
{
	return GetNextItem(TVGN_PREVIOUSVISIBLE);
}

STreeNode
STreeNode::InsertChild(const STreeItem& item, HTREEITEM how) const
{
	TV_INSERTSTRUCT	tvis;

	tvis.hParent = *this;
	tvis.hInsertAfter = how;
	tvis.item = item;

	return STreeNode(*TreeView, TreeView->InsertItem(&tvis));
}

STreeNode
STreeNode::InsertItem(const STreeItem& item) const
{
	TV_INSERTSTRUCT	tvis;

	tvis.hParent = GetParent();
	tvis.hInsertAfter = *this;
	tvis.item = item;

	return STreeNode(*TreeView, TreeView->InsertItem(&tvis));
}

STreeNode
STreeNode::AddChild(const STreeItem& item) const
{
	return InsertChild(item, TVI_LAST);
}

STreeNode
STreeNode::AddSibling(const STreeItem& item) const
{
	return InsertItem(item);
}

bool
STreeNode::SetItem(void)
{
	const bool	RetCode = TreeView->SendMessage(
											TVM_SETITEM,
											0,
											reinterpret_cast<LPARAM>(&ItemStruct)) != 0;

	ItemStruct.mask = TVIF_HANDLE;

	return RetCode;
}

bool
STreeNode::SetItem(STreeItem* item)
{
	item->SetHTreeItem(*this);

	return TreeView->SendMessage(TVM_SETITEM, 0, reinterpret_cast<LPARAM>(item)) != 0;
}

bool
STreeNode::GetItem(STreeItem* item)
{
	item->SetHTreeItem(*this);

	return TreeView->SendMessage(TVM_GETITEM, 0, reinterpret_cast<LPARAM>(item)) != 0;
}

bool
STreeNode::Delete(void)
{
	bool	RetCode;

	if(TreeView->Delete(*this)) {
		ItemStruct.mask  = 0;
		ItemStruct.hItem = 0;
		RetCode = true;
	} else {
		RetCode = false;
	}

	return RetCode;
}

HIMAGELIST
STreeNode::CreateDragImage(void)
{
	return TreeView->CreateDragImage(*this);
}

bool
STreeNode::EnsureVisible(void)
{
	return TreeView->EnsureVisible(*this);
}

bool
STreeNode::ExpandItem(UINT flag)
{
	return TreeView->ExpandItem(flag, *this);
}

bool
STreeNode::SelectItem(UINT flag)
{
	return TreeView->SelectItem(flag, *this);
}

bool
STreeNode::SortChildren(bool recurse)
{
	return TreeView->SortChildren(*this, recurse);
}

bool
STreeNode::GetItemRect(RECT& rect, bool textOnly) const
{
	return TreeView_GetItemRect(TreeView->HWindow, ItemStruct.hItem, &rect, textOnly) != 0;
}

void
STreeNode::SetHTreeItem(HTREEITEM hItem)
{
	ItemStruct.SetHTreeItem(hItem);
}

HTREEITEM
STreeNode::GetHTreeItem(void) const
{
	return ItemStruct.GetHTreeitem();
}

STreeNode::operator HTREEITEM(void) const
{
	return ItemStruct.GetHTreeitem();
}

bool
STreeNode::SetState(UINT newState, UINT newStateMask, bool Send)
{
	ItemStruct.SetState(newState, newStateMask);

	return Send ? SetItem() : true;
}

bool
STreeNode::GetState(UINT& state, UINT mask)
{
	STreeItem	Item;

	Item.mask = TVIF_STATE;
	Item.stateMask = mask;

	const bool	RetCode = GetItem(&Item);

	state = Item.state;

	return RetCode;
}

bool
STreeNode::SetImageIndex(int index, bool Send)
{
	ItemStruct.SetImageIndex(index);

	return Send ? SetItem() : true;
}

bool
STreeNode::GetImageIndex(int& index)
{
	STreeItem	Item;

	Item.mask = TVIF_IMAGE;

	const bool	RetCode = GetItem(&Item);

	index = Item.iImage;

	return RetCode;
}

bool
STreeNode::SetSelectedImageIndex(int index, bool Send)
{
	ItemStruct.SetSelectedImageIndex(index);

	return Send ? SetItem() : true;
}

bool
STreeNode::SetChildren(int index, bool Send)
{
	ItemStruct.SetChildren(index);

	return Send ? SetItem() : true;
}

bool
STreeNode::SetItemData(UINT data, bool Send)
{
	ItemStruct.SetItemData(data);

	return Send ? SetItem() : true;
}

bool
STreeNode::GetItemData(UINT& data)
{
	STreeItem	Item;

	Item.mask = TVIF_PARAM;

	const bool	RetCode = GetItem(&Item);

	data = static_cast<UINT>(Item.lParam);

	return RetCode;
}

bool
STreeNode::SetText(LPCTSTR text, bool Send)
{
	ItemStruct.SetText(text);

	return Send ? SetItem() : true;
}

bool
STreeNode::GetText(LPTSTR text, UINT length)
{
	STreeItem	Item;

	Item.mask = TVIF_TEXT;
	Item.pszText = text;
	Item.cchTextMax = length;

	return GetItem(&Item);
}

// STreeView

STreeView::STreeView(SWindow* parent, int id) : SControl(parent, WC_TREEVIEW, id)
{
	Attr.Style |= WS_CLIPCHILDREN;
}

STreeView::~STreeView(void)
{
}

STreeNode
STreeView::GetRoot(void)
{
	return STreeNode(*this);
}

STreeNode
STreeView::GetSelection(void)
{
	return GetRoot().GetNextItem(TVGN_CARET);
}

STreeNode
STreeView::GetDropHilite(void)
{
	return GetRoot().GetNextItem(TVGN_DROPHILITE);
}

STreeNode
STreeView::GetFirstVisible(void)
{
	return GetRoot().GetNextItem(TVGN_FIRSTVISIBLE);
}

HIMAGELIST
STreeView::SetImageList(HIMAGELIST newList, UINT type)
{
	return reinterpret_cast<HIMAGELIST>(SendMessage(
												TVM_SETIMAGELIST,
												type,
												reinterpret_cast<LPARAM>(newList)));
}

HIMAGELIST
STreeView::SetImageList(SImageList* newList, UINT type)
{
	return SetImageList(newList->GetHandle(), type);
}

HIMAGELIST
STreeView::GetImageList(UINT type)
{
	return reinterpret_cast<HIMAGELIST>(SendMessage(TVM_GETIMAGELIST, type));
}

bool
STreeView::DeleteAllItems(void)
{
	return Delete(TVI_ROOT);
}

bool
STreeView::SortChildren(HTREEITEM item, bool recurse)
{
	return SendMessage(TVM_SORTCHILDREN, recurse, reinterpret_cast<LPARAM>(item)) != 0;
}

bool
STreeView::Delete(HTREEITEM hItem)
{
	return SendMessage(TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(hItem)) != 0;
}

bool
STreeView::EnsureVisible(HTREEITEM hItem)
{
	return SendMessage(TVM_ENSUREVISIBLE, 0, reinterpret_cast<LPARAM>(hItem)) != 0;
}

bool
STreeView::ExpandItem(UINT flag, HTREEITEM hItem)
{
	return SendMessage(TVM_EXPAND, flag, reinterpret_cast<LPARAM>(hItem)) != 0;
}

bool
STreeView::SelectItem(UINT flag, HTREEITEM hItem)
{
	return SendMessage(TVM_SELECTITEM, flag, reinterpret_cast<LPARAM>(hItem)) != 0;
}

HTREEITEM
STreeView::GetNextItem(UINT flag, HTREEITEM hItem)
{
	if(hItem == TVI_ROOT) hItem = TVGN_ROOT;

	return reinterpret_cast<HTREEITEM>(SendMessage(
												TVM_GETNEXTITEM,
												flag,
												reinterpret_cast<LPARAM>(hItem)));
}

HTREEITEM
STreeView::InsertItem(TV_INSERTSTRUCT* tvis)
{
	return reinterpret_cast<HTREEITEM>(SendMessage(TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(tvis)));
}

HTREEITEM
STreeView::HitTest(POINT& Pnt, UINT flags)
{
	TV_HITTESTINFO	tvhst;

	tvhst.pt = Pnt;
	tvhst.flags = flags;

	return reinterpret_cast<HTREEITEM>(SendMessage(TVM_HITTEST, 0, reinterpret_cast<LPARAM>(&tvhst)));
}

HIMAGELIST
STreeView::CreateDragImage(HTREEITEM item)
{
	return reinterpret_cast<HIMAGELIST>(SendMessage(
												TVM_CREATEDRAGIMAGE,
												0,
												reinterpret_cast<LPARAM>(item)));
}

