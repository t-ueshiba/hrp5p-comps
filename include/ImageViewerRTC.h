/*
 *  $Id$
 */
#ifndef TU_IMAGEVIEWERRTC_H
#define TU_IMAGEVIEWERRTC_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/DataInPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <mutex>

namespace TU
{
/************************************************************************
*  class ImageViewerRTC<IMAGE>						*
************************************************************************/
template <class IMAGE>
class ImageViewerRTC : public RTC::DataFlowComponentBase
{
  public:
    ImageViewerRTC(RTC::Manager* manager)				;

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;
    bool			isExiting()			const	;
    bool			getImage(IMAGE& image)		const	;
    template <class VIEWER>
    bool			setImage(VIEWER&)		const	;
    
  protected:
    mutable std::mutex	_mutex;
    mutable bool	_ready;
    IMAGE		_image;
    RTC::InPort<IMAGE>	_imageIn;
};

template <class IMAGE>
ImageViewerRTC<IMAGE>::ImageViewerRTC(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _mutex(),
     _ready(false),
     _image(),
     _imageIn("image", _image)
{
}

template <class IMAGE> RTC::ReturnCode_t
ImageViewerRTC<IMAGE>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "ImageViewerRTC<IMAGE>::onInitialize" << std::endl;
#endif
    addInPort("image", _imageIn);
    
    return RTC::RTC_OK;
}

template <class IMAGE> RTC::ReturnCode_t
ImageViewerRTC<IMAGE>::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "ImageViewerRTC<IMAGE>::onActivated" << std::endl;
#endif
    _ready = false;
    
    return RTC::RTC_OK;
}

template <class IMAGE> RTC::ReturnCode_t
ImageViewerRTC<IMAGE>::onExecute(RTC::UniqueId ec_id)
{
    std::unique_lock<std::mutex>	lock(_mutex);
    
    if (!_ready && _imageIn.isNew())
    {
	_imageIn.read();
	_ready = true;
    }

    return RTC::RTC_OK;
}

template <class IMAGE> RTC::ReturnCode_t
ImageViewerRTC<IMAGE>::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "ImageViewerRTC<IMAGE>::onDeactivated" << std::endl;
#endif
    _ready = false;

    return RTC::RTC_OK;
}

template <class IMAGE> RTC::ReturnCode_t
ImageViewerRTC<IMAGE>::onAborting(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "ImageViewerRTC<IMAGE>::onAborting" << std::endl;
#endif
    _ready = false;
    
    return RTC::RTC_OK;
}

template <class IMAGE> inline bool
ImageViewerRTC<IMAGE>::isExiting() const
{
    std::unique_lock<std::mutex>	lock(_mutex);

    return m_exiting;
}
    
template <class IMAGE> inline bool
ImageViewerRTC<IMAGE>::getImage(IMAGE& image) const
{
    std::unique_lock<std::mutex>	lock(_mutex);

    if (!_ready)
	return false;

    image = _image;
    _ready = false;
    
    return true;
}

}	// namespace TU
#endif	// TU_IMAGEVIEWERRTC_H
