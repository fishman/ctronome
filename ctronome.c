#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ctronome.h"

dword bytes_read;

int dsp_device;

dword dsp_pattern_length;

/* we don't want to have slower than 30bpm for a metronome do we */
byte wav1[176400];
byte wav2[176400];

const char *wave1, *wave2;

FILE *wavfile;

dword c1,c2,c3,c4;

int main(int argc,char *argv[]){
 parm_init(argc,argv);


 /* lets calculate the appropriate pattern length for our bpm */
 dsp_pattern_length = dsp_rate * dsp_depth * (dsp_channels + 1) * 60 / bpm;

 c4 = 5;
 while (c4 >= 4){
  dsp_write(dsp_device,wav1,dsp_pattern_length);
  for (c3 = bpt;c3 > 1; c3--){
   dsp_write(dsp_device,wav2,dsp_pattern_length);
  }
 }
 
 dsp_close;
 return(0);
}

void parm_init(int argc,char *argv[]){
 /* first, get the parameters */
 int i;
 dword bytes_read;

 for (i = 1; i < argc; i++){
  if ((strcmp(argv[i], "-h") == 0) ||
      (strcmp(argv[i], "--h") == 0) ||
      (strcmp(argv[i], "-help") == 0) ||
      (strcmp(argv[i], "--help") == 0)) {
   printf(HELP);
   exit(0);
  }

  if ((strcmp(argv[i], "-v") == 0) ||
      (strcmp(argv[i], "--v") == 0) ||
      (strcmp(argv[i], "-version") == 0) ||
      (strcmp(argv[i], "--version") == 0)) {
   printf("%s v%s\n%s",MYNAME,VERSION,CREDITS);
   exit(0);
  }

  if (strcmp(argv[i], "-w1") == 0)
   metronomewav1 = argv[++i];

  if (strcmp(argv[i], "-w2") == 0)
   metronomewav2 = argv[++i];

  if (strcmp(argv[i], "-d") == 0)
   dspdev = argv[++i];

  if (strcmp(argv[i], "-b") == 0){
   bpm = atoi(argv[++i]);
   if (bpm > 250) bpm = 250;
   if (bpm < 30) bpm = 30;
  }

  if (strcmp(argv[i], "-t") == 0){
   bpt = atoi(argv[++i]);
   if (bpt > 50) bpt = 50;
   if (bpt < 1) bpt = 1;
  }
 }

 dsp_device = dsp_init(dspdev);

 /* cleanup buffers */
 for (c1 = 0; c1 < 176400; c1++){
  wav1[c1] = 0;
  wav2[c1] = 0;
 }

 /* open wav file 1 */
 if (!(wavfile = fopen(metronomewav1,"rb"))){
  /* Fatal error */
  printf("cannot open %s\n",metronomewav1);
  exit(1);
 }

 /* skip the wav header */
 fseek(wavfile,44,SEEK_SET);

 bytes_read = fread(&wav1,1,22050,wavfile);
 if (bytes_read < 10){
  /* Fatal error */
  printf("wav file %s too short\n",metronomewav1);
  exit(1);
 }

 fclose(wavfile);

 /* open wav file 2 */
 if (!(wavfile = fopen(metronomewav2,"rb"))){
  /* Fatal error */
  printf("cannot open %s\n",metronomewav2);
  exit(1);
 }

 /* skip the wav header */
 fseek(wavfile,44,SEEK_SET);

 bytes_read = fread(&wav2,1,22050,wavfile);
 if (bytes_read < 10){
  /* Fatal error */
  printf("wav file %s too short\n",metronomewav2);
  exit(1);
 }

 fclose(wavfile);
}

