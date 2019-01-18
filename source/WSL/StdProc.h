
void	srandMT(unsigned long seed);
unsigned long	randMT(void);

// 大きい方を返す

template<class T> inline T
Max(T a, T b)
{
	return (a > b) ? a : b;
}

// 小さい方を返す

template<class T> inline T
Min(T a, T b)
{
	return (a < b) ? a : b;
}

// InBuffからOutBuffにコピー(MultiByte -> WideChar)

inline int
Strcpy(WCHAR* OutBuff, const int OutBuffSize, const char* InBuff, const int InBuffSize = -1)
{
	const int	RetCode = ::MultiByteToWideChar(CP_ACP, 0, InBuff, InBuffSize, OutBuff, OutBuffSize);

	if(OutBuffSize) *(OutBuff + Min(RetCode, OutBuffSize - 1)) = L'\0';

	return RetCode;
}

// InBuffからOutBuffにコピー(UTF-8 -> WideChar)

inline int
UTF8Strcpy(WCHAR* OutBuff, const int OutBuffSize, const char* InBuff, const int InBuffSize = -1)
{
	const int	RetCode = ::MultiByteToWideChar(CP_UTF8, 0, InBuff, InBuffSize, OutBuff, OutBuffSize);

	if(OutBuffSize) *(OutBuff + Min(RetCode, OutBuffSize - 1)) = L'\0';

	return RetCode;
}

// InBuffからOutBuffにコピー(WideChar -> MultiByte)

inline int
Strcpy(char* OutBuff, const int OutBuffSize, const WCHAR* InBuff, const int InBuffSize = -1)
{
	const int	RetCode = ::WideCharToMultiByte(
											CP_ACP,
											0,
											InBuff,
											InBuffSize,
											OutBuff,
											OutBuffSize,
											NULL,
											NULL);

	if(OutBuffSize) *(OutBuff + Min(RetCode, OutBuffSize - 1)) = '\0';

	return RetCode;
}

// InBuffからOutBuffにコピー(WideChar -> MultiByte(UTF8))

inline int
UTF8Strcpy(char* OutBuff, const int OutBuffSize, const WCHAR* InBuff, const int InBuffSize = -1)
{
	const int	RetCode = ::WideCharToMultiByte(
											CP_UTF8,
											0,
											InBuff,
											InBuffSize,
											OutBuff,
											OutBuffSize,
											NULL,
											NULL);

	if(OutBuffSize) *(OutBuff + Min(RetCode, OutBuffSize - 1)) = '\0';

	return RetCode;
}

// string2からcount分、string1にコピー

#ifdef UNICODE

inline WCHAR*
Strncpy(WCHAR* string1, const WCHAR* string2, const size_t count)
{
	const size_t	Len = Min(wcslen(string2), count);

	memcpy(string1, string2, Len * sizeof WCHAR);
	*(string1 + Len) = L'\0';

	return string1;
}

#else

inline char*
Strncpy(char* string1, const char* string2, const size_t count)
{
	const size_t	Len = Min(strlen(string2), count);

	memcpy(string1, string2, Len);
	*(string1 + Len) = '\0';

	return string1;
}

#endif

// srcから3bytes分、destにコピー

inline void
Memcpy3bytes(unsigned char* dest, unsigned char* src)
{
	*reinterpret_cast<unsigned short*>(dest) = *reinterpret_cast<unsigned short*>(src);
	*(dest + 2) = *(src + 2);
}

// SwapAとSwapBを交換

template<class T> inline void
Swap(T* SwapA, T* SwapB)
{
	T	Work = *SwapA;

	*SwapA = *SwapB;
	*SwapB = Work;
}

// SwapAとSwapBを交換(24bit)

inline void
Swap24(unsigned char* SwapA, unsigned char* SwapB)
{
	unsigned char	Work[3];

	Memcpy3bytes(Work, SwapA);
	Memcpy3bytes(SwapA, SwapB);
	Memcpy3bytes(SwapB, Work);
}

// Int24をInt32に変換

inline int
Int24ToInt32(unsigned char* Num)
{
	return *Num | (*(Num + 1) << 8) | (*reinterpret_cast<char*>(Num + 2) << 16);
}

// エンディアンを変更(16bit)

inline unsigned short
BSwap16(unsigned short Num)
{
	Swap(reinterpret_cast<unsigned char*>(&Num), reinterpret_cast<unsigned char*>(&Num) + 1);

	return Num;
}

// エンディアンを変更(24bit)

inline void
BSwap24(unsigned char* Num)
{
	Swap(Num, Num + 2);
}

// エンディアンを変更(32bit)

inline unsigned int
BSwap32(unsigned int Num)
{
	union
	{
		unsigned char	Work[4];
		unsigned int	Work32;
	};

	*(Work + 0) = *(reinterpret_cast<unsigned char*>(&Num) + 3);
	*(Work + 1) = *(reinterpret_cast<unsigned char*>(&Num) + 2);
	*(Work + 2) = *(reinterpret_cast<unsigned char*>(&Num) + 1);
	*(Work + 3) = *(reinterpret_cast<unsigned char*>(&Num) + 0);

	return Work32;
}

// エンディアンを変更(64bit)

inline unsigned __int64
BSwap64(unsigned __int64 Num)
{
	union
	{
		unsigned char	Work[8];
		unsigned __int64	Work64;
	};

	*(Work + 0) = *(reinterpret_cast<unsigned char*>(&Num) + 7);
	*(Work + 1) = *(reinterpret_cast<unsigned char*>(&Num) + 6);
	*(Work + 2) = *(reinterpret_cast<unsigned char*>(&Num) + 5);
	*(Work + 3) = *(reinterpret_cast<unsigned char*>(&Num) + 4);
	*(Work + 4) = *(reinterpret_cast<unsigned char*>(&Num) + 3);
	*(Work + 5) = *(reinterpret_cast<unsigned char*>(&Num) + 2);
	*(Work + 6) = *(reinterpret_cast<unsigned char*>(&Num) + 1);
	*(Work + 7) = *(reinterpret_cast<unsigned char*>(&Num) + 0);

	return Work64;
}

// 四捨五入して整数に変換(32bit)

inline int
RInt32(double Num)
{
	union
	{
		char	Work[8];
		double	Work64;
	};

	Work64 = Num;

	const int	Sign = (Work[7] >> 7) | 1;

	Work[7] &= 0x7f;

	int		IntNum = static_cast<int>(Work64);

	if((Work64 - IntNum) == .5) {
		if(IntNum & 1) IntNum++;
	} else {
		IntNum = static_cast<int>(Work64 + .5);
	}

	return Sign * IntNum;
}

// 四捨五入して整数に変換(64bit)

inline __int64
RInt64(double Num)
{
	union
	{
		char	Work[8];
		double	Work64;
	};

	Work64 = Num;

	const __int64	Sign = (Work[7] >> 7) | 1;

	Work[7] &= 0x7f;

	__int64	IntNum = static_cast<__int64>(Work64);

	if((Work64 - IntNum) == .5) {
		if(IntNum & 1) IntNum++;
	} else {
		IntNum = static_cast<__int64>(Work64 + .5);
	}

	return Sign * IntNum;
}

// 乱数の系列をリセット

inline void
RandomSetSeed(unsigned long seed)
{
//	srand(seed);
	srandMT(seed);
}

// 0 ～ (num - 1) の乱数を発生

inline int
Random(int num)
{
//	return static_cast<int>((static_cast<__int64>(rand()) * num) / (RAND_MAX + 1));
	return static_cast<int>((static_cast<unsigned __int64>(randMT()) * num) /
														(0xffffffffUi64 + 1Ui64));
}

