
#define	STRICT

#include <windows.h>
#include <math.h>
//#include <assert.h>

#include "ssrc.h"
#include "../WSL/StdProc.h"
//#include "../../out_asio/WSL/StdProc.h"

#define	M			15

#ifndef M_PI
#define	M_PI		3.1415926535897932384626433832795028842
#endif

//#define	RANDBUFLEN	65536
//#define	RANDMT_MAX	0x7fffffff

//#define RINT(x) ((x) >= 0 ? ((__int64)((x) + 0.5)) : ((__int64)((x) - 0.5)))

//int		randMT(void);
void __fastcall	rdft(int n, int isgn, REAL* a, int* ip, REAL* w);
double __fastcall	dbesi0(double x);

//const int scoeffreq[] = {0,48000,44100,37800,32000,22050,48000,44100};

//const int scoeflen[] =  {1,16,20,16,16,15,16,15};

#if 0
const double shapercoefs[8][21] = {
  {-1}, /* triangular dither */

  {-2.8720729351043701172,   5.0413231849670410156,  -6.2442994117736816406,   5.8483986854553222656,
   -3.7067542076110839844,   1.0495119094848632812,   1.1830236911773681641,  -2.1126792430877685547,
    1.9094531536102294922,  -0.99913084506988525391,  0.17090806365013122559,  0.32615602016448974609,
   -0.39127644896507263184,  0.26876461505889892578, -0.097676105797290802002, 0.023473845794796943665,
  }, /* 48k, N=16, amp=18 */

  {-2.6773197650909423828,   4.8308925628662109375,  -6.570110321044921875,    7.4572014808654785156,
   -6.7263274192810058594,   4.8481650352478027344,  -2.0412089824676513672,  -0.7006359100341796875,
    2.9537565708160400391,  -4.0800385475158691406,   4.1845216751098632812,  -3.3311812877655029297,
    2.1179926395416259766,  -0.879302978515625,       0.031759146600961685181, 0.42382788658142089844,
   -0.47882103919982910156,  0.35490813851356506348, -0.17496839165687561035,  0.060908168554306030273,
  }, /* 44.1k, N=20, amp=27 */

  {-1.6335992813110351562,   2.2615492343902587891,  -2.4077029228210449219,   2.6341717243194580078,
   -2.1440362930297851562,   1.8153258562088012695,  -1.0816224813461303711,   0.70302653312683105469,
   -0.15991993248462677002, -0.041549518704414367676, 0.29416576027870178223, -0.2518316805362701416,
    0.27766478061676025391, -0.15785403549671173096,  0.10165894031524658203, -0.016833892092108726501,
  }, /* 37.8k, N=16 */

  {-0.82901298999786376953,  0.98922657966613769531, -0.59825712442398071289,  1.0028809309005737305,
   -0.59938216209411621094,  0.79502451419830322266, -0.42723315954208374023,  0.54492527246475219727,
   -0.30792605876922607422,  0.36871799826622009277, -0.18792048096656799316,  0.2261127084493637085,
   -0.10573341697454452515,  0.11435490846633911133, -0.038800679147243499756, 0.040842197835445404053,
  }, /* 32k, N=16 */

  {-0.065229974687099456787, 0.54981261491775512695,  0.40278548002243041992,  0.31783768534660339355,
    0.28201797604560852051,  0.16985194385051727295,  0.15433363616466522217,  0.12507140636444091797,
    0.08903945237398147583,  0.064410120248794555664, 0.047146003693342208862, 0.032805237919092178345,
    0.028495194390416145325, 0.011695005930960178375, 0.011831838637590408325,
  }, /* 22.05k, N=15 */

  {-2.3925774097442626953,   3.4350297451019287109,  -3.1853709220886230469,   1.8117271661758422852,
    0.20124770700931549072, -1.4759907722473144531,   1.7210904359817504883,  -0.97746700048446655273,
    0.13790138065814971924,  0.38185903429985046387, -0.27421241998672485352, -0.066584214568138122559,
    0.35223302245140075684, -0.37672343850135803223,  0.23964276909828186035, -0.068674825131893157959,
  }, /* 48k, N=16, amp=10 */

  {-2.0833916664123535156,   3.0418450832366943359,  -3.2047898769378662109,   2.7571926116943359375,
   -1.4978630542755126953,   0.3427594602108001709,   0.71733748912811279297, -1.0737057924270629883,
    1.0225815773010253906,  -0.56649994850158691406,  0.20968692004680633545,  0.065378531813621520996,
   -0.10322438180446624756,  0.067442022264003753662, 0.00495197344571352005,
  }, /* 44.1k, N=15, amp=9 */

#if 0
  { -3.0259189605712890625,  6.0268716812133789062,  -9.195003509521484375,   11.824929237365722656,
   -12.767142295837402344,  11.917946815490722656,   -9.1739168167114257812,   5.3712320327758789062,
    -1.1393624544143676758, -2.4484779834747314453,   4.9719839096069335938,  -6.0392003059387207031,
     5.9359521865844726562, -4.903278350830078125,    3.5527443885803222656,  -2.1909697055816650391,
     1.1672389507293701172, -0.4903914332389831543,   0.16519790887832641602, -0.023217858746647834778,
  }, /* 44.1k, N=20 */
#endif
};
#endif

