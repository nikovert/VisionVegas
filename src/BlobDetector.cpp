/*
 * BlobDetector.cpp
 *
 *  Created on: Mar 7, 2018
 *      Author: vgv_05fs18
 */

#include <BlobDetector.hpp>

const RGB white = RGB(255,255,255);
const RGB black = RGB(0,0,0);

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

std::vector<BLOB> BlobDetector::FindBlobs(Image& im)
{
	original = im;
	Threshold();
	// To implement

	std::vector<BLOB> blobs;
	return blobs;
}

void BlobDetector::Threshold()
{
	const unsigned w = original.width();
	const unsigned h = original.height();
	thresholded = Image(w,h,white);

	for (unsigned x = 0; x < w; x++) {
		for (unsigned y = 0; y < h; y++) {
			bool flag = false;
			RGB pixel = original.at(x,y);
			for (std::vector<RGB_RANGE>::iterator range = colorRanges.begin(); range != colorRanges.end(); range++) {
				if (range->begin <= pixel && pixel <= range->end) {
					flag = true;
					break;
				}
			}
			if (flag) thresholded.at(x,y) = black;
		}
	}


}
