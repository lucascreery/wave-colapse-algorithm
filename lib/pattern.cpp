#include "pattern.hpp"

using namespace std;

pattern::pattern(){}

pattern::pattern(int n, int *pixels, int intotal, int outtotal) 
    : n(n), targetFrequency(1.0/inTotal), inTotal(intotal), outTotal(outtotal), boundary(0), frequency(0.0f) {
  pixel_data = new int[n*n*4];
  for(int i = 0; i < n*n*4; i++){
    pixel_data[i] = pixels[i];
  }
}

bool pattern::isPossible(int *pixels, int dx, int dy) {
  // determing if a pattern in possible from a set of pixels
  for(int i = 0; i < n*n*4; i++){
    int color = i % 4;
    int pixel = i / 4;
    int x = (i / 4) % n;
    int y = (i / 4) / n;
    x = x + dx;
    y = y + dy;
    if(x >= 0 && x < n && y >= 0 && y < n){
      if(pixel_data[4*n*y + 4*x + color] != pixels[i]){
        return false;
      }
    }
  }
  return true;
}

float pattern::getProbability() {
  float probability = (targetFrequency * outTotal) / (available);
  return probability;
}

int pattern::getPixel(int y, int x, int rgb){
  return pixel_data[y*n*4+x*4+rgb];
}

int *pattern::getPixelData(){
  return pixel_data;
}

bool pattern::isEquivalent(int *a){
  for(int i = 0; i < n*n*4; i++){
    if(a[i] != pixel_data[i]){
      return false;
    }
  }
  return true;
}

void pattern::addTargetFrequency(){
  targetFrequency = targetFrequency * inTotal;
  targetFrequency++;
  targetFrequency = targetFrequency / float(inTotal);
}

void pattern::addFrequency(){
  frequency = frequency * outTotal;
  frequency++;
  frequency = frequency / float(outTotal);
}

float pattern::getTargetFreq(){
  return (targetFrequency - frequency / 2.0 > 0) ? targetFrequency - frequency / 2 : 0;
}