#if 0
class Shaper
{
	double **shapebuf;
	int shaper_type,shaper_len;
	__int64 shaper_clipmin,shaper_clipmax;
	REAL randbuf[RANDBUFLEN];
	int randptr;
	int dtype;
	int nch;
	
#define POOLSIZE 97

public:
	Shaper(int freq,int _nch,__int64 min,__int64 max,int _dtype,int pdf,double noiseamp)
	{
	  int i;
	  int pool[POOLSIZE];

	  nch=_nch;
	  dtype=_dtype;

	  for(i=1;i<6;i++) if (freq == scoeffreq[i]) break;
/*	  if ((dtype == 3 || dtype == 4) && i == 6) {
		fprintf(stderr,"Warning: ATH based noise shaping for destination frequency %dHz is not available, using triangular dither\n",freq);
	  }*/
	  if (dtype == 2 || i == 6) i = 0;
	  if (dtype == 4 && (i == 1 || i == 2)) i += 5;

	  shaper_type = i;

	  shapebuf = (double**)malloc(sizeof(double *)*nch);
	  shaper_len = scoeflen[shaper_type];

	  for(i=0;i<nch;i++)
		shapebuf[i] = (double*)calloc(shaper_len,sizeof(double));

	  shaper_clipmin = min;
	  shaper_clipmax = max;

	  for(i=0;i<POOLSIZE;i++) pool[i] = randMT();

	  switch(pdf)
		{
		case 0: // rectangular
		  for(i=0;i<RANDBUFLEN;i++)
		{
		  int r,p;

		  p = randMT() % POOLSIZE;
		  r = pool[p]; pool[p] = randMT();
		  randbuf[i] = noiseamp * (((double)r)/RANDMT_MAX-0.5);
		}
		  break;

		case 1: // triangular
		  for(i=0;i<RANDBUFLEN;i++)
		{
		  int r1,r2,p;

		  p = randMT() % POOLSIZE;
		  r1 = pool[p]; pool[p] = randMT();
		  p = randMT() % POOLSIZE;
		  r2 = pool[p]; pool[p] = randMT();
		  randbuf[i] = noiseamp * ((((double)r1)/RANDMT_MAX)-(((double)r2)/RANDMT_MAX));
		}
		  break;

		case 2: // gaussian
		  for(i=0;i<RANDBUFLEN;i++)
		{
		  double t,u;
		  double r;
		  int p;

		  if ( (i&1)==0 ) {

			p = randMT() % POOLSIZE;
			r = ((double)pool[p])/((double)RANDMT_MAX+1.0); pool[p] = randMT();
			//                       ^^ bug in original SSRC, causing log(0)

			t = sqrt(-2 * log(1-r));

			p = randMT() % POOLSIZE;
			r = ((double)pool[p])/RANDMT_MAX; pool[p] = randMT();
		  
			u = 2 * M_PI * r;
			
			randbuf[i] = noiseamp * t * cos(u);
		  } else {

			randbuf[i] = noiseamp * t * sin(u);
		  }
		}
		  break;
		}

	  randptr = 0;

//	  if (dtype == 0 || dtype == 1) return 1;
	  //return samp[shaper_type];
	}

	__int64 do_shaping(double s,int ch)
	{
	  double u,h;
	  int i;

	  if (dtype == 1) {
		s += randbuf[randptr++ & (RANDBUFLEN-1)];

		if (s < shaper_clipmin) {
		  s = static_cast<double>(shaper_clipmin);
		} else if (s > shaper_clipmax) {
		  s = static_cast<double>(shaper_clipmax);
		}

		return RINT(s);
	  }

	  h = 0;
	  for(i=0;i<shaper_len;i++) h += shapercoefs[shaper_type][i]*shapebuf[ch][i];
	  s += h;
	  u = s;
	  s += randbuf[randptr++ & (RANDBUFLEN-1)];
	  if (s < shaper_clipmin) {
		s = static_cast<double>(shaper_clipmin);
	  } else if (s > shaper_clipmax) {
		s = static_cast<double>(shaper_clipmax);
	  }
	  s = static_cast<double>(RINT(s));
	  for(i=shaper_len-2;i>=0;i--) shapebuf[ch][i+1] = shapebuf[ch][i];
	  shapebuf[ch][0] = s-u;

	  return static_cast<__int64>(s);
	}

	~Shaper()
	{
	  int i;

	  for(i=0;i<nch;i++) free(shapebuf[i]);
	  free(shapebuf);
	}
};
#endif

inline double __fastcall alpha(double a)
{
  if (a <= 21) return 0;
  if (a <= 50) return 0.5842*pow(a-21,0.4)+0.07886*(a-21);
  return 0.1102*(a-8.7);
}

inline double __fastcall win(double n,int len,double alp,double iza)
{
  return dbesi0(alp*sqrt(1-4*n*n/(((double)len-1)*((double)len-1))))/iza;
}

inline double __fastcall sinc(double x)
{
  return x == 0 ? 1 : sin(x)/x;
}

inline double __fastcall hn_lpf(int n,double lpf,double fs)
{
  double t = 1/fs;
  double omega = 2*M_PI*lpf;
  return 2*lpf*t*sinc(n*omega*t);
}

inline int __fastcall gcd(int x, int y)
{
    int t;

    while (y != 0) {
        t = x % y;  x = y;  y = t;
    }
    return x;
}

int __fastcall CanResample(int sfrq,int dfrq)
{
	if (sfrq==dfrq) return 1;
    int frqgcd = gcd(sfrq,dfrq);

	if (dfrq>sfrq)
	{
		int fs1 = sfrq / frqgcd * dfrq;

		if (fs1/dfrq == 1) return 1;
		else if (fs1/dfrq % 2 == 0) return 1;
		else if (fs1/dfrq % 3 == 0) return 1;
		else return 0;
	}
	else
	{
		if (dfrq/frqgcd == 1) return 1;
		else if (dfrq/frqgcd % 2 == 0) return 1;
		else if (dfrq/frqgcd % 3 == 0) return 1;
		else return 0;
	}
}

void __fastcall Buffer::Read(unsigned int size)
{
	if (size)
	{
		if (buf_data==size) buf_data=0;
		else
		{
			memmove(buffer,buffer+size,buf_data-size);
			buf_data-=size;
		}
	}
}

void __fastcall Buffer::Write(void * ptr,unsigned int size)
{
	if (!buffer)
	{
		buf_size=1024;
		while(buf_size<size) buf_size<<=1;
		buffer=(char*)malloc(buf_size);
		buf_data=0;
	}
	else if (buf_size<buf_data+size)
	{
		do
		{
			buf_size<<=1;
		} while(buf_size<buf_data+size);
		buffer=(char*)realloc(buffer,buf_size);
	}
	memcpy(buffer+buf_data,ptr,size);
	buf_data+=size;
}

void __fastcall Resampler_base::bufloop(int finish)
{
	unsigned int s;
	unsigned char * ptr=(unsigned char *)in.GetBuffer(&s);
	unsigned int done=0;
	while(done<s)
	{
		unsigned int d=Resample((unsigned char*)ptr,s-done,finish);
		if (d==0) break;
		done+=d;
		ptr+=d;
	}
	in.Read(done);
}

