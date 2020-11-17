#include "wave.hpp"

using namespace std;

wave_func::wave_func(int w, int h, int n, const uint8_t *input, int win, int hin, bool bounds) 
                : width(w), height(h), waveWidth(w-n+1), waveHeight(h-n+1), patternCount(0), n(n) {
  //create patterns from input array
  patterns = new pattern[(win-n+1)*(hin-n+1)];
  for (int j = 0; j < hin+1-n; j++){
    for (int i = 0; i < win+1-n; i++){
      int *patternData = new int[n*n*4];
      for(int dy = 0; dy < n; dy++){
        for(int dx = 0; dx < n; dx++){
          for(int color = 0; color < 4; color++){
            patternData[n*4*dy+4*dx+color] = (int)input[4*win*(j+dy)+4*(i+dx)+color];
          }
        }
      }
      bool alreadyExists = false;
      int boundNum;
      for(int a = 0; a < patternCount; a++){
        if(patterns[a].isEquivalent(patternData)){
          alreadyExists = true;
          boundNum = a;
          patterns[a].addTargetFrequency();
        }
      }
      if(!alreadyExists){
        boundNum = patternCount;
        patterns[patternCount] = pattern(n, patternData, (hin+1-n)*(win+1-n), waveWidth*waveHeight);
        patternCount++;
      }
      if(j == 0){
        patterns[boundNum].boundary = -1;
      } else if(j == hin-n){
        patterns[boundNum].boundary = 1;
      }
    }
  }
  cout << "Initialized " << patternCount << " patterns\n";

  noiseDistro = numeric_limits<float>::infinity();
  for(int i = 0; i < patternCount; i++){
    float ent = patterns[i].getTargetFreq() * log(patterns[i].getTargetFreq());
    if(abs(ent / 2) < noiseDistro){
      noiseDistro = abs(ent / 2);
    }
  }

  //initialize a wave function array and pixel data array with width and height and n
  wave = new bool**[waveHeight];
  for(int i = 0; i < waveHeight; i++){
    wave[i] = new bool*[waveWidth];
    for(int j = 0; j < waveWidth; j++){
      wave[i][j] = new bool[patternCount];
    }
  }
  out = new uint8_t[w*h*4];
  bool ***possibilities = new bool**[2*n-1];
  for(int i = 0; i < 2*n-1; i++){
    possibilities[i] = new bool*[2*n-1];
    for(int j = 0; j < 2*n-1; j++){
      possibilities[i][j] = new bool[patternCount];
    }
  }

  //initialize wave function with superposition of possible patterns
  //  possible patterns are found by checking whether all surrounding squares
  //  have possible patterns with that pattern 
  
  //set all patterns (x) to possible
  cout << "Calculating possible pattern combinations...\n";
  for(int y = 0; y < 2*n-1; y++){
    for(int x = 0; x < 2*n-1; x++){
      for(int a = 0; a < patternCount; a++){
        bool hasOptions = true;
        for(int dy = 1-n; dy < n; dy++){
          if(y+dy >= 0 && y+dy < 2*n-1){
            for(int dx = 1-n; dx < n; dx++){
              if(x+dx >= 0 && x+dx < 2*n-1){
                if(dx !=0 || dy != 0){
                  bool possible = false;
                  for(int b = 0; b < patternCount; b++){
                    pattern current = patterns[a];
                    pattern surrounding = patterns[b];
                    if(current.isPossible(surrounding.getPixelData(), dx, dy)){
                      possible = true;
                    }                      
                  }
                  if(!possible){
                    hasOptions = false;
                  }
                }
              }
            }
          }
        }
        if(bounds){
          if(y == 0 && patterns[a].boundary != -1){
            hasOptions = false;
          }else if(y == 2*n-2 && patterns[a].boundary != 1){
            hasOptions = false;
          }
        }
        possibilities[y][x][a] = hasOptions;
      }
    }
  }

  cout << "Generating initial wave function...\n";
  for(int row = 0; row < waveHeight; row++){
    for(int col = 0; col < waveWidth; col++){
      if(row < n-1) {
        if(col < n-1) {
          for(int a = 0; a < patternCount; a++){
            wave[row][col][a] = possibilities[row][col][a];
          }
        }else if(waveWidth - col < n){
          for(int a = 0; a < patternCount; a++){
            wave[row][col][a] = possibilities[row][(2*n-1)-(waveWidth - col)][a];
          }
        }else{
          for(int a = 0; a < patternCount; a++){
            wave[row][col][a] = possibilities[row][n-1][a];
          }
        }
      }else if(waveHeight - row < n){
        if(col < n-1) {
          for(int a = 0; a < patternCount; a++){
            wave[row][col][a] = possibilities[(2*n-1)-(waveHeight - row)][col][a];
          }
        }else if(waveWidth - col < n){
          for(int a = 0; a < patternCount; a++){
            wave[row][col][a] = possibilities[(2*n-1)-(waveHeight - row)][(2*n-1)-(waveWidth - col)][a];
          }
        }else{
          for(int a = 0; a < patternCount; a++){
            wave[row][col][a] = possibilities[(2*n-1)-(waveHeight - row)][n-1][a];
          }
        }
      }
      else{
        if(col < n-1) {
          for(int a = 0; a < patternCount; a++){
            wave[row][col][a] = possibilities[n-1][col][a];
          }
        }else if(waveWidth - col < n){
          for(int a = 0; a < patternCount; a++){
            wave[row][col][a] = possibilities[n-1][(2*n-1)-(waveWidth - col)][a];
          }
        }else{
          for(int a = 0; a < patternCount; a++){
            wave[row][col][a] = possibilities[n-1][n-1][a];
          }
        }
      }
    }
  }

  if(bounds){
    cout << "Propogating boundary conditions...\n";
    for(int col = 0; col < waveWidth; col++){
      propogate(col, 0, 1);
      propogate(col, waveHeight - 1, 1);
      cout << '.';
    }
    cout << '\n';
  }

  cout << "Generating superposition from wave function...\n";
  //use wave funtion to generate starting pixel map
  generateOutput();
  cout << "Wave Colapse function initialized.\n";
}

