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

double minimum(double x, double y){
    if(x > y) return y;
    else      return x;
}

double maximum(double x, double y){
    if(x < y) return y;
    else      return x;
}

std::ostream& operator<<(std::ostream& os, const RGB& color)
{
	os << int(color.r) << " " << int(color.g) << " " << int(color.b) << "\n";
	return(os);
}

std::ostream& operator<<(std::ostream& os, const HSL& color)
{
    os << int(color.h) << " " << int(color.s) << " " << int(color.l) << "\n";
    return(os);
}

bool operator==(RGB& col1, RGB& col2)
{
    if(col1.r == col2.r && col1.g == col2.g && col1.b == col2.b)
        return true;
    else
        return false;
}

HSL convert(RGB rgb)
{
    HSL hsl;
    //Convert the RGB values to the range 0-1
    double r = (double) rgb.r / 255;
    double g = (double) rgb.g / 255;
    double b = (double) rgb.b / 255;
    
    //Find the minimum and maximum values of R, G and B
    double min = minimum(r, minimum(g,b));
    double max = maximum(r, maximum(g,b));
    
    //calculate the Luminace value by adding the max and min values and divide by 2
    hsl.l = (min + max) / 2;
    
    //calculate the saturation
    if(max-min < 0.001){
        hsl.s = 0;
        hsl.h = 0;
    }else{
        if(hsl.l < 0.5) hsl.s = (max-min)/(max+min);
        else            hsl.s = (max-min)/(2.0-max-min);
        
        //calculate the hue
        if(r > g){
            if(r > b){   //Red is max
                hsl.h = (g-b)/(max-min);
            }else{        //Blue is max
                hsl.h = 4.0 + (r-g)/(max-min);
            }
        }else{
            if(g > b){   //Green is max
                hsl.h = 2.0 + (b-r)/(max-min);
            }else{        //Blue is max
                hsl.h = 4.0 + (r-g)/(max-min);
            }
        }
    }
    hsl.h *= 60; //to degrees
    if(hsl.h < 0) hsl.h += 360;
    hsl.s *= 100; //to percent
    hsl.l *= 100;
    
    //round
    hsl.h = round(hsl.h);
    hsl.s = round(hsl.s);
    hsl.l = round(hsl.l );
    return hsl;
}

RGB convert(HSL hsl)
{
    RGB rgb;
    double r(0);
    double g(0);
    double b(0);
    
    hsl.s /= 100; //to percent
    hsl.l /= 100;
    
    //temporary values
    double temporary_1;
    if(hsl.l < 0.5) temporary_1 = hsl.l * (1.0 + hsl.s);
    else            temporary_1 = hsl.l + hsl.s - hsl.l * hsl.s;
    
    double temporary_2 = 2 * hsl.l - temporary_1;
    
    //convert hue to 360 degrees
    hsl.h /=360;
    
    double temporary_R = hsl.h + 0.333;
    if(temporary_R < 0) temporary_R += 1;
    if(temporary_R > 1) temporary_R -= 1;
    
    double temporary_G = hsl.h;
    if(temporary_G < 0) temporary_G += 1;
    if(temporary_G > 1) temporary_G -= 1;
    
    double temporary_B = hsl.h - 0.333;
    if(temporary_B < 0) temporary_B += 1;
    if(temporary_B > 1) temporary_B -= 1;
    
    //3 tests to select the correct formula for each color
    
    //Red
    //test 1
    if(6 * temporary_R < 1){
        r = temporary_2 + (temporary_1 - temporary_2) * 6 * temporary_R;
    }else{
        //test 2
        if(2 * temporary_R < 1){
            r = temporary_1;
        }else{
            //test 3
            if(3 * temporary_R < 2){
                r = temporary_2 + (temporary_1 - temporary_2) * (0.666 - temporary_R) * 6;
            }else{
                r = temporary_2;
            }
        }
    }
    
    //Green
    //test 1
    if(6 * temporary_G < 1){
        g = temporary_2 + (temporary_1 - temporary_2) * 6 * temporary_G;
    }else{
        //test 2
        if(2 * temporary_G < 1){
            g = temporary_1;
        }else{
            //test 3
            if(3 * temporary_G < 2){
                g = temporary_2 + (temporary_1 - temporary_2) * (0.666 - temporary_G) * 6;
            }else{
                g = temporary_2;
            }
        }
    }
    
    //Blue
    //test 1
    if(6 * temporary_B < 1){
        b = temporary_2 + (temporary_1 - temporary_2) * 6 * temporary_B;
    }else{
        //test 2
        if(2 * temporary_B < 1){
            b = temporary_1;
        }else{
            //test 3
            if(3 * temporary_B < 2){
                b = temporary_2 + (temporary_1 - temporary_2) * (0.666 - temporary_B) * 6;
            }else{
                b = temporary_2;
            }
        }
    }
    
    rgb.r = round(255*r);
    rgb.g = round(255*g);
    rgb.b = round(255*b);
    return rgb;
}