void __fastcall Resampler_base::make_inbuf(
										int nsmplread,
										int inbuflen,
										unsigned char* rawinbuf,
										REAL* inbuf,
										int toberead)
{
	const int	MaxLoop = nsmplread * nch;
	const int	InbufBase = inbuflen * nch;

	switch(sformat) {
	case DATA_FORMAT_LINEAR_PCM:
		switch(bps) {
		case 1:
			for(int i = 0; i < MaxLoop; i++) {
			  inbuf[InbufBase + i] = (REAL)(rawinbuf[i] - 128) / (REAL)0x7f;
			}
			break;
		case 2:
#ifndef BIGENDIAN
			for(int i = 0; i < MaxLoop; i++) {
			  inbuf[InbufBase+ i] = (REAL)((short*)rawinbuf)[i] / (REAL)0x7fff;
			}
#else
			for(int i = 0; i < MaxLoop; i++) {
				const int	RawinbufBase = i * 2;

				inbuf[InbufBase + i] =
								(REAL)(((int)rawinbuf[RawinbufBase]) |
								(((int)((char*)rawinbuf)[RawinbufBase + 1]) << 8)) /
								(REAL)0x7fff;
			}
#endif
			break;
		case 3:
			for(int i = 0; i < MaxLoop; i++) {
				const int	RawinbufBase = i * 3;

				inbuf[InbufBase + i] =
								(REAL)(((int)rawinbuf[RawinbufBase]) |
								(((int)rawinbuf[RawinbufBase + 1]) << 8) |
								(((int)((char*)rawinbuf)[RawinbufBase + 2]) << 16)) /
								(REAL)0x7fffff;
			}
			break;
		case 4:
#ifndef BIGENDIAN
			for(int i = 0; i < MaxLoop; i++) {
				inbuf[InbufBase + i] = (REAL)((int*)rawinbuf)[i] / (REAL)0x7fffffff;
			}
#else
			for(i = 0; i < MaxLoop; i++) {
				const int	RawinbufBase = i * 4;

				inbuf[InbufBase + i] =
								(REAL)(((int)rawinbuf[RawinbufBase]) |
								(((int)rawinbuf[RawinbufBase + 1]) << 8) |
								(((int)rawinbuf[RawinbufBase + 2]) << 16) |
								(((int)((char*)rawinbuf)[RawinbufBase + 3]) << 24)) /
								(REAL)0x7fffffff;
			}
#endif
			break;
		}
		break;
	case DATA_FORMAT_IEEE_FLOAT:
		switch(bps) {
		case 4:
			for(int i = 0; i < MaxLoop; i++) {
				inbuf[InbufBase + i] = ((float*)rawinbuf)[i];
			}
			break;
		case 8:
			memcpy(inbuf + InbufBase, rawinbuf, MaxLoop * sizeof(double));

//			for(int i = 0; i < MaxLoop; i++) {
//				inbuf[InbufBase + i] = ((double*)rawinbuf)[i];
//			}
			break;
		}
		break;
	}

//	for(;i<nch*toberead;i++) inbuf[i] = 0; // bug ?

	size_t	ClearSize = toberead - nsmplread;

	if(ClearSize) {
		memset(inbuf + InbufBase + MaxLoop, 0, ClearSize * nch * sizeof REAL);
	}
}

void __fastcall Resampler_base::make_outbuf(int nsmplwrt2, REAL* outbuf)
{
	unsigned char	rawoutbuf[8];
	const int	MaxLoop = nsmplwrt2 * nch;
	REAL	gain2;

	switch(dformat) {
	case DATA_FORMAT_LINEAR_PCM:
		switch(dbps) {
		case 1:
			gain2 = /*gain * */(REAL)0x7f;

			for(int i = 0, ch = 0; i < MaxLoop; i++) {
				__int64	s;

//				if(shaper) {
//					s = shaper->do_shaping(outbuf[i] * gain2, ch);
//				} else {
					s = RInt64(outbuf[i] * gain2);

					if(s > 0x7fi64) {
						s =  0x7fi64;
					} else if(s < -0x80i64) {
						s = -0x80i64;
					}
//				}

				rawoutbuf[0] = static_cast<unsigned char>(s + 0x80i64);
				__output(rawoutbuf, 1);

				ch = (ch + 1) % nch;
			}
			break;
		case 2:
			gain2 = /*gain * */(REAL)0x7fff;

			for(int i = 0, ch = 0; i < MaxLoop; i++) {
				__int64	s;

//				if(shaper) {
//					s = shaper->do_shaping(outbuf[i] * gain2, ch);
//				} else {
					s = RInt64(outbuf[i] * gain2);

					if(s > 0x7fffi64) {
						s =  0x7fffi64;
					} else if(s < -0x8000i64) {
						s = -0x8000i64;
					}
//				}

#ifndef BIGENDIAN
				((short*)rawoutbuf)[0] = static_cast<short>(s);
#else
				rawoutbuf[0] = s & 255;
				s >>= 8;
				rawoutbuf[1] = s & 255;
#endif
				__output(rawoutbuf, 2);

				ch = (ch + 1) % nch;
			}
			break;
		case 3:
			gain2 = /*gain * */(REAL)0x7fffff;

			for(int i = 0, ch = 0; i < MaxLoop; i++) {
				__int64	s;

//				if(shaper) {
//					s = shaper->do_shaping(outbuf[i] * gain2, ch);
//				} else {
					s = RInt64(outbuf[i] * gain2);

					if(s > 0x7fffffi64) {
						s =  0x7fffffi64;
					} else if(s < -0x800000i64) {
						s = -0x800000i64;
					}
//				}

				rawoutbuf[0] = static_cast<unsigned char>(s & 255);
				s >>= 8;
				rawoutbuf[1] = static_cast<unsigned char>(s & 255);
				s >>= 8;
				rawoutbuf[2] = static_cast<unsigned char>(s & 255);
				__output(rawoutbuf, 3);

				ch = (ch + 1) % nch;
			}
			break;
		case 4:
			gain2 = /*gain * */(REAL)0x7fffffff;

			for(int i = 0, ch = 0; i < MaxLoop; i++) {
				__int64	s;

//				if(shaper) {
//					s = shaper->do_shaping(outbuf[i] * gain2, ch);
//				} else {
					s = RInt64(outbuf[i] * gain2);

					if(s > 0x7fffffffi64) {
						s =  0x7fffffffi64;
					} else if(s < -0x80000000i64) {
						s = -0x80000000i64;
					}
//				}

#ifndef BIGENDIAN
				((int*)rawoutbuf)[0] = static_cast<int>(s);
#else
				rawoutbuf[0] = s & 255;
				s >>= 8;
				rawoutbuf[1] = s & 255;
				s >>= 8;
				rawoutbuf[2] = s & 255;
				s >>= 8;
				rawoutbuf[3] = s & 255;
#endif
				__output(rawoutbuf, 4);

				ch = (ch + 1) % nch;
			}
			break;
		}
		break;
	case DATA_FORMAT_IEEE_FLOAT:
		switch(dbps) {
		case 4:
			for(int i = 0; i < MaxLoop; i++) {
				((float*)rawoutbuf)[0] = static_cast<float>(outbuf[i]/* * gain*/);
				__output(rawoutbuf, 4);
			}
			break;
		case 8:
//			if(gain == 1.) {
				const int	CopySize = MaxLoop - delay;

				if(CopySize > 0) out.Write(outbuf + (MaxLoop - CopySize), CopySize * sizeof(double));

				delay = Max(delay - MaxLoop, 0);
//			} else {
//				for(int i = 0; i < MaxLoop; i++) {
//					((double*)rawoutbuf)[0] = outbuf[i] * gain;
//					__output(rawoutbuf, 8);
//				}
//			}
			break;
		}
		break;
	}
}

