//
//  perceptron.hpp
//  recognition
//
//  Created by Niko Vertovec on 19.03.18.
//

#ifndef perceptron_hpp
#define perceptron_hpp

#include <stdio.h>
#include <image.hpp>
#include <vector>
#include <fstream>


struct Weights
{
    std::vector<std::vector<int>> w;
    int weight0;
    
    /*
    tl  tc  tr
    cl  cc  cr
    bl  bc  br
    */
    
    Weights() :w(9, std::vector<int>(3, 0)), weight0(0) {};
};

std::ostream& operator<<(std::ostream& os, const Weights& w);

//-------------------------------------------------------------------------------------
class Perceptron
//-------------------------------------------------------------------------------------
{
private:
    Weights w;
public:
    Perceptron() {w = Weights();};
    bool eval(std::vector<RGB> pixels) const; //returns if the pixel is card(false) or Backround(true)
    Weights getW();
    void setW(Weights weight);
    Weights learn(std::vector<RGB> pixels, bool target); //updates the weights
    Weights train(); //requires the learning database
    bool saveWeights(std::string location); //write Weights to txt file;
    Weights readWeights(std::string location); //read Weights from txt file
};

#endif /* perceptron_hpp */
