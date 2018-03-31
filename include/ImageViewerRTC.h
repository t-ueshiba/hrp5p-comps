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
*  class ImageViewerRTC<IMAGES>						*
************************************************************************/
template <class IMAGES>
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
    template <class VIEWER>
    bool			setImages(VIEWER&)		const	;
    
  protected:
    mutable std::mutex	_mutex;
    mutable bool	_ready;
    IMAGES		_images;
    RTC::InPort<IMAGES>	_imagesIn;
};

template <class IMAGES>
ImageViewerRTC<IMAGES>::ImageViewerRTC(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _mutex(),
     _ready(false),
     _images(),
     _imagesIn("images", _images)
{
}

template <class IMAGES> RTC::ReturnCode_t
ImageViewerRTC<IMAGES>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "ImageViewerRTC<IMAGES>::onInitialize" << std::endl;
#endif
    addInPort("images", _imagesIn);
    
    return RTC::RTC_OK;
}

template <class IMAGES> RTC::ReturnCode_t
ImageViewerRTC<IMAGES>::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "ImageViewerRTC<IMAGES>::onActivated" << std::endl;
#endif
    _ready = false;
    
    return RTC::RTC_OK;
}

template <class IMAGES> RTC::ReturnCode_t
ImageViewerRTC<IMAGES>::onExecute(RTC::UniqueId ec_id)
{
    std::unique_lock<std::mutex>	lock(_mutex);
    
    if (!_ready && _imagesIn.isNew())
    {
	_imagesIn.read();
	_ready = true;
    }

    return RTC::RTC_OK;
}

template <class IMAGES> RTC::ReturnCode_t
ImageViewerRTC<IMAGES>::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "ImageViewerRTC<IMAGES>::onDeactivated" << std::endl;
#endif
    _ready = false;

    return RTC::RTC_OK;
}

template <class IMAGES> RTC::ReturnCode_t
ImageViewerRTC<IMAGES>::onAborting(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "ImageViewerRTC<IMAGES>::onAborting" << std::endl;
#endif
    _ready = false;
    
    return RTC::RTC_OK;
}

template <class IMAGES> inline bool
ImageViewerRTC<IMAGES>::isExiting() const
{
    std::unique_lock<std::mutex>	lock(_mutex);

    return m_exiting;
}

}	// namespace TU
#endif	// TU_IMAGEVIEWERRTC_H
