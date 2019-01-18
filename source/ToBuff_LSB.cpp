
#define	STRICT

#include <windows.h>

#include "WSL/StdProc.h"

#include "pcmasio.h"

extern int	BuffEnd;

extern _ChannelInfo*	ChannelInfo;

// --------------------------------
//
// Int16
//

inline void __fastcall
ToBuff_Float_Int16LSB(const double Data, unsigned char* WritePnt)
{
	int		v = RInt32(Data);

	if(v > 0x7fff) {
		v = 0x7fff;
	} else if(v < -0x8000) {
		v = -0x8000;
	}

	*reinterpret_cast<short*>(WritePnt) = v;
}

//
// Int16 (Int 8bit -> Int 16bit)
//

void __fastcall
ToBuff_Int8_Int16LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;
	const int	Shift = 8;

	*reinterpret_cast<short*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) = (*ReadPnt - 128) << Shift;
}

//
// Int16 (Int 16bit -> Int 16bit)
//

void __fastcall
ToBuff_Int16_Int16LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;

	*reinterpret_cast<unsigned short*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			*reinterpret_cast<unsigned short*>(ReadPnt);
}

//
// Int16 (Int 24bit -> Int 16bit)
//

void __fastcall
ToBuff_Int24_Int16LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;
	const int	Shift = 8;

	ToBuff_Float_Int16LSB(
						static_cast<double>(Int24ToInt32(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 (Int 32bit -> Int 16bit)
//

void __fastcall
ToBuff_Int32_Int16LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;
	const int	Shift = 16;

	ToBuff_Float_Int16LSB(
						static_cast<double>(*reinterpret_cast<int*>(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 (Float 32bit -> Int 16bit)
//

void __fastcall
ToBuff_Float32_Int16LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;

	ToBuff_Float_Int16LSB(
						*reinterpret_cast<float*>(ReadPnt) * FSCALER16,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 (Float 64bit -> Int 16bit)
//

void __fastcall
ToBuff_Float64_Int16LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;

	ToBuff_Float_Int16LSB(
						*reinterpret_cast<double*>(ReadPnt) * FSCALER16,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Int24
//

inline void __fastcall
ToBuff_Float_Int24LSB(const double Data, unsigned char* WritePnt)
{
	__int64	v = RInt64(Data);

	if(v > 0x7fffffi64) {
		v = 0x7fffffi64;
	} else if(v < -0x800000i64) {
		v = -0x800000i64;
	}

	Memcpy3bytes(WritePnt, reinterpret_cast<unsigned char*>(&v));
}

//
// Int24 (Int 8bit -> Int 24bit)
//

void __fastcall
ToBuff_Int8_Int24LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;
	unsigned char*	WritePnt = ChannelInfo[Idx].Buff + BuffEnd * Bps_b;

	*reinterpret_cast<unsigned short*>(WritePnt) = 0;
	*(WritePnt + 2) = *ReadPnt - 128;
}

//
// Int24 (Int 16bit -> Int 24bit)
//

void __fastcall
ToBuff_Int16_Int24LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;
	unsigned char*	WritePnt = ChannelInfo[Idx].Buff + BuffEnd * Bps_b;

	*WritePnt = 0;
	*reinterpret_cast<unsigned short*>(WritePnt + 1) = *reinterpret_cast<unsigned short*>(ReadPnt);
}

//
// Int24 (Int 24bit -> Int 24bit)
//

void __fastcall
ToBuff_Int24_Int24LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;

	Memcpy3bytes(ChannelInfo[Idx].Buff + BuffEnd * Bps_b, ReadPnt);
}

//
// Int24 (Int 32bit -> Int 24bit)
//

void __fastcall
ToBuff_Int32_Int24LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;
	const int	Shift = 8;

	ToBuff_Float_Int24LSB(
						static_cast<double>(*reinterpret_cast<int*>(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int24 (Float 32bit -> Int 24bit)
//

void __fastcall
ToBuff_Float32_Int24LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;

	ToBuff_Float_Int24LSB(
						*reinterpret_cast<float*>(ReadPnt) * FSCALER24,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int24 (Float 64bit -> Int 24bit)
//

void __fastcall
ToBuff_Float64_Int24LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;

	ToBuff_Float_Int24LSB(
						*reinterpret_cast<double*>(ReadPnt) * FSCALER24,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Int32
//

inline void __fastcall
ToBuff_Float_Int32LSB(const double Data, unsigned char* WritePnt)
{
	__int64	v = RInt64(Data);

	if(v > 0x7fffffffi64) {
		v = 0x7fffffffi64;
	} else if(v < -0x80000000i64) {
		v = -0x80000000i64;
	}

	*reinterpret_cast<int*>(WritePnt) = static_cast<int>(v);
}

//
// Int32 (Int 8bit -> Int 32bit)
//

void __fastcall
ToBuff_Int8_Int32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 24;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) = (*ReadPnt - 128) << Shift;
}

//
// Int32 (Int 16bit -> Int 32bit)
//

void __fastcall
ToBuff_Int16_Int32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 16;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			*reinterpret_cast<short*>(ReadPnt) << Shift;
}

//
// Int32 (Int 24bit -> Int 32bit)
//

void __fastcall
ToBuff_Int24_Int32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 8;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) = Int24ToInt32(ReadPnt) << Shift;
}

//
// Int32 (Int 32bit -> Int 32bit)
//

void __fastcall
ToBuff_Int32_Int32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	*reinterpret_cast<unsigned int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			*reinterpret_cast<unsigned int*>(ReadPnt);
}

//
// Int32 (Float 32bit -> Int 32bit)
//

void __fastcall
ToBuff_Float32_Int32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32LSB(
						*reinterpret_cast<float*>(ReadPnt) * FSCALER32,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int32 (Float 64bit -> Int 32bit)
//

void __fastcall
ToBuff_Float64_Int32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32LSB(
						*reinterpret_cast<double*>(ReadPnt) * FSCALER32,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Float32
//

inline void __fastcall
ToBuff_Float_Float32LSB(float Data, unsigned char* WritePnt)
{
	if(Data > 1.0) {
		Data = 1.0;
	} else if(Data < -1.0) {
		Data = -1.0;
	}

	*reinterpret_cast<float*>(WritePnt) = Data;
}

//
// Float32 (Int 8bit -> Float 32bit)
//

void __fastcall
ToBuff_Int8_Float32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32LSB(
						static_cast<float>((*ReadPnt - 128) / FSCALER8),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float32 (Int 16bit -> Float 32bit)
//

void __fastcall
ToBuff_Int16_Float32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32LSB(
						static_cast<float>(*reinterpret_cast<short*>(ReadPnt) / FSCALER16),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float32 (Int 24bit -> Float 32bit)
//

void __fastcall
ToBuff_Int24_Float32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32LSB(
						static_cast<float>(Int24ToInt32(ReadPnt) / FSCALER24),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float32 (Int 32bit -> Float 32bit)
//

void __fastcall
ToBuff_Int32_Float32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32LSB(
						static_cast<float>(*reinterpret_cast<int*>(ReadPnt) / FSCALER32),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float32 (Float 32bit -> Float 32bit)
//

void __fastcall
ToBuff_Float32_Float32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32LSB(
						*reinterpret_cast<float*>(ReadPnt),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float32 (Float 64bit -> Float 32bit)
//

void __fastcall
ToBuff_Float64_Float32LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32LSB(
						static_cast<float>(*reinterpret_cast<double*>(ReadPnt)),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Float64
//

inline void __fastcall
ToBuff_Float_Float64LSB(double Data, unsigned char* WritePnt)
{
	if(Data > 1.0) {
		Data = 1.0;
	} else if(Data < -1.0) {
		Data = -1.0;
	}

	*reinterpret_cast<double*>(WritePnt) = Data;
}

//
// Float64 (Int 8bit -> Float 64bit)
//

void __fastcall
ToBuff_Int8_Float64LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64LSB(
						(*ReadPnt - 128) / FSCALER8,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float64 (Int 16bit -> Float 64bit)
//

void __fastcall
ToBuff_Int16_Float64LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64LSB(
						*reinterpret_cast<short*>(ReadPnt) / FSCALER16,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float64 (Int 24bit -> Float 64bit)
//

void __fastcall
ToBuff_Int24_Float64LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64LSB(
						Int24ToInt32(ReadPnt) / FSCALER24,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float64 (Int 32bit -> Float 64bit)
//

void __fastcall
ToBuff_Int32_Float64LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64LSB(
						*reinterpret_cast<int*>(ReadPnt) / FSCALER32,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float64 (Float 32bit -> Float 64bit)
//

void __fastcall
ToBuff_Float32_Float64LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64LSB(
						*reinterpret_cast<float*>(ReadPnt),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float64 (Float 64bit -> Float 64bit)
//

void __fastcall
ToBuff_Float64_Float64LSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64LSB(
						*reinterpret_cast<double*>(ReadPnt),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Int16 in Int32
//

inline void __fastcall
ToBuff_Float_Int32LSB16(const double Data, unsigned char* WritePnt)
{
	int		v = RInt32(Data);

	if(v > 0x7fff) {
		v = 0x7fff;
	} else if(v < -0x8000) {
		v = -0x8000;
	}

	*reinterpret_cast<int*>(WritePnt) = v;
}

//
// Int16 in Int32 (Int 8bit -> Int 16bit)
//

void __fastcall
ToBuff_Int8_Int32LSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 8;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) = (*ReadPnt - 128) << Shift;
}

//
// Int16 in Int32 (Int 16bit -> Int 16bit)
//

void __fastcall
ToBuff_Int16_Int32LSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			*reinterpret_cast<short*>(ReadPnt);
}

//
// Int16 in Int32 (Int 24bit -> Int 16bit)
//

void __fastcall
ToBuff_Int24_Int32LSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 8;

	ToBuff_Float_Int32LSB16(
						static_cast<double>(Int24ToInt32(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 in Int32 (Int 32bit -> Int 16bit)
//

void __fastcall
ToBuff_Int32_Int32LSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 16;

	ToBuff_Float_Int32LSB16(
						static_cast<double>(*reinterpret_cast<int*>(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 in Int32 (Float 32bit -> Int 16bit)
//

void __fastcall
ToBuff_Float32_Int32LSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32LSB16(
						*reinterpret_cast<float*>(ReadPnt) * FSCALER16,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 in Int32 (Float 64bit -> Int 16bit)
//

void __fastcall
ToBuff_Float64_Int32LSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32LSB16(
						*reinterpret_cast<double*>(ReadPnt) * FSCALER16,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Int24 in Int32
//

inline void __fastcall
ToBuff_Float_Int32LSB24(const double Data, unsigned char* WritePnt)
{
	__int64	v = RInt64(Data);

	if(v > 0x7fffffi64) {
		v = 0x7fffffi64;
	} else if(v < -0x800000i64) {
		v = -0x800000i64;
	}

	*reinterpret_cast<int*>(WritePnt) = static_cast<int>(v);
}

//
// Int24 in Int32 (Int 8bit -> Int 24bit)
//

void __fastcall
ToBuff_Int8_Int32LSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 16;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) = (*ReadPnt - 128) << Shift;
}

//
// Int24 in Int32 (Int 16bit -> Int 24bit)
//

void __fastcall
ToBuff_Int16_Int32LSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 8;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			*reinterpret_cast<short*>(ReadPnt) << Shift;
}

//
// Int24 in Int32 (Int 24bit -> Int 24bit)
//

void __fastcall
ToBuff_Int24_Int32LSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) = Int24ToInt32(ReadPnt);
}

//
// Int24 in Int32 (Int 32bit -> Int 24bit)
//

void __fastcall
ToBuff_Int32_Int32LSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 8;

	ToBuff_Float_Int32LSB24(
						static_cast<double>(*reinterpret_cast<int*>(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int24 in Int32 (Float 32bit -> Int 24bit)
//

void __fastcall
ToBuff_Float32_Int32LSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32LSB24(
						*reinterpret_cast<float*>(ReadPnt) * FSCALER24,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int24 in Int32 (Float 64bit -> Int 24bit)
//

void __fastcall
ToBuff_Float64_Int32LSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32LSB24(
						*reinterpret_cast<double*>(ReadPnt) * FSCALER24,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

