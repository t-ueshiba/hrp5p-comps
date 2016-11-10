/*
 *  $Id$
 */
#include <fstream>
#include "TU/IIDCCameraArray.h"
#include "MultiCameraRTC.h"

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_CAMERA_CONFIG	"/usr/local/etc/cameras/IEEE1394Camera.conf"
#define DEFAULT_CAMERA_CALIB	"/usr/local/etc/cameras/IEEE1394Camera.calib"
#define DEFAULT_USE_TIMESTAMP	"1"	// "0": arrival time, "1": timestamp

// Module specification
static const char* ieee1394multicamera_spec[] =
{
    "implementation_id",		"IIDCMultiCameraRTC",
    "type_name",			"IIDCMultiCamera",
    "description",			"Controlling IEEE1394 cameras",
    "version",				"1.0.0",
    "vendor",				"t.ueshiba@aist.go.jp",
    "category",				"sensor",
    "activity_type",			"PERIODIC",
    "kind",				"DataFlowComponent",
    "max_instance",			"1",
    "language",				"C++",
    "lang_type",			"compile",
    "conf.default.str_cameraConfig",	DEFAULT_CAMERA_CONFIG,
    "conf.default.str_cameraCalib",	DEFAULT_CAMERA_CALIB,
    "conf.default.int_useTimestamp",	DEFAULT_USE_TIMESTAMP,
    ""
};

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
IIDCMultiCameraRTCInit(RTC::Manager* manager)
{
    coil::Properties	profile(ieee1394multicamera_spec);
    manager->registerFactory(
		 profile,
		 RTC::Create<TU::MultiCameraRTC<TU::IIDCCameraArray> >,
		 RTC::Delete<TU::MultiCameraRTC<TU::IIDCCameraArray> >);
}
};

namespace TU
{
/************************************************************************
*  class MultiCameraRTC<IIDCCameraArray>				*
************************************************************************/
template <>
MultiCameraRTC<IIDCCameraArray>::MultiCameraRTC(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _cameras(),
     _mutex(),
     _cameraConfig(DEFAULT_CAMERA_CONFIG),
     _cameraCalib(DEFAULT_CAMERA_CALIB),
     _useTimestamp(DEFAULT_USE_TIMESTAMP[0] - '0'),
     _images(),
     _imagesOut("TimedImages", _images),
     _command(*this),
     _commandPort("Command")
{
}

template <> void
MultiCameraRTC<IIDCCameraArray>::initializeConfigurations()
{
    bindParameter("str_cameraConfig", _cameraConfig, DEFAULT_CAMERA_CONFIG);
    bindParameter("str_cameraCalib",  _cameraCalib,  DEFAULT_CAMERA_CALIB);
    bindParameter("int_useTimestamp", _useTimestamp, DEFAULT_USE_TIMESTAMP);
}

template <> void
MultiCameraRTC<IIDCCameraArray>::initializeTime()
{
    exec(_cameras, &camera_type::embedTimestamp, bool(_useTimestamp));
}

template <> RTC::Time
MultiCameraRTC<IIDCCameraArray>::getTime(const camera_type& camera) const
{
  //u_int64_t	usec = (_useTimestamp ? camera.getTimestamp()
  //				      : camera.arrivaltime());
    u_int64_t	usec = camera.getTimestamp();
    RTC::Time	time = {CORBA::ULong( usec / 1000000),
			CORBA::ULong((usec % 1000000) * 1000)};
    return time;
}

template <> inline void
MultiCameraRTC<IIDCCameraArray>::setFormat(const Cmd::Values& vals)
{
    coil::Guard<coil::Mutex>	guard(_mutex);

    if (vals.length() == 3)
    {
	TU::setFormat(_cameras, vals[1], vals[2]);
	allocateImages();
    }
    else if (vals.length() == 7)
    {
	IIDCCamera::Format
	    format7     = IIDCCamera::uintToFormat(vals[1]);
	IIDCCamera::PixelFormat
	    pixelFormat = IIDCCamera::uintToPixelFormat(vals[6]);
	
	for (size_t i = 0; i < _cameras.size(); ++i)
	    _cameras[i]->setFormat_7_ROI(format7,
					 vals[2], vals[3], vals[4], vals[5])
			.setFormat_7_PixelFormat(format7, pixelFormat)
			.setFormatAndFrameRate(format7,
					       IIDCCamera::FrameRate_x);
	
	allocateImages();
    }
}
    
template <> size_t
MultiCameraRTC<IIDCCameraArray>::setPixelFormat(const camera_type& camera,
						Img::TimedImage& image)
{
    switch (camera.pixelFormat())
    {
      case IIDCCamera::MONO_8:
	image.format = Img::MONO_8;
	return image.width * image.height;
      case IIDCCamera::YUV_411:
	image.format = Img::YUV_411;
	return image.width * image.height * 3 / 2;
      case IIDCCamera::YUV_422:
	image.format = Img::YUV_422;
	return image.width * image.height * 2;
      case IIDCCamera::YUV_444:
	image.format = Img::YUV_444;
	return image.width * image.height * 3;
      case IIDCCamera::RGB_24:
	image.format = Img::RGB_24;
	return image.width * image.height * 3;
      default:
	throw std::runtime_error("Unsupported pixel format!!");
    }

    return 0;
}

}
