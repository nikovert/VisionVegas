/*
 *
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#include <image.hpp>
#include <card.hpp>

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

	// find consecutive collinear points and fit a line to them
	std::vector<Point2d>::const_iterator it=boundary_points.begin();
	while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
	Line2d line1(*it,*(it+4));

	// find the next point not on the line
	while (line1.distance(*it) < 20) ++it;

	// find the next consecutive set of collinear points and fit another line to them
	while(!collinear(*it,*(it+2),*(it+4),2)) ++it;
	Line2d line2(*it,*(it+4));

	// draw lines
	im.drawLine(line1, RGB(0,0,0));
	im.drawLine(line2, RGB(0,0,255));

	// intersect lines and draw intersection
	Point2d corner;
	line1.intersect(line2, corner);
	im.drawMarker(corner, RGB(255,0,0), 2);

	// get value of the card
	std::cout << "Card value: " << card.getValue() << "\n";

	// write image to disk
	if(!im.writePNM("output.pnm",errmsg)) error("%s\n",errmsg.c_str());

	// display image using an external program
	system("eog ./output.pnm");
}

int main(int, const char **)
{
	demo();
	return 0;
}
