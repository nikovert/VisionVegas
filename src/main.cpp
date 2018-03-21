/*
 *
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#include <image.hpp>

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>

#include <blobdetector.hpp>



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

void thresholdtest()
{
	RGB darkred(0x99, 0, 0);
	RGB lightred(0xff, 0x66, 0x66);
	RGB black(0x0, 0x0, 0x0);
	RGB grey(0x50, 0x50, 0x50);
	BlobDetector bdetector = BlobDetector();
	bdetector.addRGBRange(darkred, lightred);

	std::string path = "../card_images/h8.1.pnm";
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

int main(int, const char **)
{
	//thresholdtest();
	redrecognitiontest();
	blackrecognitiontest();
	return 0;
}

