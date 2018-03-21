/*
 * BlobDetector.h
 *
 *  Created on: Mar 7, 2018
 *      Author: vgv_05fs18
 */

#ifndef BLOBDETECTOR_H_
#define BLOBDETECTOR_H_

#include <vector>
#include <image.hpp>

struct RGB_RANGE {
	RGB begin;
	RGB end;
	RGB_RANGE(RGB _begin, RGB _end) : begin(_begin), end(_end) {}
};

struct BLOB {
	Vector2d center;
	unsigned int area;
	BLOB(Vector2d _c, unsigned int _a) : center(_c), area(_a) {}
};

/*
 * BlobDetector detects blobs with distinctive color features
 * To initialize the detector, color ranges have to be added.
 * Objects having this color will be detected
 */
class BlobDetector {
public:
	BlobDetector();
	virtual ~BlobDetector();
public:
	// Adds a Range of colors for thresholding
	void addRGBRange(RGB start, RGB end);
	std::vector<BLOB> findBlobs(Image& im);

	// Debugging functions
	void retrieveThresholded(Image& ret) {ret = thresholded;}
    void retrieveBlobed(Image& ret) {ret = blobed;}
    void adddefaultRange() {addRGBRange(black, grey); addRGBRange(darkred, lightred);}
    void reset() {original.destroy(); blobed.destroy(); thresholded.destroy();}
private:
	std::vector<RGB_RANGE> colorRanges;
	void threshold();
	Image blobed;
    Image original;
	Image thresholded;
    
    //some  RGB colors
    RGB darkred = RGB(150, 0, 0);
    RGB lightred = RGB(255, 100, 100);
    RGB black = RGB(0,0,0);
    RGB grey = RGB(50,50,50);
};

#endif /* BLOBDETECTOR_H_ */
