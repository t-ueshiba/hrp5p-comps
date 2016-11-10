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
#include "TU/Vector++.h"
#include "Img.hh"
#include "CmdSVC_impl.h"

namespace TU
{
/************************************************************************
*  class MultiCameraRTC<CAMERAS>					*
************************************************************************/
template <class CAMERAS>
class MultiCameraRTC : public RTC::DataFlowComponentBase
{
  private:
    struct Calib
    {
	Calib()	:u0(0), v0(0), width(0), height(0), d1(0), d2(0)
					{ P[0][0] = P[1][1] = P[2][2] = 1; }

	size_t		u0, v0;
	size_t		width, height;
	Matrix34d	P;
	double		d1, d2;
    };

    typedef typename CAMERAS::camera_type	camera_type;
    
  public:
    MultiCameraRTC(RTC::Manager* manager)				;
    ~MultiCameraRTC()							;

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
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
    void		continuousShot(bool enable)			;
    void		setFormat(const Cmd::Values& vals)		;
    void		setOtherValues(const Cmd::Values& vals,
				       size_t n)			;
    
  private:
    void		initializeConfigurations()			;
    void		initializeTime()				;
    RTC::Time		getTime(const camera_type& camera)	const	;
    void		allocateImages()				;
    static size_t	setImageHeader(const camera_type& camera,
				       const Calib& calib,
				       Img::TimedImage& image)		;
    static size_t	setPixelFormat(const camera_type& camera,
				       Img::TimedImage& image)		;
    
  private:
    CAMERAS				_cameras;
    Array<Calib>			_calibs;
    mutable coil::Mutex			_mutex;
    std::string				_cameraConfig;	// config var.
    std::string				_cameraCalib;	// config var.
    int					_useTimestamp;	// config var.
    Img::TimedImages			_images;	// out data
    RTC::OutPort<Img::TimedImages>	_imagesOut;	// out data port
    v::CmdSVC_impl<CAMERAS>		_command;	// service provider
    RTC::CorbaPort			_commandPort;	// service port
};

template <class CAMERAS>
MultiCameraRTC<CAMERAS>::~MultiCameraRTC()
{
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "MultiCameraRTC::onInitialize" << std::endl;
#endif
  // コンフィグレーションをセットアップ
    initializeConfigurations();
    
  // データポートをセットアップ
    addOutPort("TimedImages", _imagesOut);

  // サービスプロバイダとサービスポートをセットアップ
    _commandPort.registerProvider("Command", "Cmd::Controller", _command);
    addPort(_commandPort);
    
    try
    {
      // 設定ファイルを読み込んでカメラを生成・セットアップ
	std::ifstream	in(_cameraConfig.c_str());
	if (!in)
	    throw std::runtime_error("MultiCameraRTC<V4L2CameraArray>::onInitialize(): failed to open " + _cameraConfig + " !");

	in >> _cameras;
	in.close();

      // キャリブレーションを読み込み
	_calibs.resize(_cameras.size());
	in.open(_cameraCalib.c_str());
	if (in)
	{
	    for (size_t n = 0; n < _calibs.size(); ++n)
	    {
		_calibs[n].width  = _cameras[n]->width();
		_calibs[n].height = _cameras[n]->height();
		in >> _calibs[n].P >> _calibs[n].d1 >> _calibs[n].d2;
	    }
	}

      // 撮影時刻の記録を初期化
	initializeTime();
    }
    catch (std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	
	return RTC::RTC_ERROR;
    }

    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiCameraRTC::onActivated" << std::endl;
#endif
    allocateImages();
    
    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiCameraRTC::onDeactivated" << std::endl;
#endif
    exec(_cameras, &camera_type::continuousShot, false);	// 連続撮影を終了
    
    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onExecute(RTC::UniqueId ec_id)
{
  //#ifdef DEBUG
#if 0
    static int	n = 0;
    std::cerr << "MultiCameraRTC::onExecute: " << n++ << std::endl;
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
MultiCameraRTC<CAMERAS>::onAborting(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiCameraRTC::onAborting" << std::endl;
#endif
    exec(_cameras, &camera_type::continuousShot, false);	// 連続撮影を終了

    return RTC::RTC_OK;
}

#ifdef DEBUG
template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onFinalize()
{
    std::cerr << "MultiCameraRTC::onFinalize" << std::endl;

    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onStartup(RTC::UniqueId ec_id)
{
    std::cerr << "MultiCameraRTC::onStartup" << std::endl;

    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onShutdown(RTC::UniqueId ec_id)
{
    std::cerr << "MultiCameraRTC::onShutdown" << std::endl;

    return RTC::RTC_OK;
}
#endif

template <class CAMERAS> inline const CAMERAS&
MultiCameraRTC<CAMERAS>::cameras() const
{
    return _cameras;
}

template <class CAMERAS> inline void
MultiCameraRTC<CAMERAS>::continuousShot(bool enable)
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    exec(_cameras, &camera_type::continuousShot, enable);
}
    
template <class CAMERAS> inline void
MultiCameraRTC<CAMERAS>::setFormat(const Cmd::Values& vals)
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    TU::setFormat(_cameras, vals[1], vals[2]);
    allocateImages();
}
    
template <class CAMERAS> inline void
MultiCameraRTC<CAMERAS>::setOtherValues(const Cmd::Values& vals, size_t n)
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    TU::setFeatureValue(_cameras, vals[0], vals[1], n);
}

template <class CAMERAS> void
MultiCameraRTC<CAMERAS>::allocateImages()
{
    _images.length(_cameras.size());
    for (size_t i = 0; i < _cameras.size(); ++i)
	_images[i].data.length(setImageHeader(*_cameras[i], _calibs[i],
					      _images[i]));
}

template <class CAMERAS> size_t
MultiCameraRTC<CAMERAS>::setImageHeader(const camera_type& camera,
					const Calib& calib,
					Img::TimedImage& image)
{
    image.width  = camera.width();
    image.height = camera.height();

    for (size_t i = 0; i < calib.P.nrow(); ++i)
	for (size_t j = 0; j < calib.P.ncol(); ++j)
	    image.P[i][j] = calib.P[i][j];
    image.d1 = calib.d1;
    image.d2 = calib.d2;

    return setPixelFormat(camera, image);
}
        
}
#endif	// !__TU_MULTICAMERA_H
