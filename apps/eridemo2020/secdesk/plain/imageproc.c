#include "imageproc.h"

int start_imageprocessor(void) {
  return 0;
}

int stop_imageprocessor(void) {
  return 0;
}

int start_recognizer(void) {
  return 0;
}

int stop_recognizer(void) {
  return 0;
}

int get_features(char *imagefile, double embedding[static 128]) {
  memset(embedding,0,128*sizeof(double));
  return 0;
}

int recognize(double embedding[static 128]) {
  double d = 0;
  for (int i=0; i<128; i++) d += embedding[i];
  return 666;
  /* return -1; */
}

