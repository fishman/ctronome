#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ctronome.h"
#include "routines.h"

dword bytes_read;

int dsp_device;

dword dsp_pattern_length;

/* we don't want to have slower than 30bpm for a metronome do we */
byte wav1[176400];
byte wav2[176400];


const char *wave1, *wave2;

FILE *wavfile, *program;

byte is_program;
dword bpm_base_length;
dword c1,c2,c3,c4,lo;
int i1,i2,i3;
byte tmp[8219];

int main(int argc,char *argv[]){
 parm_init(argc,argv);

 /* create an endless/pcount times loop */
 while(pcount > 0){
  if (is_program){
   next_program(program);
  }

  if (pcount > 0){
   if (is_program) printf("count: %d, bpm: %d/%d, bpt: %d/%d\n",count,bpm[0],bpm[1],bpt[0],bpt[1]);

   /* lets calculate the appropriate pattern length for our bpm and bpt */
   bpm_base_length = dsp_speed * dsp_depth * dsp_channels * 60 / bpm[0];
   dsp_pattern_length = bpm_base_length * bpm[1] / bpt[1];

   while(i3 = dsp_pattern_length % (dsp_depth * dsp_channels + 1)){
    dsp_pattern_length++;
   }
  
   for (c4 = 0;c4 < count;c4++){
    dsp_write(dsp_device,wav1,dsp_pattern_length);
    for (c3 = bpt[0];c3 > 1; c3--){
     dsp_write(dsp_device,wav2,dsp_pattern_length);
    }
   }
   if (!(is_program)) pcount -= pdecrease;
  }
 }
 dsp_close;
 return(0);
}

void next_program(FILE *programfile){
 byte temp[8192];
 dword lo1;
 int lo2,lo3,lo4;
 /* a normal line should be at least 5 characters long + newline */
 lo2 = 0;
 while (lo2 >= 0){
  while ( (lo1 = getnextline(temp,programfile,8192)) < 6){
   /* if 0 character has been read, seek to the beginning of the file */
   if (lo1 < 1){
    fseek(programfile,SEEK_SET,0);
    pcount -= pdecrease;
    if (pcount) printf("repeat\n");
   }
  }
  lo2 = str_search(temp,hashmark);
 }
 count = atoi(temp);

 lo2 = str_search(temp,space);
 lo2++;
 lo2 += str_search(&temp[lo2],space);

 if (lo2) {
  lo3 = str_search(&temp[++lo2],slash);
  bpt[0] = atoi(&temp[lo2]);

  if (lo3) bpt[1] = atoi(&temp[lo2] + ++lo3);

  if (bpt[0] < 1) bpt[0] = 1;
  if (bpt[0] > 50) bpt[0] = 50;
  if (bpt[1] < 1) bpt[1] = 1;
  if (bpt[1] > 50) bpt[1] = 50;
  bpm[1] = bpt[1];
 }

 lo2 = str_search(temp,space);
 if (lo2) {
  lo3 = str_search(&temp[++lo2],slash);
  bpm[0] = atoi(&temp[lo2]);
  if (lo3) bpm[1] = atoi(&temp[lo2] + ++lo3);
  if (bpm[0] > 250) bpm[0] = 250;
  if (bpm[0] < 30) bpm[0] = 30;
  if (bpm[1] > 50) bpm[1] = 50;
  if (bpm[1] < 1) bpm[1] = 1;
 }

}


