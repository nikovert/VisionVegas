/*
 * BlobDetector.h
 *
 *  Created on: Mar 7, 2018
 *      Author: vgv_05fs18
 */

#ifndef BLOBDETECTOR_H_
#define BLOBDETECTOR_H_

#include <vector>
#include "image.hpp"
#include "geom.hpp"

struct RGB_RANGE {
	RGB begin;
	RGB end;
	RGB_RANGE(RGB _begin, RGB _end) : begin(_begin), end(_end) {}
};

struct BLOB {
	Vector2d center;
	unsigned int area;
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
	void AddRGBRange(RGB start, RGB end);
	std::vector<BLOB> FindBlobs(Image& im);

	// Debugging functions
	void retrieveThresholded(Image& ret) {ret = thresholded;}


private:
	std::vector<RGB_RANGE> colorRanges;
	void Threshold();
	Image original;
	Image thresholded;
};

#endif /* BLOBDETECTOR_H_ */
