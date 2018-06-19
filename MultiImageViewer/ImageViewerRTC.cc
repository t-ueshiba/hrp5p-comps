/*
 *  $Id$
 */
#include "MultiImage.hh"
#include "MyCmdWindow.h"

namespace TU
{
/************************************************************************
*  class ImageViewerRTC<IMAGES>						*
************************************************************************/
template <> template <> bool
ImageViewerRTC<MultiImage::TimedImages>
::setImages(v::MyCmdWindow<MultiImage::TimedImages,
	    MultiImage::PixelFormat>& win) const
{
    std::lock_guard<std::mutex>	lock(_mutex);

    if (!_ready)		// 新データが到着していなければ...
  	return false;		// falseを返す.
    
    const auto	resized = win.resize(_images.headers.length());

    auto	data = _images.data.get_buffer();
    for (size_t	i = 0; i < win.size(); ++i)
    {
	auto&		canvas = win[i];
	const auto&	header = _images.headers[i];
	
	if (canvas == nullptr ||
	    !canvas->conform(header.width, header.height, header.format))
	{
	    switch (header.format)
	    {
	      case MultiImage::MONO_8:
		canvas.reset(
		    new v::MyCanvas<u_char, MultiImage::PixelFormat>(
			win, header.width, header.height, header.format));
		break;
	      case MultiImage::YUV_411:
		canvas.reset(
		    new v::MyCanvas<TU::YUV411, MultiImage::PixelFormat>(
			win, header.width, header.height, header.format));
		break;
	      case MultiImage::YUV_422:
		canvas.reset(
		    new v::MyCanvas<TU::YUV422, MultiImage::PixelFormat>(
			win, header.width, header.height, header.format));
		break;
	      case MultiImage::YUYV_422:
		canvas.reset(
		    new v::MyCanvas<TU::YUYV422, MultiImage::PixelFormat>(
			win, header.width, header.height, header.format));
		break;
	      case MultiImage::YUV_444:
		canvas.reset(
		    new v::MyCanvas<TU::YUV444, MultiImage::PixelFormat>(
			win, header.width, header.height, header.format));
		break;
	      case MultiImage::RGB_24:
		canvas.reset(
		    new v::MyCanvas<TU::RGB, MultiImage::PixelFormat>(
			win, header.width, header.height, header.format));
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
	win.show();

    _ready = false;	// 新データは既にセットしてしまったので，falseにする．
    return true;	// 新データがセットされたことを呼出側に知らせる．
}

}	// namespace TU
