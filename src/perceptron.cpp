//
//  perceptron.cpp
//  recognition
//
//  Created by Niko Vertovec on 19.03.18.
//

#include "perceptron.hpp"

std::ostream& operator<<(std::ostream& os, const Weights& w)
{
    os << w.weight0 << " " << w.weightR << " " << w.weightG << " " << w.weightB << "\n";
    return(os);
}

bool Perceptron::eval(const RGB& pixel) const
{
    //std::cout << w.weight0 + w.weightR * pixel.r + w.weightG * pixel.g + w.weightB * pixel.b << std::endl;
    return (w.weight0 + w.weightR * pixel.r + w.weightG * pixel.g + w.weightB * pixel.b >= 0);
}

bool Perceptron::saveWeights(std::string location)
{
    std::ofstream Weightfile;
    Weightfile.open(location);
    
    Weightfile << "Weights:" << std::endl;
    
    Weightfile << w.weight0 << std::endl;
    Weightfile << w.weightR << std::endl;
    Weightfile << w.weightG << std::endl;
    Weightfile << w.weightB << std::endl;
    
    Weightfile.close();
    return true;
}

Weights Perceptron::readWeights(std::string location)
{
    Weights w;
    std::ifstream in(location.c_str());
    
    std::string str;
    //for performance
    str.reserve(30);
    
    //skip N lines
    for(int i = 0; i < 1; ++i)
        std::getline(in, str);
    
    std::getline(in,str);
    w.weight0 = stod(str);
    
    std::getline(in,str);
    w.weightR = stod(str);
    
    std::getline(in,str);
    w.weightG = stod(str);
    
    std::getline(in,str);
    w.weightB = stod(str);
    
    return w;
}

Weights Perceptron::getW()
{
    return w;
}

void Perceptron::setW(Weights weight)
{
    w = weight;
}

Weights Perceptron::learn(RGB& pixel, bool target)
{
    //std::cout << "Target: " << target << " Weight_old: " << w << " pixel: " << pixel << std::endl;
    bool y = eval(pixel); //is backround
    w.weight0 += (target-y) * 1;
    w.weightR += (target-y) * pixel.r;
    w.weightG += (target-y) * pixel.g;
    w.weightB += (target-y) * pixel.b;
    return w;
}

std::string ReadNthLinefromFile(const std::string& filename, int N)
{
    std::ifstream in(filename.c_str());
    
    std::string s;
    //for performance
    s.reserve(105);
    
    //skip N lines
    for(int i = 0; i < N; ++i)
        std::getline(in, s);
    
    std::getline(in,s);
    return s;
}

Weights Perceptron::train()
{
    /*
        loop:   -Read in a photo and it's mask
                -learn with each pixel and it's mask and update the weights
     */
    Image im, mask;
    std::string errmsg;
    
    std::cout << "Training Perceptron... " << "\n" << std::endl;
    
    for(int j = 0; j < 100; j++){
        for(int i = 0; i < 30; i += 2){
            //Load new Image and Mask
            std::string fileOriginal = ReadNthLinefromFile("../../trainingdata/training_list.txt", i);
            std::string fileMask = ReadNthLinefromFile("../../trainingdata/training_list.txt", i+1);
            //std::cout << "Reading File: " << fileOriginal << " with Mask: " << fileMask << std::endl;
            
            std::string location = "../../trainingdata/";
            location.append(fileOriginal);
            im.readPNM(location,errmsg);
            
            location = "../../trainingdata/";
            location.append(fileMask);
            mask.readPNM(location,errmsg);
            
            if(!mask.isAllocated())
                std::cerr << "ERROR, mask not allocated!" << std::endl;
            if(!im.isAllocated())
                std::cerr << "ERROR, im not allocated!" << std::endl;
            
            //Train each pixel
            if(mask.pixels() != im.pixels()){
                std::cerr << "Image dimensions don't match" << std::endl;
                continue;
            }
            
            for(unsigned x0 = 1; x0 < im.width(); x0++){
                for(unsigned y0 = 1; y0 < im.height(); y0++){
                    //w = learn(im.at(x0, y0), im.at(x0, y0).r < 128);
                    w = learn(im.at(x0, y0), (mask.at(x0, y0) < RGB(50, 50, 50)));
                }
            }
        }
        std::cout << "Weights: " << w << " \t Iteration: " << j << std::endl;
    }
    return w;
}

