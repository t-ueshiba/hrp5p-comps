/*
 *  $Id$
 */
#include "TU/V4L2CameraArray.h"
#include "MultiCamera.h"

/************************************************************************
*  static data								*
************************************************************************/
// Module specification
static const char* v4l2multicamera_spec[] =
{
    "implementation_id",	"V4L2MultiCamera",
    "type_name",		"V4L2MultiCamera",
    "description",		"Controlling multiple V4L2 cameras",
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
V4L2MultiCameraInit(RTC::Manager* manager)
{
    coil::Properties	profile(v4l2multicamera_spec);
    manager->registerFactory(profile,
			     RTC::Create<MultiCamera<TU::V4L2CameraArray> >,
			     RTC::Delete<MultiCamera<TU::V4L2CameraArray> >);
}
};

/************************************************************************
*  class MultiCamera<TU::Ieee1394CameraArray>				*
************************************************************************/
template <> RTC::ReturnCode_t
MultiCamera<TU::V4L2CameraArray>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "MultiCamera::onInitialize" << std::endl;
#endif
    addOutPort("TimedImages", _imagesOut);

    try
    {
	_cameras.initialize(TU_V4L2_DEFAULT_CAMERA_NAME,
			    TU_V4L2_DEFAULT_CONFIG_DIRS, -1);
    }
    catch (std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	
	return RTC::RTC_ERROR;
    }

    return RTC::RTC_OK;
}

template <> size_t
MultiCamera<TU::V4L2CameraArray>::setImageHeader(const camera_type& camera,
						 Img::TimedImage& image)
{
    image.width  = camera.width();
    image.height = camera.height();

    switch (camera.pixelFormat())
    {
      case TU::V4L2Camera::GREY:
	image.format = Img::MONO_8;
	return camera.width() * camera.height();
      case TU::V4L2Camera::YUYV:
	image.format = Img::YUV_422;
	return camera.width() * camera.height() * 2;
      case TU::V4L2Camera::RGB24:
	image.format = Img::RGB_24;
	return camera.width() * camera.height() * 3;
    }

    throw std::runtime_error("Unsupported pixel format!!");
    return 0;
}
