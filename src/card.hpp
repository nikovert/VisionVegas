/*
 *
 * Card processing class header
 * C++ template code by Andras Bodis-Szomoru, Sep 2015
 * Vision Goes Vegas course @ Computer Vision Lab, ETH Zurich
 *
 */

#ifndef __CARD__
#define __CARD__

#include <image.hpp>
#include <geom.hpp>
#include <vector>
#include <perceptron.hpp>

class Card
{
private:
	Image im;
    Perceptron percep;
    bool usingPerceptron; // can be removed in later versions once Perceptron works;
public:
    Card() : usingPerceptron(false) {}
	bool updateImage(std::string& errmsg);
    bool readImage(std::string& errmsg, std::string& str);
	void cloneImageTo(Image& out) const { out = im; }
	bool isBackground(const Point2d& point, uchar red_threshold) const;
    bool isBackground(const Point2d& point) const;
	bool detectCardBoundary(std::vector<Point2d>& boundary_points, double distance=20, uchar threshold=128, double delta_angle=1.0, unsigned max_points=500);
	int getValue(void);
    bool loadPerceptron(); //loads the perceptron weights from a file
};

#endif