/*
#ifdef WIN32
extern "C" {_declspec(dllimport) int _stdcall MulDiv(int nNumber,int nNumerator,int nDenominator);}
#else
#define MulDiv(x,y,z) ((x)*(y)/(z))
#endif
*/

unsigned int __fastcall Resampler_base::GetLatency()
{
	return MulDiv(in.Size() / (bps * nch), 1000, sfrq) + MulDiv(out.Size() / (dbps * nch), 1000, dfrq);
}

class Upsampler : public Resampler_base
{
  int frqgcd,osf,fs1,fs2;
  REAL **stage1,*stage2;
  int n1,n1x,n1y,n2,n2b;
  int filter2len;
  int *f1order,*f1inc;
  int *fft_ip;// = NULL;
  REAL *fft_w;// = NULL;
  //unsigned char *rawinbuf,*rawoutbuf;
  REAL *inbuf,*outbuf;
  REAL **buf1,**buf2;
  int spcount;
  int i,j;

		int n2b2;//=n2b/2;
		int rp;        // inbufのfs1での次に読むサンプルの場所を保持
		int ds;        // 次にdisposeするsfrqでのサンプル数
		int nsmplwrt1; // 実際にファイルからinbufに読み込まれた値から計算した
					   // stage2 filterに渡されるサンプル数
		int nsmplwrt2; // 実際にファイルからinbufに読み込まれた値から計算した
					   // stage2 filterに渡されるサンプル数
		int s1p;       // stage1 filterから出力されたサンプルの数をn1y*osfで割った余り
		int osc;
		REAL *ip,*ip_backup;
		int s1p_backup,osc_backup;
		int p;
		int inbuflen;

public:
	Upsampler(CONFIG& c) : Resampler_base(c)
	{
		fft_ip = NULL;
		fft_w = NULL;
		spcount = 0;

		filter2len = FFTFIRLEN; /* stage 2 filter length */

	  /* Make stage 1 filter */

	  {
		double aa = AA; /* stop band attenuation(dB) */
		double lpf,delta,d,df,alp,iza;
		double guard = 2;

		frqgcd = gcd(sfrq,dfrq);

		fs1 = sfrq / frqgcd * dfrq;

		if (fs1/dfrq == 1) osf = 1;
		else if (fs1/dfrq % 2 == 0) osf = 2;
		else if (fs1/dfrq % 3 == 0) osf = 3;
		else {
//		  fprintf(stderr,"Resampling from %dHz to %dHz is not supported.\n",sfrq,dfrq);
//		  fprintf(stderr,"%d/gcd(%d,%d)=%d must be divided by 2 or 3.\n",sfrq,sfrq,dfrq,fs1/dfrq);
//		  exit(-1);
			return;
		}

		df = (dfrq*osf/2 - sfrq/2) * 2 / guard;
		lpf = sfrq/2 + (dfrq*osf/2 - sfrq/2)/guard;

		delta = pow(10.,-aa/20);
		if (aa <= 21) d = 0.9222; else d = (aa-7.95)/14.36;

		n1 = static_cast<int>(fs1/df*d+1);
		if (n1 % 2 == 0) n1++;

		alp = alpha(aa);
		iza = dbesi0(alp);
		//printf("iza = %g\n",iza);

		n1y = fs1/sfrq;
		n1x = n1/n1y+1;

		f1order = (int*)malloc(sizeof(int)*n1y*osf);
		for(i=0;i<n1y*osf;i++) {
		  f1order[i] = fs1/sfrq-(i*(fs1/(dfrq*osf)))%(fs1/sfrq);
		  if (f1order[i] == fs1/sfrq) f1order[i] = 0;
		}

		f1inc = (int*)malloc(sizeof(int)*n1y*osf);
		for(i=0;i<n1y*osf;i++) {
		  f1inc[i] = f1order[i] < fs1/(dfrq*osf) ? nch : 0;
		  if (f1order[i] == fs1/sfrq) f1order[i] = 0;
		}

		stage1 = (REAL**)malloc(sizeof(REAL *)*n1y);
		stage1[0] = (REAL*)malloc(sizeof(REAL)*n1x*n1y);

		for(i=1;i<n1y;i++) {
		  stage1[i] = &(stage1[0][n1x*i]);
		  for(j=0;j<n1x;j++) stage1[i][j] = 0;
		}

		for(i=-(n1/2);i<=n1/2;i++)
		  {
		stage1[(i+n1/2)%n1y][(i+n1/2)/n1y] = win(i,n1,alp,iza)*hn_lpf(i,lpf,fs1)*fs1/sfrq;
		  }
	  }

	  /* Make stage 2 filter */

	  {
		double aa = AA; /* stop band attenuation(dB) */
		double lpf,delta,d,df,alp,iza;
		int ipsize,wsize;

		delta = pow(10.,-aa/20);
		if (aa <= 21) d = 0.9222; else d = (aa-7.95)/14.36;

		fs2 = dfrq*osf;

		for(i=1;;i = i * 2)
		  {
		n2 = filter2len * i;
		if (n2 % 2 == 0) n2--;
		df = (fs2*d)/(n2-1);
//		lpf = sfrq/2;
		if (df < DF) break;
		  }

		lpf = sfrq/2;

		alp = alpha(aa);

		iza = dbesi0(alp);

		for(n2b=1;n2b<n2;n2b*=2);
		n2b *= 2;

		stage2 = (REAL*)malloc(sizeof(REAL)*n2b);

		for(i=0;i<n2b;i++) stage2[i] = 0;

		for(i=-(n2/2);i<=n2/2;i++) {
		  stage2[i+n2/2] = win(i,n2,alp,iza)*hn_lpf(i,lpf,fs2)/n2b*2;
		}

		ipsize    = static_cast<int>(2+sqrt((double)n2b));
		fft_ip    = (int*)malloc(sizeof(int)*ipsize);
		fft_ip[0] = 0;
		wsize     = n2b/2;
		fft_w     = (REAL*)malloc(sizeof(REAL)*wsize);

		rdft(n2b,1,stage2,fft_ip,fft_w);
	  }

	  n2b2=n2b/2;

		buf1 = (REAL**)malloc(sizeof(REAL *)*nch);
		for(i=0;i<nch;i++)
		  {
		buf1[i] = (REAL*)malloc(sizeof(REAL)*(n2b2/osf+1));
		for(j=0;j<(n2b2/osf+1);j++) buf1[i][j] = 0;
		  }

		buf2 = (REAL**)malloc(sizeof(REAL *)*nch);
		for(i=0;i<nch;i++) buf2[i] = (REAL*)malloc(sizeof(REAL)*n2b);

		//rawinbuf  = (unsigned char*)calloc(nch*(n2b2+n1x),bps);
		//rawoutbuf = (unsigned char*)malloc(dbps*nch*(n2b2/osf+1));

		inbuf  = (REAL*)calloc(nch*(n2b2+n1x),sizeof(REAL));
		outbuf = (REAL*)malloc(sizeof(REAL)*nch*(n2b2/osf+1));

		s1p = 0;
		rp  = 0;
		ds  = 0;
		osc = 0;

		inbuflen = n1/2/(fs1/sfrq)+1;
		delay = static_cast<int>((double)n2/2/((double)fs2/dfrq)) * nch;
	}

