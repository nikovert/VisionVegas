//
//  numberPerceptron.cpp
//  recognition
//
//  Created by Niko Vertovec on 19.03.18.
//

#include "perceptron.hpp"

std::ostream& operator<<(std::ostream& os, const NumberWeights& w)
{
    //CHECK
    os << w.weight0 << "\n";
    for (auto &weight : w.w) {
        os << weight << " ";
    }
    return(os);
}

int NumberPerceptron::eval() const
{
    //TO DO
    return -1;
}

bool NumberPerceptron::saveWeights(std::string location)
{
    //CHECK
    std::ofstream Weightfile;
    Weightfile.open(location);
    
    Weightfile << "Weights:" << std::endl;
    
    Weightfile << w.weight0 << std::endl;
    
    std::vector<int>::iterator iterWeight;
    
    for (auto &weight : w.w) {
            Weightfile << weight << "\n";
    }
    
    Weightfile.close();
    return true;
}

NumberWeights NumberPerceptron::readNumberWeights(std::string location)
{
    //CHECK
    NumberWeights w;
    std::ifstream in(location.c_str());
    
    std::string str;
    //for performance
    str.reserve(30);
    
    //skip N lines
    for(int i = 0; i < 1; ++i)
        std::getline(in, str);
    
    std::getline(in,str);
    w.weight0 = stoi(str);
    
    std::vector<int> vec;
    
    for (auto &weight : w.w) {
        vec.push_back(stoi(str));
    }
    w.w = vec;
    return w;
}

NumberWeights NumberPerceptron::getW()
{
    return w;
}

void NumberPerceptron::setW(NumberWeights weight)
{
    w = weight;
}

NumberWeights NumberPerceptron::learn(std::vector<RGB> pixels, bool target)
{
    //TO DO
    return w;
}

static std::string ReadNthLinefromFile(const std::string& filename, int N)
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

NumberWeights NumberPerceptron::train()
{
    //TO DO
    return w;
}

