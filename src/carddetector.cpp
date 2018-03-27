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
    if(!isolateCard_Rotationonly()){
        return false;
    }
    playingcard.setImage(crop);
    
    if(!isolateCard_Translationonly()){
        return false;
    }
    playingcard.setImage(crop);
    
    Image pic;
    playingcard.cloneImageTo(pic);
    if(!pic.isAllocated()){
        std::cerr << "ERROR in isolateCard: pic isn't Allocated" << std::endl;
        return false;
    }
    Point2d cropValue(0,0);
    
    for(unsigned x0 = pic.width()-1; x0 > 0; x0--){
        for(unsigned y0 = pic.height()-1; y0 > 0; y0--){
            if(!(pic.at(x0, y0) == defaultBackround)){
                if(cropValue.x < x0)
                    cropValue.x = x0;
                if(cropValue.y < y0)
                    cropValue.y = y0;
            }
        }
    }
    std::cout << "cropValue: " << cropValue << std::endl;
    
    crop.create(cropValue.x, cropValue.y);
    
    for(unsigned x0 = 1; x0 < crop.width(); x0++){
        for(unsigned y0 = 1; y0 < crop.height(); y0++){
            crop.at(x0, y0) = pic.at(x0, y0);
        }
    }
    return true;
}

bool Carddetector::detectCard(std::vector<Point2d>& boundary_points, double distance, double delta_angle, unsigned max_points)
{
    Image im;
    playingcard.cloneImageTo(im); //copies image to pic
    
    simpleMask(); //creates mask
    
    boundary_points.clear();
    if(!im.isAllocated()){
        std::cerr << "ERROR in detectCard: im not allocated" << std::endl;
        return false;
    }
    
    const double pi = 3.141592653589793;
    delta_angle *= pi/180.0;
    
    // find seed point
    unsigned x0 = im.width()/2;
    unsigned y0 = 0;
    while(y0<im.height() && !binmask.at(x0, y0)) y0++;
    if(y0<=0 && y0>=im.height()-1){
        //try alternative seed
        x0 = im.width()/4;
        y0 = 0;
        while(y0<im.height() && !binmask.at(x0, y0)) y0++;
        if(y0<=0 && y0>=im.height()-1){
            //try alternative seed
            x0 = im.width()*3/4;
            y0 = 0;
            while(y0<im.height() && !binmask.at(x0, y0)) y0++;
            if(y0<=0 && y0>=im.height()-1){
                std::cerr << "ERROR in detectCard: y0 >0 or y0 > im.height()-1" << std::endl;
                return false;
            }
        }
    }
    Point2d seed_point(x0,y0);
    boundary_points.push_back(seed_point);
    
    // trace boundary
    Point2d point = seed_point;
    double angle = 0;
    do
    {
        // search card counter-clockwise (starting from previous angle)
        while((point.x + distance*cos(angle)) < binmask.width() && (point.y + distance*sin(angle)) < binmask.height() && (point.x + distance*cos(angle)) > 0 && (point.y + distance*sin(angle)) > 0 && binmask.at(point.x + distance*cos(angle), point.y + distance*sin(angle)))
        {
            angle += delta_angle;
            if(angle>4*pi){
                std::cerr << "ERROR in detectCard: angle > 4*pi" << std::endl;
                return false;
            }
        }
        
        // search background clockwise
        while((point.x + distance*cos(angle)) < binmask.width() && (point.y + distance*sin(angle)) < binmask.height() && (point.x + distance*cos(angle)) > 0 && (point.y + distance*sin(angle)) > 0 && !binmask.at(point.x + distance*cos(angle), point.y + distance*sin(angle)))
        {
            angle -= delta_angle;
            if(angle<-4*pi){
                std::cerr << "ERROR in detectCard: angle > -4*pi" << std::endl;
                return false;
            }
        }
        
        // add point to the list of boundary points
        point = Point2d(point.x + distance*cos(angle), point.y + distance*sin(angle));
        boundary_points.push_back(point);
        if(boundary_points.size() > max_points){
            std::cerr << "ERROR in detectCard: located to many boundary points" << std::endl;
            return false;
        }
        
    } while (boundary_points.size()<3 || seed_point.distance(point) > 1.1 * distance);
    
    return true;
}

