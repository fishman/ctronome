#include <linux/soundcard.h>
#define MYNAME "ctronome"
#define VERSION "0.1"
#define CREDITS "homepage: http://ctronome.kign.org/\n"

/* set up these three variables to your system */
static char *metronomewav1 = "/usr/share/ctronome/metronome1.wav\0";
static char *metronomewav2 = "/usr/share/ctronome/metronome2.wav\0";
static char *dspdev = "/dev/dsp\0";

#define HELP "usage: ctronome <parameters>\n\
               valid parameters are:\n\
                -b <bpm>            beat per minute\n\
                -t <bpt>            beat per tact\n\
                -w1 <filename>      wav to use for first beat of tact\n\
                -w2 <filename>      wav to use for other beat of tact\n\
                -d <device>         dsp device\n\
                -h                  display this help screen\n\
                -v                  print version\n\
           e.g: ctronome -b 60 -t 4\n\
           for defaults/limits and required WAV format see README\n"

/* my lazy type definitions */
typedef unsigned long int DWORD;
typedef unsigned short int WORD;
typedef unsigned char BYTE;
typedef DWORD dword;
typedef WORD word;
typedef BYTE byte;

static int dsp_rate = 44100; /* samples per second, Hz */
static int dsp_channels = 0; /* 0 = mono, 1 = stereo */
static int dsp_depth = 2; /* bytes per sample */
static int dsp_format = AFMT_S16_LE; /* signed 16 bit little endianess*/

static int bpm = 60;
static int bpt = 1;

int dsp_init(byte *);
void parm_init(int, char *[]);
void dsp_close(byte);
void dsp_write(byte, byte *, dword);

