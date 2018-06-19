/*
 *  $Id$
 */
#include "ImageView.h"
#include "TU/Image++.h"
#include "MultiImage.hh"

namespace TU
{
template <> void
ImageView::setImage(MultiImage::PixelFormat pixelFormat,
		    size_t width, size_t height, const uchar* data)
{
    switch (pixelFormat)
    {
      case MultiImage::MONO_8:
	_data.resize(width*height);
	std::copy_n(data, width*height, _data.data());
	_qimage = QImage(_data.data(),
			 width, height, width, QImage::Format_Indexed8);
	_qimage.setColorTable(_colors);
	break;
	
      case MultiImage::YUV_411:
	_data.resize(3*width*height);
	std::copy_n(make_pixel_iterator(reinterpret_cast<const YUV411*>(data)),
		    width*height/4,
		    make_pixel_iterator(reinterpret_cast<RGB*>(_data.data())));
	_qimage = QImage(_data.data(),
			 width, height, 3*width, QImage::Format_RGB888);
	_qimage.setColorTable(_colors);
	break;

      case MultiImage::YUV_422:
	_data.resize(3*width*height);
	std::copy_n(make_pixel_iterator(reinterpret_cast<const YUV422*>(data)),
		    width*height/2,
		    make_pixel_iterator(reinterpret_cast<RGB*>(_data.data())));
	_qimage = QImage(_data.data(),
			 width, height, 3*width, QImage::Format_RGB888);
	_qimage.setColorTable(_colors);
	break;

      case MultiImage::YUYV_422:
	_data.resize(3*width*height);
	std::copy_n(make_pixel_iterator(reinterpret_cast<const YUYV422*>(data)),
		    width*height/2,
		    make_pixel_iterator(reinterpret_cast<RGB*>(_data.data())));
	_qimage = QImage(_data.data(),
			 width, height, 3*width, QImage::Format_RGB888);
	_qimage.setColorTable(_colors);
	break;

      case MultiImage::YUV_444:
	_data.resize(3*width*height);
	std::copy_n(make_pixel_iterator(reinterpret_cast<const YUV444*>(data)),
		    width*height,
		    make_pixel_iterator(reinterpret_cast<RGB*>(_data.data())));
	_qimage = QImage(_data.data(),
			 width, height, 3*width, QImage::Format_RGB888);
	_qimage.setColorTable(_colors);
	break;

      case MultiImage::FLT:
      {
	auto	p = reinterpret_cast<const float*>(data);

	_data.resize(width * height);
	for (auto q = _data.begin(); q != _data.end(); ++q)
	{
	    int	delta = int(*p++ * 0.5f + 0.5f);
	    if (delta < -128)
		delta = -128;
	    else if (delta > 127)
		delta = 127;
	    *q = 128 + delta;
	}
	_qimage = QImage(_data.data(),
			 width, height, width, QImage::Format_Indexed8);
	_qimage.setColorTable(_colorsF);
      }
	break;

      case MultiImage::RGB_24:
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

}	// namespace TU
