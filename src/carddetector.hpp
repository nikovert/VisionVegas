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
#include <queue>

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
    BinaryImage binmask; //holds binary version of mask //true for card, false for backround
    Image mask;
    bool debug;
public:
    std::string currentCard;
    Card playingcard;
    
private:
    bool fillHoles();
    bool updateMask(RGB replace);// Sets binmask to maks
    bool detectCard(std::vector<Point2d>& boundary_points, double distance=20, double delta_angle=1.0, unsigned max_points=500); //used by  isolate
public:
    Carddetector(Card& ca) {playingcard = ca; debug = false; blob = BlobDetector();}
    
    void setdebug() {debug = true;}
    bool isdebug() {return debug;}
    
    bool isolateCard();
    bool simpleMask();
    void retrieveCrop(Image& im) {im = crop;}
    void retrieveMask(Image& im) {im = mask;}
    void retrieveMask(BinaryImage& im) {im = binmask;}
    
    BlobDetector blob;
    void initBlobdetection() {blob.reset(); blob.adddefaultRange();}
    void detectBlobs() {blob.findBlobs(crop); blob.retrieveBlobed(crop);}
};
#endif /* carddetector_h */
