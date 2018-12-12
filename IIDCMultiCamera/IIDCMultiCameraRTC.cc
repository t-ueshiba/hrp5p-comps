/*
 *  $Id$
 */
#include <fstream>
#include <cstdlib>			// for atoi()
#include "TU/IIDCCameraArray.h"
#include "MultiImage.hh"
#include "CameraRTC.h"

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_RECFILE_PREFIX	"/tmp/IIDCMultiCameraRTC"
#define DEFAULT_SYNCED_SNAP	"0"	// "0": not synced, otherwise: sync.
					//   accuracy with micro seconds.
#define DEFAULT_START_WITH_FLOW	"0"	// "0": without flow, "1": with flow

// Module specification
static const char* iidcmulticamera_spec[] =
{
    "implementation_id",		"IIDCMultiCameraRTC",
    "type_name",			"IIDCMultiCamera",
    "description",			"Controlling IIDC cameras",
    "version",				"1.0.0",
    "vendor",				"t.ueshiba@aist.go.jp",
    "category",				"sensor",
    "activity_type",			"PERIODIC",
    "kind",				"DataFlowComponent",
    "max_instance",			"0",
    "language",				"C++",
    "lang_type",			"compile",
    "conf.default.str_cameraName",	TU::IIDCCameraArray::DEFAULT_CAMERA_NAME,
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
IIDCMultiCameraRTCInit(RTC::Manager* manager)
{
    using	rtc_type = TU::CameraRTC<TU::IIDCCameraArray,
					 MultiImage::TimedImages>;
    
    coil::Properties	profile(iidcmulticamera_spec);
    manager->registerFactory(profile,
			     RTC::Create<rtc_type>, RTC::Delete<rtc_type>);
}
};

namespace TU
{
/************************************************************************
*  class CameraRTC<IIDCCameraArray, MultiImage::TimedImages>		*
************************************************************************/
/*
 *  public member functions
 */
template <>
CameraRTC<IIDCCameraArray, MultiImage::TimedImages>
::CameraRTC(RTC::Manager* manager)
    :super(manager),
     _cameraName(IIDCCameraArray::DEFAULT_CAMERA_NAME),
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
CameraRTC<IIDCCameraArray, MultiImage::TimedImages>::initializeConfigurations()
{
    bindParameter("str_cameraName",
		  _cameraName, IIDCCameraArray::DEFAULT_CAMERA_NAME);
    bindParameter("str_recFilePrefix",
		  _recFilePrefix, DEFAULT_RECFILE_PREFIX);
    bindParameter("int_syncedSnap",
		  _syncedSnap, DEFAULT_SYNCED_SNAP);
    bindParameter("int_startWithFlow",
		  _startWithFlow, DEFAULT_START_WITH_FLOW);
    _cameras.setName(_cameraName.c_str());
}

template <> template <> size_t
CameraRTC<IIDCCameraArray, MultiImage::TimedImages>
::setImageFormat(const camera_type& camera, MultiImage::Header& header)
{
    header.width  = camera.width();
    header.height = camera.height();
    
    switch (camera.pixelFormat())
    {
      case IIDCCamera::MONO_8:
	header.format = MultiImage::MONO_8;
	header.size = header.width * header.height;
	break;
      case IIDCCamera::YUV_411:
	header.format = MultiImage::YUV_411;
	header.size = header.width * header.height * 3 / 2;
	break;
      case IIDCCamera::YUV_422:
	header.format = MultiImage::YUV_422;
	header.size = header.width * header.height * 2;
	break;
      case IIDCCamera::YUV_444:
	header.format = MultiImage::YUV_444;
	header.size = header.width * header.height * 3;
	break;
      case IIDCCamera::RGB_24:
	header.format = MultiImage::RGB_24;
	header.size = header.width * header.height * 3;
	break;
      default:
	throw std::runtime_error("Unsupported pixel format!!");
	break;
    }

    return header.size;
}

}