void parm_init(int argc,char *argv[]){

 /* first, get the parameters */
 int i;
 dword bytes_read;

 debug = 0;
 for (i = 1; i < argc; i++){
  /* debug */
  if ((strcmp(argv[i], "-debug") == 0) ||
      (strcmp(argv[i], "--debug") == 0)) {
   printf("debug mode\n");
   debug = 1;
  }
 }
 
 for (i = 1; i < argc; i++){
  /* help */
  if ((strcmp(argv[i], "-h") == 0) ||
      (strcmp(argv[i], "--h") == 0) ||
      (strcmp(argv[i], "-help") == 0) ||
      (strcmp(argv[i], "--help") == 0)) {
   printf(HELP);
   exit(0);
  }

  /* version and credits */
  if ((strcmp(argv[i], "-v") == 0) ||
      (strcmp(argv[i], "--v") == 0) ||
      (strcmp(argv[i], "-version") == 0) ||
      (strcmp(argv[i], "--version") == 0)) {
   printf("%s v%s\n%s",MYNAME,VERSION,CREDITS);
  }

  /* wav1 */
  if ((strcmp(argv[i], "-w1") == 0) && (i + 1 < argc)){
   metronomewav1 = argv[++i];
   if (debug) printf("debug: wav1: '%s'\n",metronomewav1);
  }

  /* wav2 */
  if ((strcmp(argv[i], "-w2") == 0) && (i + 1 < argc)){
   metronomewav2 = argv[++i];
   if (debug) printf("debug: wav2: '%s'\n",metronomewav2);
  }

  /* dsp device */
  if ((strcmp(argv[i], "-d") == 0) && (i + 1 < argc)){
   dspdev = argv[++i];
   if (debug) printf("debug: dsp: '%s'\n",dspdev);
  }

  /* bpt */
  if ( (strcmp(argv[i], "-t") == 0) && (i + 1 < argc)){
   i1 = str_search(argv[++i],slash);
   bpt[0] = atoi(argv[i]);
   if (i1 >= 0) bpt[1] = atoi(argv[i] + ++i1);
   if (bpt[0] < 1) bpt[0] = 1;
   if (bpt[0] > 50) bpt[0] = 50;
   if (bpt[1] < 1) bpt[1] = 1;
   if (bpt[1] > 50) bpt[1] = 50;
   bpm[1] = bpt[1];
   if (debug) printf("debug: bpt: '%s'\n",bpt);
  }

  /* bpm */
  if ((strcmp(argv[i], "-b") == 0) && (i +1 < argc)){
   i1 = str_search(argv[++i],slash);
   bpm[0] = atoi(argv[i]);
   if (c1) bpm[1] = atoi(argv[i] + ++i1);
   if (bpm[0] > 250) bpm[0] = 250;
   if (bpm[0] < 30) bpm[0] = 30;
   if (bpm[1] > 50) bpm[1] = 50;
   if (bpm[1] < 1) bpm[1] = 1;
   if (debug) printf("debug: bpm: '%s'\n",bpm);
  }

  /* pcount */
  if ((strcmp(argv[i], "-c") == 0) && (i + 1 < argc)){
   pcount = atoi(argv[++i]);
   pdecrease = 1;
   if (debug) printf("debug: count: '%s'\n",pcount);
  }

  /* program file */
  if ((strcmp(argv[i], "-p") == 0) && (i + 1 < argc)){
   programfile = argv[++i];
   is_program = 1;
   if (debug) printf("debug: program: '%s'\n",programfile);
  }
 }

 if (debug) printf("debug: calling dsp_init(%s)\n",dspdev);
 dsp_device = dsp_init(dspdev);

 /* cleanup buffers */
 for (c1 = 0; c1 < 176400; c1++){
  wav1[c1] = 0;
  wav2[c1] = 0;
 }

 /* open wav file 1 */
 wavfile = openread(metronomewav1);

 /* skip the wav header */
 fseek(wavfile,44,SEEK_SET);

 bytes_read = fread(&wav1,1,22050,wavfile);
 if (bytes_read < 10){
  printf("wav file %s too short\n",metronomewav1);
  exit(1);
 }

 fclose(wavfile);

 /* open wav file 2 */
 wavfile = openread(metronomewav2);

 /* skip the wav header */
 fseek(wavfile,44,SEEK_SET);

 bytes_read = fread(&wav2,1,22050,wavfile);
 if (bytes_read < 10){
  printf("wav file %s too short\n",metronomewav2);
  exit(1);
 }
 fclose(wavfile);

 /* open wav file 2 */
 if (is_program) program = openread(programfile);

 printf ("bpm: %d/%d, bpt: %d/%d",bpm[0],bpm[1],bpt[0],bpt[1]);
 if (pdecrease) printf ("repeat count: %d",pcount);
 if (is_program) printf ("\nprogram file: %s",programfile);
 printf ("\n");
}