void Image::grey_histequalization(){
    
    int* m_dataGrey = new int[pixels()];
    for(int i = 0; i < pixels(); i++){
        m_dataGrey[i] = (m_data[i].r + m_data[i].g + m_data[i].b)/3;
    }
    
    for(int i = 0; i < pixels(); i++){
        //std::cout << "pixel: " << i << " \t " << m_dataGrey[i] << std::endl;
        RGB color(m_dataGrey[i],m_dataGrey[i],m_dataGrey[i]);
        m_data[i] = color;
    }
    
    //number of occurences
    double occurrences[256] = { };
    for(int i = 0; i < pixels(); i++){
        occurrences[m_dataGrey[i]] += 1;
    }
    
    //probability
    double prob[256] = { };
    for(int i = 0; i < 256; i++){
        prob[i] =  occurrences[i]/pixels();
    }
    
    //cumulative distribution function
    double cdf[256] = { };
    cdf[0] = prob[0];
    int numberofValues = 0;
    double cdfmax = 0;
    double cdfmin = 256;
    
    for(int i = 1; i < 256; i++){
        cdf[i] = cdf[i-1] + prob[i];
        if(cdfmin > cdf[i]) cdfmin = cdf[i];
        if(cdfmax < cdf[i]) cdfmax = cdf[i];
        if(prob[i] > 0.0000001){
            numberofValues++;
        }
        else prob[i] = 0;
    }
    
    //calculate max
    double max = 0;
    for(int i = 1; i < 256; i++){
        if(max < prob[i]) max = prob[i]; //not sure
    }
    
    //histogram equalization formula
    double h[256] = { };
    for(int i = 1; i < 256; i++){
        h[i] = (255 * (cdf[i]*pixels() - cdfmin*255)/(pixels() - cdfmin*255)); //not sure
    }
    
     std::cout << "numberofValues: " << numberofValues << " cdfmax: " << cdfmax  << " cdfmin: " << cdfmin << std::endl;
     
     for(int i = 0; i < 256; i++){
         //std::cout << "pixel: " << i << " \t " << occurrences[i] << std::endl;
         std::cout << h[i] << " " ;
     }  std::cout << std::endl;
    
    for(int i = 0; i < pixels(); i++){
        m_dataGrey[i] = h[m_dataGrey[i]];
        //std::cout << "pixel: " << i << " \t " << m_dataGrey[i] << std::endl;
        RGB color(m_dataGrey[i],m_dataGrey[i],m_dataGrey[i]);
        at(i) = color;
    }
}

void Image::histequalization(){
    //std::cout << "inHist1" << std::endl;
    HSL* m_dataHSL = new HSL[pixels()];
    for(int i = 0; i < pixels(); i++){
        m_dataHSL[i] = convert(m_data[i]);
    }
    //std::cout << "inHist2" << std::endl;
    //number of occurences
    double occurrences[100] = { };
    for(int i = 0; i < pixels(); i++){
        occurrences[(int) m_dataHSL[i].l] += 1;
    }
    //std::cout << "inHist3" << std::endl;
    //probability
    double prob[100] = { };
    for(int i = 0; i < 100; i++){
        prob[i] =  occurrences[i]/pixels();
    }
    //std::cout << "inHist4" << std::endl;
    //cumulative distribution function
    double cdf[100] = { };
    cdf[0] = prob[0];
    int numberofValues = 0;
    double cdfmax = 0;
    double cdfmin = 100;
    std::cout << "inHist5" << std::endl;
    for(int i = 1; i < 100; i++){
        cdf[i] = cdf[i-1] + prob[i];
        if(cdfmin > cdf[i]) cdfmin = cdf[i];
        if(prob[i] > 0.0000001){
            numberofValues++;
            cdfmax = cdf[i];
        }
        else prob[i] = 0;
    }
    
    //calculate max
    double max = 0;
    for(int i = 1; i < 100; i++){
        if(max < prob[i]) max = prob[i]; //not sure
        
    }
    std::cout << "inHist6" << std::endl;
    //histogram equalization formula
    double h[100] = { };
    for(int i = 1; i < 100; i++){
        h[i] = (99 * (cdf[i]*pixels() - cdfmin*100)/(pixels() - cdfmin*100)); //not sure
    }
    
    /*
     std::cout << "numberofValues: " << numberofValues << " cdfmax: " << cdfmax  << " cdfmin: " << cdfmin << std::endl;
    
    for(int i = 0; i < 100; i++){
        //std::cout << "pixel: " << i << " \t " << occurrences[i] << std::endl;
        std::cout<< h[i] << " " ;
    }
     */
    std::cout << "inHist7" << std::endl;
    for(int i = 0; i < pixels(); i++){
        m_dataHSL[i].l = h[(int)(m_dataHSL[i].l)];
        m_data[i] = convert(m_dataHSL[i]);
    }
    std::cout << "inHistDONE" << std::endl;
}

void Image::erosion(int repeats){ //erodes noise in mask
    Image tmp;
    tmp.create(width(), height());
    for(unsigned x0 = 1; x0 < width()-1; x0++){
        for(unsigned y0 = 1; y0 < height()-1; y0++){
            if(at(x0-1, y0) == RGB(255,255,255) && at(x0+1, y0) == RGB(255,255,255) && at(x0, y0-1) == RGB(255,255,255) && at(x0, y0) == RGB(255,255,255) && at(x0, y0+1) == RGB(255,255,255)){
                tmp.at(x0, y0) = RGB(255,255,255);
            }
            else
                tmp.at(x0, y0) = RGB(0,0,0);
        }
    }
    for(unsigned x0 = 1; x0 < width()-1; x0++){
        for(unsigned y0 = 1; y0 < height()-1; y0++){
            at(x0, y0) = tmp.at(x0, y0);
        }
    }
    if(repeats>1) erosion(repeats-1);
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


