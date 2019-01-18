
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
ToBuff_Float_Int16MSB(const double Data, unsigned char* WritePnt)
{
	int		v = RInt32(Data);

	if(v > 0x7fff) {
		v = 0x7fff;
	} else if(v < -0x8000) {
		v = -0x8000;
	}

	*reinterpret_cast<short*>(WritePnt) = BSwap16(v);
}

//
// Int16 (Int 8bit -> Int 16bit)
//

void __fastcall
ToBuff_Int8_Int16MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;
	const int	Shift = 8;

	*reinterpret_cast<short*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			BSwap16((*ReadPnt - 128) << Shift);
}

//
// Int16 (Int 16bit -> Int 16bit)
//

void __fastcall
ToBuff_Int16_Int16MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;

	*reinterpret_cast<unsigned short*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			BSwap16(*reinterpret_cast<unsigned short*>(ReadPnt));
}

//
// Int16 (Int 24bit -> Int 16bit)
//

void __fastcall
ToBuff_Int24_Int16MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;
	const int	Shift = 8;

	ToBuff_Float_Int16MSB(
						static_cast<double>(Int24ToInt32(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 (Int 32bit -> Int 16bit)
//

void __fastcall
ToBuff_Int32_Int16MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;
	const int	Shift = 16;

	ToBuff_Float_Int16MSB(
						static_cast<double>(*reinterpret_cast<int*>(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 (Float 32bit -> Int 16bit)
//

void __fastcall
ToBuff_Float32_Int16MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;

	ToBuff_Float_Int16MSB(
						*reinterpret_cast<float*>(ReadPnt) * FSCALER16,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 (Float 64bit -> Int 16bit)
//

void __fastcall
ToBuff_Float64_Int16MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 2;

	ToBuff_Float_Int16MSB(
						*reinterpret_cast<double*>(ReadPnt) * FSCALER16,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Int24
//

inline void __fastcall
ToBuff_Float_Int24MSB(const double Data, unsigned char* WritePnt)
{
	__int64	v = RInt64(Data);

	if(v > 0x7fffffi64) {
		v = 0x7fffffi64;
	} else if(v < -0x800000i64) {
		v = -0x800000i64;
	}

	*(WritePnt + 0) = *(reinterpret_cast<unsigned char*>(&v) + 2);
	*(WritePnt + 1) = *(reinterpret_cast<unsigned char*>(&v) + 1);
	*(WritePnt + 2) = *(reinterpret_cast<unsigned char*>(&v) + 0);
}

//
// Int24 (Int 8bit -> Int 24bit)
//

void __fastcall
ToBuff_Int8_Int24MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;
	unsigned char*	WritePnt = ChannelInfo[Idx].Buff + BuffEnd * Bps_b;

	*WritePnt = *ReadPnt - 128;
	*reinterpret_cast<unsigned short*>(WritePnt + 1) = 0;
}

//
// Int24 (Int 16bit -> Int 24bit)
//

void __fastcall
ToBuff_Int16_Int24MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;
	unsigned char*	WritePnt = ChannelInfo[Idx].Buff + BuffEnd * Bps_b;

	*reinterpret_cast<unsigned short*>(WritePnt) =
			BSwap16(*reinterpret_cast<unsigned short*>(ReadPnt));
	*(WritePnt + 2) = 0;
}

//
// Int24 (Int 24bit -> Int 24bit)
//

void __fastcall
ToBuff_Int24_Int24MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;
	unsigned char*	WritePnt = ChannelInfo[Idx].Buff + BuffEnd * Bps_b;

	*(WritePnt + 0) = *(ReadPnt + 2);
	*(WritePnt + 1) = *(ReadPnt + 1);
	*(WritePnt + 2) = *(ReadPnt + 0);
}

//
// Int24 (Int 32bit -> Int 24bit)
//

void __fastcall
ToBuff_Int32_Int24MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;
	const int	Shift = 8;

	ToBuff_Float_Int24MSB(
						static_cast<double>(*reinterpret_cast<int*>(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int24 (Float 32bit -> Int 24bit)
//

void __fastcall
ToBuff_Float32_Int24MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;

	ToBuff_Float_Int24MSB(
						*reinterpret_cast<float*>(ReadPnt) * FSCALER24,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int24 (Float 64bit -> Int 24bit)
//

void __fastcall
ToBuff_Float64_Int24MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 3;

	ToBuff_Float_Int24MSB(
						*reinterpret_cast<double*>(ReadPnt) * FSCALER24,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Int32
//

inline void __fastcall
ToBuff_Float_Int32MSB(const double Data, unsigned char* WritePnt)
{
	__int64	v = RInt64(Data);

	if(v > 0x7fffffffi64) {
		v = 0x7fffffffi64;
	} else if(v < -0x80000000i64) {
		v = -0x80000000i64;
	}

	*reinterpret_cast<int*>(WritePnt) = BSwap32(static_cast<int>(v));
}

//
// Int32 (Int 8bit -> Int 32bit)
//

void __fastcall
ToBuff_Int8_Int32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 24;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			BSwap32((*ReadPnt - 128) << Shift);
}

//
// Int32 (Int 16bit -> Int 32bit)
//

void __fastcall
ToBuff_Int16_Int32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 16;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			BSwap32(*reinterpret_cast<short*>(ReadPnt) << Shift);
}

//
// Int32 (Int 24bit -> Int 32bit)
//

void __fastcall
ToBuff_Int24_Int32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 8;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			BSwap32(Int24ToInt32(ReadPnt) << Shift);
}

//
// Int32 (Int 32bit -> Int 32bit)
//

void __fastcall
ToBuff_Int32_Int32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	*reinterpret_cast<unsigned int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			BSwap32(*reinterpret_cast<unsigned int*>(ReadPnt));
}

//
// Int32 (Float 32bit -> Int 32bit)
//

void __fastcall
ToBuff_Float32_Int32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32MSB(
						*reinterpret_cast<float*>(ReadPnt) * FSCALER32,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int32 (Float 64bit -> Int 32bit)
//

void __fastcall
ToBuff_Float64_Int32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32MSB(
						*reinterpret_cast<double*>(ReadPnt) * FSCALER32,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Float32
//

inline void __fastcall
ToBuff_Float_Float32MSB(float Data, unsigned char* WritePnt)
{
	if(Data > 1.0) {
		Data = 1.0;
	} else if(Data < -1.0) {
		Data = -1.0;
	}

	*reinterpret_cast<unsigned int*>(WritePnt) = BSwap32(*reinterpret_cast<unsigned int*>(&Data));
}

//
// Float32 (Int 8bit -> Float 32bit)
//

void __fastcall
ToBuff_Int8_Float32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32MSB(
						static_cast<float>((*ReadPnt - 128) / FSCALER8),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float32 (Int 16bit -> Float 32bit)
//

void __fastcall
ToBuff_Int16_Float32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32MSB(
						static_cast<float>(*reinterpret_cast<short*>(ReadPnt) / FSCALER16),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float32 (Int 24bit -> Float 32bit)
//

void __fastcall
ToBuff_Int24_Float32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32MSB(
						static_cast<float>(Int24ToInt32(ReadPnt) / FSCALER24),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float32 (Int 32bit -> Float 32bit)
//

void __fastcall
ToBuff_Int32_Float32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32MSB(
						static_cast<float>(*reinterpret_cast<int*>(ReadPnt) / FSCALER32),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float32 (Float 32bit -> Float 32bit)
//

void __fastcall
ToBuff_Float32_Float32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32MSB(
						*reinterpret_cast<float*>(ReadPnt),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float32 (Float 64bit -> Float 32bit)
//

void __fastcall
ToBuff_Float64_Float32MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Float32MSB(
						static_cast<float>(*reinterpret_cast<double*>(ReadPnt)),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Float64
//

inline void __fastcall
ToBuff_Float_Float64MSB(double Data, unsigned char* WritePnt)
{
	if(Data > 1.0) {
		Data = 1.0;
	} else if(Data < -1.0) {
		Data = -1.0;
	}

	*reinterpret_cast<unsigned __int64*>(WritePnt) =
			BSwap64(*reinterpret_cast<unsigned __int64*>(&Data));
}

//
// Float64 (Int 8bit -> Float 64bit)
//

void __fastcall
ToBuff_Int8_Float64MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64MSB(
						(*ReadPnt - 128) / FSCALER8,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float64 (Int 16bit -> Float 64bit)
//

void __fastcall
ToBuff_Int16_Float64MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64MSB(
						*reinterpret_cast<short*>(ReadPnt) / FSCALER16,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float64 (Int 24bit -> Float 64bit)
//

void __fastcall
ToBuff_Int24_Float64MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64MSB(
						Int24ToInt32(ReadPnt) / FSCALER24,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float64 (Int 32bit -> Float 64bit)
//

void __fastcall
ToBuff_Int32_Float64MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64MSB(
						*reinterpret_cast<int*>(ReadPnt) / FSCALER32,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float64 (Float 32bit -> Float 64bit)
//

void __fastcall
ToBuff_Float32_Float64MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64MSB(
						*reinterpret_cast<float*>(ReadPnt),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Float64 (Float 64bit -> Float 64bit)
//

void __fastcall
ToBuff_Float64_Float64MSB(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 8;

	ToBuff_Float_Float64MSB(
						*reinterpret_cast<double*>(ReadPnt),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Int16 in Int32
//

inline void __fastcall
ToBuff_Float_Int32MSB16(const double Data, unsigned char* WritePnt)
{
	int		v = RInt32(Data);

	if(v > 0x7fff) {
		v = 0x7fff;
	} else if(v < -0x8000) {
		v = -0x8000;
	}

	*reinterpret_cast<int*>(WritePnt) = BSwap32(v);
}

//
// Int16 in Int32 (Int 8bit -> Int 16bit)
//

void __fastcall
ToBuff_Int8_Int32MSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 8;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			BSwap32((*ReadPnt - 128) << Shift);
}

//
// Int16 in Int32 (Int 16bit -> Int 16bit)
//

void __fastcall
ToBuff_Int16_Int32MSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			BSwap32(*reinterpret_cast<short*>(ReadPnt));
}

//
// Int16 in Int32 (Int 24bit -> Int 16bit)
//

void __fastcall
ToBuff_Int24_Int32MSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 8;

	ToBuff_Float_Int32MSB16(
						static_cast<double>(Int24ToInt32(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 in Int32 (Int 32bit -> Int 16bit)
//

void __fastcall
ToBuff_Int32_Int32MSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 16;

	ToBuff_Float_Int32MSB16(
						static_cast<double>(*reinterpret_cast<int*>(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 in Int32 (Float 32bit -> Int 16bit)
//

void __fastcall
ToBuff_Float32_Int32MSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32MSB16(
						*reinterpret_cast<float*>(ReadPnt) * FSCALER16,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int16 in Int32 (Float 64bit -> Int 16bit)
//

void __fastcall
ToBuff_Float64_Int32MSB16(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32MSB16(
						*reinterpret_cast<double*>(ReadPnt) * FSCALER16,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

// --------------------------------
//
// Int24 in Int32
//

inline void __fastcall
ToBuff_Float_Int32MSB24(const double Data, unsigned char* WritePnt)
{
	__int64	v = RInt64(Data);

	if(v > 0x7fffffi64) {
		v = 0x7fffffi64;
	} else if(v < -0x800000i64) {
		v = -0x800000i64;
	}

	*reinterpret_cast<int*>(WritePnt) = BSwap32(static_cast<int>(v));
}

//
// Int24 in Int32 (Int 8bit -> Int 24bit)
//

void __fastcall
ToBuff_Int8_Int32MSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 16;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			BSwap32((*ReadPnt - 128) << Shift);
}

//
// Int24 in Int32 (Int 16bit -> Int 24bit)
//

void __fastcall
ToBuff_Int16_Int32MSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 8;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) =
			BSwap32(*reinterpret_cast<short*>(ReadPnt) << Shift);
}

//
// Int24 in Int32 (Int 24bit -> Int 24bit)
//

void __fastcall
ToBuff_Int24_Int32MSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	*reinterpret_cast<int*>(ChannelInfo[Idx].Buff + BuffEnd * Bps_b) = BSwap32(Int24ToInt32(ReadPnt));
}

//
// Int24 in Int32 (Int 32bit -> Int 24bit)
//

void __fastcall
ToBuff_Int32_Int32MSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;
	const int	Shift = 8;

	ToBuff_Float_Int32MSB24(
						static_cast<double>(*reinterpret_cast<int*>(ReadPnt)) / (1 << Shift),
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int24 in Int32 (Float 32bit -> Int 24bit)
//

void __fastcall
ToBuff_Float32_Int32MSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32MSB24(
						*reinterpret_cast<float*>(ReadPnt) * FSCALER24,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

//
// Int24 in Int32 (Float 64bit -> Int 24bit)
//

void __fastcall
ToBuff_Float64_Int32MSB24(const int Idx, unsigned char* ReadPnt)
{
	const int	Bps_b = 4;

	ToBuff_Float_Int32MSB24(
						*reinterpret_cast<double*>(ReadPnt) * FSCALER24,
						ChannelInfo[Idx].Buff + BuffEnd * Bps_b);
}

