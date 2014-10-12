#ifndef CORE_H
#define CORE_H

#include <sstream>
#include <stdio.h>

#include <opencv2/core/core.hpp>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x) ).str()

typedef std::vector<cv::Point> Contour;
typedef std::vector<cv::Point> Pip;

#endif // CORE_H


