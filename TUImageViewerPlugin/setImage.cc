/*
 *  $Id$
 */
#include "ImageView.h"
#include "Img.hh"

namespace TU
{
/************************************************************************
*  class ImageView							*
************************************************************************/
template <> void
ImageView::setImage(Img::ColorFormat format,
		    size_t width, size_t height, const uchar* data)
{
    switch (format)
    {
      case Img::CF_GRAY:
	_data.resize(width*height);
	std::copy_n(data, width*height, _data.data());
	_qimage = QImage(_data.data(),
			 width, height, width, QImage::Format_Indexed8);
	_qimage.setColorTable(_colors);
	break;
	
      case Img::CF_RGB:
	_data.resize(3*width*height);
	std::copy_n(data, 3*width*height, _data.data());
	_qimage = QImage(_data.data(),
			 width, height, 3*width, QImage::Format_RGB888);
	_qimage.setColorTable(_colors);
	break;

      default:
	break;
    }
}
    
}
