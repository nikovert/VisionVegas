/*
 *
 * Image class and related disk I/O (only PPM/PNM support)
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "image.hpp"
#include "geom.hpp"

std::ostream& operator<<(std::ostream& os, const RGB& color)
{
	os << int(color.r) << " " << int(color.g) << " " << int(color.b) << "\n";
	return(os);
}

bool operator==(RGB& col1, RGB& col2)
{
    if(col1.r == col2.r && col1.g == col2.g && col1.b == col2.b)
        return true;
    else
        return false;
}

Image::Image() : m_data(0), m_width(0), m_height(0)
{

}

Image::Image(unsigned width, unsigned height, const RGB& color)
{
	m_data   = 0;
	m_width  = 0;
	m_height = 0;

	if(width==0 || height==0) return;
	m_data   = new RGB[width*height]; // throws std::bad_alloc if unsuccessful
	m_width  = width;
	m_height = height;
	for(unsigned i=0;i<width*height;i++) m_data[i] = color;
}

Image::~Image()
{
	if(m_data) delete [] m_data;
}

void Image::create(unsigned width, unsigned height, const RGB& color)
{
	destroy();
	if(width==0 || height==0) return;

	m_data = new RGB[width*height]; // throws std::bad_alloc if unsuccessful
	m_width  = width;
	m_height = height;
	for(unsigned i=0;i<width*height;i++) m_data[i] = color;
}

void Image::destroy()
{
	if(m_data)
	{
		delete [] m_data;
		m_width  = 0;
		m_height = 0;
	}
}

void Image::operator=(const Image& other)
{
	destroy();
	if(!other.isAllocated()) return;

	m_data   = new RGB[other.pixels()];
	m_width  = other.width();
	m_height = other.height();

	// copy image pixels
	for(unsigned i=0;i<pixels();i++) m_data[i] = other.at(i);
}

void Image::operator=(const BinaryImage& mask)
{
    destroy();
    if(!mask.isAllocated()) return;
    
    m_data   = new RGB[mask.pixels()];
    m_width  = mask.width();
    m_height = mask.height();
    for(unsigned i=0;i<pixels();i++){
        if(!mask.at(i))
            m_data[i] = RGB(0,0,0);
        else
            m_data[i] = RGB(255,255,255);
    }
}

RGB& Image::at(unsigned x, unsigned y)
{
	if(m_data==0 || x>=m_width || y>=m_height) throw std::out_of_range("Image::at(x,y) out of range");
	return(m_data[y*m_width+x]);
}

const RGB& Image::at(unsigned x, unsigned y) const
{
	if(m_data==0 || x>=m_width || y>=m_height) throw std::out_of_range("Image::at(x,y) out of range");
	return(m_data[y*m_width+x]);
}

RGB& Image::at(unsigned index)
{
	if(m_data==0 || index>=m_width*m_height) throw std::out_of_range("Image::at(i) out of range");
	return(m_data[index]);
}

const RGB& Image::at(unsigned index) const
{
	if(m_data==0 || index>=m_width*m_height) throw std::out_of_range("Image::at(i) out of range");
	return(m_data[index]);
}


void Image::drawLine(int x1, int y1, int x2, int y2, const RGB& color)
{
	const float dy = y2 - y1;
	const float dx = x2 - x1;
	const float length = hypot(dy, dx);
	const float dt = 1.0 / length;
	for (float t = 0; t <= 1; t += dt)
	{
		int y = y1 + int(t*dy);
		int x = x1 + int(t*dx);
		if (y < 0 || y >= int(m_height) || x < 0 || x >= int(m_width)) continue;
		at(x,y) = color;
	}
}

void Image::drawMarker(int x0, int y0, const RGB& color, int size)
{
	if(m_data==0 || m_width==0 || m_height==0) return;

	for(int y=y0-size; y<=y0+size;y++)
		for (int x=x0-size; x<=x0+size;x++)
		{
			if(y<0 || y>=int(m_height) || x<0 || x>=int(m_width)) continue;
			unsigned long index = y*m_width + x;
			m_data[index].r = color.r;
			m_data[index].g = color.g;
			m_data[index].b = color.b;
		}
}

bool Image::readNextLine(std::fstream& file, std::stringstream& line_ss, long* linecount)
{
	std::string line;
	if(!file.is_open() || file.eof()) return false;
    do
    {
    	std::getline(file,line);
    	if(linecount) (*linecount)++;

    	// cut off the comment part of the string
    	unsigned long sharp_at = line.find_first_of('#');
    	if(sharp_at!=std::string::npos)
    		line = line.substr(0,sharp_at);

    	// clear string if it is all whitespaces
    	bool only_whitespaces = true;
    	for(unsigned i=0;i<line.length();i++)
    		if(!std::isspace(line[i]))
    		{
    			only_whitespaces = false;
    			break;
    		}

    	if(only_whitespaces) line.clear();

    } while(!file.eof() && line.empty());

    line_ss.clear();
    line_ss.str(line);

    return(!line.empty());
}


bool Image::readPNM(const std::string& filename, std::string& errmsg)
{
	destroy();

	unsigned width, height, im_colors=0;

	std::fstream file;
	file.open(filename.c_str(),std::ios::in);
	if(!file.is_open()) { errmsg = std::string("File not found or unable to access: ") + filename; return false; }

	// read image signature (type)
	std::stringstream line;
	readNextLine(file, line);
	std::string id = line.str();
	if(id!="P6")
	{
		errmsg = "unsupported image file format";
		return false;
	}

	// read image width and height
	readNextLine(file, line);
	line >> width; if(line.fail()) { errmsg = "Unable to read image width"; return false; }
	line >> height; if(line.fail()) { errmsg = "Unable to read image height"; return false; }
	//std::cout << "image: " << width << "x" << height << "\n";

	// read image color values
	readNextLine(file, line);
	line >> im_colors;
	if(line.fail()) { errmsg = "Unable to read image colors"; return false; }

	// check image size
	std::streampos image_start = file.tellg();
	file.seekg(0,std::ios::end);
	std::streampos file_size = file.tellg();
	unsigned long image_bytes = file_size-image_start;
	file.seekg(image_start);
	if(image_bytes != width*height*3)
	{
		errmsg = "Unexpected image size\n";
		return false;
	}

	// read image data at once into a temporary raw buffer
	unsigned char* data = new unsigned char[image_bytes];
	file.read((char*)data,image_bytes-1);
	if(!file.good())
	{
		errmsg = "Unable to read image data";
		delete [] data;
		return false;
	}

	// allocate image buffer and copy raw data into it
	m_data = new RGB[width*height]; // throws std::bad_alloc if unsuccessful
	m_width  = width;
	m_height = height;
	for(unsigned i=0;i<width*height;i++)
	{
		m_data[i].r = data[3*i+0];
		m_data[i].g = data[3*i+1];
		m_data[i].b = data[3*i+2];
	}

	delete [] data;
	file.close();
	return(true);
}

bool Image::writePNM(const std::string& filename, std::string& errmsg) const
{
	if(m_data==0 || m_width*m_height==0)
	{
		errmsg = "No image data to save";
		return false;
	}

	std::fstream file;
	file.open(filename.c_str(),std::ios::out | std::ios::trunc);
	if(!file.is_open())
	{
		errmsg = "unable to open file for writing: " + filename;
		return false;
	}

	// write header
	file << "P6\n" << m_width << " " << m_height << "\n255\n";

	// convert image data to a raw buffer
	unsigned char* data = new unsigned char[m_width*m_height*3];
	for(unsigned i=0;i<m_width*m_height;i++)
	{
		data[3*i+0] = m_data[i].r;
		data[3*i+1] = m_data[i].g;
		data[3*i+2] = m_data[i].b;
	}

	// write raw buffer to file
	file.write((char*)data,m_width*m_height*3);
	if(!file.good())
	{
		errmsg = "Unable to write image data to file";
		return false;
	}

	delete [] data;
	file.close();
	return true;
}

BinaryImage::BinaryImage() : m_data(0), m_width(0), m_height(0)
{
    
}

BinaryImage::BinaryImage(unsigned width, unsigned height, const bool fillvalue)
{
    m_data   = 0;
    m_width  = 0;
    m_height = 0;
    
    if(width==0 || height==0) return;
    m_data   = new bool[width*height]; // throws std::bad_alloc if unsuccessful
    m_width  = width;
    m_height = height;
    for(unsigned i=0;i<width*height;i++) m_data[i] = fillvalue;
}

BinaryImage::~BinaryImage()
{
    if(m_data) delete [] m_data;
}

void BinaryImage::create(unsigned width, unsigned height, const bool fillvalue)
{
    destroy();
    if(width==0 || height==0) return;
    
    m_data = new bool[width*height]; // throws std::bad_alloc if unsuccessful
    m_width  = width;
    m_height = height;
    for(unsigned i=0;i<width*height;i++) m_data[i] = fillvalue;
}

void BinaryImage::destroy()
{
    if(m_data)
    {
        delete [] m_data;
        m_width  = 0;
        m_height = 0;
    }
}

void BinaryImage::operator=(const BinaryImage& other)
{
    destroy();
    if(!other.isAllocated()) return;
    
    m_data   = new bool[other.pixels()];
    m_width  = other.width();
    m_height = other.height();
    
    // copy image pixels
    for(unsigned i=0;i<pixels();i++) m_data[i] = other.at(i);
}


bool BinaryImage::at(unsigned x, unsigned y)
{
    if(m_data==0 || x>=m_width || y>=m_height) throw std::out_of_range("Image::at(x,y) out of range");
    return(m_data[y*m_width+x]);
}

const bool& BinaryImage::at(unsigned x, unsigned y) const
{
    if(m_data==0 || x>=m_width || y>=m_height) throw std::out_of_range("Image::at(x,y) out of range");
    return(m_data[y*m_width+x]);
}

bool BinaryImage::at(unsigned index)
{
    if(m_data==0 || index>=m_width*m_height) throw std::out_of_range("Image::at(i) out of range");
    return(m_data[index]);
}

const bool& BinaryImage::at(unsigned index) const
{
    if(m_data==0 || index>=m_width*m_height) throw std::out_of_range("Image::at(i) out of range");
    return(m_data[index]);
}

void BinaryImage::setPixel(unsigned x, unsigned y, bool value)
{
    m_data[y*m_width+x] = value;
}

bool BinaryImage::readNextLine(std::fstream& file, std::stringstream& line_ss, long* linecount)
{
    std::string line;
    if(!file.is_open() || file.eof()) return false;
    do
    {
        std::getline(file,line);
        if(linecount) (*linecount)++;
        
        // cut off the comment part of the string
        unsigned long sharp_at = line.find_first_of('#');
        if(sharp_at!=std::string::npos)
            line = line.substr(0,sharp_at);
        
        // clear string if it is all whitespaces
        bool only_whitespaces = true;
        for(unsigned i=0;i<line.length();i++)
            if(!std::isspace(line[i]))
            {
                only_whitespaces = false;
                break;
            }
        
        if(only_whitespaces) line.clear();
        
    } while(!file.eof() && line.empty());
    
    line_ss.clear();
    line_ss.str(line);
    
    return(!line.empty());
}

bool BinaryImage::readPNM(const std::string& filename, std::string& errmsg)
{
    destroy();
    
    unsigned width, height, im_colors=0;
    
    std::fstream file;
    file.open(filename.c_str(),std::ios::in);
    if(!file.is_open()) { errmsg = std::string("File not found or unable to access: ") + filename; return false; }
    
    // read image signature (type)
    std::stringstream line;
    readNextLine(file, line);
    std::string id = line.str();
    if(id!="P6")
    {
        errmsg = "unsupported image file format";
        return false;
    }
    
    // read image width and height
    readNextLine(file, line);
    line >> width; if(line.fail()) { errmsg = "Unable to read image width"; return false; }
    line >> height; if(line.fail()) { errmsg = "Unable to read image height"; return false; }
    //std::cout << "image: " << width << "x" << height << "\n";
    
    // read image color values
    readNextLine(file, line);
    line >> im_colors;
    if(line.fail()) { errmsg = "Unable to read image colors"; return false; }
    
    // check image size
    std::streampos image_start = file.tellg();
    file.seekg(0,std::ios::end);
    std::streampos file_size = file.tellg();
    unsigned long image_bytes = file_size-image_start;
    file.seekg(image_start);
    if(image_bytes != width*height*3)
    {
        errmsg = "Unexpected image size\n";
        return false;
    }
    
    // read image data at once into a temporary raw buffer
    unsigned char* data = new unsigned char[image_bytes];
    file.read((char*)data,image_bytes-1);
    if(!file.good())
    {
        errmsg = "Unable to read image data";
        delete [] data;
        return false;
    }
    RGB* m_dataRGB;
    // allocate image buffer and copy raw data into it
    m_dataRGB = new RGB[width*height]; // throws std::bad_alloc if unsuccessful
    m_width  = width;
    m_height = height;
    for(unsigned i=0;i<width*height;i++)
    {
        m_dataRGB[i].r = data[3*i+0];
        m_dataRGB[i].g = data[3*i+1];
        m_dataRGB[i].b = data[3*i+2];
    }
    
    delete [] data;
    file.close();
    m_data = new bool[width*height];
    for(unsigned i=0;i<pixels();i++){
        if(m_dataRGB[i].r > 50 && m_dataRGB[i].g > 50 && m_dataRGB[i].b > 50)
            m_data[i] = true;
        else
            m_data[i] = false;
    }
    
    return(true);
}


