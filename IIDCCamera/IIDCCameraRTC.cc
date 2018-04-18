/*
 *  $Id$
 */
#include <cstdlib>		// for atoi()
#include "TU/IIDCCameraArray.h"
#include "Img.hh"
#include "CameraRTC.h"
#ifdef DEBUG
#  include "RTCTime.h"
#endif

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_RECFILE_PREFIX	"/tmp/IIDCCameraRTC"
#define DEFAULT_DEVICE_NAME	"/dev/video0"
#define DEFAULT_START_WITH_FLOW	"1"

// Module specification
static const char* v4l2camera_spec[] =
{
    "implementation_id",		"IIDCCameraRTC",
    "type_name",			"IIDCCamera",
    "description",			"Controlling a IIDC camera",
    "version",				"1.0.0",
    "vendor",				"t.ueshiba@aist.go.jp",
    "category",				"sensor",
    "activity_type",			"SPORADIC",
    "kind",				"DataFlowComponent",
    "max_instance",			"0",
    "language",				"C++",
    "lang_type",			"compile",
    "conf.default.str_deviceName",	DEFAULT_DEVICE_NAME,
    "conf.default.str_recFilePrefix",	DEFAULT_RECFILE_PREFIX,
    "conf.default.int_startWithFlow",	DEFAULT_START_WITH_FLOW,
    ""
};

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
IIDCCameraRTCInit(RTC::Manager* manager)
{
    using	rtc_type = TU::CameraRTC<TU::IIDCCameraArray,
					 Img::TimedCameraImage>;
    
    coil::Properties	profile(v4l2camera_spec);
    manager->registerFactory(profile,
			     RTC::Create<rtc_type>, RTC::Delete<rtc_type>);
}
};

namespace TU
{
/************************************************************************
*  class CameraRTC<IIDCCameraArray>					*
************************************************************************/
template <> inline RTC::Time
CameraRTCBase<IIDCCameraArray>::getTimestamp(const camera_type& camera)
{
    const auto	timestamp   = camera.getTimestamp();
#ifdef DEBUG
    const auto	arrivaltime = camera.getArrivaltime();
    std::cerr << "timestamp: " << timestamp << '\t'
	      << "arrival: "   << arrivaltime << std::endl;
#endif
    const std::chrono::nanoseconds	nsec = timestamp.time_since_epoch();
    
    return {CORBA::ULong(nsec.count() / 1000000000),
	    CORBA::ULong(nsec.count() % 1000000000)};
}

/************************************************************************
*  class CameraRTC<IIDCCameraArray, Img::TimedCameraImage>		*
************************************************************************/
/*
 *  private member functions
 */
template <> void
CameraRTC<IIDCCameraArray, Img::TimedCameraImage>::initializeConfigurations()
{
    bindParameter("str_cameraConfig",
		  _cameraName, DEFAULT_DEVICE_NAME);
    bindParameter("str_recFilePrefix",
		  _recFilePrefix, DEFAULT_RECFILE_PREFIX);
    bindParameter("int_startWithFlow",
		  _startWithFlow, DEFAULT_START_WITH_FLOW);
}

template <> void
CameraRTC<IIDCCameraArray, Img::TimedCameraImage>::allocateImages()
{
    auto&	imageData = _images.data.image;

    imageData.width  = _cameras[0].width();
    imageData.height = _cameras[0].height();

    switch (_cameras[0].pixelFormat())
    {
      case IIDCCamera::MONO_8:
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
CameraRTC<IIDCCameraArray, Img::TimedCameraImage>::recordImages(bool enable)
{
    if (enable == inRecordingImages())
	return;

    std::unique_lock<std::mutex>	lock(_mutex);

    if (enable)
    {
	using	std::operator +;
	
	char		recFileSuffix[128];
	const auto	t = std::time(NULL);
	std::strftime(recFileSuffix, sizeof(recFileSuffix),
		      "-%F-%T.epbms", std::localtime(&t));
	const auto	recFileName = _recFilePrefix
				    + std::string(recFileSuffix);
	_fout.open(recFileName.c_str(),
		   std::ofstream::out | std::ofstream::binary);
	if (!_fout)
	    throw std::runtime_error("CameraRTC::recordImages(): failed to open "
				     + recFileName + " !");
	
	_fout << "M1" << std::endl;
	saveImageHeader(_fout, _images.data.image);
    }
    else
    {
	_fout.close();
    }
}

/*
 *  public member functions
 */
template <>
CameraRTC<IIDCCameraArray, Img::TimedCameraImage>
::CameraRTC(RTC::Manager* manager)
    :super(manager),
     _cameraName(DEFAULT_DEVICE_NAME),
     _recFilePrefix(DEFAULT_RECFILE_PREFIX),
     _syncedSnap(false),
     _startWithFlow(atoi(DEFAULT_START_WITH_FLOW)),
     _images(),
     _imagesOut("TimedCameraImage", _images)
{
}

template <> RTC::ReturnCode_t
CameraRTC<IIDCCameraArray, Img::TimedCameraImage>
::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "CameraRTC::onActivated" << std::endl;
#endif
    try
    {
      // 設定ファイルを読み込んでカメラを生成・セットアップ
	_cameras.resize(1);
	_cameras[0].initialize();
	allocateImages();		// 画像データ領域を確保
	embedTimestamp(true);		// 撮影時刻の記録を初期化
	continuousShot(_startWithFlow);	// 連続撮影/停止のいずれかで開始
    }
    catch (std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	
	return RTC::RTC_ERROR;
    }

    return RTC::RTC_OK;
}

template <> RTC::ReturnCode_t
CameraRTC<IIDCCameraArray, Img::TimedCameraImage>::onExecute(RTC::UniqueId ec_id)
{
    std::unique_lock<std::mutex>	lock(super::_mutex);

    if (inContinuousShot())
    {
#ifdef DEBUG
	countTime();
#endif
	auto&	camera = _cameras[0];
	camera.snap();

	auto&	imageData = _images.data.image;
	switch (camera.pixelFormat())
	{
	  case IIDCCamera::MONO_8:
	  {
	    Image<u_char>	image(reinterpret_cast<u_char*>(
					  imageData.raw_data.get_buffer(false)),
				      imageData.width, imageData.height);
	    camera >> image;
	  }
	    break;

	  default:
	  {
	    Image<RGB>		image(reinterpret_cast<RGB*>(
					  imageData.raw_data.get_buffer(false)),
				      imageData.width, imageData.height);
	    camera >> image;
	  }
	    break;
	}
	_images.tm = getTimestamp(camera);
	_imagesOut.write();
    }

    return RTC::RTC_OK;
}

}
