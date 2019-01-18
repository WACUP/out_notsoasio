
class Buffer
{
private:
	char * buffer;
	unsigned int buf_size,buf_data;
public:
	inline Buffer() {buf_size=0;buf_data=0;buffer=0;}
	inline ~Buffer() {if (buffer) free(buffer);}
	inline void* __fastcall GetBuffer(unsigned int * siz) {*siz=buf_data;return buffer;}
	inline unsigned int __fastcall Size() {return buf_data;}
	void __fastcall Read(unsigned int size);
	void __fastcall Write(void * ptr,unsigned int size);
	inline void __fastcall Flush(void) {buf_data = 0;}
};

//#ifndef HIGHPREC
//typedef float REAL;
//#else
typedef double REAL;
//#endif

class Shaper;

class Resampler_base
{
public:
	class CONFIG
	{
	public:
		int sfrq, dfrq, sformat, dformat, bps, dbps, nch, quality/*, dither, pdf*/;
		_inline CONFIG(
					int _sfrq,
					int _dfrq,
					int _sformat,
					int _dformat,
					int _bps,
					int _dbps,
					int _nch,
					int _quality/*,
					int _dither,
					int _pdf*/)
		{
			sfrq=_sfrq;
			dfrq=_dfrq;
			sformat=_sformat;
			dformat=_dformat;
			bps=_bps;
			dbps=_dbps;
			nch=_nch;
			quality=_quality;
//			dither=_dither;
//			pdf=_pdf;
		}
	};

private:
	Buffer in,out;
	void __fastcall bufloop(int finish);

protected:
	double AA, DF;
	int FFTFIRLEN;

	int nch, sfrq, dfrq, sformat, dformat, bps, dbps;
//	Shaper * shaper;
	double gain;
	int delay;

	Resampler_base(CONFIG & c);

	void inline __fastcall __output(void * ptr, int size)
	{
		if(delay) {
			delay--;
		} else {
			out.Write(ptr, size);
		}
	}
	virtual unsigned int __fastcall Resample(unsigned char *input, unsigned int size, int ending)=0;
	void __fastcall make_outbuf(int nsmplwrt2, REAL* outbuf);
	void __fastcall make_inbuf(
							int nsmplread,
							int inbuflen,
							unsigned char* rawinbuf,
							REAL* inbuf,
							int toberead);

public:
	enum
	{
		DATA_FORMAT_LINEAR_PCM,
		DATA_FORMAT_IEEE_FLOAT,
	};

	inline void __fastcall Write(void* input, unsigned int size) {in.Write(input,size); bufloop(0);}
	inline void __fastcall Finish() {bufloop(1);}

	inline void* __fastcall GetBuffer(unsigned int* s) {return out.GetBuffer(s);}
	inline void __fastcall Read(unsigned int s) {out.Read(s);}
	inline void __fastcall Flush(void) {in.Flush(); out.Flush();}

	unsigned int __fastcall GetLatency();//returns amount of audio data in in/out buffers in milliseconds

	inline unsigned int __fastcall GetDataInInbuf() {return in.Size();}
	inline unsigned int __fastcall GetDataInOutbuf() {return out.Size();}

	virtual ~Resampler_base() {}

	static Resampler_base* __fastcall Create(CONFIG& c);
};

#define	SSRC_create(sfrq, dfrq, sformat, dformat, bps, dbps, nch, quality/*, dither, pdf*/) \
			Resampler_base::Create(Resampler_base::CONFIG(\
			sfrq, dfrq, sformat, dformat, bps, dbps, nch, quality/*, dither, pdf*/))

/*
USAGE:
Resampler_base::Create() / SSRC_create with your resampling params (see source for exact info what they do)

 loop:
Write() your PCM data to be resampled
GetBuffer() to get pointer to buffer with resampled data and amount of resampled data available (in bytes)
(note: SSRC operates on blocks, don't expect it to return any data right after your first Write() )

Read() to tell the resampler how much data you took from the buffer ( <= size returned by GetBuffer )

you can use GetLatency() to get amount of audio data (in ms) currently being kept in resampler's buffers
(quick hack for determining current output time without extra stupid counters)

Finish() to force-convert remaining PCM data after last Write() (warning: never Write() again after Flush() )

delete when done

*/
