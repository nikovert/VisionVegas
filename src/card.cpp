/*
 *
 * Card processing class implementation
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#include <card.hpp>

bool Card::updateImage(std::string& errmsg)
{
	// uncomment when you want to use a new snapshot from the camera instead of using the same image over and over ...
	// system("/home/ppsadm/FrameGrabber/requestframe.sh"); // image will be copied to ./frame.pnm
	//return(im.readPNM("../captured.pnm",errmsg));
	return(im.readPNM("../../card_images/h3.2.pnm",errmsg));
}

bool Card::isBackground(const Point2d& point, uchar red_threshold) const
{
	if(!im.isAllocated()) return false;
	int x = point.X();
	int y = point.Y();
	if (x>=0 && x<int(im.width()) && y>=0 && y<int(im.height()))
		return(im.at(x,y).r < red_threshold);

	else return true;
}

bool Card::detectCardBoundary(std::vector<Point2d>& boundary_points, double distance, uchar threshold, double delta_angle, unsigned max_points)
{
	boundary_points.clear();
	if(!im.isAllocated()) return false;

	const double pi = 3.141592653589793;
	delta_angle *= pi/180.0;

	// find seed point
	unsigned x0 = im.width()/2;
	unsigned y0 = 0;
	while(y0<im.height() && isBackground(Point2d(x0,y0), threshold)) y0++;
	if(y0<=0 && y0>=im.height()-1) return false;
	Point2d seed_point(x0,y0);
	boundary_points.push_back(seed_point);

	// trace boundary
	Point2d point = seed_point;
	double angle = 0;
	do
	{
		// search card counter-clockwise (starting from previous angle)
		while(!isBackground(Point2d(point.x + distance*cos(angle), point.y + distance*sin(angle)), threshold))
		{
			angle += delta_angle;
			if(angle>4*pi) return false;
		}

		// search background clockwise
		while(isBackground(Point2d(point.x + distance*cos(angle), point.y + distance*sin(angle)), threshold))
		{
			angle -= delta_angle;
			if(angle<-4*pi) return false;
		}

		// add point to the list of boundary points
		point = Point2d(point.x + distance*cos(angle), point.y + distance*sin(angle));
		boundary_points.push_back(point);
		if(boundary_points.size()>max_points) return false;

	} while (boundary_points.size()<3 || seed_point.distance(point) > 1.1 * distance);

	return true;
}

int Card::getValue(void)
{
	if(!im.isAllocated()) return 0;

	// TODO: recognize card value

	return 20;
}

