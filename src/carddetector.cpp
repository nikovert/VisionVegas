//
//  carddetection.cpp
//  recognition
//
//  Created by Niko Vertovec on 01.03.18.
//

#include <carddetector.hpp>

Point2d mintranslation(Point2d corner1,Point2d corner2,Point2d corner3,Point2d corner4);

bool Carddetector::mask(){
    return false;
}

bool Carddetector::isolateCard(std::vector<Point2d> boundary_points)
{
    Image pic;
    playingcard.cloneImageTo(pic);
    
    // detect corners
    // find consecutive collinear points and fit a line to them
    std::vector<Point2d>::const_iterator it=boundary_points.begin();
    //Line 1
    while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
    Line2d line1(*it,*(it+4));
    // find the next point not on the line
    while (line1.distance(*it) < 20) ++it;
    
    // find the next consecutive set of collinear points and fit another line to them
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
    
    // draw lines
    pic.drawLine(line1, RGB(0,0,0));     // 1 -> 2
    pic.drawLine(line2, RGB(0,255,255)); // 2 -> 3
    pic.drawLine(line3, RGB(0,255,0));   // 3 -> 4
    pic.drawLine(line4, RGB(0,0,255));   // 4 -> 1
    
    // intersect lines and draw intersection
    Point2d corner1, corner2, corner3, corner4;
    /*
     1-------2
     |       |
     |       |
     |       |
     |       |
     4-------3
     */
    
    //Mark corners
    line1.intersect(line4, corner1);
    pic.drawMarker(corner1, RGB(255,0,0), 2);
    
    line1.intersect(line2, corner2);
    pic.drawMarker(corner2, RGB(255,0,0), 2);
    
    line2.intersect(line3, corner3);
    pic.drawMarker(corner3, RGB(255,0,0), 2);
    
    line4.intersect(line3, corner4);
    pic.drawMarker(corner4, RGB(255,0,0), 2);
    
    //Mark card boundaries
    Line2d top(corner1, corner2), bottom(corner4, corner3), left(corner1, corner4), right(corner2, corner3);
    pic.drawLine(top, RGB(255,125,125));
    pic.drawLine(bottom, RGB(255,125,125));
    pic.drawLine(left, RGB(255,125,125));
    pic.drawLine(right, RGB(255,125,125));
    
    //determine if we have to flip by 90 degrees.
    bool flip = line1 > line4;
    
    //calculate the rotation angle
    double rotation_angle_error = (line4.angle()-180 - line2.angle())/2;
    if(rotation_angle_error > 5){
        std::cerr << "Angles don't match!" << std::endl;
        return false;
    }
    double rotation_angle = -(line2.angle() + rotation_angle_error/2);
    if(flip) rotation_angle+= 90;
    std::cout << "rotate by " << rotation_angle << std::endl;
    
    //calculate the translation
    Point2d translation = mintranslation(corner1,corner2,corner3,corner4);
    std::cout << "translation: " << translation << std::endl;
    
    
    
    
    //wirte image
    std::string errmsg;
    if(!pic.writePNM("output.pnm",errmsg)) return false;
    
    return true;
}

Point2d mintranslation(Point2d corner1,Point2d corner2,Point2d corner3,Point2d corner4){
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



