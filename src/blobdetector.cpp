/*
 * BlobDetector.cpp
 *
 *  Created on: Mar 7, 2018
 *      Author: vgv_05fs18
 */

#include <blobdetector.hpp>

const RGB white = RGB(255,255,255);
const RGB black = RGB(0,0,0);

BlobDetector::BlobDetector()
{
	// TODO Auto-generated constructor stub
}

BlobDetector::~BlobDetector()
{
	// TODO Auto-generated destructor stub
}

void BlobDetector::addRGBRange(RGB start, RGB end)
{
	colorRanges.push_back(RGB_RANGE(start,end));
}

std::vector<BLOB> BlobDetector::findBlobs(Image& imb)
{
	original = imb;
	threshold();
    std::vector<BLOB> blobs;

    bool blob_continuation = false;
    int blobcount = 0;
    int maxblobcount = 0;
    Vector2d blobstart;
    Vector2d blobend;
    
    int blobfield[original.width()][original.height()];
    
    //blobdetection
    
    for (unsigned x = 0; x < original.width(); x++) {
        for (unsigned y = 0; y < original.height(); y++) {
            if (thresholded.at(x,y) == black){ //if pixel is in threshhold
                if(blob_continuation)          //if we were just setting a blob, continue doing that
                    blobfield[x][y] = blobcount; // continuation of a blob
                else
                    if(thresholded.at(x+1,y) == black || thresholded.at(x,y+1) == black){   //might be a new blob
                        blob_continuation = true;
                        if(x!= 0 && blobfield[x-1][y] > 0)         //check previous fields to see if it was a blob
                            blobcount = blobfield[x][y];
                        else if(y!= 0 && blobfield[x][y-1] > 0) // might be ambiguous
                            blobcount = blobfield[x][y-1];
                        else
                            blobcount = ++maxblobcount;      // new blob
                        blobfield[x][y] = blobcount;
                    }
            }else{
                if(blob_continuation){
                    blob_continuation = false;
                }
            }
        }
    }
    
    //TO DO: merging, blobs that are close and taking out blobs that are too small
    
    std::cout << "maxblobcount: " << maxblobcount << std::endl;

	return blobs;
}

void BlobDetector::threshold()
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
