#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/soundcard.h>
#include "ctronome.h"

int dsp_init(byte *dsp_dev){
 int dsp_handle;
 /* Initialise dsp_dev */
 if ((dsp_handle = open (dsp_dev, O_WRONLY)) == -1){
  printf ("FATAL: cannot open %s\n",dsp_dev);
  exit (1);
 }
 
 /* Set up Output format */
 if (dsp_depth == 1)
  dsp_format == AFMT_S8;
 if ((ioctl (dsp_handle, SNDCTL_DSP_SETFMT, &dsp_format)) == -1){
  printf ("FATAL: unable to set output format for %s\n",dsp_dev);
  exit(1);  
 }        

 /* Set dsp to Mono */
 ioctl (dsp_handle, SNDCTL_DSP_STEREO, &dsp_channels);

 /* Set the DSP rate (in Hz) */
 ioctl (dsp_handle, SNDCTL_DSP_SPEED, &dsp_rate);

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