	~Upsampler(void)
	{
	  free(f1order);
	  free(f1inc);
	  free(stage1[0]);
	  free(stage1);
	  free(stage2);
	  free(fft_ip);
	  free(fft_w);
	  for(i=0;i<nch;i++) free(buf1[i]);
	  free(buf1);
	  for(i=0;i<nch;i++) free(buf2[i]);
	  free(buf2);
	  free(inbuf);
	  free(outbuf);
	  //free(rawoutbuf);
//	  if(shaper) delete shaper;
	}

	unsigned int __fastcall Resample(unsigned char * rawinbuf,unsigned int in_size,int ending)
	{
	  /* Apply filters */

		int nsmplread,toberead,toberead2;
		unsigned int rv=0;
		int ch;


		toberead2 = toberead = static_cast<int>(floor((double)n2b2*sfrq/(dfrq*osf))+1+n1x)-inbuflen;

		if (!ending)
		{
			rv=bps*nch*toberead;
			if (in_size<rv) return 0;
			nsmplread=toberead;
		}
		else
		{
			nsmplread=in_size/(bps*nch);
			rv=nsmplread*(bps*nch);
		}

		make_inbuf(nsmplread,inbuflen,rawinbuf,inbuf,toberead);

		inbuflen += toberead2;

		//nsmplwrt1 = ((rp-1)*sfrq/fs1+inbuflen-n1x)*dfrq*osf/sfrq;
		//if (nsmplwrt1 > n2b2) nsmplwrt1 = n2b2;
		nsmplwrt1 = n2b2;


		// apply stage 1 filter

		ip = &inbuf[((sfrq*(rp-1)+fs1)/fs1)*nch];

		s1p_backup = s1p;
		ip_backup  = ip;
		osc_backup = osc;

		for(ch=0;ch<nch;ch++)
		  {
			REAL *op = &outbuf[ch];
			int fdo = fs1/(dfrq*osf),no = n1y*osf;

			s1p = s1p_backup; ip = ip_backup+ch;

			switch(n1x) {
			case 2:
				for(p = 0; p < nsmplwrt1; p++) {
					const int	s1o = f1order[s1p];

					buf2[ch][p] =
								stage1[s1o][0] * *(ip + 0 * nch) +
								stage1[s1o][1] * *(ip + 1 * nch);

					ip += f1inc[s1p];
					s1p++;
					if(s1p == no) s1p = 0;
				}
				break;
			case 5:
				for(p = 0; p < nsmplwrt1; p++) {
					const int	s1o = f1order[s1p];

					buf2[ch][p] =
								stage1[s1o][0] * *(ip + 0 * nch) +
								stage1[s1o][1] * *(ip + 1 * nch) +
								stage1[s1o][2] * *(ip + 2 * nch) +
								stage1[s1o][3] * *(ip + 3 * nch) +
								stage1[s1o][4] * *(ip + 4 * nch);

					ip += f1inc[s1p];
					s1p++;
					if(s1p == no) s1p = 0;
				}
				break;
			case 8:
				for(p = 0; p < nsmplwrt1; p++) {
					const int	s1o = f1order[s1p];

					buf2[ch][p] =
								stage1[s1o][0] * *(ip + 0 * nch) +
								stage1[s1o][1] * *(ip + 1 * nch) +
								stage1[s1o][2] * *(ip + 2 * nch) +
								stage1[s1o][3] * *(ip + 3 * nch) +
								stage1[s1o][4] * *(ip + 4 * nch) +
								stage1[s1o][5] * *(ip + 5 * nch) +
								stage1[s1o][6] * *(ip + 6 * nch) +
								stage1[s1o][7] * *(ip + 7 * nch);

					ip += f1inc[s1p];
					s1p++;
					if(s1p == no) s1p = 0;
				}
				break;
			case 10:
				for(p = 0; p < nsmplwrt1; p++) {
					const int	s1o = f1order[s1p];

					buf2[ch][p] =
								stage1[s1o][0] * *(ip + 0 * nch) +
								stage1[s1o][1] * *(ip + 1 * nch) +
								stage1[s1o][2] * *(ip + 2 * nch) +
								stage1[s1o][3] * *(ip + 3 * nch) +
								stage1[s1o][4] * *(ip + 4 * nch) +
								stage1[s1o][5] * *(ip + 5 * nch) +
								stage1[s1o][6] * *(ip + 6 * nch) +
								stage1[s1o][7] * *(ip + 7 * nch) +
								stage1[s1o][8] * *(ip + 8 * nch) +
								stage1[s1o][9] * *(ip + 9 * nch);

					ip += f1inc[s1p];
					s1p++;
					if(s1p == no) s1p = 0;
				}
				break;
			case 24:
				for(p = 0; p < nsmplwrt1; p++) {
					const int	s1o = f1order[s1p];

					buf2[ch][p] =
								stage1[s1o][0] * *(ip + 0 * nch) +
								stage1[s1o][1] * *(ip + 1 * nch) +
								stage1[s1o][2] * *(ip + 2 * nch) +
								stage1[s1o][3] * *(ip + 3 * nch) +
								stage1[s1o][4] * *(ip + 4 * nch) +
								stage1[s1o][5] * *(ip + 5 * nch) +
								stage1[s1o][6] * *(ip + 6 * nch) +
								stage1[s1o][7] * *(ip + 7 * nch) +
								stage1[s1o][8] * *(ip + 8 * nch) +
								stage1[s1o][9] * *(ip + 9 * nch) +
								stage1[s1o][10] * *(ip + 10 * nch) +
								stage1[s1o][11] * *(ip + 11 * nch) +
								stage1[s1o][12] * *(ip + 12 * nch) +
								stage1[s1o][13] * *(ip + 13 * nch) +
								stage1[s1o][14] * *(ip + 14 * nch) +
								stage1[s1o][15] * *(ip + 15 * nch) +
								stage1[s1o][16] * *(ip + 16 * nch) +
								stage1[s1o][17] * *(ip + 17 * nch) +
								stage1[s1o][18] * *(ip + 18 * nch) +
								stage1[s1o][19] * *(ip + 19 * nch) +
								stage1[s1o][20] * *(ip + 20 * nch) +
								stage1[s1o][21] * *(ip + 21 * nch) +
								stage1[s1o][22] * *(ip + 22 * nch) +
								stage1[s1o][23] * *(ip + 23 * nch);

					ip += f1inc[s1p];
					s1p++;
					if(s1p == no) s1p = 0;
				}
				break;
			default:
				for(p = 0; p < nsmplwrt1; p++) {
					REAL	tmp = 0;
					REAL*	ip2 = ip;

					const int	s1o = f1order[s1p];

					for(i = 0; i < n1x; i++) {
						tmp += stage1[s1o][i] * *ip2;
						ip2 += nch;
					}

					buf2[ch][p] = tmp;
					ip += f1inc[s1p];
					s1p++;
					if(s1p == no) s1p = 0;
				}
				break;
			}

			osc = osc_backup;

			// apply stage 2 filter

			for(p=nsmplwrt1;p<n2b;p++) buf2[ch][p] = 0;

			//for(i=0;i<n2b2;i++) printf("%d:%g ",i,buf2[ch][i]);

			rdft(n2b,1,buf2[ch],fft_ip,fft_w);

			buf2[ch][0] = stage2[0]*buf2[ch][0];
			buf2[ch][1] = stage2[1]*buf2[ch][1]; 

			for(i=1;i<n2b/2;i++)
			  {
			REAL re,im;

			re = stage2[i*2  ]*buf2[ch][i*2] - stage2[i*2+1]*buf2[ch][i*2+1];
			im = stage2[i*2+1]*buf2[ch][i*2] + stage2[i*2  ]*buf2[ch][i*2+1];

			//printf("%d : %g %g %g %g %g %g\n",i,stage2[i*2],stage2[i*2+1],buf2[ch][i*2],buf2[ch][i*2+1],re,im);

			buf2[ch][i*2  ] = re;
			buf2[ch][i*2+1] = im;
			  }

			rdft(n2b,-1,buf2[ch],fft_ip,fft_w);

			for(i=osc,j=0;i<n2b2;i+=osf,j++) {
				op[j*nch] = buf1[ch][j] + buf2[ch][i];
			}

			nsmplwrt2 = j;

			osc = i - n2b2;

			for(j=0;i<n2b;i+=osf,j++)
			  buf1[ch][j] = buf2[ch][i];
		  }

		rp += nsmplwrt1 * (sfrq / frqgcd) / osf;

		make_outbuf(nsmplwrt2,outbuf);

		{
		  int ds = (rp-1)/(fs1/sfrq);

//		  assert(inbuflen >= ds);

		  memmove(inbuf,inbuf+nch*ds,sizeof(REAL)*nch*(inbuflen-ds));
		  inbuflen -= ds;
		  rp -= ds*(fs1/sfrq);
		}

		return rv;
	}
};

