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


class Carddetector
{
public:
    Card playingcard;
    Carddetector(Card& ca) {playingcard = ca;}
    bool isolateCard(std::vector<Point2d> boundary_points);
    bool mask();
};
#endif /* carddetector_h */
