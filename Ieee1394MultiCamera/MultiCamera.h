/*
 *  $Id$
 */
#ifndef MULTICAMERA_H
#define MULTICAMERA_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include "Img.hh"
#include "CamSVC_impl.h"

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
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;
#ifdef DEBUG
    virtual RTC::ReturnCode_t	onFinalize()				;
    virtual RTC::ReturnCode_t	onStartup(RTC::UniqueId ec_id);
    virtual RTC::ReturnCode_t	onShutdown(RTC::UniqueId ec_id);
  //virtual RTC::ReturnCode_t	onError(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onReset(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onStateUpdate(RTC::UniqueId ec_id)	;
  //virtual RTC::ReturnCode_t	onRateChanged(RTC::UniqueId ec_id)	;

    size_t		ncameras()				const	;
    
#endif
    
  private:
    static size_t	setImageHeader(const camera_type& camera,
				       Img::TimedImage& image)		;
    RTC::Time		getTime(const camera_type& camera)	const	;
    
  private:
    CAMERAS				_cameras;
    std::string				_cameraConfig;
    int					_useTimestamp;

  protected:
    Img::TimedImages			_images;
    RTC::OutPort<Img::TimedImages>	_imagesOut;
    CamSVC_impl<CAMERAS>		_command;
    RTC::CorbaPort			_commandPort;
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
    try
    {
	_images.length(_cameras.size());
	for (size_t i = 0; i < _cameras.size(); ++i)
	    _images[i].data.length(setImageHeader(*_cameras[i], _images[i]));
    
	exec(_cameras, &camera_type::continuousShot);	// 連続撮影を開始
    }
    catch (std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	
	return RTC::RTC_ERROR;
    }
    
    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCamera<CAMERAS>::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiCamera::onDeactivated" << std::endl;
#endif
    exec(_cameras, &camera_type::stopContinuousShot);	// 連続撮影を終了
    
    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCamera<CAMERAS>::onExecute(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    static int	n = 0;
    std::cerr << "MultiCamera::onExecute: " << n++ << std::endl;
#endif
    exec(_cameras, &camera_type::snap);
    for (size_t i = 0; i < _cameras.size(); ++i)
    {
	_cameras[i]->captureRaw(_images[i].data.get_buffer());
	_images[i].tm = getTime(*_cameras[i]);
    }
    _imagesOut.write();
    
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

#ifdef DEUBUG
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

#endif // MULTICAMERA_H