class Downsampler : public Resampler_base
{
private:
  int frqgcd,osf,fs1,fs2;
  REAL *stage1,**stage2;
  int n2,n2x,n2y,n1,n1b;
  int filter1len;
  int *f2order,*f2inc;
  int *fft_ip;// = NULL;
  REAL *fft_w;// = NULL;
  //unsigned char *rawinbuf,*rawoutbuf;
  REAL *inbuf,*outbuf;
  REAL **buf1,**buf2;
  int i,j;
  int spcount;// = 0;

    int n1b2;// = n1b/2;
    int rp;        // inbufのfs1での次に読むサンプルの場所を保持
    int rps;       // rpを(fs1/sfrq=osf)で割った余り
    int rp2;       // buf2のfs2での次に読むサンプルの場所を保持
    int ds;        // 次にdisposeするsfrqでのサンプル数
    int nsmplwrt2; // 実際にファイルからinbufに読み込まれた値から計算した
                   // stage2 filterに渡されるサンプル数
    int s2p;       // stage1 filterから出力されたサンプルの数をn1y*osfで割った余り
//    int init; //,ending;
    int osc;
    REAL *bp; // rp2から計算される．buf2の次に読むサンプルの位置
    int rps_backup,s2p_backup;
    int k,ch,p;
    int inbuflen;//=0;
    REAL *op;

public:
	Downsampler(CONFIG& c) : Resampler_base(c)
  {
		spcount=0;
		fft_ip=0;
		fft_w=0;

  filter1len = FFTFIRLEN; /* stage 1 filter length */

  /* Make stage 1 filter */

  {
    double aa = AA; /* stop band attenuation(dB) */
    double lpf,delta,d,df,alp,iza;
    int ipsize,wsize;

    frqgcd = gcd(sfrq,dfrq);

    if (dfrq/frqgcd == 1) osf = 1;
    else if (dfrq/frqgcd % 2 == 0) osf = 2;
    else if (dfrq/frqgcd % 3 == 0) osf = 3;
    else {
//      fprintf(stderr,"Resampling from %dHz to %dHz is not supported.\n",sfrq,dfrq);
//      fprintf(stderr,"%d/gcd(%d,%d)=%d must be divided by 2 or 3.\n",dfrq,sfrq,dfrq,dfrq/frqgcd);
//      exit(-1);
		return;
    }

    fs1 = sfrq*osf;

    delta = pow(10.,-aa/20);
    if (aa <= 21) d = 0.9222; else d = (aa-7.95)/14.36;

    n1 = filter1len;
    for(i=1;;i = i * 2)
      {
	n1 = filter1len * i;
	if (n1 % 2 == 0) n1--;
	df = (fs1*d)/(n1-1);
	lpf = (dfrq-df)/2;
	if (df < DF) break;
      }

    alp = alpha(aa);

    iza = dbesi0(alp);

    for(n1b=1;n1b<n1;n1b*=2);
    n1b *= 2;

    stage1 = (REAL*)malloc(sizeof(REAL)*n1b);

    for(i=0;i<n1b;i++) stage1[i] = 0;

    for(i=-(n1/2);i<=n1/2;i++) {
      stage1[i+n1/2] = win(i,n1,alp,iza)*hn_lpf(i,lpf,fs1)*fs1/sfrq/n1b*2;
    }

    ipsize    = static_cast<int>(2+sqrt((double)n1b));
    fft_ip    = (int*)malloc(sizeof(int)*ipsize);
    fft_ip[0] = 0;
    wsize     = n1b/2;
    fft_w     = (REAL*)malloc(sizeof(REAL)*wsize);

    rdft(n1b,1,stage1,fft_ip,fft_w);
  }

  /* Make stage 2 filter */

  if (osf == 1) {
    fs2 = sfrq/frqgcd*dfrq;
    n2 = 1;
    n2y = n2x = 1;
    f2order = (int*)malloc(sizeof(int)*n2y);
    f2order[0] = 0;
    f2inc = (int*)malloc(sizeof(int)*n2y);
    f2inc[0] = sfrq/dfrq;
    stage2 = (REAL**)malloc(sizeof(REAL *)*n2y);
    stage2[0] = (REAL*)malloc(sizeof(REAL)*n2x*n2y);
    stage2[0][0] = 1;
  } else {
    double aa = AA; /* stop band attenuation(dB) */
    double lpf,delta,d,df,alp,iza;
    double guard = 2;

    fs2 = sfrq / frqgcd * dfrq ;

    df = (fs1/2 - sfrq/2) * 2 / guard;
    lpf = sfrq/2 + (fs1/2 - sfrq/2)/guard;

    delta = pow(10.,-aa/20);
    if (aa <= 21) d = 0.9222; else d = (aa-7.95)/14.36;

    n2 = static_cast<int>(fs2/df*d+1);
    if (n2 % 2 == 0) n2++;

    alp = alpha(aa);
    iza = dbesi0(alp);

    n2y = fs2/fs1; // 0でないサンプルがfs2で何サンプルおきにあるか？
    n2x = n2/n2y+1;

    f2order = (int*)malloc(sizeof(int)*n2y);
    for(i=0;i<n2y;i++) {
      f2order[i] = fs2/fs1-(i*(fs2/dfrq))%(fs2/fs1);
      if (f2order[i] == fs2/fs1) f2order[i] = 0;
    }

    f2inc = (int*)malloc(sizeof(int)*n2y);
    for(i=0;i<n2y;i++) {
      f2inc[i] = (fs2/dfrq-f2order[i])/(fs2/fs1)+1;
      if (f2order[i+1==n2y ? 0 : i+1] == 0) f2inc[i]--;
    }

    stage2 = (REAL**)malloc(sizeof(REAL *)*n2y);
    stage2[0] = (REAL*)malloc(sizeof(REAL)*n2x*n2y);

    for(i=1;i<n2y;i++) {
      stage2[i] = &(stage2[0][n2x*i]);
      for(j=0;j<n2x;j++) stage2[i][j] = 0;
    }

    for(i=-(n2/2);i<=n2/2;i++)
      {
	stage2[(i+n2/2)%n2y][(i+n2/2)/n2y] = win(i,n2,alp,iza)*hn_lpf(i,lpf,fs2)*fs2/fs1;
      }
  }

  /* Apply filters */

    n1b2 = n1b/2;
	inbuflen=0;
//    delay = 0;

    //    |....B....|....C....|   buf1      n1b2+n1b2
    //|.A.|....D....|             buf2  n2x+n1b2
    //
    // まずinbufからBにosf倍サンプリングしながらコピー
    // Cはクリア
    // BCにstage 1 filterをかける
    // DにBを足す
    // ADにstage 2 filterをかける
    // Dの後ろをAに移動
    // CをDにコピー

    buf1 = (REAL**)malloc(sizeof(REAL *)*nch);
    for(i=0;i<nch;i++)
      buf1[i] = (REAL*)malloc(n1b*sizeof(REAL));

    buf2 = (REAL**)malloc(sizeof(REAL *)*nch);
    for(i=0;i<nch;i++) {
      buf2[i] = (REAL*)malloc(sizeof(REAL)*(n2x+1+n1b2));
      for(j=0;j<n2x+1+n1b2;j++) buf2[i][j] = 0;
    }

    //rawoutbuf = (unsigned char*)malloc(dbps*nch*((double)n1b2*sfrq/dfrq+1));
    inbuf = (REAL*)calloc(nch*(n1b2/osf+osf+1),sizeof(REAL));
    outbuf = (REAL*)malloc(static_cast<size_t>(sizeof(REAL)*nch*((double)n1b2*sfrq/dfrq+1)));

    op = outbuf;

    s2p = 0;
    rp  = 0;
    rps = 0;
    ds  = 0;
    osc = 0;
    rp2 = 0;

    delay = static_cast<int>((double)n1/2/((double)fs1/dfrq)+(double)n2/2/((double)fs2/dfrq)) * nch;
  };

