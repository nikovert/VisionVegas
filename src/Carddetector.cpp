//
//  carddetection.cpp
//  recognition
//
//  Created by Niko Vertovec on 01.03.18.
//

#include <carddetector.hpp>
#define PI 3.14159265

Point2d mintranslation(Vector3d corner1,Vector3d corner2,Vector3d corner3,Vector3d corner4);

bool Carddetector::isolateCard()
{
    Image pic;
    playingcard.cloneImageTo(pic);
    
    //______________detect corners__________________________________________________________________
    // detect boundary points of the card
        std::vector<Point2d> boundary_points;
        if(!playingcard.detectCardBoundary(boundary_points)) return false;
        //std::cout << boundary_points.size() << " boundary points\n";
    
    // find consecutive collinear points and fit a line to them
        std::vector<Point2d>::const_iterator it=boundary_points.begin();
    
    //Line 1
        while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
        Line2d line1(*it,*(it+4));
        // find the next point not on the line
        while (line1.distance(*it) < 20) ++it;
    
    //Line 2
        while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
        Line2d line2(*it,*(it+4));
        while (line2.distance(*it) < 20) ++it;
    
    //Line 3
        while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
        Line2d line3(*it,*(it+4));
        while (line3.distance(*it) < 20) ++it;
    
    // //Line 4
        while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
        Line2d line4(*it,*(it+4));
    
    /*
    // draw lines
    pic.drawLine(line1, RGB(0,0,0));     // 1 -> 2
    pic.drawLine(line2, RGB(0,255,255)); // 2 -> 3
    pic.drawLine(line3, RGB(0,255,0));   // 3 -> 4
    pic.drawLine(line4, RGB(0,0,255));   // 4 -> 1
    */
    
    // intersect lines and draw intersection
    Point2d corner1, corner2, corner3, corner4;
    Point2d tmpcorner1, tmpcorner2, tmpcorner3, tmpcorner4;
    /*
     1-------2
     |       |
     |       |
     |       |
     |       |
     4-------3
     */
    
    //find intersections
        line1.intersect(line4, tmpcorner1);
        line1.intersect(line2, tmpcorner2);
        line2.intersect(line3, tmpcorner3);
        line4.intersect(line3, tmpcorner4);
    
    //swap definition of corners as necessary
        if(tmpcorner1.x < tmpcorner2.x){
            corner1 = tmpcorner1;
            corner2 = tmpcorner2;
            corner3 = tmpcorner3;
            corner4 = tmpcorner4;
        }
        else{
            corner1 = tmpcorner2;
            corner2 = tmpcorner1;
            corner3 = tmpcorner4;
            corner4 = tmpcorner3;
        }
    
    Line2d top(corner1, corner2), bottom(corner4, corner3), left(corner1, corner4), right(corner2, corner3);
        /*
        //Mark card corners
        pic.drawMarker(corner1, RGB(255,255,0), 2);
        pic.drawMarker(corner2, RGB(255,0,0), 2);
        pic.drawMarker(corner3, RGB(255,0,0), 2);
        pic.drawMarker(corner4, RGB(255,0,255), 2);
        
        //Mark card boundaries
        pic.drawLine(top, RGB(255,125,125));
        pic.drawLine(bottom, RGB(255,125,125));
        pic.drawLine(left, RGB(255,125,125));
        pic.drawLine(right, RGB(255,125,125));
         */
    
    //Get dimesnions of the card
        unsigned cardHeight, cardWidth;
        if(top.length() > bottom.length())
            cardWidth = (unsigned) (top.length() + 1);
        else
            cardWidth = (unsigned) (bottom.length() + 1);
        if(left.length() > right.length())
            cardHeight = (unsigned) (left.length() + 1);
        else
            cardHeight = (unsigned) (right.length() + 1);
    
    //______________determine rotation matrix__________________________________________________________________
    //calculate the rotation angle
    double line4_angle = line4.angle() - 180.0; //-180. because the line shows in the other direction
    if(line4_angle < 0) line4_angle += 360.0;
    double line2_angle = line2.angle();
    if(line2_angle < 0) line2_angle += 360.0;
    
    double rotation_angle_error = abs(line4_angle - line2_angle)/2;
    std::cout << "line4 angle: " << line4_angle << std::endl;
    std::cout << "line2 angle: " << line2_angle << std::endl;
    std::cout << "Rotation angle error: " << rotation_angle_error << std::endl;
    if(rotation_angle_error > 175.0) rotation_angle_error = abs(rotation_angle_error-180);
    if(abs(rotation_angle_error) > 5.0){
        std::cout << "Angles don't match!" << std::endl;
        return false;
    }
    
    //determine if we have to flip by 90 degrees.
    bool flip = top > left;
    
    double rotation_angle = (line2_angle + line4_angle)/2;
    if(flip) rotation_angle += 90;
    std::cout << "flip: " << flip << std::endl;
    std::cout << "rotate by " << rotation_angle << std::endl;
    
    rotation_angle = rotation_angle *(PI/180);
    Matrix3x3 rotationMatrix(cos(rotation_angle),-sin(rotation_angle),0, sin(rotation_angle),cos(rotation_angle),0, 0,0,1);
    
    //______________determine translation matrix__________________________________________________________________
    Point2d center;
    Line2d crossLine1(corner1, corner3);
    Line2d crossLine2(corner2, corner4);
    crossLine1.intersect(crossLine2, center);
    Matrix3x3 centerTranslationMatrix(1,0,-center.x, 0,1,-center.y, 0,0,1);
    Matrix3x3 centerTranslationMatrixBack(1,0,center.x, 0,1,center.y, 0,0,1);
    
    //calculate the translation
    Vector3d cornerVec1(corner1.x, corner1.y, 1);
    Vector3d cornerVec2(corner2.x, corner2.y, 1);
    Vector3d cornerVec3(corner3.x, corner3.y, 1);
    Vector3d cornerVec4(corner4.x, corner4.y, 1);
    
    cornerVec1 = centerTranslationMatrixBack * rotationMatrix * centerTranslationMatrix * cornerVec1;
    cornerVec2 = centerTranslationMatrixBack * rotationMatrix * centerTranslationMatrix * cornerVec2;
    cornerVec3 = centerTranslationMatrixBack * rotationMatrix * centerTranslationMatrix * cornerVec3;
    cornerVec4 = centerTranslationMatrixBack * rotationMatrix * centerTranslationMatrix * cornerVec4;
    
    Point2d translation = mintranslation(cornerVec1,cornerVec2,cornerVec3,cornerVec4);
    std::cout << "translation: " << translation << std::endl;
    
    Matrix3x3 translationMatrix(1,0,-translation.x, 0,1,-translation.y, 0,0,1);
    
    Image cardimage;
    if(cardHeight < cardWidth){
        unsigned tmp = cardHeight;
        cardHeight = cardWidth;
        cardWidth = tmp;
    }
    //tmp, shouldn't be needed
    if(debug){
        cardHeight = pic.height();
        cardWidth = pic.width();
    }
    cardimage.create(cardWidth, cardHeight, RGB(0,0,0));
    
    std::cout << "cardWidth: " << cardWidth << " cardHeight: " << cardHeight << std::endl;
    for(unsigned x0 = 1; x0 < pic.width(); x0++){
        for(unsigned y0 = 1; y0 < pic.height(); y0++){
            Point2d pixel(x0, y0);
            if(inRectangle(pixel, corner1, corner2, corner3, corner4)){ // only if the pixel is part of the card;
                Vector3d pixel3d(x0,y0,1);
                Vector3d newPixel;
                
                if(!debug)
                    newPixel = translationMatrix * centerTranslationMatrixBack * rotationMatrix * centerTranslationMatrix * pixel3d;
                else
                    newPixel = pixel3d;
                
                if(newPixel.x >= cardimage.width() || newPixel.y >= cardimage.height()){
                    std::cout << "out of dimensions: " << newPixel.x << " " << newPixel.y << std::endl;
                    return false;
                }
                cardimage.at(newPixel.x, newPixel.y) = pic.at(x0, y0);
            }
        }
    }
    std::string errmsg;
    crop = cardimage;
    return true;
}

Point2d mintranslation(Vector3d corner1,Vector3d corner2,Vector3d corner3,Vector3d corner4){
    //x
    double xtrans;
        if(corner1.x > corner2.x)
            xtrans = corner2.x;
        else
            xtrans = corner1.x;
        if(xtrans > corner3.x)
            xtrans = corner3.x;
        if(xtrans > corner4.x)
            xtrans = corner4.x;
    //y
    double ytrans;
    if(corner1.y > corner2.y)
        ytrans = corner2.y;
    else
        ytrans = corner1.y;
    if(ytrans > corner3.y)
        ytrans = corner3.y;
    if(ytrans > corner4.y)
        ytrans = corner4.y;
    return Point2d(xtrans, ytrans);
}
