//
//  carddetector.h
//  PPS_JASS
//
//  Created by Niko Vertovec on 07.03.18.
//

#ifndef __CARDDETECTOR__
#define __CARDDETECTOR__

#include <image.hpp>
#include <card.hpp>
#include <vector>

/*
 Please use as follows
 
 Carddetector detector(card);   //init
 detector.isolateCard();        //isolate
 detector.retrieveCrop(im);     //retrieve
 */
class Carddetector
{
private:
    Image crop;
    bool debug;
public:
    Card playingcard;
    Carddetector(Card& ca) {playingcard = ca;}
    void setdebug() {debug = true;}
    bool isolateCard();
    void retrieveCrop(Image& im) {im = crop;}
};
#endif /* carddetector_h */
