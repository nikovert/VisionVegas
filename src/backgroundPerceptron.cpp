//
//  backgroundPerceptron.cpp
//  recognition
//
//  Created by Niko Vertovec on 19.03.18.
//

#include "perceptron.hpp"

std::ostream& operator<<(std::ostream& os, const BackgroundWeights& w)
{
    os << w.weight0 << "\n";
    for (auto &weight : w.w) {
        for (auto &color : weight) {
            os << color << " ";
        }
        os << "\t";
    }
    return(os);
}

double BackgroundPerceptron::evalValue(std::vector<RGB> pixels) const
{
    double value = w.weight0;
    
    std::vector<std::vector<double>>::const_iterator iterWeightpixel;
    std::vector<double>::const_iterator iterWeightcolor;
    std::vector<RGB>::const_iterator iterPixel;
    
    iterWeightpixel = w.w.begin();
    
    for(iterPixel = pixels.begin(); iterPixel < pixels.end(); iterPixel++){
        for(iterWeightcolor = iterWeightpixel->begin(); iterWeightcolor <  iterWeightpixel->end(); iterWeightcolor++){
            value += (*iterWeightcolor) * double((*iterPixel).r) + *(++iterWeightcolor) * double((*iterPixel).g) + *(++iterWeightcolor) * double((*iterPixel).b);
        }
        iterWeightpixel++;
    }
    return value;
}

bool BackgroundPerceptron::eval(std::vector<RGB> pixels) const
{
    double value = w.weight0;
    
    std::vector<std::vector<double>>::const_iterator iterWeightpixel;
    std::vector<double>::const_iterator iterWeightcolor;
    std::vector<RGB>::const_iterator iterPixel;
    
    iterWeightpixel = w.w.begin();
    
    for(iterPixel = pixels.begin(); iterPixel < pixels.end(); iterPixel++){
        for(iterWeightcolor = iterWeightpixel->begin(); iterWeightcolor <  iterWeightpixel->end(); iterWeightcolor++){
            value += (*iterWeightcolor) * double((*iterPixel).r) + *(++iterWeightcolor) * double((*iterPixel).g) + *(++iterWeightcolor) * double((*iterPixel).b);
        }
        iterWeightpixel++;
    }
    return value >= 0;
}

bool BackgroundPerceptron::saveWeights(std::string location)
{
    std::ofstream Weightfile;
    Weightfile.open(location);
    
    Weightfile << "Weights:" << std::endl;
    
    Weightfile << w.weight0 << std::endl;
    
    std::vector<double>::iterator iterWeight;
    
    for (auto &weight : w.w) {
        for (auto &color : weight) {
            Weightfile << color << "\n";
        }
    }
    
    Weightfile.close();
    return true;
}

BackgroundWeights BackgroundPerceptron::readBackgroundWeights(std::string location)
{
    BackgroundWeights w;
    std::ifstream in(location.c_str());
    
    std::string str;
    //for performance
    str.reserve(30);
    
    //skip N lines
    for(int i = 0; i < 1; ++i)
        std::getline(in, str);
    
    std::getline(in,str);
    w.weight0 = stod(str);
    
    std::vector<std::vector<double>> vec;
    
    for (auto &weight : w.w) {
        std::vector<double> colorWeight;
        for (auto &color : weight) {
            std::getline(in,str);
            colorWeight.push_back(stod(str));
        }
        vec.push_back(colorWeight);
    }
    w.w = vec;
    return w;
}

BackgroundWeights BackgroundPerceptron::getW()
{
    return w;
}

void BackgroundPerceptron::setW(BackgroundWeights weight)
{
    w = weight;
}

BackgroundWeights BackgroundPerceptron::learn(std::vector<RGB> pixels, int target)
{
    //Weights weight;
    //std::cout << "pixelSize: " << pixels.size();
    double y = eval(pixels); //is background
    w.weight0 = w.weight0 + (target-y);
    
    std::vector<std::vector<double>>::const_iterator iterWeightpixel;
    std::vector<double>::const_iterator iterWeightcolor;
    std::vector<RGB>::iterator iterPixel;
    
    std::vector<std::vector<double>> vec;
    iterWeightpixel = w.w.begin();
    
    for(iterPixel = pixels.begin(); iterPixel < pixels.end(); iterPixel++){
        for(iterWeightcolor = iterWeightpixel->begin(); iterWeightcolor <  iterWeightpixel->end(); iterWeightcolor++){
            std::vector<double> colorWeight;
            colorWeight.push_back(  (*iterWeightcolor) + (target-y) * (*iterPixel).r/255);
            colorWeight.push_back((*++iterWeightcolor) + (target-y) * (*iterPixel).g/255);
            colorWeight.push_back((*++iterWeightcolor) + (target-y) * (*iterPixel).b/255);
            vec.push_back(colorWeight);
        }
        iterWeightpixel++;
    }
    w.w = vec;
    return w;
}

static std::string ReadNthLinefromFile(const std::string& filename, int N)
{
    std::ifstream in(filename.c_str());
    
    std::string s;
    //for performance
    //s.reserve(105);
    
    //skip N lines
    for(int i = 0; i < N; ++i)
        std::getline(in, s);
    
    std::getline(in,s);
    return s;
}

BackgroundWeights BackgroundPerceptron::train()
{
    Image im, mask;
    std::string errmsg;
    
    std::cout << "Training Perceptron... " << "\n" << std::endl;
    for(int i = 0; i < 40; i += 2){
        //Load new Image and Mask
        std::string fileOriginal = ReadNthLinefromFile("../../trainingdata_withnoise/training_list.txt", i);
        std::string fileMask = ReadNthLinefromFile("../../trainingdata_withnoise/training_list.txt", i+1);
        std::cout << "Reading File: " << fileOriginal << " with Mask: " << fileMask << std::endl;
        
        std::string location = "../../trainingdata_withnoise/";
        location.append(fileOriginal);
        im.readPNM(location,errmsg);
        
        location = "../../trainingdata_withnoise/";
        location.append(fileMask);
        mask.readPNM(location,errmsg);
        
        if(!mask.isAllocated()){
            std::cerr << "ERROR in train, mask not allocated!" << std::endl;
            continue;
        }
        if(!im.isAllocated()){
            std::cerr << "ERROR in train, im not allocated!" << std::endl;
            continue;
        }
        
        if(mask.pixels() != im.pixels()){
            std::cerr << "Image dimensions don't match" << std::endl;
            continue;
        }
        
        for(unsigned x0 = 1; x0 < im.width()-1; x0++){
            for(unsigned y0 = 1; y0 < im.height()-1; y0++){
                std::vector<RGB> pixels;
                for(int n = -1; n<= 1; n++){
                    for(int k = -1; k <= 1; k++){
                        pixels.push_back(im.at(x0+k, y0+n));
                        //std::cout << im.at(x0+k, y0+n);
                    }
                }
                w = learn(pixels, (mask.at(x0, y0) < RGB(50, 50, 50)));
            }
        }
        std::cout << "temporary Weights: " << w << " \n sub-Iteration: " << i/2 << std::endl;
    }
    return w;
}
