/*
 *  $Id$
 */
#ifndef __TU_MULTICAMERA_H
#define __TU_MULTICAMERA_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include <coil/Guard.h>
#include "Img.hh"
#include "CmdSVC_impl.h"

namespace TU
{
/************************************************************************
*  class MultiCamera<CAMERAS>						*
************************************************************************/
template <class CAMERAS>
class MultiCamera : public RTC::DataFlowComponentBase
{
  private:
    typedef typename CAMERAS::camera_type	camera_type;

  public:
    MultiCamera(RTC::Manager* manager)					;
    ~MultiCamera()							;

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;
#ifdef DEBUG
    virtual RTC::ReturnCode_t	onFinalize()				;
    virtual RTC::ReturnCode_t	onStartup(RTC::UniqueId ec_id);
    virtual RTC::ReturnCode_t	onShutdown(RTC::UniqueId ec_id);
#endif
  //virtual RTC::ReturnCode_t	onError(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onReset(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onStateUpdate(RTC::UniqueId ec_id)	;
  //virtual RTC::ReturnCode_t	onRateChanged(RTC::UniqueId ec_id)	;

    const CAMERAS&	cameras()				const	;
    void		continuousShot()				;
    void		stopContinuousShot()				;
    void		setFormat(u_int id, int val)			;
    void		setFeatureValue(u_int id, int val, size_t n)	;
    
  private:
    static size_t	setImageHeader(const camera_type& camera,
				       Img::TimedImage& image)		;
    RTC::Time		getTime(const camera_type& camera)	const	;
    void		allocateImages()				;
    
  private:
    CAMERAS				_cameras;
    mutable coil::Mutex			_mutex;
    std::string				_cameraConfig;	// config var.
    int					_useTimestamp;	// config var.
    Img::TimedImages			_images;	// out data
    RTC::OutPort<Img::TimedImages>	_imagesOut;	// out data port
    v::CmdSVC_impl<CAMERAS>		_command;	// service provider
    RTC::CorbaPort			_commandPort;	// service port
};

template <class CAMERAS>
MultiCamera<CAMERAS>::~MultiCamera()
{
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCamera<CAMERAS>::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiCamera::onActivated" << std::endl;
#endif
    allocateImages();
    
    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCamera<CAMERAS>::onExecute(RTC::UniqueId ec_id)
{
  //#ifdef DEBUG
#if 0
    static int	n = 0;
    std::cerr << "MultiCamera::onExecute: " << n++ << std::endl;
#endif
    coil::Guard<coil::Mutex>	guard(_mutex);
    
    if (_cameras.size() && _cameras[0]->inContinuousShot())
    {
	exec(_cameras, &camera_type::snap);		// invalid for MacOS
	for (size_t i = 0; i < _cameras.size(); ++i)
	{
	    _cameras[i]->captureRaw(_images[i].data.get_buffer());
	    _images[i].tm = getTime(*_cameras[i]);
	}
	_imagesOut.write();
    }

    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCamera<CAMERAS>::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiCamera::onDeactivated" << std::endl;
#endif
    exec(_cameras, &camera_type::stopContinuousShot);
    
    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCamera<CAMERAS>::onAborting(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiCamera::onAborting" << std::endl;
#endif
    exec(_cameras, &camera_type::stopContinuousShot);	// 連続撮影を終了

    return RTC::RTC_OK;
}

#ifdef DEBUG
template <class CAMERAS> RTC::ReturnCode_t
MultiCamera<CAMERAS>::onFinalize()
{
    std::cerr << "MultiCmaera::onFinalize" << std::endl;

    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCamera<CAMERAS>::onStartup(RTC::UniqueId ec_id)
{
    std::cerr << "MultiCamera::onStartup" << std::endl;

    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCamera<CAMERAS>::onShutdown(RTC::UniqueId ec_id)
{
    std::cerr << "MultiCamera::onShutdown" << std::endl;

    return RTC::RTC_OK;
}
#endif

template <class CAMERAS> inline const CAMERAS&
MultiCamera<CAMERAS>::cameras() const
{
    return _cameras;
}

template <class CAMERAS> inline void
MultiCamera<CAMERAS>::continuousShot()
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    exec(_cameras, &camera_type::continuousShot);
}
    
template <class CAMERAS> inline void
MultiCamera<CAMERAS>::stopContinuousShot()
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    exec(_cameras, &camera_type::stopContinuousShot);
}

template <class CAMERAS> inline void
MultiCamera<CAMERAS>::setFormat(u_int id, int val)
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    TU::setFormat(_cameras, id, val);
    allocateImages();
}
    
template <class CAMERAS> inline void
MultiCamera<CAMERAS>::setFeatureValue(u_int id, int val, size_t n)
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    TU::setFeatureValue(_cameras, id, val, n);
}
    
template <class CAMERAS> void
MultiCamera<CAMERAS>::allocateImages()
{
    _images.length(_cameras.size());
    for (size_t i = 0; i < _cameras.size(); ++i)
	_images[i].data.length(setImageHeader(*_cameras[i], _images[i]));
}

}
#endif	// !__TU_MULTICAMERA_H
