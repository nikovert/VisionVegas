/*
 *
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#include <image.hpp>
#include <card.hpp>
#include <carddetector.hpp>

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <vector>


void error(const char *fmt, ...)
{
    va_list args;
    char buf[4000];
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    std::cerr << buf;
    std::exit(-1);
}

void demo(void)
{
	Card card;
	std::string errmsg;
	if(!card.updateImage(errmsg)) error("%s\n",errmsg.c_str());

	// create a copy of the loaded image
	Image im;
	card.cloneImageTo(im);

	// detect boundary points of the card
	std::vector<Point2d> boundary_points;
	if(!card.detectCardBoundary(boundary_points)) error("unable to detect card boundary\n");
	std::cout << boundary_points.size() << " boundary points\n";

	// draw boundary points on the output image
	for(std::vector<Point2d>::const_iterator it=boundary_points.begin();it!=boundary_points.end();++it)
		im.drawMarker(*it, RGB(255,255,0),1);
    
    Carddetector detector(card);
    detector.isolateCard(boundary_points);
    
    
	// get value of the card
	std::cout << "Card value: " << card.getValue() << "\n";

	// write image to disk
	//if(!im.writePNM("output.pnm",errmsg)) error("%s\n",errmsg.c_str());

	// display image using an external program
	system("eog ./output.pnm");
}

int main(int, const char **)
{
    /*
        Vector3d vec1(4,4,4);
        Matrix3x3 mat1(1,0,0, 0,1,0, 0,0,1);
        Matrix3x3 mat2(11,12,13, 21,22,23, 31,32,33);
        Matrix3x3 mat3(1,1,1, 1,1,1, 1,1,1);
        std::cout << mat2*mat2 <<std::endl;
     */
    
	demo();
	return 0;
}
