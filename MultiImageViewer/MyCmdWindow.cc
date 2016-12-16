/*
 *  $Id$
 */
#include "MyCmdWindow.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
MyCmdWindow::MyCmdWindow(App& app, MultiImageViewerRTC* rtc)
    :CmdWindow(app, "MultiImageViewer", Colormap::RGBColor, 16, 0, 0),
     _rtc(rtc),
     _canvases(0),
     _timer(*this, 0)
{
    _timer.start(5);
}

MyCmdWindow::~MyCmdWindow()
{
    if (_rtc)
    {
	_rtc->exit();
	RTC::Manager::instance().cleanupComponents();
    }
}
    
void
MyCmdWindow::setImages()
{
    const auto	resized = _canvases.resize(_images.headers.length());

    auto	data = _images.data.get_buffer();
    for (size_t	i = 0; i < _canvases.size(); ++i)
    {
	auto&		canvas = _canvases[i];
	const auto&	header = _images.headers[i];
	
	if (canvas == nullptr || !canvas->conform(header))
	{
	    switch (header.format)
	    {
	      case Img::MONO_8:
		canvas.reset(new Canvas<u_char>(*this, header));
		break;
	      case Img::YUV_411:
		canvas.reset(new Canvas<TU::YUV411>(*this, header));
		break;
	      case Img::YUV_422:
		canvas.reset(new Canvas<TU::YUV422>(*this, header));
		break;
	      case Img::YUV_444:
		canvas.reset(new Canvas<TU::YUV444>(*this, header));
		break;
	      case Img::RGB_24:
		canvas.reset(new Canvas<TU::RGB>(*this, header));
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
    
void
MyCmdWindow::tick()
{
    if (_rtc->isExiting())
    {
	_timer.stop();
	_rtc = nullptr;
	app().exit();
    }
    else if (_rtc->getImages(_images))
    {
	setImages();
	
	for (auto& canvas : _canvases)
	    canvas->repaintUnderlay();
    }
}

}	// namespace v
}	// namespace TU

