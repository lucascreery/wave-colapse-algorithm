#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <string>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <random>
#include "wave.hpp"

using namespace std;


int main(int argc, char **argv) {
  if(argc < 6){
    cout << "Invalid usage\nProper use: (width) (height) (n) (input file) (output file)";
    return 0;
  }
  bool boundaries = false;
  if(argc == 7){
    boundaries = true;
  }
  cout << "Bounds: " << boundaries << '\n';
  srand(time(NULL));
  int width = stoi(argv[1]);
  int height = stoi(argv[2]);
  int n = stoi(argv[3]);

  sf::Image initImage;
  if(!initImage.loadFromFile(argv[4])){
    cout << "Unable to open image" << argv[3] << '\n';
    return 0;
  }
  const uint8_t *imagedata = initImage.getPixelsPtr();
  sf::Vector2u image_size = initImage.getSize();
  int inwidth = image_size.x, inheight  = image_size.y;
  cout << inwidth << 'x' << inheight << "image\n";
  for(int row = 0; row < inwidth; row++){
    for(int col = 0; col < inheight; col++){
      for(int color = 0; color < 4; color++){
        int pixel = imagedata[inwidth*4*row + 4*col + color];
      }
    }
  }
  wave_func wave = wave_func(width, height, n, imagedata, inwidth, inheight, boundaries);
  sf::RenderWindow window(sf::VideoMode(width*4,height*4), "WFC", sf::Style::Titlebar | sf::Style::Close);
  sf::Texture outTexture;
  outTexture.create(width, height);
  outTexture.update(wave.getOutput());
  sf::Sprite sprite;
  sprite.setTexture(outTexture);
  sprite.setScale(4.0f, 4.0f);
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear(sf::Color::Black);
    if(wave.colapse() < 0){
      outTexture.update(wave.getOutput());
      sf::Image outImage = outTexture.copyToImage();
      outImage.saveToFile(argv[5]);
      return 0;
    }
    outTexture.update(wave.getOutput());
    window.draw(sprite);
    window.display();
  }

  return 0;
}