void wave_func::generateOutput(){
  for(int row = 0; row < height; row++){
    for(int col = 0; col < width; col++){
      int rgb[4] = {0,0,0,0};
      int count = 0;
      int patterncount = 0;
      int pattern;
      for(int dy = 0; dy < n; dy++){
        if(row-dy >= 0 && row-dy < waveHeight){
          for(int dx = 0; dx < n; dx++){
            if(col-dx >= 0 && col-dx < waveWidth){
              for(int a = 0; a < patternCount; a++){
                if(wave[row-dy][col-dx][a]){
                  for(int color = 0; color < 4; color++){
                    rgb[color] += patterns[a].getPixel(dy, dx, color);
                  }
                  count++;
                }
              }
            }
          }
        }
      }
      if(count == 0){
        cout << "0 :(\n";
      }
      for(int color = 0; color < 4; color++){
        out[width*row*4+col*4+color] = uint8_t(rgb[color] / count);
      }
    }
  }
}

int wave_func::colapse() {

  //loop through wave funtion and find element with min non-zero entropy
  //entropy = sum(P(x)*ln(P(x))) where P(x) is the probability of x
  //p(x) = width * height * pattern[x].getProbability / (available patterns)

  for(int a = 0; a < patternCount; a++){
    int count = 0;
    for(int row = 0; row < waveHeight; row++){
      for(int col = 0; col < waveWidth; col++){
        if(wave[row][col][a]) count++;
      }
    }
    patterns[a].available = count;
  }
  float minEntropy = numeric_limits<float>::infinity();
  int minRow, minCol;
  bool defined = true;
  for(int row = 0; row < waveHeight; row++){
    for(int col = 0; col < waveWidth; col++){
      int n = 0;
      float *probabilities = new float[patternCount];
      for(int a = 0; a < patternCount; a++){
        if(wave[row][col][a]){
          n += (waveWidth * waveHeight * patterns[a].getProbability());
        }
      }
      for(int a = 0; a < patternCount; a++){
        probabilities[a] = 0;
        if(wave[row][col][a]){
          probabilities[a] = n ? float(waveWidth * waveHeight * patterns[a].getProbability()) / float(n) : 0;
        }
      }
      float entropy = 0;
      bool singular;
      int count = 0;
      for(int a = 0; a < patternCount; a++){
        if(wave[row][col][a]){
          count++;
        }
        if(wave[row][col][a] && probabilities[a]){
          entropy -= probabilities[a] * log(probabilities[a]); 
        }
      }
      singular = (count == 1);
      if(!singular){
        defined = false;
        if(entropy <= minEntropy){
          uniform_real_distribution<float> dis(0.0, noiseDistro);
          float noise = dis(generator);
          if((entropy + noise) <= minEntropy){
            minEntropy = entropy + noise;
            minRow = row;
            minCol = col;
          }
        }
      }
    }
  }
  if(defined){
    generateOutput();
    cout << "Done.\n";
    return -1;
  }

  //collapse element to single pattern
  //find the pattern with highest probability
  int element;
  vector<int> bucket1, bucket2;
  int bucketNum = 1;
  for(int a = 0; a < patternCount; a++){
    if(wave[minRow][minCol][a]){
      if(bucketNum == 1){
        if(patterns[a].getProbability() >= 1.0){
          int prob = patterns[a].getProbability() * waveWidth * waveHeight;
          for(int i = 0; i < prob; i++){
            bucket2.push_back(a);
          }
          bucketNum = 2;
        }else{
          int prob = patterns[a].getTargetFreq() * waveWidth * waveHeight;
          for(int i = 0; i < prob; i++){
            bucket1.push_back(a);
          }
        }
      }else{
        if(patterns[a].getProbability() >= 1.0){
          int prob = patterns[a].getProbability() * waveWidth * waveHeight;
          for(int i = 0; i < prob; i++){
            bucket2.push_back(a);
          }
          bucketNum = 2;
        }
      }
    }
  }
  int choice;
  uniform_real_distribution<float> dis1(0.0, bucket1.size());
  uniform_real_distribution<float> dis2(0.0, bucket2.size());
  switch(bucketNum) {
    case 1:
    choice = dis1(generator);
    element = bucket1[choice];
    break;

    case 2:
    choice = dis2(generator);
    element = bucket2[choice];
    break;      
  }
  cout << "Colapsing " << minCol << ',' << minRow << '\n';
  patterns[element].addFrequency();

  //colapse//
  for(int a = 0; a < patternCount; a++){
    if(a != element){
      wave[minRow][minCol][a] = 0;
    }else{
    }
  }

  //update//
  propogate(minCol, minRow);
  generateOutput();
  return 0;
}

