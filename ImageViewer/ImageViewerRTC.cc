/*
 *  $Id$
 */
#include "Img.hh"
#include "MyCmdWindow.h"

namespace TU
{
/************************************************************************
*  class ImageViewerRTC<IMAGES>						*
************************************************************************/
template <> template <> bool
ImageViewerRTC<Img::TimedCameraImage>
::setImages(v::MyCmdWindow<Img::TimedCameraImage, Img::ColorFormat>& win) const
{
    std::lock_guard<std::mutex>	lock(_mutex);

    if (!_ready)		// 新データが到着していなければ...
  	return false;		// falseを返す.

    const auto	resized   = win.resize(1);
    auto&	canvas	  = win[0];
    const auto&	imageData = _images.data.image;
	
    if (canvas == nullptr ||
	!canvas->conform(imageData.width, imageData.height, imageData.format))
    {
	switch (imageData.format)
	{
	  case Img::CF_GRAY:
	    canvas.reset(
		new v::MyCanvas<u_char, Img::ColorFormat>(
		    win, imageData.width, imageData.height, imageData.format));
	    break;
	  case Img::CF_RGB:
	    canvas.reset(
		new v::MyCanvas<TU::RGB, Img::ColorFormat>(
		    win, imageData.width, imageData.height, imageData.format));
	    break;
	  default:
	    throw std::runtime_error("Unsupported format!!");
	    break;
	}

	canvas->place(0, 0, 1, 1);
	canvas->setImage(imageData.width, imageData.height,
			 imageData.raw_data.get_buffer());
    }

    if (resized)
	win.show();

    _ready = false;	// 新データは既にセットしてしまったので，falseにする．
    return true;	// 新データがセットされたことを呼出側に知らせる．
}
    
}	// namespace TU

