/*
 *  $Id$
 */
#include <iostream>
#include "Img.hh"
#include "CameraRTC.h"

namespace TU
{
template <> void
saveImageHeader<Img::ImageData>(std::ostream& out,
				const Img::ImageData& imageData)
{
    using namespace	std;
    
    if (imageData.format == Img::CF_GRAY)
    {
	out << "P5"
	    << "# PixelLenth: " << 1 << endl
	    << "# DataType: "   << "Char" << endl
	    << "# Sign: "	<< "Unsigned" << endl;
    }
    else
    {
	out << "P6"
	    << "# PixelLenth: " << 3 << endl
	    << "# DataType: "   << "RGB24" << endl
	    << "# Sign: "	<< "Unsigned" << endl;
    }

    out << imageData.width << ' ' << imageData.height << endl
	<< 255 << endl;
}
    
}
