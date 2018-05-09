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
	system("/home/ppsadm/FrameGrabber/requestframe.sh"); // image will be copied to ./frame.pnm
	//return(im.readPNM("../captured.pnm",errmsg));
	return(im.readPNM("captured.pnm",errmsg));
}

bool Card::readImage(std::string& errmsg, std::string& str)
{
    // uncomment when you want to use a new snapshot from the camera instead of using the same image over and over ...
    // system("/home/ppsadm/FrameGrabber/requestframe.sh"); // image will be copied to ./frame.pnm
    //return(im.readPNM("../captured.pnm",errmsg));
    return(im.readPNM(str,errmsg));
}

bool Card::setImage(Image& in)
{
    if(!in.isAllocated()) return false;
    im.create(in.width(), in.height());
    for(unsigned x0 = 1; x0 < in.width(); x0++){
        for(unsigned y0 = 1; y0 < in.height(); y0++){
            im.at(x0, y0) = in.at(x0, y0);
        }
    }
    return true;
}

bool Card::isBackground(const Point2d& point, uchar red_threshold) const
{
    if(usingPerceptron){
        return isBackground(point);
    }
    else{
        std::cout << " not using perceptron!";
        if(!im.isAllocated()) return false;
        int x = point.X();
        int y = point.Y();
        if (x>=0 && x<int(im.width()) && y>=0 && y<int(im.height()))
            return(im.at(x,y).r < red_threshold);
        else
            return true;
    }
}

bool Card::isBackground(const Point2d& point) const
{
    if(!im.isAllocated()) return false;
    int x = point.X();
    int y = point.Y();
    if (x >= 1 && x< int(im.width())-1 && y >= 1 && y < int(im.height())-1){
        std::vector<RGB> pixels;
        for(int j = -1; j<= 1; j++){
            for(int i = -1; i <= 1; i++)
                pixels.push_back(im.at(x+i, y+j));
        }
        return percep.eval(pixels);
    }
    else
        return true;
}

//outdated
bool Card::detectCardBoundary(std::vector<Point2d>& boundary_points, double distance, uchar threshold, double delta_angle, unsigned max_points)
{
	boundary_points.clear();
    if(!im.isAllocated()){
        std::cerr << "ERROR in detectCardBoundary: im not allocated" << std::endl;
        return false;
    }

	const double pi = 3.141592653589793;
	delta_angle *= pi/180.0;

	// find seed point
	unsigned x0 = im.width()/2;
	unsigned y0 = 0;
	while(y0<im.height() && isBackground(Point2d(x0,y0), threshold)) y0++;
    if(y0<=0 && y0>=im.height()-1){
        //try alternative seed
        x0 = im.width()/4;
        y0 = 0;
        while(y0<im.height() && isBackground(Point2d(x0,y0), threshold)) y0++;
        if(y0<=0 && y0>=im.height()-1){
            //try alternative seed
            x0 = im.width()*3/4;
            y0 = 0;
            while(y0<im.height() && isBackground(Point2d(x0,y0), threshold)) y0++;
            if(y0<=0 && y0>=im.height()-1){
                std::cerr << "ERROR in detectCardBoundary: y0 >0 or y0 > im.height()-1" << std::endl;
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
		while(!isBackground(Point2d(point.x + distance*cos(angle), point.y + distance*sin(angle)), threshold))
		{
			angle += delta_angle;
            if(angle>4*pi){
                std::cerr << "ERROR in detectCardBoundary: angle > 4*pi" << std::endl;
                return false;
            }
		}

		// search background clockwise
		while(isBackground(Point2d(point.x + distance*cos(angle), point.y + distance*sin(angle)), threshold))
		{
			angle -= delta_angle;
            if(angle<-4*pi){
                std::cerr << "ERROR in detectCardBoundary: angle > -4*pi" << std::endl;
                return false;
            }
		}

		// add point to the list of boundary points
		point = Point2d(point.x + distance*cos(angle), point.y + distance*sin(angle));
		boundary_points.push_back(point);
        if(boundary_points.size() > max_points){
            std::cerr << "ERROR in detectCardBoundary: located to many boundary points" << std::endl;
            return false;
        }

	} while (boundary_points.size()<3 || seed_point.distance(point) > 1.1 * distance);

	return true;
}

bool Card::loadBackgroundPerceptron()
{
    percep.setW(percep.readBackgroundWeights("../weights"));
    if(percep.getW().weight0 == 0) return false;
    usingPerceptron = true;
    //std::cout << "Using weights: " << percep.getW() << std::endl;
    return true;
}

int Card::getValue(void)
{
	
}

