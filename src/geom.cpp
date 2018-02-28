/*
 *
 * Classes for geometric entities and operations
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#include <geom.hpp>
#include <iostream>

using namespace std;

double Line2d::distance(const Point2d& p)
{
	// construct homogeneous line as ax + by + c = 0, where the normal is n=(a,b)
	double a = p2.y-p1.y;
	double b = p1.x-p2.x;
	double c = -(a*p1.x + b*p1.y);

	// point-line distance formula (warning: result is Inf if a=b=0)
	double dist = std::fabs(a*p.x + b*p.y + c)/sqrt(a*a+b*b);
	return(dist);
}

Vector3d Line2d::homogenize() const
{
	double a = p1.y-p2.y;
	double b = p2.x-p1.x;
	double c = -(a*p1.x + b*p1.y);
	return(Vector3d(a,b,c));
}

// intersect two lines, returns false if they do not intersect (parallel lines)
bool Line2d::intersect(const Line2d& line, Point2d& result)
{
	Vector3d l1 = this->homogenize(); // homogeneous line1 (a,b,c) where a*x+b*y+c=0
	Vector3d l2 = line.homogenize();  // homogeneous line2 (a,b,c) where a*x+b*y+c=0
	Vector3d p  = l1.cross(l2); // intersection point in homogeneous form (x,y,h) ~ 2D Euclidean point (x/h,y/h)

	if(p.z==0)
		return false;
	else
	{
		result.x = p.x/p.z;
		result.y = p.y/p.z;
		return true;
	}
}

// check for collinearity of three points, i.e. whether they are along the same line up to a tolerance
bool collinear(const Point2d& p1, const Point2d& p2, const Point2d& p3, double tolerance)
{
	Line2d line(p1,p3);
	if(line.isDegenerate()) return false;
	return(line.distance(p2)<tolerance);
}

std::ostream& operator<<(std::ostream& os, const Vector2d& v)
{
	os << v.x << " " << v.y;
	return(os);
}

std::ostream& operator<<(std::ostream& os, const Vector3d& v)
{
	os << v.x << " " << v.y << " " << v.z;
	return(os);
}

std::ostream& operator<<(std::ostream& os, const Line2d& l)
{
	os << l.p1 << " " << l.p2;
	return(os);
}
