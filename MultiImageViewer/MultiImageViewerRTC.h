/*
 *  $Id$
 */
#ifndef MULTIIMAGEVIEWERRTC_H
#define MULTIIMAGEVIEWERRTC_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
//#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
//#include <rtm/idl/ExtendedDataTypesSkel.h>
//#include <rtm/idl/InterfaceDataTypesSkel.h>
#include "MyCmdWindow.h"

namespace TU
{
/************************************************************************
*  class MultiImageViewerRTC						*
************************************************************************/
class MultiImageViewerRTC : public RTC::DataFlowComponentBase
{
  public:
    MultiImageViewerRTC(RTC::Manager* manager)				;
    ~MultiImageViewerRTC();

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
#ifdef DEBUG
    virtual RTC::ReturnCode_t	onStartup(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onShutdown(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onFinalize()				;
  //virtual RTC::ReturnCode_t	onError(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onReset(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onStateUpdatedRTC::UniqueId ec_id)	;
  //virtual RTC::ReturnCode_t	onRateChanged(RTC::UniqueId ec_id)	;
#endif
    bool	setImages(v::MyCmdWindow& win)				;
    bool	alive()						const	;
    
  protected:
    mutable coil::Mutex			_mutex;
    bool				_alive;
    bool				_isNew;
    Img::TimedImages			_images;
    RTC::InPort<Img::TimedImages>	_imagesIn;
};

inline bool
MultiImageViewerRTC::alive() const
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    return _alive;
}

}	// namespace TU
#endif	// MULTIIMAGEVIEWERRTC_H
