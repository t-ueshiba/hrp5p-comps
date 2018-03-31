/*
 *  $Id$
 */
#ifndef __TU_CAMERARTC_H
#define __TU_CAMERARTC_H

#include "Img.hh"
#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
//#include "CmdSVC_impl.h"
#include <mutex>

namespace TU
{
/************************************************************************
*  class CameraRTC<CAMERA>					*
************************************************************************/
template <class CAMERA>
class CameraRTC : public RTC::DataFlowComponentBase
{
  private:
    using camera_type	= CAMERA;
    
  public:
    CameraRTC(RTC::Manager* manager)					;

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;

  // 外部からはカメラ状態の変更を許さない
    const camera_type&	camera()				const	;
    
  /*
    void		setFormat(const Cmd::Values& vals)		;
    bool		setFeature(const Cmd::Values& vals,
				   size_t n, bool all)			;
  */
  private:
    void		initializeConfigurations()			;
    void		allocateImage()					;
    void		captureImage()					;
    void		enableTimestamp()				;
    RTC::Time		getTimestamp()				const	;
    void		snap()						;
    
  private:
    camera_type				_camera;
    std::string				_deviceName;	// config var.
    int					_startWithFlow;	// config var.
    Img::TimedCameraImage		_image;		// out data
    RTC::OutPort<Img::TimedCameraImage>	_imageOut;	// out data port
  //v::CmdSVC_impl<CAMERA>		_command;	// service provider
  //RTC::CorbaPort			_commandPort;	// service port
    mutable std::mutex			_mutex;
};

/*
 *  public member functions
 */
template <class CAMERA> RTC::ReturnCode_t
CameraRTC<CAMERA>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "CameraRTC::onInitialize" << std::endl;
#endif
  // コンフィグレーションをセットアップ
    initializeConfigurations();
    
  // データポートをセットアップ
    addOutPort("TimedCameraImage", _imageOut);
    
  // サービスプロバイダとサービスポートをセットアップ
  //_commandPort.registerProvider("Command", "Cmd::Controller", _command);
  //addPort(_commandPort);
    
    return RTC::RTC_OK;
}

template <class CAMERA> RTC::ReturnCode_t
CameraRTC<CAMERA>::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "CameraRTC::onActivated" << std::endl;
#endif
    try
    {
	_camera.initialize(_deviceName.c_str());
	allocateImage();			// 画像データ領域を確保
	enableTimestamp();			// 撮影時刻の記録を初期化
	_camera.continuousShot(_startWithFlow);	// 連続撮影を開始	
    }
    catch (std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	
	return RTC::RTC_ERROR;
    }

    return RTC::RTC_OK;
}

template <class CAMERA> RTC::ReturnCode_t
CameraRTC<CAMERA>::onExecute(RTC::UniqueId ec_id)
{
    std::unique_lock<std::mutex>	lock(_mutex);

    if (_camera.inContinuousShot())
    {
	_camera.snap();
	captureImage();
	_image.tm = getTimestamp();
	_imageOut.write();
    }

    return RTC::RTC_OK;
}

template <class CAMERA> RTC::ReturnCode_t
CameraRTC<CAMERA>::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "CameraRTC::onDeactivated" << std::endl;
#endif
    _camera.continuousShot(false);		// 連続撮影を終了
    _camera.terminate();			// デバイスを終了

    return RTC::RTC_OK;
}

template <class CAMERA> RTC::ReturnCode_t
CameraRTC<CAMERA>::onAborting(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "CameraRTC::onAborting" << std::endl;
#endif
    _camera.terminate();			// デバイスを終了

    return RTC::RTC_OK;
}

template <class CAMERA> inline const CAMERA&
CameraRTC<CAMERA>::camera() const
{
    return _camera;
}
    
template <class CAMERA> inline RTC::Time
CameraRTC<CAMERA>::getTimestamp() const
{
    const std::chrono::nanoseconds
	nsec = _camera.getTimestamp().time_since_epoch();
    
    return {CORBA::ULong(nsec.count() / 1000000000),
	    CORBA::ULong(nsec.count() % 1000000000)};
}

}
#endif	// !__TU_CAMERARTC_H
