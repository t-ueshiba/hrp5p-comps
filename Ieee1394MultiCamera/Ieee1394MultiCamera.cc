/*
 *  $Id$
 */
#include "TU/Ieee1394CameraArray.h"
#include "MultiCamera.h"

/************************************************************************
*  static data								*
************************************************************************/
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
template <> RTC::ReturnCode_t
MultiCamera<TU::Ieee1394CameraArray>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "MultiCamera::onInitialize" << std::endl;
#endif
    addOutPort("TimedImages", _imagesOut);

    try
    {
	_cameras.initialize(DEFAULT_CAMERA_NAME, DEFAULT_CONFIG_DIRS,
			    TU::Ieee1394Node::SPD_400M, -1);
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
