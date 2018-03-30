/*
 *  $Id$
 */
#ifndef MYCMDWINDOW_H
#define MYCMDWINDOW_H

#include <memory>	// for std::unique_ptr<T>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CanvasPane.h"
#include "TU/v/CanvasPaneDC.h"
#include "TU/v/Timer.h"
#include "MultiImageViewerRTC.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCmdWindow<IMAGES, FORMAT>					*
************************************************************************/
template <class IMAGES, class FORMAT>
class MyCmdWindow : public CmdWindow
{
  private:
    using rtc_type	= MultiImageViewerRTC<IMAGES>;
    
    class CanvasBase : public CanvasPane
    {
      public:
	CanvasBase(Window& win, size_t width, size_t height)
	    :CanvasPane(win, 320, 240), _dc(*this, width, height)	{}
	virtual		~CanvasBase()					{}

	virtual bool	conform(size_t width,
				size_t height, FORMAT format)	const	= 0;
	virtual void	setImage(size_t width,
				 size_t height, const void* data)	= 0;

      protected:
	CanvasPaneDC	_dc;
    };

    template <class T>
    class Canvas : public CanvasBase
    {
      private:
	using	CanvasBase::_dc;
	using	CanvasBase::parent;
	
      public:
	Canvas(Window& win, size_t width, size_t height, FORMAT format)
	    :CanvasBase(win, width, height), _format(format), _image()	{}

	virtual bool	conform(size_t width,
				size_t height, FORMAT format) const
			{
			    return ((_format == format) &&
				    (_image.width()  == width) &&
				    (_image.height() == height));
			}
	virtual void	setImage(size_t width,
				 size_t height, const void* data)
			{
			    _image.resize((T*)data, height, width);
			}
	
	virtual void	repaintUnderlay()
			{
			    _dc << _image;
			}
	virtual void	callback(TU::v::CmdId id, TU::v::CmdVal val)
			{
			    switch (id)
			    {
			      case Id_MouseButton1Press:
			      case Id_MouseButton1Drag:
			      case Id_MouseButton1Release:
			      case Id_MouseMove:
				parent().callback(
					id,
					CmdVal(_dc.dev2logU(val.u()),
					       _dc.dev2logV(val.v())));
				return;
			    }

			    parent().callback(id, val);
			}
	
      private:
	const FORMAT	_format;
	Image<T>	_image;
    };
    
  public:
    MyCmdWindow(App& vapp, rtc_type* rtc)				;
    ~MyCmdWindow()							;
    
    void		setImages()					;
    virtual void	tick()						;
    
  private:
    rtc_type*				_rtc;
    IMAGES				_images;
    Array<std::unique_ptr<CanvasBase> >	_canvases;
    Timer				_timer;
};

template <class IMAGES, class FORMAT>
MyCmdWindow<IMAGES, FORMAT>::MyCmdWindow(App& app, rtc_type* rtc)
    :CmdWindow(app, "ImageViewer", Colormap::RGBColor, 16, 0, 0),
     _rtc(rtc), _canvases(0), _timer(*this, 0)
{
    _timer.start(5);
}

template <class IMAGES, class FORMAT>
MyCmdWindow<IMAGES, FORMAT>::~MyCmdWindow()
{
    if (_rtc)
	_rtc->exit();
    RTC::Manager::instance().cleanupComponents();
}
    
template <class IMAGES, class FORMAT> void
MyCmdWindow<IMAGES, FORMAT>::tick()
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
#endif	// MYCMDWINDOW_H