bool Carddetector::isolateCard_Translationonly()
{
    Image pic;
    playingcard.cloneImageTo(pic); //copies image to pic
    
    std::vector<Point2d> corners = detectCorners();
    if(corners.empty()){
        return false;
    }
    Point2d corner1 = corners.back(); corners.pop_back();
    Point2d corner2 = corners.back(); corners.pop_back();
    Point2d corner3 = corners.back(); corners.pop_back();
    Point2d corner4 = corners.back(); corners.pop_back();
    
    
    //______________determine translation matrix__________________________________________________________________
    Line2d crossLine1(corner1, corner3);
    Line2d crossLine2(corner2, corner4);
    
    //calculate the translation
    Vector3d cornerVec1(corner1.x, corner1.y, 1);
    Vector3d cornerVec2(corner2.x, corner2.y, 1);
    Vector3d cornerVec3(corner3.x, corner3.y, 1);
    Vector3d cornerVec4(corner4.x, corner4.y, 1);
    
    Point2d translation = mintranslation(cornerVec1,cornerVec2,cornerVec3,cornerVec4);
    std::cout << "translation: " << translation << std::endl;
    
    Matrix3x3 translationMatrix(1,0,-translation.x, 0,1,-translation.y, 0,0,1);
    
    bool draw = true;
    if(draw){
        pic.drawMarker(corner1, RGB(100, 200, 0));
        pic.drawMarker(corner2, RGB(100, 200, 0));
        pic.drawMarker(corner3, RGB(100, 200, 0));
        pic.drawMarker(corner4, RGB(100, 200, 0));
        
    }
    
    Image cardimage;
    
    cardimage.create(pic.width(), pic.height(), defaultBackround); //set default backround
    
    std::cerr << "writing Card: " << currentCard;
    for(unsigned x0 = 1; x0 < pic.width(); x0++){
        for(unsigned y0 = 1; y0 < pic.height(); y0++){
            Point2d pixel(x0, y0);
            if(inRectangle(pixel, corner1, corner2, corner3, corner4) && binmask.at(x0, y0)){ // only if the pixel is part of the card;
                Vector3d pixel3d(x0,y0,1);
                Vector3d newPixel;
                if(!debug)
                    newPixel = translationMatrix * pixel3d;
                else
                    newPixel = pixel3d;
                
                if(newPixel.x > cardimage.width() || newPixel.y > cardimage.height() || newPixel.x < 0 || newPixel.y < 0){
                    std::cerr << "out of dimensions: " << newPixel.x << " " << newPixel.y << std::endl;
                    std::cerr << std::endl;
                    return false;
                }
                cardimage.at(newPixel.x, newPixel.y) = pic.at(x0, y0);
            }
        }
    }
    std::string errmsg;
    crop = cardimage;
    
    std::cerr << " ... success!" << std::endl;
    return true;
}

