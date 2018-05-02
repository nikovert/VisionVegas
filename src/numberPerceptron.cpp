//
//  numberPerceptron.cpp
//  recognition
//
//  Created by Niko Vertovec on 19.03.18.
//

#include "perceptron.hpp"

std::ostream& operator<<(std::ostream& os, const NumberWeights& w)
{
    for (auto &value : w.w) {
        for (auto &weight : value) {
            os << weight << " ";
        }
        os << "\t";
    }
    return(os);
}

//Used by SoftMax and learn, not meant for all types of vectors
std::ostream& operator<<(std::ostream& os, const std::vector<double> vec)
{
    std::string numbers[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    for(int i = 0; i < vec.size(); i++)
        std::cout << numbers[i] << ": " << vec.at(i) << std::endl;
    return(os);
}

int NumberPerceptron::evalMax()
{
    std::vector<double> result;
    result = eval();
    //Choose the best possible Card
    double max = 0;
    double maxIndex = 0;
    for(int i = 0; i < result.size(); i++){ //get the max value of result vector
        if(max < result.at(i)) {
            max = result.at(i);
            maxIndex = i;
        }
    }
    return maxIndex;
}

std::vector<double> NumberPerceptron::eval()
{
    //CHECK
    // First Layer
    std::vector< std::vector<double> >::const_iterator row; //need 13 rows
    std::vector<double>::const_iterator col; //need 936 columns
    
    std::vector<double> result;
    
    if(!pic.isAllocated()){
        std::cerr << "ERROR in learn, pic not allocated" << std::endl;
        return result;
    }
    
    for (row = weights.w.begin(); row != weights.w.end(); ++row)
    {
        int value = 0;
        int i = 0;
        for (col = row->begin(); col != row->end(); ++col)
        {
            if(i == 936)
                value += (*col);
            else{
                if(!pic.at(i++)) value += (*col); //pic == false == black
                else             value -= (*col); //pic == true == white
            }
        }
        result.push_back(value);
    }
    
    //Soft Max
    softMax(result);
    
    return result;
}

void NumberPerceptron::softMax(std::vector<double>& vec)
{
    double max = vec.at(0);
    
    for(int i = 1; i < vec.size(); i++){ //get the max value of result vector
        if(max < vec.at(i)) max = vec.at(i);
    }
    
    double sum = 0;
    
    for(int i = 0; i < vec.size(); i++){
        vec.at(i) = std::exp(vec.at(i) - max);
        sum += vec.at(i);
    }
    
    for(int i = 1; i < vec.size(); i++){ //get the max value of result vector
        vec.at(i) /= sum;
    }
}
bool NumberPerceptron::saveWeights(std::string location)
{
    std::ofstream Weightfile;
    Weightfile.open(location);
    
    Weightfile << "Weights:" << std::endl;
    
    std::vector<int>::iterator iterWeight;
    
    for (auto &weightNumber : weights.w) {
        for (auto &value : weightNumber) {
            Weightfile << value << "\n";
        }
    }
    
    Weightfile.close();
    return true;
}

NumberWeights NumberPerceptron::readNumberWeights(std::string location)
{
    NumberWeights weight;
    std::ifstream in(location.c_str());
    
    std::string str;
    //for performance
    str.reserve(30);
    
    //skip N lines
    for(int i = 0; i < 1; ++i)
        std::getline(in, str);
    
    std::vector<std::vector<double>> vec;
    
    for (auto &weightNumber : weight.w) {
        std::vector<double> weightvalue;
        for (auto &value : weightNumber) {
            std::getline(in,str);
            weightvalue.push_back(stod(str));
        }
        vec.push_back(weightvalue);
    }
    
    weight.w = vec;
    return weight;
}

NumberWeights NumberPerceptron::getW()
{
    return weights;
}

void NumberPerceptron::setW(NumberWeights weight)
{
    weights = weight;
}

NumberWeights NumberPerceptron::learn(int target)
{
    std::vector<double> targetVec(13);
    targetVec.at(target) = 1;
    std::cout << "target Vector: \n" << targetVec << std::endl;
    
    if(!pic.isAllocated()){
        std::cerr << "ERROR in learn, pic not allocated" << std::endl;
        return weights;
    }
    
    std::vector<double> y = eval(); //which value (from  0 - 12)
    
    std::vector< std::vector<double> >::const_iterator row; //need 13 rows
    std::vector<double>::const_iterator col; //need 936 columns
    std::vector<std::vector<double>> vec;
    
    int rowInt = -1;
    for (row = weights.w.begin(); row != weights.w.end(); ++row)
    {
        int i = 0;
        rowInt++;
        std::vector<double> columnWeight;
        for (col = row->begin(); col != row->end(); ++col)
        {
            if(i == 936)
                columnWeight.push_back((*col) + 100*(targetVec.at(rowInt) - y.at(rowInt)));
            else{
                if(!pic.at(i++)) columnWeight.push_back((*col) + (targetVec.at(rowInt) - y.at(rowInt)));    //pic == false == black
                else            columnWeight.push_back((*col)  - (targetVec.at(rowInt) - y.at(rowInt)));     //pic == true == white
            }
        }
        vec.push_back(columnWeight);
    }
    weights.w = vec;
    return weights;
}

static std::string ReadNthLinefromFile(const std::string& filename, int N)
{
    std::ifstream in(filename.c_str());
    
    std::string s;
    
    //skip N lines
    for(int i = 0; i < N; ++i)
        std::getline(in, s);
    
    std::getline(in,s);
    return s;
}

NumberWeights NumberPerceptron::train()
{
    std::string errmsg;
    std::string numbers[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    std::cout << "Training Perceptron... " << "\n" << std::endl;
    
    for(int j = 0; j < 500; j++){
        for(int i = 0; i < 191; i += 2){
            //Load new Image and Mask
            std::string file = ReadNthLinefromFile("../../trainingdataValues/training_list.txt", i);
            std::string fileValue = ReadNthLinefromFile("../../trainingdataValues/training_list.txt", i+1);
            std::cout << "Reading File: " << file << " with Value: " << stoi(fileValue) << " --> " << numbers[stoi(fileValue)] << std::endl;
            
            std::string location = "../../trainingdataValues/";
            location.append(file);
            pic.readPNM(location,errmsg);
            
            if(!pic.isAllocated())
                std::cerr << "ERROR in train, im not allocated!" << std::endl;
            
            if(pic.pixels() != 936){
                std::cerr << "Image dimensions don't match" << std::endl;
                continue;
            }
            
            weights = learn(stoi(fileValue));
            std::cout << "sub-Iteration: " << i/2 << std::endl;
        }
        std::cout << "Iteration: " << j << std::endl;
    }
    
    return weights;
}

bool NumberPerceptron::setImage(BinaryImage& in)
{
    if(!in.isAllocated()) return false;
    pic.create(in.width(), in.height());
    for(unsigned x0 = 1; x0 < in.width(); x0++){
        for(unsigned y0 = 1; y0 < in.height(); y0++){
            pic.setPixel(x0, y0, in.at(x0, y0));
        }
    }
    return true;
}

bool NumberPerceptron::setImage(Image& in)
{
    if(!in.isAllocated()) return false;
    pic.create(in.width(), in.height());
    bool value = false;
    for(unsigned x0 = 1; x0 < in.width(); x0++){
        for(unsigned y0 = 1; y0 < in.height(); y0++){
            value = (in.at(x0, y0).r > 50 && in.at(x0, y0).g > 50 && in.at(x0, y0).b > 50);
            pic.setPixel(x0, y0, value);
        }
    }
    return true;
}
