/*
 *
 * Classes for geometric entities and operations
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#ifndef __GEOMETRY__
#define __GEOMETRY__

#include <cmath>
#include <iostream>

//-------------------------------------------------------------------------------------
class Vector2d
//-------------------------------------------------------------------------------------
{
public:
	double x;
	double y;

public:
	Vector2d() : x(0), y(0) { }
	Vector2d(double x0, double y0) : x(x0), y(y0) { }
	Vector2d(const Vector2d& other, double angle, double length) : x(other.x + length*cos(angle)), y(other.y + length*sin(angle)) {  }
	Vector2d(const Vector2d& other) : x(other.x), y(other.y) { }

public:
	int X() const { return(int(x+0.5)); }
	int Y() const { return(int(y+0.5)); }
	bool operator==(const Vector2d& other) const  { return(x==other.x && y==other.y); }
	Vector2d& operator=(const Vector2d& other) { x=other.x; y=other.y; return(*this); }
	Vector2d operator+(const Vector2d& other) const { return(Vector2d(x+other.x, y+other.y)); }
	Vector2d operator-(const Vector2d& other) const { return(Vector2d(x-other.x, y-other.y)); }
	Vector2d operator*(double scalar) const { return(Vector2d(x*scalar, y*scalar)); }
	friend Vector2d operator*(double scalar, const Vector2d& point) { return(Vector2d(scalar*point.x, scalar*point.y)); }

	double dot(const Vector2d& other) { return(x*other.x + y*other.y); }
	double distance(const Vector2d& other) const { return(std::sqrt((x-other.x)*(x-other.x) + (y-other.y)*(y-other.y))); }
};

typedef Vector2d Point2d; // a bit loose mathematically but ok for our purpose

//-------------------------------------------------------------------------------------
class Vector3d
//-------------------------------------------------------------------------------------
{
public:
	double x;
	double y;
	double z;

public:
	Vector3d() : x(0), y(0), z(0) { }
	Vector3d(double x0, double y0, double z0) : x(x0), y(y0), z(z0) { }
	Vector3d(const Vector3d& other) : x(other.x), y(other.y), z(other.z) { }

public:
	Vector3d cross(const Vector3d& other) const { return(Vector3d(y*other.z-other.y*z, other.x*z-x*other.z, x*other.y-other.x*y)); }
};

//-------------------------------------------------------------------------------------
class Line2d
//-------------------------------------------------------------------------------------
{
public:
	Point2d p1;
	Point2d p2;

public:
	Line2d() : p1(0,0), p2(0,1) { }
	Line2d(const Point2d& point1, const Point2d& point2) : p1(point1), p2(point2) {  }
	Line2d(const Line2d& other) : p1(other.p1), p2(other.p2) { }

public:
	bool isDegenerate() const { return(p1==p2); }
	double distance(const Point2d& p);
	bool intersect(const Line2d& other, Point2d& result); // intersect two lines, returns false if they do not intersect (parallel lines)
	Vector3d homogenize() const; // convert {p1,p2} line representation to Vector3(a,b,c), where the line is defined by a*x+b*y+c=0
};

//-------------------------------------------------------------------------------------
//			Global functions
//-------------------------------------------------------------------------------------

// check for collinearity of three points, i.e. whether they are along the same line up to a tolerance
bool collinear(const Point2d& p1, const Point2d& p2, const Point2d& p3, double tolerance = 1.0);

// functions that allow you to send vectors, points and lines to std::cout with the << operator
std::ostream& operator<<(std::ostream& os, const Vector2d& v);
std::ostream& operator<<(std::ostream& os, const Vector3d& v);
std::ostream& operator<<(std::ostream& os, const Line2d& l);

#endif