bool Carddetector::isolateCard_Rotationonly()
{
    Image pic;
    playingcard.cloneImageTo(pic); //copies image to pic
    
    std::vector<Point2d> corners = detectCorners();
    if(corners.empty()){
        return false;
    }
    Point2d corner1 = corners.back(); corners.pop_back();
    Point2d corner2 = corners.back(); corners.pop_back();
    Point2d corner3 = corners.back(); corners.pop_back();
    Point2d corner4 = corners.back(); corners.pop_back();
    
    Line2d top(corner1, corner2), bottom(corner4, corner3), left(corner1, corner4), right(corner2, corner3);
    
    Point2d Angleline2Point1 = corners.back(); corners.pop_back();
    Point2d Angleline2Point2 = corners.back(); corners.pop_back();
    
    Point2d Angleline4Point1 = corners.back(); corners.pop_back();
    Point2d Angleline4Point2 = corners.back(); corners.pop_back();
    
    Line2d line2(Angleline2Point1, Angleline2Point2), line4(Angleline4Point1, Angleline4Point2);
    
    //______________determine rotation matrix__________________________________________________________________
    //calculate the rotation angle
    double line4_angle = line4.angle() - 180.0; //-180. because the line shows in the other direction
    if(line4_angle < 0) line4_angle += 360.0;
    double line2_angle = line2.angle();
    if(line2_angle < 0) line2_angle += 360.0;
    
    double rotation_angle_error = std::abs(line4_angle - line2_angle)/2;
    std::cout << "line4 angle: " << line4_angle << std::endl;
    std::cout << "line2 angle: " << line2_angle << std::endl;
    std::cout << "Rotation angle error: " << rotation_angle_error << std::endl;
    if(rotation_angle_error > 175.0) rotation_angle_error = std::abs(rotation_angle_error-180);
    if(std::abs(rotation_angle_error) > 5.0){
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
    
    Image cardimage;
    
    cardimage.create(pic.width(), pic.height(), defaultBackround); //set default backround
    
    std::cerr << "writing Card: " << currentCard;
    for(unsigned x0 = 1; x0 < pic.width(); x0++){
        for(unsigned y0 = 1; y0 < pic.height(); y0++){
            Point2d pixel(x0, y0);
            if(inRectangle(pixel, corner1, corner2, corner3, corner4) && binmask.at(x0, y0)){ // only if the pixel is part of the card;
                Vector3d pixel3d(x0,y0,1);
                Vector3d newPixel;
                if(!debug)
                    newPixel = centerTranslationMatrixBack * rotationMatrix * centerTranslationMatrix * pixel3d;
                else
                    newPixel = pixel3d;
                
                if(newPixel.x > cardimage.width() || newPixel.y > cardimage.height() || newPixel.x < 0 || newPixel.y < 0){
                    std::cerr << "out of dimensions: " << newPixel.x << " " << newPixel.y << std::endl;
                    std::cerr << std::endl;
                    return false;
                }
                cardimage.at(newPixel.x, newPixel.y) = pic.at(x0, y0);
            }
        }
    }
    std::string errmsg;
    crop = cardimage;
    
    std::cerr << " ... success!" << std::endl;
    return true;
}

std::vector<Point2d> Carddetector::detectCorners()
{
    //______________detect corners__________________________________________________________________
    // detect boundary points of the card
    std::vector<Point2d> corners;
    std::vector<Point2d> boundary_points;
    if(!detectCard(boundary_points)) return corners;
    std::cout << boundary_points.size() << " boundary points\n";
    
    // find consecutive collinear points and fit a line to them
    std::vector<Point2d>::const_iterator it=boundary_points.begin();
    
    //Line 1
    while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
    Line2d line1(*it,*(it+4));
    // find the next point not on the line
    while (line1.distance(*it) < 20) ++it;
    
    //Line 2
    while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
    Point2d Angleline2Point1 = *it; //to return the line for rotation angle detection
    Point2d Angleline2Point2 = *(it+4);
    Line2d line2(*it,*(it+4));
    while (line2.distance(*it) < 20) ++it;
    
    //Line 3
    while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
    Line2d line3(*it,*(it+4));
    while (line3.distance(*it) < 20) ++it;
    
    // //Line 4
    while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
    Point2d Angleline4Point1 = *it; //to return the line for rotation angle detection
    Point2d Angleline4Point2 = *(it+4);
    Line2d line4(*it,*(it+4));
    
    // intersect lines and draw intersection
    Point2d corner1, corner2, corner3, corner4;
    Point2d tmpcorner1, tmpcorner2, tmpcorner3, tmpcorner4;
    
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
    
    corners.push_back(Angleline4Point2);
    corners.push_back(Angleline4Point1);
    corners.push_back(Angleline2Point2);
    corners.push_back(Angleline2Point1);
    
    corners.push_back(corner4);
    corners.push_back(corner3);
    corners.push_back(corner2);
    corners.push_back(corner1);
    
    /*
    std::cerr << "corner1: " << corner1 << std::endl;
    std::cerr << "corner2: " << corner2 << std::endl;
    std::cerr << "corner3: " << corner3 << std::endl;
    std::cerr << "corner4: " << corner4 << std::endl;
    
    std::cerr << "Angleline2Point1: " << Angleline2Point1 << std::endl;
    std::cerr << "Angleline2Point2: " << Angleline2Point2 << std::endl;
    std::cerr << "Angleline4Point1: " << Angleline4Point1 << std::endl;
    std::cerr << "Angleline4Point2: " << Angleline4Point2 << std::endl;
    */
    
    return corners;
}



//overwrites mask
bool Carddetector::fillHoles()
{
    /*
     Flood-fill (node, target-color, replacement-color):
     1. If target-color is equal to replacement-color, return.
     2. If color of node is not equal to target-color, return.
     3. Set Q to the empty queue.
     4. Add node to Q.
     5. For each element N of Q:
     6.         Set w and e equal to N.
     7.         Move w to the west until the color of the node to the west of w no longer matches target-color.
     8.         Move e to the east until the color of the node to the east of e no longer matches target-color.
     9.         For each node n between w and e:
     10.             Set the color of n to replacement-color.
     11.             If the color of the node to the north of n is target-color, add that node to Q.
     12.             If the color of the node to the south of n is target-color, add that node to Q.
     13. Continue looping until Q is exhausted.
     14. Return.
     */
    
    //simpleMask needs to have been perfomed on mask
    if(!mask.isAllocated()) return false;
    RGB target(0, 0, 0);
    RGB replace(150,150,0);
    
    std::queue<Point2d> queue; //Step 3
    queue.push(Point2d(0,0)); //Step 4
    
    do{                         //Step 5
        Point2d node = queue.front();
        queue.pop();
        int w(node.x), e(node.x), y0(node.y);  //Step 6
        mask.at(w, y0) = replace;
        while(--w > 0 && mask.at(w, y0) == target){          //Step 7
            mask.at(w, y0) = replace;    //Step 10
            if(y0 > 0 && mask.at(w, y0-1) == target)   //Step 11
                queue.push(Point2d(w,y0-1));
            if(y0 < mask.height()-1 && mask.at(w, y0+1) == target)   //Step 12
                queue.push(Point2d(w,y0+1));
        }
        while(++e < mask.width()-1 && mask.at(e+1, y0) == target){      //Step 8
            mask.at(e, y0) = replace;    //Step 10
            if(y0 > 0 && mask.at(e, y0-1) == target)   //Step 11
                queue.push(Point2d(w,y0-1));
            if(y0 < mask.height()-1 && mask.at(e, y0+1) == target)   //Step 12
                queue.push(Point2d(e,y0+1));
        }
    }while (queue.size() > 0);
    if(!updateMask(replace)) return false;
    return true;
}

bool Carddetector::updateMask(RGB replace) //follow up of fill holes
{
    binmask.create(mask.width(), mask.height());
    if(!mask.isAllocated()){
        std::cerr << "ERROR in updateMask: mask isn't Allocated" << std::endl;
        return false;
        
    }
    for(unsigned x0 = 1; x0 < mask.width(); x0++){
        for(unsigned y0 = 1; y0 < mask.height(); y0++){
            binmask.setPixel(x0, y0, !(mask.at(x0, y0) == replace));
        }
    }
    mask = binmask;
    return true;
}

bool Carddetector::simpleMask()
{
    Image pic;
    playingcard.cloneImageTo(pic);
    
    if(!pic.isAllocated())
        std::cerr << "ERROR in simpleMask: pic isn't Allocated" << std::endl;
    
    BinaryImage maskimage(pic.width(), pic.height());
    for(unsigned x0 = 1; x0 < pic.width(); x0++){
        for(unsigned y0 = 1; y0 < pic.height(); y0++){
            Point2d pixel(x0, y0);
            if(playingcard.isBackground(pixel)){ // only if the pixel is part of the card;
                maskimage.setPixel(x0, y0, false);
            }
            else{
                maskimage.setPixel(x0, y0, true);
            }
        }
    }
    mask = maskimage;
    if(!fillHoles()) return false;     //fillHoles of mask
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
