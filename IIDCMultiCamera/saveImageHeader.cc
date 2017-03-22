/*
 *  $Id$
 */
#include <iostream>
#include "Img.hh"

namespace TU
{
void
saveImageHeader(std::ostream& out, const Img::Header& header)
{
    using namespace	std;
    
    if (header.format == Img::RGB_24)
	out << "P6" << endl;
    else
	out << "P5" << endl;

    switch (header.format)
    {
      case Img::MONO_8:
	out << "# PixelLenth: " << 1 << endl
	    << "# DataType: "   << "Char" << endl
	    << "# Sign: "	<< "Unsigned" << endl;
	break;
      case Img::YUV_411:
	out << "# PixelLenth: " << 2 << endl
	    << "# DataType: "   << "YUV411" << endl
	    << "# Sign: "	<< "Unsigned" << endl;
	break;
      case Img::YUV_422:
	out << "# PixelLenth: " << 2 << endl
	    << "# DataType: "   << "YUV422" << endl
	    << "# Sign: "	<< "Unsigned" << endl;
	break;
      case Img::YUYV_422:
	out << "# PixelLenth: " << 2 << endl
	    << "# DataType: "   << "YUYV422" << endl
	    << "# Sign: "	<< "Unsigned" << endl;
	break;
      case Img::YUV_444:
	out << "# PixelLenth: " << 3 << endl
	    << "# DataType: "   << "YUV444" << endl
	    << "# Sign: "	<< "Unsigned" << endl;
	break;
      case Img::RGB_24:
	out << "# PixelLenth: " << 3 << endl
	    << "# DataType: "   << "RGB24" << endl
	    << "# Sign: "	<< "Unsigned" << endl;
	break;
      case Img::FLT:
	out << "# PixelLenth: " << 4 << endl
	    << "# DataType: "   << "Float" << endl
	    << "# Sign: "	<< "Signed" << endl;
	break;
      default:
	break;
    }
    
    out << "# PinHoleParameterH11: " << header.P[0][0] << endl
	<< "# PinHoleParameterH12: " << header.P[0][1] << endl
	<< "# PinHoleParameterH13: " << header.P[0][2] << endl
	<< "# PinHoleParameterH14: " << header.P[0][3] << endl
	<< "# PinHoleParameterH21: " << header.P[1][0] << endl
	<< "# PinHoleParameterH22: " << header.P[1][1] << endl
	<< "# PinHoleParameterH23: " << header.P[1][2] << endl
	<< "# PinHoleParameterH24: " << header.P[1][3] << endl
	<< "# PinHoleParameterH31: " << header.P[2][0] << endl
	<< "# PinHoleParameterH32: " << header.P[2][1] << endl
	<< "# PinHoleParameterH33: " << header.P[2][2] << endl
	<< "# PinHoleParameterH34: " << header.P[2][3] << endl;
    if (header.d1 != 0.0 || header.d2 != 0.0)
	out << "# DistortionParameterD1: " << header.d1 << endl
	    << "# DistortionParameterD2: " << header.d2 << endl;
    out << header.width << ' ' << header.height << endl
	<< 255 << endl;
    
}
    
}
