/*
 *  $Id$
 */
#include <cstdlib>		// for atoi()
#include "TU/V4L2++.h"
#include "CameraRTC.h"
#ifdef DEBUG
#  include "RTCTime.h"
#endif

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_DEVICE_NAME	"/dev/video0"
#define DEFAULT_START_WITH_FLOW	"1"

// Module specification
static const char* v4l2camera_spec[] =
{
    "implementation_id",		"V4L2CameraRTC",
    "type_name",			"V4L2Camera",
    "description",			"Controlling a V4L2 camera",
    "version",				"1.0.0",
    "vendor",				"t.ueshiba@aist.go.jp",
    "category",				"sensor",
    "activity_type",			"SPORADIC",
    "kind",				"DataFlowComponent",
    "max_instance",			"0",
    "language",				"C++",
    "lang_type",			"compile",
    "conf.default.str_deviceName",	DEFAULT_DEVICE_NAME,
    "conf.default.int_startWithFlow",	DEFAULT_START_WITH_FLOW,
    ""
};

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
V4L2CameraRTCInit(RTC::Manager* manager)
{
    coil::Properties	profile(v4l2camera_spec);
    manager->registerFactory(profile,
			     RTC::Create<TU::CameraRTC<TU::V4L2Camera> >,
			     RTC::Delete<TU::CameraRTC<TU::V4L2Camera> >);
}
};

namespace TU
{
/************************************************************************
*  class CameraRTC<V4L2Camera>						*
************************************************************************/
/*
 *  public member functions
 */
template <>
CameraRTC<V4L2Camera>::CameraRTC(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _camera(),
     _deviceName(DEFAULT_DEVICE_NAME),
     _startWithFlow(DEFAULT_START_WITH_FLOW[0] - '0'),
     _image(),
     _imageOut("TimedCameraImage", _image),
   //_command(*this),
   //_commandPort("Command")
     _mutex()
{
}
  /*
template <> void
CameraRTC<V4L2Camera>::setFormat(const Cmd::Values& vals)
{
    std::unique_lock<std::mutex>	lock(_mutex);

    TU::setFormat(_camera, vals[1].i, vals[2].i);
    allocateImage();
}
    
template <> bool
CameraRTC<V4L2Camera>::setFeature(const Cmd::Values& vals,
					size_t n, bool all)
{
    std::unique_lock<std::mutex>	lock(_mutex);
    
    if (vals[0].i == V4L2Camera::UNKNOWN_PIXEL_FORMAT)
    {
	_camera.setROI(vals[1].i, vals[2].i, vals[3].i, vals[4].i);
	allocateImage();

	return true;
    }
    else
}
  */
/*
 *  private member functions
 */
template <> void
CameraRTC<V4L2Camera>::initializeConfigurations()
{
    bindParameter("str_deviceName",    _deviceName,    DEFAULT_DEVICE_NAME);
    bindParameter("int_startWithFlow", _startWithFlow, DEFAULT_START_WITH_FLOW);
}

template <> void
CameraRTC<V4L2Camera>::allocateImage()
{
    auto&	imageData = _image.data.image;

    imageData.width  = _camera.width();
    imageData.height = _camera.height();

    switch (_camera.pixelFormat())
    {
      case V4L2Camera::GREY:
      case V4L2Camera::Y16:
	imageData.format = Img::CF_GRAY;
	imageData.raw_data.length(imageData.width * imageData.height);
	break;

      default:
	imageData.format = Img::CF_RGB;
	imageData.raw_data.length(imageData.width * imageData.height * 3);
	break;
    }
}

template <> void
CameraRTC<V4L2Camera>::captureImage()
{
    auto&	imageData = _image.data.image;

    switch (_camera.pixelFormat())
    {
      case V4L2Camera::GREY:
      case V4L2Camera::Y16:
      {
	Image<u_char>	image(reinterpret_cast<u_char*>(
				  imageData.raw_data.get_buffer(false)),
			      imageData.width, imageData.height);
	_camera >> image;
      }
	break;

      default:
      {
	Image<RGB>	image(reinterpret_cast<RGB*>(
				  imageData.raw_data.get_buffer(false)),
			      imageData.width, imageData.height);
	_camera >> image;
      }
	break;
    }
}
    
template <> inline void
CameraRTC<V4L2Camera>::enableTimestamp()
{
}
    
template <> inline RTC::Time
CameraRTC<V4L2Camera>::getTimestamp() const
{
    const auto	timestamp   = _camera.getTimestamp();
#ifdef DEBUG
    const auto	arrivaltime = _camera.getArrivaltime();
    std::cerr << "timestamp: " << timestamp << '\t'
	      << "arrival: "   << arrivaltime << std::endl;
#endif
    const std::chrono::nanoseconds	nsec = timestamp.time_since_epoch();
    
    return {CORBA::ULong(nsec.count() / 1000000000),
	    CORBA::ULong(nsec.count() % 1000000000)};
}

}
