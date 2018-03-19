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
#include <card.hpp>

#include <fstream>


struct Weights
{
    double weight0, weightR, weightG, weightB;
    Weights() : weight0(0), weightR(0), weightG(0), weightB(0) {};
};

std::ostream& operator<<(std::ostream& os, const Weights& w);

//-------------------------------------------------------------------------------------
class Perceptron
//-------------------------------------------------------------------------------------
{
private:
    Weights w;
public:
    Perceptron(Weights weight) {w = weight;};
    Perceptron() {w = Weights();};
    bool eval(RGB& pixel); //returns if the pixel is card(false) or Backround(true)
    Weights getW();
    void setW(Weights weight);
    Weights learn(RGB& pixel, bool target); //updates the weights
    Weights train(); //requires the learning database
    bool saveWeights(std::string& location); //write Weights to txt file;
    Weights readWeights(std::string& location); //read Weights from txt file
};

#endif /* perceptron_hpp */
