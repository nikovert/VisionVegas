/*
 *
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#include <image.hpp>
#include <card.hpp>
#include <carddetector.hpp>
#include <BlobDetector.hpp>

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>



void error(const char *fmt, ...)
{
    va_list args;
    char buf[4000];
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    std::cerr << buf;
    std::exit(-1);
}

std::string ReadNthLine(const std::string& filename, int N)
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

bool cardcheck(){
    Card card;
    std::string errmsg;
    
    //redirect cout
    std::streambuf *psbuf, *backup;
    std::ofstream filestr;
    filestr.open ("cardcheck.txt");
    
    backup = std::cout.rdbuf();     // back up cout's streambuf
    
    psbuf = filestr.rdbuf();        // get file's streambuf
    std::cout.rdbuf(psbuf);         // assign streambuf to cout
    
    std::cout << "Playingcard checks: " << "\n" << std::endl;
    
    for(int i = 1; i < 105; i++){
        
        Carddetector detector(card);
        
        std::string str = "../../card_images/";
        std::string file = ReadNthLine("../../card_images/card_list.txt", i);
        std::cout << "Reading File: " << file << std::endl;
        
        detector.currentCard = file;
        str.append(file);
        
        card.readImage(errmsg, str);
        
        // create a copy of the loaded image
        Image im;
        card.cloneImageTo(im);
        //detector.setdebug();
        if(detector.isolateCard()){
            detector.retrieveCrop(im);
            
            // get value of the card
            std::cout << "Card value: " << card.getValue() << "\n";
            
            std::string output;
            // write image to disk
            if(detector.isdebug())
                output = std::to_string(i) + "_output_debug_" + file;
            else
                output = std::to_string(i) + "_output_" + file;
            if(!im.writePNM(output,errmsg)) return false;
        }
        else
            std::cout << "Card Failed " << "\n";
        
        std::cout << "\n";
    }
    
    std::cout.rdbuf(backup);        // restore cout's original streambuf
    
    filestr.close();
    
    return true;
}

void thresholdtest()
{
	RGB darkred(0x99, 0, 0);
	RGB lightred(0xff, 0x66, 0x66);
	BlobDetector bdetector = BlobDetector();
	bdetector.AddRGBRange(darkred, lightred);

	std::string path = "../card_images/e2.1.pnm";
	std::string wpath = "../thresholded.pnm";
	std::string errorMessage;
	Image im, th;
	im.readPNM(path,errorMessage);
	bdetector.FindBlobs(im);
	bdetector.retrieveThresholded(th);
	th.writePNM(wpath, errorMessage);

}


int main(int, const char **)
{
    thresholdtest();
}

