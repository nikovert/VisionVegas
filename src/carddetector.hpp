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
    //bool** mask(const Point2d &corner1, const Point2d &corner2, const Point2d &corner3, const Point2d &corner4);
};
#endif /* carddetector_h */
