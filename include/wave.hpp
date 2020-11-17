#ifndef WAVE_HPP
#define WAVE_HPP
#include <random>
#include <cstdlib>
#include <vector>
#include <iostream>
#include "pattern.hpp"

using namespace std;

class wave_func {
  public:
  wave_func(int w, int h, int n, const uint8_t *input, int win, int hin, bool bounds); 
  void generateOutput();
  int colapse();
  void propogate(int x, int y, int direction = 0);
  uint8_t *getOutput();
  ~wave_func(){
    for(int i = 0; i < width; i++){
      for(int j = 0; j < height; j++){
        delete [] wave[i][j];
      }
      delete [] wave[i];
    }
    delete [] wave;
    delete [] patterns;
  }

  private:
  bool ***wave;
  uint8_t *out;
  pattern *patterns;
  int patternCount, n;
  int width, height, waveWidth, waveHeight;
  int waveElements;
  float noiseDistro;
  default_random_engine generator;
};


#endif