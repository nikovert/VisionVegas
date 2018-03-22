//
//  carddetector.h
//  PPS_JASS
//
//  Created by Niko Vertovec on 07.03.18.
//

#ifndef __CARDDETECTOR__
#define __CARDDETECTOR__

#include <card.hpp>
#include <vector>

#include <blobdetector.hpp>

/*
 Please use as follows
 
 Carddetector detector(card);   //init
 detector.isolateCard();        //isolate
 detector.initBlobdetection();   //configure the blobdetection
 detetor.detectBlobs();         //detect blobs
 detector.retrieveCrop(im);     //retrieve
 */
class Carddetector
{
private:
    Image crop;
    BinaryImage binmask;
    Image mask;
    bool debug;
public:
    std::string currentCard;
    Card playingcard;
    Carddetector(Card& ca) {playingcard = ca; debug = false; blob = BlobDetector();}
    void setdebug() {debug = true;}
    bool isdebug() {return debug;}
    bool isolateCard();
    bool maskCard();
    bool simpleMask();
    void retrieveCrop(Image& im) {im = crop;}
    
    BlobDetector blob;
    void initBlobdetection() {blob.reset(); blob.adddefaultRange();};
    void detectBlobs() {blob.findBlobs(crop); blob.retrieveBlobed(crop);}
};
#endif /* carddetector_h */
