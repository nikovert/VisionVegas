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
#include <stdexcept>

#include <blobdetector.hpp>

typedef enum CARD_TYPE {jqk, number, ace};

struct card_value {
	CARD_TYPE type;
	int val;
};

class Carddetector
{
private:
    Image crop;
    BinaryImage binmask; //holds binary version of mask //true for card, false for backround
    Image mask;
    Image value;
    bool debug;
    RGB defaultBackround = RGB(0, 0, 0);
    std::vector<RGB_RANGE> colorRanges;
    
    //some  RGB colors
    RGB darkred = RGB(150, 0, 0);
    RGB lightred = RGB(255, 100, 100);
    RGB black = RGB(0,0,0);
    RGB grey = RGB(50,50,50);
    
public:
    std::string currentCard;
    Card playingcard;
    
private:
    bool fillHoles(RGB target = RGB(0, 0, 0), RGB replace = RGB(150,150,0));
    bool updateMask(RGB replace);// Sets binmask to maks
    bool detectCard(std::vector<Point2d>& boundary_points, double distance=20, double delta_angle=1.0, unsigned max_points=500); //used by  isolate
    bool isolateCard_Translationonly(); //used by isolate
    bool isolateCard_Rotationonly(); //used by isolate
    std::vector<Point2d> detectCorners(); //finds the corners, used by isolate
    
public:
    Carddetector(Card& ca) {playingcard = ca; debug = false; blob = BlobDetector();}
    
    void setdebug() {debug = true;}
    bool isdebug() {return debug;}
    bool isolateCard();
    bool simpleMask();  //creates a mask from the playingcard image
    void retrieveCrop(Image& im) {im = crop;}
    void retrieveMask(Image& im) {im = mask;}
    void retrieveValue(Image& im) {im = value;}
    void retrieveMask(BinaryImage& im) {im = binmask;}
    void setCard(Card& card) {playingcard = card;}
    BlobDetector blob;
    void initBlobdetection() {blob.reset(); blob.adddefaultRange();}
    void detectBlobs() {blob.findBlobs(crop); blob.retrieveBlobed(crop);}
    bool isolateValue();
};
#endif /* carddetector_h */
