
typedef	void	(__fastcall *TO_BUFF_FUNC)(const int Idx, unsigned char* ReadPnt);

inline void __fastcall	ToBuff_Float_Int16LSB(const double Data, unsigned char* WritePnt);
void __fastcall	ToBuff_Int8_Int16LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int16_Int16LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int24_Int16LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int32_Int16LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float32_Int16LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float64_Int16LSB(const int Idx, unsigned char* ReadPnt);

inline void __fastcall	ToBuff_Float_Int24LSB(const double Data, unsigned char* WritePnt);
void __fastcall	ToBuff_Int8_Int24LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int16_Int24LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int24_Int24LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int32_Int24LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float32_Int24LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float64_Int24LSB(const int Idx, unsigned char* ReadPnt);

inline void __fastcall	ToBuff_Float_Int32LSB(const double Data, unsigned char* WritePnt);
void __fastcall	ToBuff_Int8_Int32LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int16_Int32LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int24_Int32LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int32_Int32LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float32_Int32LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float64_Int32LSB(const int Idx, unsigned char* ReadPnt);

inline void __fastcall	ToBuff_Float_Float32LSB(float Data, unsigned char* WritePnt);
void __fastcall	ToBuff_Int8_Float32LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int16_Float32LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int24_Float32LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int32_Float32LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float32_Float32LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float64_Float32LSB(const int Idx, unsigned char* ReadPnt);

inline void __fastcall	ToBuff_Float_Float64LSB(double Data, unsigned char* WritePnt);
void __fastcall	ToBuff_Int8_Float64LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int16_Float64LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int24_Float64LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int32_Float64LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float32_Float64LSB(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float64_Float64LSB(const int Idx, unsigned char* ReadPnt);

inline void __fastcall	ToBuff_Float_Int32LSB16(const double Data, unsigned char* WritePnt);
void __fastcall	ToBuff_Int8_Int32LSB16(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int16_Int32LSB16(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int24_Int32LSB16(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int32_Int32LSB16(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float32_Int32LSB16(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float64_Int32LSB16(const int Idx, unsigned char* ReadPnt);

inline void __fastcall	ToBuff_Float_Int32LSB24(const double Data, unsigned char* WritePnt);
void __fastcall	ToBuff_Int8_Int32LSB24(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int16_Int32LSB24(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int24_Int32LSB24(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Int32_Int32LSB24(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float32_Int32LSB24(const int Idx, unsigned char* ReadPnt);
void __fastcall	ToBuff_Float64_Int32LSB24(const int Idx, unsigned char* ReadPnt);