  ~Downsampler(void)
  {
	free(stage1);
	free(fft_ip);
	free(fft_w);
	free(f2order);
	free(f2inc);
	free(stage2[0]);
	free(stage2);
	for(i=0;i<nch;i++) free(buf1[i]);
	free(buf1);
	for(i=0;i<nch;i++) free(buf2[i]);
	free(buf2);
	free(inbuf);
	free(outbuf);
	//free(rawoutbuf);
//	if(shaper) delete shaper;
  }

	unsigned int __fastcall Resample(unsigned char * rawinbuf,unsigned int in_size,int ending)
	{
		unsigned int rv;
	int nsmplread;
	int toberead;

	toberead = (n1b2-rps-1)/osf+1;

	if (!ending)
	{
		rv=bps*nch*toberead;
		if (in_size<rv) return 0;
		nsmplread=toberead;
	}
	else
	{
		nsmplread=in_size/(bps*nch);
		rv=nsmplread*(bps*nch);
	}

	make_inbuf(nsmplread,inbuflen,rawinbuf,inbuf,toberead);

	rps_backup = rps;
	s2p_backup = s2p;

	for(ch=0;ch<nch;ch++)
	  {
	    rps = rps_backup;

	    for(k=0;k<rps;k++) buf1[ch][k] = 0;

	    for(i=rps,j=0;i<n1b2;i+=osf,j++)
	      {
//		assert(j < ((n1b2-rps-1)/osf+1));

		buf1[ch][i] = inbuf[j*nch+ch];

		for(k=i+1;k<i+osf;k++) buf1[ch][k] = 0;
	      }

//	    assert(j == ((n1b2-rps-1)/osf+1));

	    for(k=n1b2;k<n1b;k++) buf1[ch][k] = 0;

	    rps = i - n1b2;
	    rp += j;

	    rdft(n1b,1,buf1[ch],fft_ip,fft_w);

	    buf1[ch][0] = stage1[0]*buf1[ch][0];
	    buf1[ch][1] = stage1[1]*buf1[ch][1]; 

	    for(i=1;i<n1b2;i++)
	      {
		REAL re,im;

		re = stage1[i*2  ]*buf1[ch][i*2] - stage1[i*2+1]*buf1[ch][i*2+1];
		im = stage1[i*2+1]*buf1[ch][i*2] + stage1[i*2  ]*buf1[ch][i*2+1];

		buf1[ch][i*2  ] = re;
		buf1[ch][i*2+1] = im;
	      }

	    rdft(n1b,-1,buf1[ch],fft_ip,fft_w);

	    for(i=0;i<n1b2;i++) {
	      buf2[ch][n2x+1+i] += buf1[ch][i];
	    }

	    {
	      int t1 = rp2/(fs2/fs1);
	      if (rp2%(fs2/fs1) != 0) t1++;

	      bp = &(buf2[ch][t1]);
	    }

	    s2p = s2p_backup;

	    for(p=0;bp-buf2[ch]<n1b2+1;p++)
	      {
		REAL tmp = 0;
		REAL *bp2;
		int s2o;

		bp2 = bp;
		s2o = f2order[s2p];
		bp += f2inc[s2p];
		s2p++;

		if (s2p == n2y) s2p = 0;

//		assert((bp2-&(buf2[ch][0]))*(fs2/fs1)-(rp2+p*(fs2/dfrq)) == s2o);

		for(i=0;i<n2x;i++)
		  tmp += stage2[s2o][i] * *bp2++;

		op[p*nch+ch] = tmp;
	      }

	    nsmplwrt2 = p;
	  }

	rp2 += nsmplwrt2 * (fs2 / dfrq);

	make_outbuf(nsmplwrt2,outbuf);

	{
	  int ds = (rp2-1)/(fs2/fs1);

	  if (ds > n1b2) ds = n1b2;

	  for(ch=0;ch<nch;ch++)
	    memmove(buf2[ch],buf2[ch]+ds,sizeof(REAL)*(n2x+1+n1b2-ds));

	  rp2 -= ds*(fs2/fs1);
	}

	for(ch=0;ch<nch;ch++)
	  memcpy(buf2[ch]+n2x+1,buf1[ch]+n1b2,sizeof(REAL)*n1b2);

		return rv;
	}
};

