#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/soundcard.h>
#include "ctronome.h"

static int debug_dsp_speed, debug_dsp_channels, debug_dsp_depth, supported_dsp_formats;

int dsp_init(byte *dsp_dev){
 int dsp_handle;
 /* Initialise dsp_dev */
 if (debug) printf ("debug: opening dsp '%s'\n",dsp_dev);
 if ((dsp_handle = open (dsp_dev, O_WRONLY)) == -1){
  printf ("FATAL: cannot open dsp device %s\n",dsp_dev);
  perror(dsp_dev);
  exit (1);
 }
 if (debug) printf ("debug: dsp opened successfully\n",dsp_dev);

 /* Set up Output format */
 if (debug) printf ("debug: dsp_depth: '%d'\n",dsp_depth);
 if (dsp_depth == 1)
  dsp_format == AFMT_S8;

 if (debug){
  if (ioctl(dsp_handle, SNDCTL_DSP_GETFMTS, &supported_dsp_formats) == -1){
   printf ("FATAL: unable to get supported formats for %s\n",dsp_dev);
   perror("SNDCTL_DSP_GETFMT");
   exit(1);  
  }
  printf("debug: supported dsp formats: '%d'\n",supported_dsp_formats);
 }

 if (debug) printf ("debug: dsp: set format: SNDCTL_DSP_SETFMT(%d)\n",dsp_format);
 if ((ioctl (dsp_handle, SNDCTL_DSP_SETFMT, &dsp_format)) == -1){
  printf ("FATAL: unable to set output format for %s\n",dsp_dev);
  perror("SNDCTL_DSP_SETFMT");
  exit(1);  
 }

 if (debug) printf ("debug: dsp: set format returned '%d'\n",dsp_format);
 if (dsp_format != AFMT_S16_LE){
  printf("FATAL: your dsp device does not support signed 16 bit little endian (AFMT_S16_LE) format\n");
  exit(1);
 }

 /* Set dsp channels */
 if (debug) printf ("debug: dsp: set up channels: SNDCTL_DSP_CHANNELS(%d)\n",dsp_channels);
 /* ioctl (dsp_handle, SNDCTL_DSP_CHANNELS, &dsp_channels); */
 if (ioctl (dsp_handle, SNDCTL_DSP_CHANNELS, &dsp_channels) == -1){
  printf ("FATAL: unable to set no. of channels for %s\n",dsp_dev);
  perror("SNDCTL_DSP_CHANNELS");
  exit(1);
 }

 if (debug) printf ("debug: dsp: set no. of channels returned '%d'\n",dsp_channels);
 if (dsp_channels != 1){
  printf ("FATAL: strange... seems like your dps doesnt support mono mode.\n");
  exit(1);
 }

 /* Set the DSP rate (in Hz) */
 if (debug) printf ("debug: dsp: set up speed (Hz): SNDCTL_DSP_SPEED(%d)\n",dsp_speed);
 if (ioctl (dsp_handle, SNDCTL_DSP_SPEED, &dsp_speed) == -1){
  printf ("FATAL: unable to set DSP speed for %s\n",dsp_dev);
  perror("SNDCTL_DSP_SPEED");
  exit(1);
 }

 if (debug) printf ("debug: dsp: set speed returned '%d'\n",dsp_speed);
 if (dsp_speed != 44100){
  printf ("FATAL: your dsp doesnt support 44100Hz.\n");
  exit(1);
 }

 return(dsp_handle);
}

void dsp_close (byte dsp_handle)
{
 close(dsp_handle);
}

void dsp_write (byte dsp_handle,byte *from,dword count){
 dword bytes_written;
 bytes_written = write (dsp_handle,from,count);
 if (bytes_written < count){
  printf ("FATAL: tried to write %d bytes to dsp device,\n\
only %d is written\n",count,bytes_written);
  exit(1);  
 }
 return;
}

