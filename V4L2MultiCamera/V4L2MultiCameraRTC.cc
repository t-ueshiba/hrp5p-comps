/*
 *  $Id$
 */
#include <fstream>
#include "TU/V4L2CameraArray.h"
#include "MultiCameraRTC.h"

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_CAMERA_CONFIG	"/usr/local/etc/cameras/V4L2Camera.conf"
#define DEFAULT_CAMERA_CALIB	"/usr/local/etc/cameras/V4L2Camera.calib"

// Module specification
static const char* v4l2multicamera_spec[] =
{
    "implementation_id",		"V4L2MultiCameraRTC",
    "type_name",			"V4L2MultiCamera",
    "description",			"Controlling V4L2 cameras",
    "version",				"1.0.0",
    "vendor",				"t.ueshiba@aist.go.jp",
    "category",				"sensor",
    "activity_type",			"SPORADIC",
    "kind",				"DataFlowComponent",
    "max_instance",			"0",
    "language",				"C++",
    "lang_type",			"compile",
    "conf.default.str_cameraConfig",	DEFAULT_CAMERA_CONFIG,
    "conf.default.str_cameraCalib",	DEFAULT_CAMERA_CALIB,
    ""
};

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
V4L2MultiCameraRTCInit(RTC::Manager* manager)
{
    coil::Properties	profile(v4l2multicamera_spec);
    manager->registerFactory(
		 profile,
		 RTC::Create<TU::MultiCameraRTC<TU::V4L2CameraArray> >,
		 RTC::Delete<TU::MultiCameraRTC<TU::V4L2CameraArray> >);
}
};

namespace TU
{
/************************************************************************
*  class MultiCameraRTC<V4L2CameraArray>				*
************************************************************************/
/*
 *  public member functions
 */
template <>
MultiCameraRTC<V4L2CameraArray>::MultiCameraRTC(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _cameras(),
     _mutex(),
     _cameraConfig(DEFAULT_CAMERA_CONFIG),
     _cameraCalib(DEFAULT_CAMERA_CALIB),
     _images(),
     _imagesOut("TimedImages", _images),
     _command(*this),
     _commandPort("Command")
{
}

template <> void
MultiCameraRTC<V4L2CameraArray>::setFormat(const Cmd::Values& vals)
{
    coil::Guard<coil::Mutex>	guard(_mutex);

    TU::setFormat(_cameras, vals[1], vals[2]);
    allocateImages();
}
    
template <> bool
MultiCameraRTC<V4L2CameraArray>::setFeature(const Cmd::Values& vals,
					    size_t n, bool all)
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    
    if (vals[0] == V4L2Camera::UNKNOWN_PIXEL_FORMAT)
    {
	for (auto& camera : _cameras)
	    camera.setROI(vals[1], vals[2], vals[3], vals[4]);
	allocateImages();

	return true;
    }
    else if (all)
	return TU::setFeature(_cameras, vals[0], vals[1]);
    else
    {
	auto	camera = std::begin(_cameras);
	std::advance(camera, n);
	return TU::setFeature(*camera, vals[0], vals[1]);
    }
}

/*
 *  private member functions
 */
template <> void
MultiCameraRTC<V4L2CameraArray>::initializeConfigurations()
{
    bindParameter("str_cameraConfig", _cameraConfig, DEFAULT_CAMERA_CONFIG);
    bindParameter("str_cameraCalib",  _cameraCalib,  DEFAULT_CAMERA_CALIB);
}

template <> void
MultiCameraRTC<V4L2CameraArray>::enableTimestamp()
{
}
    
template <> RTC::Time
MultiCameraRTC<V4L2CameraArray>::getTimestamp(const camera_type& camera) const
{
    u_int64_t	usec = camera.arrivaltime();
    RTC::Time	time = {CORBA::ULong( usec / 1000000),
			CORBA::ULong((usec % 1000000) * 1000)};
    
    return time;
}

template <> size_t
MultiCameraRTC<V4L2CameraArray>::setImageFormat(const camera_type& camera,
						Img::Header& header)
{
    switch (camera.pixelFormat())
    {
      case V4L2Camera::GREY:
	header.format = Img::MONO_8;
	header.size = header.width * header.height;
	break;
      case V4L2Camera::YUYV:
	header.format = Img::YUV_422;
	header.size = header.width * header.height * 2;
	break;
      case V4L2Camera::RGB24:
	header.format = Img::RGB_24;
	header.size = header.width * header.height * 3;
	break;
      default:
	throw std::runtime_error("Unsupported pixel format!!");
	break;
    }

    return header.size;
}

}
