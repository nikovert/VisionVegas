/*
 *
 * Image class and related disk I/O (only PPM/PNM support)
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#ifndef __IMAGE__
#define __IMAGE__

#include <string>
#include <iostream>
#include "geom.hpp"

// class BlobDetector;

typedef unsigned char uchar;

struct RGB
{
	uchar r;
	uchar g;
	uchar b;
	RGB() : r(0), g(0), b(0) { }
	RGB(uchar red, uchar green, uchar blue) : r(red), g(green), b(blue) { }
	void operator=(const RGB& other) { r=other.r; g=other.g; b=other.b; }
	bool operator<=(const RGB& other)
	{
		return (r <= other.r) && (g <= other.g) && (b <= other.b);
	}
	bool operator<(const RGB& other)
	{
		return (r < other.r) && (g < other.g) && (b < other.b);
	}
	bool operator>=(const RGB& other)
	{
		return (r >= other.r) && (g >= other.g) && (b >= other.b);
	}
	bool operator>(const RGB& other)
	{
		return (r > other.r) && (g > other.g) && (b > other.b);
	}

};

std::ostream& operator<<(std::ostream& os, const RGB& color);

//-------------------------------------------------------------------------------------
class Image
//-------------------------------------------------------------------------------------
{
// friend BlobDetector;

private:
	RGB* m_data; // note: storage in row-major order
	unsigned m_width;
	unsigned m_height;

public:
	Image();
	Image(unsigned width, unsigned height, const RGB& fillcolor = RGB(0,0,0));
	Image(const Image& other);
	virtual ~Image();
	void create(unsigned width, unsigned height, const RGB& fillcolor = RGB(0,0,0));
	void destroy();
	void operator=(const Image& other);

public:
	unsigned width()  const { return(m_width); }
	unsigned height() const { return(m_height); }
	unsigned pixels() const { return(m_width*m_height); }
	bool isAllocated() const { return(m_data); }

public:
	RGB& at(unsigned x, unsigned y);
	const RGB& at(unsigned x, unsigned y) const;
	RGB& at(unsigned index);
	const RGB& at(unsigned index) const;

public:
	void drawLine(int x1, int y1, int x2, int y2, const RGB& color);
	void drawLine(const Line2d& line, const RGB& color) { drawLine(line.p1.X(),line.p1.Y(),line.p2.X(),line.p2.Y(),color); }
	void drawMarker(int x, int y, const RGB& color, int size=2);
	void drawMarker(const Point2d& point, const RGB& color, int size=2) { drawMarker(point.X(),point.Y(),color,size); }

public: // file input/output
	bool readNextLine(std::fstream& file, std::stringstream& line_ss, long* linecount=0);
	bool readPNM(const std::string& filename, std::string& errmsg);
	bool writePNM(const std::string& filename, std::string& errmsg) const;
};


#endif
