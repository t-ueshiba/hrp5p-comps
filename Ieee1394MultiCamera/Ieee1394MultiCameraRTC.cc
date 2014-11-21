/*
 *  $Id$
 */
#include <fstream>
#include "TU/Ieee1394CameraArray.h"
#include "MultiCameraRTC.h"

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_CAMERA_CONFIG	"/usr/local/etc/cameras/IEEE1394Camera.conf"
#define DEFAULT_USE_TIMESTAMP	"1"	// "0": arrival time, "1": timestamp

// Module specification
static const char* ieee1394multicamera_spec[] =
{
    "implementation_id",	"Ieee1394MultiCameraRTC",
    "type_name",		"Ieee1394MultiCamera",
    "description",		"Controlling multiple IEEE1394 cameras",
    "version",			"1.0.0",
    "vendor",			"t.ueshiba@aist.go.jp",
    "category",			"sensor",
    "activity_type",		"PERIODIC",
    "kind",			"DataFlowComponent",
    "max_instance",		"1",
    "language",			"C++",
    "lang_type",		"compile",
    "conf.default.str_cameraConfig",	DEFAULT_CAMERA_CONFIG,
    "conf.default.int_useTimestamp",	DEFAULT_USE_TIMESTAMP,
    ""
};

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
Ieee1394MultiCameraRTCInit(RTC::Manager* manager)
{
    coil::Properties	profile(ieee1394multicamera_spec);
    manager->registerFactory(
		 profile,
		 RTC::Create<TU::MultiCameraRTC<TU::Ieee1394CameraArray> >,
		 RTC::Delete<TU::MultiCameraRTC<TU::Ieee1394CameraArray> >);
}
};

namespace TU
{
/************************************************************************
*  class MultiCameraRTC<Ieee1394CameraArray>				*
************************************************************************/
template <>
MultiCameraRTC<Ieee1394CameraArray>::MultiCameraRTC(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _cameras(),
     _mutex(),
     _cameraConfig(DEFAULT_CAMERA_CONFIG),
     _useTimestamp(DEFAULT_USE_TIMESTAMP[0] - '0'),
     _images(),
     _imagesOut("TimedImages", _images),
     _command(*this),
     _commandPort("Command")
{
}

template <> RTC::ReturnCode_t
MultiCameraRTC<Ieee1394CameraArray>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "MultiCameraRTC::onInitialize" << std::endl;
#endif
  // コンフィギュレーションのセットアップ
    bindParameter("str_cameraConfig", _cameraConfig, DEFAULT_CAMERA_CONFIG);
    bindParameter("int_useTimestamp", _useTimestamp, DEFAULT_USE_TIMESTAMP);

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
	    throw std::runtime_error("MultiCameraRTC<Ieee1394CameraArray>::onInitialize(): failed to open " + _cameraConfig + " !");

	in >> _cameras;
	in.close();

      // キャリブレーションを読み込み
	in.open(_cameras.calibFile().c_str());
	if (in)
	{
	    _calibs.resize(_cameras.size());

	    for (auto& calib : _calibs)
		in >> calib.P >> calib.d1 >> calib.d2;
	}

	if (_useTimestamp)
	    exec(_cameras, &camera_type::embedTimestamp);
    }
    catch (std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	
	return RTC::RTC_ERROR;
    }

    return RTC::RTC_OK;
}

template <> size_t
MultiCameraRTC<Ieee1394CameraArray>::setImageHeader(const camera_type& camera,
						    Img::TimedImage& image)
{
    image.width  = camera.width();
    image.height = camera.height();

    switch (camera.pixelFormat())
    {
      case Ieee1394Camera::MONO_8:
	image.format = Img::MONO_8;
	return image.width * image.height;
      case Ieee1394Camera::YUV_411:
	image.format = Img::YUV_411;
	return image.width * image.height * 3 / 2;
      case Ieee1394Camera::YUV_422:
	image.format = Img::YUV_422;
	return image.width * image.height * 2;
      case Ieee1394Camera::YUV_444:
	image.format = Img::YUV_444;
	return image.width * image.height * 3;
      case Ieee1394Camera::RGB_24:
	image.format = Img::RGB_24;
	return image.width * image.height * 3;
      default:
	throw std::runtime_error("Unsupported pixel format!!");
    }

    return 0;
}

template <> RTC::Time
MultiCameraRTC<Ieee1394CameraArray>::getTime(const camera_type& camera) const
{
    u_int64_t	usec = (_useTimestamp ? camera.getTimestamp()
				      : camera.arrivaltime());
    RTC::Time	time = {CORBA::ULong( usec / 1000000),
			CORBA::ULong((usec % 1000000) * 1000)};
    return time;
}

template <> inline void
MultiCameraRTC<Ieee1394CameraArray>::setFormat(const Cmd::Values& vals)
{
    coil::Guard<coil::Mutex>	guard(_mutex);

    if (vals.length() == 3)
    {
	TU::setFormat(_cameras, vals[1], vals[2]);
	allocateImages();
    }
    else if (vals.length() == 7)
    {
	Ieee1394Camera::Format
	    format7     = Ieee1394Camera::uintToFormat(vals[1]);
	Ieee1394Camera::PixelFormat
	    pixelFormat = Ieee1394Camera::uintToPixelFormat(vals[6]);
	
	for (size_t i = 0; i < _cameras.size(); ++i)
	    _cameras[i]->setFormat_7_ROI(format7,
					 vals[2], vals[3], vals[4], vals[5])
			.setFormat_7_PixelFormat(format7, pixelFormat)
			.setFormatAndFrameRate(format7,
					       Ieee1394Camera::FrameRate_x);
	
	allocateImages();
    }
}
    
}
