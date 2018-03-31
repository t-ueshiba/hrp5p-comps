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
#include "ImageViewerRTC.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCanvas<FORMAT>						*
************************************************************************/
template <class FORMAT>
class MyCanvasBase : public CanvasPane
{
  public:
    MyCanvasBase(Window& win, size_t width, size_t height)
	:CanvasPane(win, 640, 480), _dc(*this, width, height)		{}
    virtual		~MyCanvasBase()					{}

    virtual bool	conform(size_t width,
				size_t height, FORMAT format)	const	= 0;
    virtual void	setImage(size_t width,
				 size_t height, const void* data)	= 0;

  protected:
    CanvasPaneDC	_dc;
};

template <class T, class FORMAT>
class MyCanvas : public MyCanvasBase<FORMAT>
{
  private:
    using	MyCanvasBase<FORMAT>::_dc;
    using	MyCanvasBase<FORMAT>::parent;
	
  public:
    MyCanvas(Window& win, size_t width, size_t height, FORMAT format)
	:MyCanvasBase<FORMAT>(win, width, height), _format(format)	{}

    virtual bool	conform(size_t width,
				size_t height, FORMAT format) const
			{
			    return ((_format == format) &&
				    (_image.width()  == width) &&
				    (_image.height() == height));
			}
    virtual void	setImage(size_t width, size_t height, const void* data)
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
    Image<T>		_image;
};
    
/************************************************************************
*  class MyCmdWindow<IMAGES, FORMAT>					*
************************************************************************/
template <class IMAGES, class FORMAT>
class MyCmdWindow : public CmdWindow,
		    public Array<std::unique_ptr<MyCanvasBase<FORMAT> > >
{
  private:
    using array_type	= Array<std::unique_ptr<MyCanvasBase<FORMAT> > >;
    using rtc_type	= ImageViewerRTC<IMAGES>;
    
  public:
    MyCmdWindow(App& vapp, rtc_type* rtc)	;
    ~MyCmdWindow()				;
    
    virtual void	tick()			;
    
  private:
    rtc_type*		_rtc;
    Timer		_timer;
};

template <class IMAGES, class FORMAT>
MyCmdWindow<IMAGES, FORMAT>::MyCmdWindow(App& app, rtc_type* rtc)
    :CmdWindow(app, "ImageViewer", Colormap::RGBColor, 16, 0, 0),
     array_type(0), _rtc(rtc), _timer(*this, 0)
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
    else if (_rtc->setImages(*this))
    {
	for (auto& canvas : *this)
	    canvas->repaintUnderlay();
    }
}

}	// namespace v
}	// namespace TU
#endif	// MYCMDWINDOW_H