/*
class Noresampler : public Resampler_base
{
public:
	Noresampler(CONFIG & c) : Resampler_base(c)
	{
		delay = 0;
	}
	virtual unsigned int Resample(unsigned char * _buf,unsigned int size,int ending)
	{
		int ch=0;
		int os=size;

		while(size>=(unsigned)bps)
		{
			unsigned char buf[4];
			memcpy(buf,_buf,4);
		  REAL f;
		  __int64 s;

		  switch(bps) {
		  case 1:
		f = (1 / (REAL)0x7f) * ((REAL)((unsigned char *)buf)[0]-128);
		break;
		  case 2:
	#ifndef BIGENDIAN
		f = (1 / (REAL)0x7fff) * ((REAL)((short *)buf)[0]);
	#else
		f = (1 / (REAL)0x7fff) * (((int)buf[0]) | (((int)(((char *)buf)[1])) << 8));
	#endif
		break;
		  case 3:
		f = (1 / (REAL)0x7fffff) * 
		  ((((int)buf[0]) << 0 ) |
		   (((int)buf[1]) << 8 ) |
		   (((int)((char *)buf)[2]) << 16));
		break;
		  case 4:
		f = (1 / (REAL)0x7fffffff) * 
		  ((((int)buf[0]) << 0 ) |
		   (((int)buf[1]) << 8 ) |
		   (((int)buf[2]) << 16) |
		   (((int)((char *)buf)[3]) << 24));
		break;
		  };

		  f *= gain;

		switch(dbps) {
		case 1:
		  f *= 0x7f;
		  s = shaper ? shaper->do_shaping(f,ch) : RINT(f);
		  buf[0] = static_cast<unsigned char>(s + 128);
		  __output(buf, 1);
		  break;
		case 2:
		  f *= 0x7fff;
		  s = shaper ? shaper->do_shaping(f,ch) : RINT(f);
		  buf[0] = static_cast<unsigned char>(s & 255);
		  s >>= 8;
		  buf[1] = static_cast<unsigned char>(s & 255);
		  __output(buf, 2);
		  break;
		case 3:
		  f *= 0x7fffff;
		  s = shaper ? shaper->do_shaping(f,ch) : RINT(f);
		  buf[0] = static_cast<unsigned char>(s & 255);
		  s >>= 8;
		  buf[1] = static_cast<unsigned char>(s & 255);
		  s >>= 8;
		  buf[2] = static_cast<unsigned char>(s & 255);
		  __output(buf, 3);
		  break;
		case 4:
		  f *= 0x7fffffff;
		  s = shaper ? shaper->do_shaping(f,ch) : RINT(f);
		  buf[0] = static_cast<unsigned char>(s & 255);
		  s >>= 8;
		  buf[1] = static_cast<unsigned char>(s & 255);
		  s >>= 8;
		  buf[2] = static_cast<unsigned char>(s & 255);
		  s >>= 8;
		  buf[3] = static_cast<unsigned char>(s & 255);
		  __output(buf, 4);
		  break;
		}
		ch=(ch+1)%nch;
		_buf+=bps;
		size-=bps;
		}
		return os-size;
	}
};
*/

Resampler_base::Resampler_base(CONFIG & c)
{
	switch(c.quality) {
	case 0:
		AA = 96;
		DF = 8000;
		FFTFIRLEN = 1024;
		break;
	case 1:
		AA = 120;
		DF = 100;
		FFTFIRLEN = 16384;
		break;
	case 2:
		AA = 170;
		DF = 100;
		FFTFIRLEN = 16384;
		break;
	case 3:
		AA = 170;
		DF = 100;
		FFTFIRLEN = 65536;
		break;
	case 4:
		AA = 200;
		DF = 50;
		FFTFIRLEN = 262144;
		break;
	}

//	shaper = NULL;

	nch = c.nch;
	sfrq = c.sfrq;
	dfrq = c.dfrq;
	sformat = c.sformat;
	dformat = c.dformat;
	bps = c.bps >> 3;
	dbps = c.dbps >> 3;

	//double att=0;
//	gain = 1.; //pow(10, -att / 20);

/*
	if((dformat == DATA_FORMAT_LINEAR_PCM) && c.dither) {
		const double	noiseamp = 0.18;
		__int64	min;
		__int64	max;

		if(dbps == 1) {min = -0x80i64; max = 0x7fi64;}
		if(dbps == 2) {min = -0x8000i64; max = 0x7fffi64;}
		if(dbps == 3) {min = -0x800000i64; max = 0x7fffffi64;}
		if(dbps == 4) {min = -0x80000000i64; max = 0x7fffffffi64;}

		shaper = new Shaper(dfrq, nch, min, max, c.dither, c.pdf, noiseamp);
	}
*/
}

Resampler_base* __fastcall Resampler_base::Create(CONFIG& c)
{
	if(!CanResample(c.sfrq,c.dfrq)) return 0;

	if(c.sfrq < c.dfrq) {
		return new Upsampler(c);
	} else if(c.sfrq > c.dfrq) {
		return new Downsampler(c);
/*
	} else if(c.dformat == DATA_FORMAT_LINEAR_PCM) {
		return new Noresampler(c);
*/
	}

	return 0;
}

