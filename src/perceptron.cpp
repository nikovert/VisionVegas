//
//  perceptron.cpp
//  recognition
//
//  Created by Niko Vertovec on 19.03.18.
//

#include "perceptron.hpp"

std::ostream& operator<<(std::ostream& os, const Weights& w)
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

bool Perceptron::eval(std::vector<RGB> pixels) const
{
    int value = w.weight0;
    
    std::vector<std::vector<int>>::const_iterator iterWeightpixel;
    std::vector<int>::const_iterator iterWeightcolor;
    std::vector<RGB>::const_iterator iterPixel;
    
    iterWeightpixel = w.w.begin();
    
    for(iterPixel = pixels.begin(); iterPixel < pixels.end(); iterPixel++){
        for(iterWeightcolor = iterWeightpixel->begin(); iterWeightcolor <  iterWeightpixel->end(); iterWeightcolor++){
            RGB color = *iterPixel;
            value += (*iterWeightcolor) * int((*iterPixel).r) + *(++iterWeightcolor) * int((*iterPixel).g) + *(++iterWeightcolor) * int((*iterPixel).b);
            /*
            std::cout << "List for eval: " << std::endl;
            std::cout << (*iterWeightcolor) << std::endl;
            std::cout << int(color.r) <<std::endl;
            std::cout << *(++iterWeightcolor) <<std::endl;
            std::cout << int(color.g) <<std::endl;
            std::cout << *(++iterWeightcolor) <<std::endl;
            std::cout << int(color.b) <<std::endl;
             */
        }
        iterWeightpixel++;
    }
    return value >= 0;
}

bool Perceptron::saveWeights(std::string location)
{
    std::ofstream Weightfile;
    Weightfile.open(location);
    
    Weightfile << "Weights:" << std::endl;
    
    Weightfile << w.weight0 << std::endl;
    
    std::vector<int>::iterator iterWeight;
    
    for (auto &weight : w.w) {
        for (auto &color : weight) {
            Weightfile << color << "\n";
        }
    }
    
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
    w.weight0 = stoi(str);
    
    std::vector<std::vector<int>> vec;
    
    for (auto &weight : w.w) {
        std::vector<int> colorWeight;
        for (auto &color : weight) {
            std::getline(in,str);
            colorWeight.push_back(stoi(str));
        }
        vec.push_back(colorWeight);
    }
    w.w = vec;
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

Weights Perceptron::learn(std::vector<RGB> pixels, bool target)
{
    //Weights weight;
    //std::cout << "pixelSize: " << pixels.size();
    bool y = eval(pixels); //is backround
    
    w.weight0 = w.weight0 + (target-y) * 1;
    
    
    std::vector<std::vector<int>>::const_iterator iterWeightpixel;
    std::vector<int>::const_iterator iterWeightcolor;
    std::vector<RGB>::iterator iterPixel;
    
    std::vector<std::vector<int>> vec;
    iterWeightpixel = w.w.begin();
    
    for(iterPixel = pixels.begin(); iterPixel < pixels.end(); iterPixel++){
        for(iterWeightcolor = iterWeightpixel->begin(); iterWeightcolor <  iterWeightpixel->end(); iterWeightcolor++){
            std::vector<int> colorWeight;
            colorWeight.push_back(  (*iterWeightcolor) + (target-y) * (*iterPixel).r);
            colorWeight.push_back((*++iterWeightcolor) + (target-y) * (*iterPixel).g);
            colorWeight.push_back((*++iterWeightcolor) + (target-y) * (*iterPixel).b);
            vec.push_back(colorWeight);
        }
        iterWeightpixel++;
    }
    w.w = vec;
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
    Image im, mask;
    std::string errmsg;
    
    std::cout << "Training Perceptron... " << "\n" << std::endl;
    
    for(int j = 0; j < 50; j++){
        for(int i = 30; i < 40; i += 2){
            //Load new Image and Mask
            std::string fileOriginal = ReadNthLinefromFile("../../trainingdata/training_list.txt", i);
            std::string fileMask = ReadNthLinefromFile("../../trainingdata/training_list.txt", i+1);
            std::cout << "Reading File: " << fileOriginal << " with Mask: " << fileMask << std::endl;
            
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
        }
        std::cout << "Weights: " << w << " \n Iteration: " << j << std::endl;
    }
    return w;
}

