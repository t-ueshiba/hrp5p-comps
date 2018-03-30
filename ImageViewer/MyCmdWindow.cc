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
MyCmdWindow<Img::TimedCameraImage, Img::ColorFormat>::setImages()
{
    const auto	generated = _canvases.resize(1);
    auto&	canvas	  = _canvases[0];
    const auto&	imageData = _images.data.image;
	
    if (canvas == nullptr ||
	!canvas->conform(imageData.width, imageData.height, imageData.format))
    {
	switch (imageData.format)
	{
	  case Img::CF_GRAY:
	    canvas.reset(new Canvas<u_char>(*this, imageData.width,
					    imageData.height,
					    imageData.format));
	    break;
	  case Img::CF_RGB:
	    canvas.reset(new Canvas<TU::RGB>(*this, imageData.width,
					     imageData.height,
					     imageData.format));
	    break;
	  default:
	    throw std::runtime_error("Unsupported format!!");
	    break;
	}

	canvas->place(0, 0, 1, 1);
	canvas->setImage(imageData.width, imageData.height,
			  imageData.raw_data.get_buffer());
    }

    if (generated)
	show();
}
    
}	// namespace v
}	// namespace TU

