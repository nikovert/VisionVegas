/*
 * BlobDetector.cpp
 *
 *  Created on: Mar 7, 2018
 *      Author: vgv_05fs18
 */

#include <BlobDetector.h>

BlobDetector::BlobDetector() {
	// TODO Auto-generated constructor stub

}

BlobDetector::~BlobDetector() {
	// TODO Auto-generated destructor stub
}

void BlobDetector::AddRGBRange(RGB start, RGB end)
{
	colorRanges.push_back(RGB_RANGE(start,end));
}

std::vector<BLOB> BlobDetector::FindBlobs(Image im)
{
	// To implement

	std::vector<BLOB> blobs;
	return blobs;
}

void BlobDetector::Threshold()
{
	// To implement
}
