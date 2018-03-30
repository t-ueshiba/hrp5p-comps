/*
 *  $Id$
 */
#include "MyCmdWindow.h"
#include "Img.hh"

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCmdWindow<IMAGES, FORMAT>					*
************************************************************************/
template <> void
MyCmdWindow<Img::TimedImages, Img::PixelFormat>::setImages()
{
    const auto	resized = _canvases.resize(_images.headers.length());

    auto	data = _images.data.get_buffer();
    for (size_t	i = 0; i < _canvases.size(); ++i)
    {
	auto&		canvas = _canvases[i];
	const auto&	header = _images.headers[i];
	
	if (canvas == nullptr ||
	    !canvas->conform(header.width, header.height, header.format))
	{
	    switch (header.format)
	    {
	      case Img::MONO_8:
		canvas.reset(new Canvas<u_char>(*this, header.width,
						header.height,
						header.format));
		break;
	      case Img::YUV_411:
		canvas.reset(new Canvas<TU::YUV411>(*this, header.width,
						    header.height,
						    header.format));
		break;
	      case Img::YUV_422:
		canvas.reset(new Canvas<TU::YUV422>(*this, header.width,
						    header.height,
						    header.format));
		break;
	      case Img::YUYV_422:
		canvas.reset(new Canvas<TU::YUYV422>(*this, header.width,
						    header.height,
						    header.format));
		break;
	      case Img::YUV_444:
		canvas.reset(new Canvas<TU::YUV444>(*this, header.width,
						    header.height,
						    header.format));
		break;
	      case Img::RGB_24:
		canvas.reset(new Canvas<TU::RGB>(*this, header.width,
						    header.height,
						    header.format));
		break;
	      default:
		throw std::runtime_error("Unsupported format!!");
		break;
	    }

	    canvas->place(i % 2, i / 2, 1, 1);
	}

	canvas->setImage(header.width, header.height, data);

	data += header.size;
    }

    if (resized)
	show();
}

}	// namespace v
}	// namespace TU

