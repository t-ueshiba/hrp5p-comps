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
#include "Img.hh"

namespace TU
{
class MultiImageViewerRTC;

namespace v
{
/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
class MyCmdWindow : public CmdWindow
{
  private:
    class CanvasBase : public CanvasPane
    {
      public:
	CanvasBase(Window& win, size_t width, size_t height)
	    :CanvasPane(win, width, height), _dc(*this)			{}
	virtual		~CanvasBase()					{}

	virtual bool	conform(const Img::Header& header)	const	= 0;
	virtual void	setImageData(size_t width,
				     size_t height, const void* data)	= 0;

      protected:
	CanvasPaneDC	_dc;
    };

    template <class T>
    class Canvas : public CanvasBase
    {
      public:
	Canvas(Window& win, const Img::Header& header)
	    :CanvasBase(win, header.width, header.height),
	     _format(header.format), _image()				{}

	virtual bool	conform(const Img::Header& header)	const	;
	virtual void	setImageData(size_t width,
				     size_t height, const void* data)	;
	virtual void	repaintUnderlay()				;
	virtual void	callback(TU::v::CmdId id, TU::v::CmdVal val)	;
	
      private:
	const Img::PixelFormat	_format;
	Image<T>		_image;
    };
    
  public:
    MyCmdWindow(App& vapp, MultiImageViewerRTC* rtc)			;
    ~MyCmdWindow()							;

    size_t		nviews()				const	;
    bool		resize(size_t nviews)				;
    void		setImage(size_t view, const Img::Header& header,
				 const void* data)			;
  //virtual void	callback(CmdId id, CmdVal val)			;
    virtual void	tick()						;
    
  private:
    std::unique_ptr<MultiImageViewerRTC>	_rtc;
    TU::Array<std::unique_ptr<CanvasBase> >	_canvases;
    Timer					_timer;
};

inline size_t
MyCmdWindow::nviews() const
{
    return _canvases.size();
}

template <class T> inline bool
MyCmdWindow::Canvas<T>::conform(const Img::Header& header) const
{
    return ((_format == header.format) &&
	    (_image.width()  == header.width) &&
	    (_image.height() == header.height));
}

template <class T> void
MyCmdWindow::Canvas<T>::setImageData(size_t width,
				     size_t height, const void* data)
{
    _dc.setSize(width, height, 1);

    _image.resize(height, width);
    auto	p = static_cast<const T*>(data);
    for (auto& imageLine : _image)
	for (auto& pixel : imageLine)
	    pixel = *p++;
}

template <class T> void
MyCmdWindow::Canvas<T>::repaintUnderlay()
{
    _dc << _image;
}

template <class T> void
MyCmdWindow::Canvas<T>::callback(CmdId id, CmdVal val)
{
    switch (id)
    {
      case Id_MouseButton1Press:
      case Id_MouseButton1Drag:
      case Id_MouseButton1Release:
      case Id_MouseMove:
	parent().callback(id,
			  CmdVal(_dc.dev2logU(val.u()), _dc.dev2logV(val.v())));
	return;
    }

    parent().callback(id, val);
}

}	// namespace v
}	// namespace TU
#endif	// MYCMDWINDOW_H
