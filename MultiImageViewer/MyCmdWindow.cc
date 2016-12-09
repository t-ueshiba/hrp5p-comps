/*
 *  $Id$
 */
#include "MultiImageViewerRTC.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
MyCmdWindow::MyCmdWindow(App& app, TU::MultiImageViewerRTC* rtc)
    :CmdWindow(app, "MultiImageViewer", Colormap::RGBColor, 16, 0, 0),
     _rtc(rtc),
     _canvases(0),
     _timer(*this, 0)
{
    _timer.start(5);
}

MyCmdWindow::~MyCmdWindow()
{
}
	
bool
MyCmdWindow::resize(size_t nviews)
{
    return _canvases.resize(nviews);
}

void
MyCmdWindow::setImage(size_t view, const Img::Header& header, const void* data)
{
    auto&	canvas = _canvases[view];

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

	canvas->place(view % 2, view / 2, 1, 1);
    }

    canvas->setImageData(header.width, header.height, data);
}
    
void
MyCmdWindow::tick()

{
    if (_rtc->alive())
    {
	if (_rtc->setImages(*this))
	{
	    for (auto& canvas : _canvases)
		canvas->repaintUnderlay();
	}
    }
    else
    {
	_rtc = nullptr;
	_timer.stop();
	hide();
    }
}

}	// namespace v
}	// namespace TU

