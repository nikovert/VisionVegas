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

#define PI 3.14159265

bool inRectangle(const Point2d pixel, const Point2d &corner1, const Point2d &corner2, const Point2d &corner3, const Point2d &corner4)
{
    Line2d line1(corner1, corner2);
    Line2d line2(corner2, corner3);
    Line2d line3(corner3, corner4);
    Line2d line4(corner4, corner1);
    
    //length of the boundary lines
    double a1 = line1.length();
    //std::cout << "a1: " << a1 << std::endl;
    double a2 = line2.length();
    //std::cout << "a2: " << a2 << std::endl;
    double a3 = line3.length();
    //std::cout << "a3: " << a3 << std::endl;
    double a4 = line4.length();
    //std::cout << "a4: " << a4 << std::endl;
    
    //distance to the corners
    Line2d b1line(corner1, pixel), b2line(corner2, pixel), b3line(corner3, pixel), b4line(corner4, pixel);
    double b1 = b1line.length();
    //std::cout << "b1: " << b1 << std::endl;
    double b2 = b2line.length();
    //std::cout << "b2: " << b2 << std::endl;
    double b3 = b3line.length();
    //std::cout << "b3: " << b3 << std::endl;
    double b4 = b4line.length();
    //std::cout << "b4: " << b4 << std::endl;
    
    //Now we use Heron's Formula
    
    //Area of the rectangle
    double A = (a1*a2 + a2*a3 + a3*a4 + a4*a1)/4;
    //std::cout << "A: " << A << std::endl;
    
    //our triangle between the corners
    double u1((a1+b1+b2)/2);
    //std::cout << "u1: " << u1 << std::endl;
    double u2 = (a2+b2+b3)/2;
    //std::cout << "u2: " << u2 << std::endl;
    double u3 = (a3+b3+b4)/2;
    //std::cout << "u3: " << u3 << std::endl;
    double u4 = (a4+b4+b1)/2;
    //std::cout << "u4: " << u4 << std::endl;
    
    double A1 = sqrt(u1*(u1-a1)*(u1-b1)*(u1-b2));
    //std::cout << "A1: " << A1 << std::endl;
    double A2 = sqrt(u2*(u2-a2)*(u2-b2)*(u2-b3));
    //std::cout << "A2: " << A2 << std::endl;
    double A3 = sqrt(u3*(u3-a3)*(u3-b3)*(u3-b4));
    //std::cout << "A3: " << A3 << std::endl;
    double A4 = sqrt(u4*(u4-a4)*(u4-b4)*(u4-b1));
    //std::cout << "A4: " << A4 << std::endl;
    
    //If the corner is in the rectangle: A = A1 + A2 + A3 + A4
    //if the corner is outside the rectangle: A < A1 + A2 + A3 + A4
    
    //aloud error
    double epsilon = 10;
    
    return (A > (A1 + A2 + A3 + A4 - epsilon));
}

double Line2d::angle(void) const
{
    //returns the angle of a line in degrees
    Point2d p = p2 - p1;
    return atan2(p.x, p.y)*180/PI;
}

double Line2d::length(void) const
{
    double x11 = p1.x;
    double x12 = p2.x;
    
    double y11 = p1.y;
    double y12 = p2.y;
    
    return sqrt((x11-x12)*(x11-x12) + (y11-y12)*(y11-y12));
}

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

std::ostream& operator<<(std::ostream& os, const Matrix3x3& mat)
{
    os << mat.m11 << " " << mat.m12 << " " << mat.m13 << '\n' << mat.m21 << " " << mat.m22 << " " << mat.m23 << '\n' << mat.m31 << " " << mat.m32 << " " << mat.m33;
    return(os);
}

Vector3d operator*(const Matrix3x3& mat, const Vector3d& vec)
{
    return Vector3d(mat.m11*vec.x + mat.m12*vec.y +mat.m13*vec.z, mat.m21*vec.x + mat.m22*vec.y +mat.m23*vec.z, mat.m31*vec.x + mat.m32*vec.y +mat.m33*vec.z);
}

Matrix3x3 operator*(const Matrix3x3& mat1, const Matrix3x3& mat2)
{
    Matrix3x3 matresult;
    matresult.m11 = (mat1.m11 * mat2.m11 + mat1.m12 * mat2.m21 + mat1.m13 * mat2.m31);
    matresult.m12 = (mat1.m11 * mat2.m12 + mat1.m12 * mat2.m22 + mat1.m13 * mat2.m32);
    matresult.m13 = (mat1.m11 * mat2.m13 + mat1.m12 * mat2.m23 + mat1.m13 * mat2.m33);
    
    matresult.m21 = (mat1.m21 * mat2.m11 + mat1.m22 * mat2.m21 + mat1.m23 * mat2.m31);
    matresult.m22 = (mat1.m21 * mat2.m12 + mat1.m22 * mat2.m22 + mat1.m23 * mat2.m32);
    matresult.m23 = (mat1.m21 * mat2.m13 + mat1.m22 * mat2.m23 + mat1.m23 * mat2.m33);
    
    matresult.m31 = (mat1.m31 * mat2.m11 + mat1.m32 * mat2.m21 + mat1.m33 * mat2.m31);
    matresult.m32 = (mat1.m31 * mat2.m12 + mat1.m32 * mat2.m22 + mat1.m33 * mat2.m32);
    matresult.m33 = (mat1.m31 * mat2.m13 + mat1.m32 * mat2.m23 + mat1.m33 * mat2.m33);
    
    return matresult;
}

bool operator>(const Line2d &line1, const Line2d &line2)
{
    return line1.length() > line2.length();
}

bool operator< (const Line2d &line1, const Line2d &line2)
{
    double x11 = line1.p1.x;
    double x12 = line1.p2.x;
    
    double x21 = line2.p1.x;
    double x22 = line2.p2.x;
    
    double y11 = line1.p1.y;
    double y12 = line1.p2.y;
    
    double y21 = line2.p1.y;
    double y22 = line2.p2.y;
    
    double length1 = (x11-x12)*(x11-x12) + (y11-y12)*(y11-y12);
    double length2 = (x21-x22)*(x21-x22) + (y21-y22)*(y21-y22);
    
    return length1 < length2;
}