void wave_func::propogate(int x, int y, int direction) {
  //propogate new information to create new wave function
  //when checking the surrounding wave elements, if there is
  //a possible 

  for(int dy = 1-n; dy < n; dy++){
    if(y+dy >=0 && y+dy < waveHeight){
      if(direction == 0){
        for(int dx = 1-n; dx < n; dx++){
          if(x+dx >=0 && x+dx < waveWidth){
            if(dx !=0 || dy != 0){
              bool canContinue = false;
              bool changed = false;
              for(int a = 0; a < patternCount; a++){
                if(wave[y+dy][x+dx][a]){
                  bool canUse = false;
                  for(int b = 0; b < patternCount; b++){
                    if(wave[y][x][b]){
                      if(patterns[b].isPossible(patterns[a].getPixelData(), dx, dy)){
                        canUse = true;
                      }
                    }
                  }
                  wave[y+dy][x+dx][a] = canUse;
                  if(!canUse){
                    changed  = true;
                  }
                }
                if(wave[y+dy][x+dx][a]){
                  canContinue = true;
                }
              }
              if(!canContinue){
                cout << "failure at " << x+dx << ',' << y+dy << " from " << x << ',' << y << '\n';
                return;
              }else if(changed){
                propogate(x+dx, y+dy);
              }
            }
          }
        }  
      }else{ 
        int dx = 0;
        bool canContinue = false;
        bool changed = false;
        for(int a = 0; a < patternCount; a++){
          if(wave[y+dy][x+dx][a]){
            bool canUse = false;
            for(int b = 0; b < patternCount; b++){
              if(wave[y][x][b]){
                if(patterns[b].isPossible(patterns[a].getPixelData(), dx, dy)){
                  canUse = true;
                }
              }
            }
            wave[y+dy][x+dx][a] = canUse;
            if(!canUse){
              changed  = true;
            }
          }
          if(wave[y+dy][x+dx][a]){
            canContinue = true;
          }
        }
        if(!canContinue){
          cout << "failure at " << x+dx << ',' << y+dy << " from " << x << ',' << y << '\n';
          return;
        }else if(changed){
          propogate(x+dx, y+dy, 1);
        }
      }
    }
  }
}

uint8_t *wave_func::getOutput(){
  return out;
}
