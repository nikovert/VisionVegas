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
    
    BackgroundWeights learn(std::vector<RGB> pixels, bool target); //updates the weights
public:
    BackgroundPerceptron() {w = BackgroundWeights();};
    bool eval(std::vector<RGB> pixels) const; //returns if the pixel is card(false) or Background(true)
    BackgroundWeights getW();
    void setW(BackgroundWeights weight);
    BackgroundWeights train(); //requires the learning database
    bool saveWeights(std::string location); //write Weights to txt file;
    BackgroundWeights readBackgroundWeights(std::string location); //read Weights from txt file
};

struct NumberWeights
{
    std::vector<std::vector<double>> w;
    NumberWeights() :w(13, std::vector<double>(936, 0)) {}; //assuming we have a 26x36 image and weight0 is at pos 936;
};

std::ostream& operator<<(std::ostream& os, const NumberWeights& w);

//-------------------------------------------------------------------------------------
class NumberPerceptron
//-------------------------------------------------------------------------------------
{
    // order of Values: {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"}
    //                 { 0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8  ,  9 ,  10,  11,  12};
private:
    NumberWeights weights;
    BinaryImage pic;
    
    void softMax(std::vector<double>& vec);
    NumberWeights learn(int target); //updates the weights
public:
    NumberPerceptron() {weights = NumberWeights();};
    std::vector<double> eval(); //returns the value % of the card
    int evalMax(); //returns the value of the card
    NumberWeights getW();
    void setW(NumberWeights weight);
    NumberWeights train(); //requires the learning database
    bool saveWeights(std::string location); //write Weights to txt file;
    NumberWeights readNumberWeights(std::string location); //read Weights from txt file
    bool setImage(BinaryImage& in);
    bool setImage(Image& in);
};
#endif /* perceptron_hpp */
