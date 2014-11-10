/*
 *  $Id$
 */
#include <fstream>
#include "TU/V4L2CameraArray.h"
#include "MultiCamera.h"

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_CAMERA_CONFIG	"/usr/local/etc/cameras/V4L2Camera.conf"

// Module specification
static const char* v4l2multicamera_spec[] =
{
    "implementation_id",	"V4L2MultiCamera",
    "type_name",		"V4L2MultiCamera",
    "description",		"Controlling multiple V4L2 cameras",
    "version",			"1.0.0",
    "vendor",			"t.ueshiba@aist.go.jp",
    "category",			"sensor",
    "activity_type",		"SPORADIC",
    "kind",			"DataFlowComponent",
    "max_instance",		"0",
    "language",			"C++",
    "lang_type",		"compile",
    "conf.default.str_cameraConfig",	DEFAULT_CAMERA_CONFIG,
    ""
};

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
V4L2MultiCameraInit(RTC::Manager* manager)
{
    coil::Properties	profile(v4l2multicamera_spec);
    manager->registerFactory(
		 profile,
		 RTC::Create<TU::MultiCamera<TU::V4L2CameraArray> >,
		 RTC::Delete<TU::MultiCamera<TU::V4L2CameraArray> >);
}
};

namespace TU
{
/************************************************************************
*  class MultiCamera<TU::Ieee1394CameraArray>				*
************************************************************************/
template <>
MultiCamera<V4L2CameraArray>::MultiCamera(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _cameras(),
     _cameraConfig(DEFAULT_CAMERA_CONFIG),
     _images(),
     _imagesOut("TimedImages", _images),
     _command(_cameras),
     _commandPort("Command")
{
}

template <> RTC::ReturnCode_t
MultiCamera<V4L2CameraArray>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "MultiCamera::onInitialize" << std::endl;
#endif
  // コンフィグレーションをセットアップ
    bindParameter("str_cameraConfig", _cameraConfig, DEFAULT_CAMERA_CONFIG);

  // データポートをセットアップ
    addOutPort("TimedImages", _imagesOut);

  // サービスプロバイダとサービスポートをセットアップ
    _commandPort.registerProvider("Command", "Cmd::Controller", _command);
    addPort(_commandPort);
    
    try
    {
	std::ifstream	in(_cameraConfig.c_str());
	if (!in)
	    throw std::runtime_error("MultiCamera<V4L2CameraArray>::onInitialize(): failed to open " + _cameraConfig + " !");

	in >> _cameras;
    }
    catch (std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	
	return RTC::RTC_ERROR;
    }

    return RTC::RTC_OK;
}

template <> size_t
MultiCamera<V4L2CameraArray>::setImageHeader(const camera_type& camera,
					     Img::TimedImage& image)
{
    image.width  = camera.width();
    image.height = camera.height();

    switch (camera.pixelFormat())
    {
      case V4L2Camera::GREY:
	image.format = Img::MONO_8;
	return image.width * image.height;
      case V4L2Camera::YUYV:
	image.format = Img::YUV_422;
	return image.width * image.height * 2;
      case V4L2Camera::RGB24:
	image.format = Img::RGB_24;
	return image.width * image.height * 3;
      default:
	throw std::runtime_error("Unsupported pixel format!!");
    }

    return 0;
}

template <> RTC::Time
MultiCamera<V4L2CameraArray>::getTime(const camera_type& camera) const
{
    u_int64_t	usec = camera.arrivaltime();
    RTC::Time	time = {CORBA::ULong( usec / 1000000),
			CORBA::ULong((usec % 1000000) * 1000)};
    
    return time;
}

}
