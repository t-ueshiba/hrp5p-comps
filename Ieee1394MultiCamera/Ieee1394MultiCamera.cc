/*
 *  $Id$
 */
#include <fstream>
#include "TU/Ieee1394CameraArray.h"
#include "MultiCamera.h"

namespace TU
{
namespace v
{
CmdDefs	createFormatCmds (const Ieee1394Camera& camera)			;
CmdDefs	createFeatureCmds(const Ieee1394Camera& camera)			;
}
}

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_CAMERA_CONFIG	"/usr/local/etc/cameras/IEEE1394Camera.conf"
#define DEFAULT_USE_TIMESTAMP	"1"	// "0": arrival time, "1": timestamp

// Module specification
static const char* ieee1394multicamera_spec[] =
{
    "implementation_id",	"Ieee1394MultiCamera",
    "type_name",		"Ieee1394MultiCamera",
    "description",		"Controlling multiple IEEE1394 cameras",
    "version",			"1.0.0",
    "vendor",			"t.ueshiba@aist.go.jp",
    "category",			"sensor",
    "activity_type",		"PERIODIC",
    "kind",			"DataFlowComponent",
    "max_instance",		"0",
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
Ieee1394MultiCameraInit(RTC::Manager* manager)
{
    coil::Properties	profile(ieee1394multicamera_spec);
    manager->registerFactory(
		 profile,
		 RTC::Create<MultiCamera<TU::Ieee1394CameraArray> >,
		 RTC::Delete<MultiCamera<TU::Ieee1394CameraArray> >);
}
};

/************************************************************************
*  class MultiCamera<TU::Ieee1394CameraArray>				*
************************************************************************/
template <>
MultiCamera<TU::Ieee1394CameraArray>::MultiCamera(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _cameras(),
     _cameraConfig(DEFAULT_CAMERA_CONFIG),
     _useTimestamp(DEFAULT_USE_TIMESTAMP[0] - '0'),
     _images(),
     _imagesOut("TimedImages", _images),
     _command(_cameras),
     _commandPort("Command")
{
}

template <> RTC::ReturnCode_t
MultiCamera<TU::Ieee1394CameraArray>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "MultiCamera::onInitialize" << std::endl;
#endif
    bindParameter("str_cameraConfig", _cameraConfig, DEFAULT_CAMERA_CONFIG);
    bindParameter("int_useTimestamp", _useTimestamp, DEFAULT_USE_TIMESTAMP);
    
    addOutPort("TimedImages", _imagesOut);

    _commandPort.registerProvider("Command", "Cam::Controller", _command);
    addPort(_commandPort);

    try
    {
	std::ifstream	in(_cameraConfig.c_str());
	if (!in)
	    throw std::runtime_error("MultiCamera<TU::Ieee1394CameraArray>::onInitialize(): failed to open " + _cameraConfig + " !");

	in >> _cameras;

	if (_useTimestamp)
	    TU::exec(_cameras, &TU::Ieee1394Camera::embedTimestamp);
    }
    catch (std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	
	return RTC::RTC_ERROR;
    }

    return RTC::RTC_OK;
}

template <> size_t
MultiCamera<TU::Ieee1394CameraArray>::setImageHeader(const camera_type& camera,
						     Img::TimedImage& image)
{
    image.width  = camera.width();
    image.height = camera.height();

    switch (camera.pixelFormat())
    {
      case TU::Ieee1394Camera::MONO_8:
	image.format = Img::MONO_8;
	return camera.width() * camera.height();
      case TU::Ieee1394Camera::YUV_411:
	image.format = Img::YUV_411;
	return camera.width() * camera.height() * 3 / 2;
      case TU::Ieee1394Camera::YUV_422:
	image.format = Img::YUV_422;
	return camera.width() * camera.height() * 2;
      case TU::Ieee1394Camera::YUV_444:
	image.format = Img::YUV_444;
	return camera.width() * camera.height() * 3;
      case TU::Ieee1394Camera::RGB_24:
	image.format = Img::RGB_24;
	return camera.width() * camera.height() * 3;
    }

    throw std::runtime_error("Unsupported pixel format!!");
    return 0;
}

template <> RTC::Time
MultiCamera<TU::Ieee1394CameraArray>::getTime(const camera_type& camera) const
{
    u_int64_t	usec = (_useTimestamp ? camera.getTimestamp()
				      : camera.arrivaltime());
    RTC::Time	time = {usec / 1000000, (usec % 1000000) * 1000};
    return time;
}
