/*
 *  $Id$
 */
#ifndef MULTIIMAGEVIEWERRTC_H
#define MULTIIMAGEVIEWERRTC_H

#include "Img.hh"
#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/DataInPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <mutex>

namespace TU
{
/************************************************************************
*  class MultiImageViewerRTC						*
************************************************************************/
class MultiImageViewerRTC : public RTC::DataFlowComponentBase
{
  public:
    MultiImageViewerRTC(RTC::Manager* manager)				;

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;
#ifdef DEBUG
    virtual RTC::ReturnCode_t	onStartup(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onShutdown(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onFinalize()				;
  //virtual RTC::ReturnCode_t	onError(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onReset(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onStateUpdatedRTC::UniqueId ec_id)	;
  //virtual RTC::ReturnCode_t	onRateChanged(RTC::UniqueId ec_id)	;
#endif
    bool			isExiting()			const	;
    bool			getImages(Img::TimedImages& images)	;
    
  protected:
    mutable std::mutex			_mutex;
    bool				_ready;
    Img::TimedImages			_images;
    RTC::InPort<Img::TimedImages>	_imagesIn;
};

inline bool
MultiImageViewerRTC::isExiting() const
{
    std::unique_lock<std::mutex>	lock(_mutex);

    return m_exiting;
}
    
inline bool
MultiImageViewerRTC::getImages(Img::TimedImages& images)
{
    std::unique_lock<std::mutex>	lock(_mutex);

    if (!_ready)
	return false;

    images = _images;
    _ready = false;
    
    return true;
}

}	// namespace TU
#endif	// MULTIIMAGEVIEWERRTC_H
