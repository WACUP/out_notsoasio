
class
SHeader : public SControl
{
public:
	SHeader(SWindow* parent, int id);
	SHeader(SWindow* parent, HWND hwnd);
	~SHeader(void);

	int		InsertItem(int index, LPHDITEM item);
	bool	DeleteItem(int index);
	bool	SetItem(int index, LPHDITEM item);
	HIMAGELIST	SetImageList(HIMAGELIST himl);
};

