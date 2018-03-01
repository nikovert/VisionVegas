//
//  carddetection.cpp
//  recognition
//
//  Created by Niko Vertovec on 01.03.18.
//

#include <card.hpp>

// Based on the blog: http://arnab.org/blog/so-i-suck-24-automating-card-games-using-opencv-and-python

/*
Once we have the contours of the card, we can go about identifying the rectangle representation of each card.
This is done by approximating a polynomial from the contour (which is simply a vector of points) and then finding the minimum rotation-free bounding box:
 In python:
     for i in range(numcards):
     card = contours[i]
     peri = cv2.arcLength(card,True)
     approx = cv2.approxPolyDP(card,0.02*peri,True)
     rect = cv2.minAreaRect(contours[2])
     r = cv2.cv.BoxPoints(rect)
 
 Then, for each rectangle, we perform an Affine Transform to transform the image of each card into a rectangular representation, so that it can be easily recognized:
  In python:
     h = np.array([ [0,0],[449,0],[449,449],[0,449] ],np.float32)
     transform = cv2.getPerspectiveTransform(approx,h)
     warp = cv2.warpPerspective(im,transform,(450,450))
*/

bool isolateCard(std::vector<Point2d>& boundary_points)
{
    return false;
}



