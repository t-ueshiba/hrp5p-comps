/*
 *  $Id$
 */
#include <fstream>
#include <cstdlib>			// for atoi()
#include "TU/V4L2CameraArray.h"
#include "Img.hh"
#include "CameraRTC.h"

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_RECFILE_PREFIX	"/tmp/V4L2MultiCameraRTC"
#define DEFAULT_SYNCED_SNAP	"0"	// "0": not synced, otherwise: sync.
					//   accuracy with micro seconds.
#define DEFAULT_START_WITH_FLOW	"0"	// "0": without flow, "1": with flow

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
    "conf.default.str_cameraName",	TU::V4L2CameraArray::DEFAULT_CAMERA_NAME,
    "conf.default.str_recFilePrefix",	DEFAULT_RECFILE_PREFIX,
    "conf.default.int_syncedSnap",	DEFAULT_SYNCED_SNAP,
    "conf.default.int_startWithFlow",	DEFAULT_START_WITH_FLOW,
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
    using	rtc_type = TU::CameraRTC<TU::V4L2CameraArray, Img::TimedImages>;
    
    coil::Properties	profile(v4l2multicamera_spec);
    manager->registerFactory(profile,
			     RTC::Create<rtc_type>, RTC::Delete<rtc_type>);
}
};

namespace TU
{
/************************************************************************
*  class CameraRTC<V4L2CameraArray, Img::TimedImages>			*
************************************************************************/
/*
 *  public member functions
 */
template <>
CameraRTC<V4L2CameraArray, Img::TimedImages>::CameraRTC(RTC::Manager* manager)
    :super(manager),
     _cameraName(V4L2CameraArray::DEFAULT_CAMERA_NAME),
     _recFilePrefix(DEFAULT_RECFILE_PREFIX),
     _syncedSnap(atoi(DEFAULT_SYNCED_SNAP)),
     _startWithFlow(atoi(DEFAULT_START_WITH_FLOW)),
     _images(),
     _imagesOut("TimedImages", _images)
{
}

/*
 *  private member functions
 */
template <> void
CameraRTC<V4L2CameraArray, Img::TimedImages>::initializeConfigurations()
{
    bindParameter("str_cameraConfig",
		  _cameraName, V4L2CameraArray::DEFAULT_CAMERA_NAME);
    bindParameter("str_recFilePrefix",
		  _recFilePrefix, DEFAULT_RECFILE_PREFIX);
    bindParameter("int_syncedSnap",
		  _syncedSnap, DEFAULT_SYNCED_SNAP);
    bindParameter("int_startWithFlow",
		  _startWithFlow, DEFAULT_START_WITH_FLOW);
}

template <> template <> size_t
CameraRTC<V4L2CameraArray, Img::TimedImages>
::setImageFormat(const camera_type& camera, Img::Header& header)
{
    header.width  = camera.width();
    header.height = camera.height();
    
    switch (camera.pixelFormat())
    {
      case V4L2Camera::GREY:
	header.format = Img::MONO_8;
	header.size = header.width * header.height;
	break;
      case V4L2Camera::YUYV:
	header.format = Img::YUYV_422;
	header.size = header.width * header.height * 2;
	break;
      case V4L2Camera::UYVY:
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
