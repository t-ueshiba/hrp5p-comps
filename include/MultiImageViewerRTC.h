/*
 *  $Id$
 */
#ifndef TU_MULTIIMAGEVIEWERRTC_H
#define TU_MULTIIMAGEVIEWERRTC_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/DataInPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <mutex>

namespace TU
{
/************************************************************************
*  class MultiImageViewerRTC<IMAGES>					*
************************************************************************/
template <class IMAGES>
class MultiImageViewerRTC : public RTC::DataFlowComponentBase
{
  public:
    MultiImageViewerRTC(RTC::Manager* manager)				;

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;
    bool			isExiting()			const	;
    bool			getImages(IMAGES& images)		;
    template <class VIEWER>
    bool			setImages(VIEWER*)			;
    
  protected:
    mutable std::mutex	_mutex;
    bool		_ready;
    IMAGES		_images;
    RTC::InPort<IMAGES>	_imagesIn;
};

template <class IMAGES>
MultiImageViewerRTC<IMAGES>::MultiImageViewerRTC(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _mutex(),
     _ready(false),
     _images(),
     _imagesIn("images", _images)
{
}

template <class IMAGES> RTC::ReturnCode_t
MultiImageViewerRTC<IMAGES>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "MultiImageViewerRTC<IMAGES>::onInitialize" << std::endl;
#endif
    addInPort("images", _imagesIn);
    
    return RTC::RTC_OK;
}

template <class IMAGES> RTC::ReturnCode_t
MultiImageViewerRTC<IMAGES>::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiImageViewerRTC<IMAGES>::onActivated" << std::endl;
#endif
    _ready = false;
    
    return RTC::RTC_OK;
}

template <class IMAGES> RTC::ReturnCode_t
MultiImageViewerRTC<IMAGES>::onExecute(RTC::UniqueId ec_id)
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
MultiImageViewerRTC<IMAGES>::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiImageViewerRTC<IMAGES>::onDeactivated" << std::endl;
#endif
    _ready = false;

    return RTC::RTC_OK;
}

template <class IMAGES> RTC::ReturnCode_t
MultiImageViewerRTC<IMAGES>::onAborting(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiImageViewerRTC<IMAGES>::onAborting" << std::endl;
#endif
    _ready = false;
    
    return RTC::RTC_OK;
}

template <class IMAGES> inline bool
MultiImageViewerRTC<IMAGES>::isExiting() const
{
    std::unique_lock<std::mutex>	lock(_mutex);

    return m_exiting;
}
    
template <class IMAGES> inline bool
MultiImageViewerRTC<IMAGES>::getImages(IMAGES& images)
{
    std::unique_lock<std::mutex>	lock(_mutex);

    if (!_ready)
	return false;

    images = _images;
    _ready = false;
    
    return true;
}

}	// namespace TU
#endif	// TU_MULTIIMAGEVIEWERRTC_H
