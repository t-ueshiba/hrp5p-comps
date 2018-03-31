/*
 *  $Id$
 */
#include <fstream>
#include <cstdlib>			// for atoi()
#include "TU/V4L2CameraArray.h"
#include "MultiCameraRTC.h"

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
     _cameraName(V4L2CameraArray::DEFAULT_CAMERA_NAME),
     _recFilePrefix(DEFAULT_RECFILE_PREFIX),
     _syncedSnap(atoi(DEFAULT_SYNCED_SNAP)),
     _startWithFlow(atoi(DEFAULT_START_WITH_FLOW)),
     _images(),
     _imagesOut("TimedImages", _images),
     _command(*this),
     _commandPort("Command")
{
}

template <> void
MultiCameraRTC<V4L2CameraArray>::setFormat(const Cmd::Values& vals)
{
    std::unique_lock<std::mutex>	lock(_mutex);

    TU::setFormat(_cameras, vals[1].i, vals[2].i);
    allocateImages();
}
    
template <> bool
MultiCameraRTC<V4L2CameraArray>::setFeature(const Cmd::Values& vals,
					    size_t n, bool all)
{
    std::unique_lock<std::mutex>	lock(_mutex);
    
    if (vals[0].i == V4L2Camera::UNKNOWN_PIXEL_FORMAT)
    {
	for (auto& camera : _cameras)
	    camera.setROI(vals[1].i, vals[2].i, vals[3].i, vals[4].i);
	allocateImages();

	return true;
    }
    else if (all)
	return TU::setFeature(_cameras, vals[0].i, vals[1].i);
    else
    {
	auto	camera = std::begin(_cameras);
	std::advance(camera, n);
	return TU::setFeature(*camera, vals[0].i, vals[1].i);
    }
}

/*
 *  private member functions
 */
template <> void
MultiCameraRTC<V4L2CameraArray>::initializeConfigurations()
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

template <> void
MultiCameraRTC<V4L2CameraArray>::embedTimestamp(bool)
{
}
    
template <> size_t
MultiCameraRTC<V4L2CameraArray>::setImageFormat(const camera_type& camera,
						Img::Header& header)
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
