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


struct BackgroundWeights
{
    std::vector<std::vector<int>> w;
    int weight0;
    
    /*
    tl  tc  tr
    cl  cc  cr
    bl  bc  br
    */
    
    BackgroundWeights() :w(9, std::vector<int>(3, 0)), weight0(0) {};
};

std::ostream& operator<<(std::ostream& os, const BackgroundWeights& w);

//-------------------------------------------------------------------------------------
class BackgroundPerceptron
//-------------------------------------------------------------------------------------
{
private:
    BackgroundWeights w;
public:
    BackgroundPerceptron() {w = BackgroundWeights();};
    bool eval(std::vector<RGB> pixels) const; //returns if the pixel is card(false) or Background(true)
    BackgroundWeights getW();
    void setW(BackgroundWeights weight);
    BackgroundWeights learn(std::vector<RGB> pixels, bool target); //updates the weights
    BackgroundWeights train(); //requires the learning database
    bool saveWeights(std::string location); //write Weights to txt file;
    BackgroundWeights readBackgroundWeights(std::string location); //read Weights from txt file
};

struct NumberWeights
{
    std::vector<int> w;
    int weight0;
    
    NumberWeights() :w(676), weight0(0) {}; //assuming we have a 26x36 image
};

std::ostream& operator<<(std::ostream& os, const NumberWeights& w);

//-------------------------------------------------------------------------------------
class NumberPerceptron
//-------------------------------------------------------------------------------------
{
private:
    NumberWeights w;
    BinaryImage pic;
public:
    NumberPerceptron() {w = NumberWeights();};
    int eval() const; //returns the value of the card
    NumberWeights getW();
    void setW(NumberWeights weight);
    NumberWeights learn(std::vector<RGB> pixels, bool target); //updates the weights
    NumberWeights train(); //requires the learning database
    bool saveWeights(std::string location); //write Weights to txt file;
    NumberWeights readNumberWeights(std::string location); //read Weights from txt file
};
#endif /* perceptron_hpp */
