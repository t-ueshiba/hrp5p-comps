/*
 *  $Id$
 */
#ifndef MULTIIMAGEVIEWER_H
#define MULTIIMAGEVIEWER_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CanvasPane.h"
#include "TU/v/CanvasPaneDC.h"
#include "Img.hh"

using namespace	RTC;

/************************************************************************
*  class MultiImageViewer						*
************************************************************************/
class MultiImageViewer : public RTC::DataFlowComponentBase
{
  private:
    class KernelBase : public TU::v::CanvasPane
    {
      public:
	KernelBase(TU::v::Window& win, Img::PixelFormat fmt)
	    :TU::v::CanvasPane(win, 320, 240),
	     _dc(*this, 320, 240),
	     format(fmt)						{}
	virtual		~KernelBase()					{}

	virtual bool	conform(const Img::TimedImage& image)	const	= 0;
	virtual void	display(const Img::TimedImage& image)		= 0;

      protected:
	TU::v::CanvasPaneDC	_dc;
	
      public:
	const Img::PixelFormat	format;
    };

    template <class T>
    class Kernel : public KernelBase
    {
      public:
	Kernel(TU::v::Window& win, Img::PixelFormat fmt)
	    :KernelBase(win, fmt), _image()				{}

	virtual bool	conform(const Img::TimedImage& image)	const	;
	virtual void	display(const Img::TimedImage& image)		;
	virtual void	repaintUnderlay()				;
	virtual void	callback(TU::v::CmdId id, TU::v::CmdVal val)	;
	
      private:
	TU::Image<T>	_image;
    };
    
  public:
    MultiImageViewer(RTC::Manager* manager)				;
    ~MultiImageViewer();

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
#ifdef DEBUG
    virtual RTC::ReturnCode_t	onFinalize()				;
    virtual RTC::ReturnCode_t	onStartup(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onShutdown(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onError(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onReset(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onStateUpdate(RTC::UniqueId ec_id)	;
  //virtual RTC::ReturnCode_t	onRateChanged(RTC::UniqueId ec_id)	;
#endif
    
  protected:
    Img::TimedImages		_images;
    InPort<Img::TimedImages>	_imagesIn;

  private:
    int				_argc;
    TU::v::App			_vapp;
    TU::v::CmdWindow		_win;
    TU::Array<KernelBase*>	_kernels;
};

template <class T> bool
MultiImageViewer::Kernel<T>::conform(const Img::TimedImage& image) const
{
    return ((format == image.format) &&
	    (_image.width()  == image.width) &&
	    (_image.height() == image.height));
}

template <class T> void
MultiImageViewer::Kernel<T>::display(const Img::TimedImage& image)
{
    _image.resize((T*)image.data.get_buffer(), image.height, image.width);
    _dc.setSize(_image.width(), _image.height(), 1);

    repaintUnderlay();
}

template <class T> void
MultiImageViewer::Kernel<T>::repaintUnderlay()
{
    _dc << _image;
}

template <class T> void
MultiImageViewer::Kernel<T>::callback(TU::v::CmdId id, TU::v::CmdVal val)
{
    using namespace	TU::v;

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

extern "C"
{
DLL_EXPORT void	MultiImageViewerInit(RTC::Manager* manager)		;
};

#endif // MULTIIMAGEVIEWER_H
