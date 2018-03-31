/*
 *  $Id$
 */
#include <fstream>
#include <cstdlib>			// for atoi()
#include "TU/IIDCCameraArray.h"
#include "MultiCameraRTC.h"

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
    coil::Properties	profile(iidcmulticamera_spec);
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
/*
 *  public member functions
 */
template <>
MultiCameraRTC<IIDCCameraArray>::MultiCameraRTC(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _cameras(),
     _mutex(),
     _cameraName(IIDCCameraArray::DEFAULT_CAMERA_NAME),
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
MultiCameraRTC<IIDCCameraArray>::saveConfig()
{
    for (auto& camera : _cameras)
	camera.saveConfig(1);
}
    
template <> void
MultiCameraRTC<IIDCCameraArray>::restoreConfig()
{
    for (auto& camera : _cameras)
	camera.restoreConfig(1);
}
    
template <> void
MultiCameraRTC<IIDCCameraArray>::setFormat(const Cmd::Values& vals)
{
    std::unique_lock<std::mutex>	lock(_mutex);

    if (vals.length() == 3)
	TU::setFormat(_cameras, vals[1].i, vals[2].i);
    else if (vals.length() == 8)
    {
	const auto	format7	    = IIDCCamera::uintToFormat(vals[1].i);
	const auto	pixelFormat = IIDCCamera::uintToPixelFormat(vals[7].i);
	
	for (auto& camera : _cameras)
	    camera.setFormat_7_ROI(format7,
				   vals[2].i, vals[3].i, vals[4].i, vals[5].i)
		  .setFormat_7_PixelFormat(format7, pixelFormat)
		  .setFormat_7_PacketSize(format7, vals[6].i)
		  .setFormatAndFrameRate(format7, IIDCCamera::FrameRate_x);
    }

    allocateImages();
}
    
template <> bool
MultiCameraRTC<IIDCCameraArray>::setFeature(const Cmd::Values& vals,
					    size_t n, bool all)
{
    if (all)
	return TU::setFeature(_cameras, vals[0].i, vals[1].i, vals[1].f);
    else
    {
	auto	camera = std::begin(_cameras);
	std::advance(camera, n);
	return TU::setFeature(*camera, vals[0].i, vals[1].i, vals[1].f);
    }
}

/*
 *  private member functions
 */
template <> void
MultiCameraRTC<IIDCCameraArray>::initializeConfigurations()
{
    bindParameter("str_cameraName",
		  _cameraName, IIDCCameraArray::DEFAULT_CAMERA_NAME);
    bindParameter("str_recFilePrefix",
		  _recFilePrefix, DEFAULT_RECFILE_PREFIX);
    bindParameter("int_syncedSnap",
		  _syncedSnap, DEFAULT_SYNCED_SNAP);
    bindParameter("int_startWithFlow",
		  _startWithFlow, DEFAULT_START_WITH_FLOW);
}

template <> void
MultiCameraRTC<IIDCCameraArray>::enableTimestamp()
{
    std::for_each(std::begin(_cameras), std::end(_cameras),
		  [](auto& camera){ camera.embedTimestamp(true); });
}

template <> RTC::Time
MultiCameraRTC<IIDCCameraArray>::getTimestamp(const camera_type& camera) const
{
    const std::chrono::nanoseconds
		nsec = camera.getTimestamp().time_since_epoch();
    return {CORBA::ULong(nsec.count() / 1000000000),
	    CORBA::ULong(nsec.count() % 1000000000)};
}

template <> size_t
MultiCameraRTC<IIDCCameraArray>::setImageFormat(const camera_type& camera,
						Img::Header& header)
{
    switch (camera.pixelFormat())
    {
      case IIDCCamera::MONO_8:
	header.format = Img::MONO_8;
	header.size = header.width * header.height;
	break;
      case IIDCCamera::YUV_411:
	header.format = Img::YUV_411;
	header.size = header.width * header.height * 3 / 2;
	break;
      case IIDCCamera::YUV_422:
	header.format = Img::YUV_422;
	header.size = header.width * header.height * 2;
	break;
      case IIDCCamera::YUV_444:
	header.format = Img::YUV_444;
	header.size = header.width * header.height * 3;
	break;
      case IIDCCamera::RGB_24:
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
