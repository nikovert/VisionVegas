/*
 *
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#include <image.hpp>
#include <card.hpp>
#include <carddetector.hpp>

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <perceptron.hpp>



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

bool cardcheck2(){
    Card card;
    Image im;
    card.loadPerceptron();
    std::string errmsg;
    std::string numbers[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    
    std::cout << "Playingcard checks: " << "\n" << std::endl;
    double success(0), counter(0);
    for(int i = 0; i < 123; i++){
        counter++;
        std::string str = "../../card_images/";
        std::string file = ReadNthLine("../../card_images/card_list.txt", i);
        std::cout << "Reading File: " << file << std::endl;
        
        str.append(file);
        card.readImage(errmsg, str);
        
        Carddetector detector(card);
        detector.currentCard = file;
        
        // create a copy of the loaded image
        card.cloneImageTo(im);
        
        if(detector.isolateCard()){
            detector.isolateValue();
            detector.retrieveValue(im);
            NumberPerceptron p;
            p.setW(p.readNumberWeights("../../Numberweights"));
            p.setImage(im);
            int result = p.evalMax();
            
            // get value of the card
            std::cout << "Card value: " << numbers[result] << "\n";
            success++;
            std::string output;
            
            // write image to disk
            output = std::to_string(i) + "_output_" + file;
            if(!im.writePNM(output,errmsg)) return false;
        }
        else
            std::cout << "Card Failed!" << "\n";
        
        std::cout << "\n";
    }
    std::cout << "success rate: " << success/counter * 100 << "%" << std::endl;
    return true;
}

bool cardcheck(){
    Card card;
    card.loadPerceptron();
    std::string errmsg;
    
    //redirect cout
    std::streambuf *psbuf, *backup;
    std::ofstream filestr;
    filestr.open ("cardcheck.txt");
    
    backup = std::cout.rdbuf();     // back up cout's streambuf
    
    psbuf = filestr.rdbuf();        // get file's streambuf
    std::cout.rdbuf(psbuf);         // assign streambuf to cout
    
    std::cout << "Playingcard checks: " << "\n" << std::endl;
    double success(0), counter(0);
    for(int i = 0; i < 123; i++){
        counter++;
        std::string str = "../../card_images/";
        std::string file = ReadNthLine("../../card_images/card_list.txt", i);
        std::cout << "Reading File: " << file << std::endl;
        
        str.append(file);
        card.readImage(errmsg, str);
        
        Carddetector detector(card);
        detector.currentCard = file;
        
        // create a copy of the loaded image
        Image im;
        card.cloneImageTo(im);
        //detector.setdebug();
        detector.initBlobdetection();
        
        if(detector.isolateCard()){
            detector.isolateValue();
            detector.retrieveValue(im);
            // get value of the card
            //std::cout << "Card value: " << card.getValue() << "\n";
            success++;
            std::string output;
            // write image to disk
            if(detector.isdebug())
                output = std::to_string(i) + "_output_debug_" + file;
            else
                output = std::to_string(i) + "_output_" + file;
            if(!im.writePNM(output,errmsg)) return false;
        }
        else
            std::cout << "Card Failed!" << "\n";
        
        std::cout << "\n";
    }
    
    std::cout.rdbuf(backup);        // restore cout's original streambuf
    
    filestr.close();
    
    std::cout << "success rate: " << success/counter * 100 << "%" << std::endl;
    return true;
}

bool simpleMasktest(){
    Card card;
    card.loadPerceptron();
    std::string errmsg;
    
    std::string str = "/Users/nikovertovec/Documents/VisionVegas/card_images/h8.1.pnm";
    card.readImage(errmsg, str);
    
    Carddetector detector(card);
    
    // create a copy of the loaded image
    Image im;
    
    if(detector.simpleMask()){
        detector.retrieveMask(im);
        
        if(!im.isAllocated()){
            std::cerr << "ERROR in simpleMasktest: im not allocated!" << std::endl;
        }
        
        // write image to disk
        std::string output = "output_h8.1.pnm";
        if(!im.writePNM(output,errmsg)){
            std::cerr << "ERROR in simpleMasktest: writing image to file" << std::endl;
            return false;
        }
    }
    else
        std::cout << "Card Failed " << "\n";
    
    std::cout << "\n";
    return true;
}

void thresholdtest()
{
	RGB darkred(0x80, 0x0, 0x0);
	RGB lightred(0xff, 0x66, 0x66);
	RGB black(0x0, 0x0, 0x0);
	RGB grey(0x50, 0x50, 0x50);
	BlobDetector bdetector = BlobDetector();
	bdetector.addRGBRange(darkred, lightred);
	bdetector.addRGBRange(black, grey);

	std::string path = "../card_images/k8.1.pnm";
	std::string wpath = "../thresholded.pnm";
	std::string wbpath = "../blobed.pnm";
	std::string errorMessage;
    
	Image imag, th, bl;
	bool retval = imag.readPNM(path,errorMessage);
	if (!retval) {
		std::cout << errorMessage;
		return;
	}
	std::vector<BLOB> blobs = bdetector.findBlobs(imag);
	std::cout << "Evaluating " << blobs.size() << " blobs" << std::endl;
	// Evaluate blobs

	for(std::vector<BLOB>::iterator blob_it = blobs.begin(); blob_it != blobs.end(); blob_it++) {
		std::cout << "Area: " << blob_it->area << " (x,y)=" << blob_it->center << std::endl;
	}

	bdetector.retrieveThresholded(th);
	bdetector.retrieveBlobed(bl);
	th.writePNM(wpath, errorMessage);
	bl.writePNM(wbpath, errorMessage);

}

void redrecognitiontest()
{
	RGB darkred(0x99, 0, 0);
	RGB lightred(0xff, 0x66, 0x66);
	BlobDetector bdetector = BlobDetector();
	bdetector.addRGBRange(darkred, lightred);

	std::string path = "../card_images/";
	std::string errorMessage;

	Image ime1, imh1, ime2, imh2;

	for (int i = 2; i <= 9; i++) {
		char c = 0x30 + (char)i;
		std::string epath1 = path + "e" + c + "." + "1.pnm";
		std::string epath2 = path + "e" + c + "." + "2.pnm";
		std::string hpath1 = path + "h" + c + "." + "1.pnm";
		std::string hpath2 = path + "h" + c + "." + "2.pnm";


		bool retval = ime1.readPNM(epath1,errorMessage);
		retval &= imh1.readPNM(hpath1,errorMessage);
		retval &= ime2.readPNM(epath2,errorMessage);
		retval &= imh2.readPNM(hpath2,errorMessage);
		if (!retval) {
			std::cout << errorMessage;
			return;
		}

		// Test
		std::vector<BLOB> b1e = bdetector.findBlobs(ime1);
		std::vector<BLOB> b2e = bdetector.findBlobs(ime2);
		std::vector<BLOB> b1h = bdetector.findBlobs(imh1);
		std::vector<BLOB> b2h = bdetector.findBlobs(imh2);

		std::cout << "Should be:" << i << std::endl;
		std::cout << "Is:" << b1e.size() << ',' << b2e.size() << ',' << b1e.size() << ',' << b2e.size() << std::endl;
	}

	// Test Aces
	std::string epath1 = path + "ea." + "1.pnm";
	std::string epath2 = path + "ea." + "2.pnm";
	std::string hpath1 = path + "ha." + "1.pnm";
	std::string hpath2 = path + "ha." + "2.pnm";


	bool retval = ime1.readPNM(epath1,errorMessage);
	retval &= imh1.readPNM(hpath1,errorMessage);
	retval &= ime2.readPNM(epath2,errorMessage);
	retval &= imh2.readPNM(hpath2,errorMessage);
	if (!retval) {
		std::cout << errorMessage;
		return;
	}

	// Test
	std::vector<BLOB> b1e = bdetector.findBlobs(ime1);
	std::vector<BLOB> b2e = bdetector.findBlobs(ime2);
	std::vector<BLOB> b1h = bdetector.findBlobs(imh1);
	std::vector<BLOB> b2h = bdetector.findBlobs(imh2);

	std::cout << "Should be: 1"<< std::endl;
	std::cout << "Is:" << b1e.size() << ',' << b2e.size() << ',' << b1e.size() << ',' << b2e.size() << std::endl;

	// Test Aces
	epath1 = path + "e10." + "1.pnm";
	epath2 = path + "e10." + "2.pnm";
	hpath1 = path + "h10." + "1.pnm";
	hpath2 = path + "h10." + "2.pnm";


	retval = ime1.readPNM(epath1,errorMessage);
	retval &= imh1.readPNM(hpath1,errorMessage);
	retval &= ime2.readPNM(epath2,errorMessage);
	retval &= imh2.readPNM(hpath2,errorMessage);
	if (!retval) {
		std::cout << errorMessage;
		return;
	}

	// Test
	b1e = bdetector.findBlobs(ime1);
	b2e = bdetector.findBlobs(ime2);
	b1h = bdetector.findBlobs(imh1);
	b2h = bdetector.findBlobs(imh2);

	std::cout << "Should be: 10"<< std::endl;
	std::cout << "Is:" << b1e.size() << ',' << b2e.size() << ',' << b1h.size() << ',' << b2h.size() << std::endl;


}

void blackrecognitiontest()
{
	RGB black(0, 0, 0);
	RGB darkgrey(0x50, 0x50, 0x50);
	BlobDetector bdetector = BlobDetector();
	bdetector.addRGBRange(black, darkgrey);

	std::string path = "../card_images/";
	std::string errorMessage;

	Image ime1, imh1, ime2, imh2;

	for (int i = 2; i <= 9; i++) {
		char c = 0x30 + (char)i;
		std::string epath1 = path + "k" + c + "." + "1.pnm";
		std::string epath2 = path + "k" + c + "." + "2.pnm";
		std::string hpath1 = path + "s" + c + "." + "1.pnm";
		std::string hpath2 = path + "s" + c + "." + "2.pnm";


		bool retval = ime1.readPNM(epath1,errorMessage);
		retval &= imh1.readPNM(hpath1,errorMessage);
		retval &= ime2.readPNM(epath2,errorMessage);
		retval &= imh2.readPNM(hpath2,errorMessage);
		if (!retval) {
			std::cout << errorMessage;
			return;
		}

		// Test
		std::vector<BLOB> b1e = bdetector.findBlobs(ime1);
		std::vector<BLOB> b2e = bdetector.findBlobs(ime2);
		std::vector<BLOB> b1h = bdetector.findBlobs(imh1);
		std::vector<BLOB> b2h = bdetector.findBlobs(imh2);

		std::cout << "Should be:" << i << std::endl;
		std::cout << "Is:" << b1e.size() << ',' << b2e.size() << ',' << b1e.size() << ',' << b2e.size() << std::endl;
	}

	// Test Aces
	std::string epath1 = path + "ka." + "1.pnm";
	std::string epath2 = path + "ka." + "2.pnm";
	std::string hpath1 = path + "sa." + "1.pnm";
	std::string hpath2 = path + "sa." + "2.pnm";


	bool retval = ime1.readPNM(epath1,errorMessage);
	retval &= imh1.readPNM(hpath1,errorMessage);
	retval &= ime2.readPNM(epath2,errorMessage);
	retval &= imh2.readPNM(hpath2,errorMessage);
	if (!retval) {
		std::cout << errorMessage;
		return;
	}

	// Test
	std::vector<BLOB> b1e = bdetector.findBlobs(ime1);
	std::vector<BLOB> b2e = bdetector.findBlobs(ime2);
	std::vector<BLOB> b1h = bdetector.findBlobs(imh1);
	std::vector<BLOB> b2h = bdetector.findBlobs(imh2);

	std::cout << "Should be: 1"<< std::endl;
	std::cout << "Is:" << b1e.size() << ',' << b2e.size() << ',' << b1e.size() << ',' << b2e.size() << std::endl;

	// Test Aces
	epath1 = path + "k10." + "1.pnm";
	epath2 = path + "k10." + "2.pnm";
	hpath1 = path + "s10." + "1.pnm";
	hpath2 = path + "s10." + "2.pnm";


	retval = ime1.readPNM(epath1,errorMessage);
	retval &= imh1.readPNM(hpath1,errorMessage);
	retval &= ime2.readPNM(epath2,errorMessage);
	retval &= imh2.readPNM(hpath2,errorMessage);
	if (!retval) {
		std::cout << errorMessage;
		return;
	}

	// Test
	b1e = bdetector.findBlobs(ime1);
	b2e = bdetector.findBlobs(ime2);
	b1h = bdetector.findBlobs(imh1);
	b2h = bdetector.findBlobs(imh2);

	std::cout << "Should be: 10"<< std::endl;
	std::cout << "Is:" << b1e.size() << ',' << b2e.size() << ',' << b1h.size() << ',' << b2h.size() << std::endl;

}

void trainBackground()
{
    BackgroundPerceptron p;
    p.setW(p.readBackgroundWeights("../../weights"));
    
    BackgroundWeights w = p.getW();
    std::cout << std::endl;
    std::cout << "old weights: " << w << std::endl;
    
    w = p.train();
    p.setW(w);
    p.saveWeights("../../weights");
}

void trainNumbers()
{
    NumberPerceptron p;
    //p.setW(p.readNumberWeights("../../Numberweights"));
    
    NumberWeights w = p.getW();
    std::cout << std::endl;
    
    w = p.train();
    p.saveWeights("../../Numberweights");
}


int main(int, const char **)
{
    //trainNumbers();
    cardcheck2();
}
