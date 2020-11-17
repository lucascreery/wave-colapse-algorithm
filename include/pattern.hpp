#ifndef PATTERN_HPP
#define PATTERN_HPP

class pattern{
  public:
  int available;
  int boundary;
  pattern();
  pattern(int n, int *pixels, int intotal, int outtotal);

  bool isPossible(int *pixels, int dx, int dy);
  float getProbability();
  int getPixel(int y, int x, int rgb);
  int *getPixelData();
  bool isEquivalent(int *a);
  void addTargetFrequency();
  void addFrequency();
  float getTargetFreq();

  private:
  int *pixel_data;
  int n;
  int inTotal, outTotal;
  float  targetFrequency, frequency;
};


#endif