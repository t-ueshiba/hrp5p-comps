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
ImageViewerRTC<Img::TimedImages>
::setImages(v::MyCmdWindow<Img::TimedImages, Img::PixelFormat>& win) const
{
    std::unique_lock<std::mutex>	lock(_mutex);

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
	      case Img::MONO_8:
		canvas.reset(
		    new v::MyCanvas<u_char, Img::PixelFormat>(
			win, header.width, header.height, header.format));
		break;
	      case Img::YUV_411:
		canvas.reset(
		    new v::MyCanvas<TU::YUV411, Img::PixelFormat>(
			win, header.width, header.height, header.format));
		break;
	      case Img::YUV_422:
		canvas.reset(
		    new v::MyCanvas<TU::YUV422, Img::PixelFormat>(
			win, header.width, header.height, header.format));
		break;
	      case Img::YUYV_422:
		canvas.reset(
		    new v::MyCanvas<TU::YUYV422, Img::PixelFormat>(
			win, header.width, header.height, header.format));
		break;
	      case Img::YUV_444:
		canvas.reset(
		    new v::MyCanvas<TU::YUV444, Img::PixelFormat>(
			win, header.width, header.height, header.format));
		break;
	      case Img::RGB_24:
		canvas.reset(
		    new v::MyCanvas<TU::RGB, Img::PixelFormat>(
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
