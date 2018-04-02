/*
 *  $Id$
 */
#include "Img.hh"
#include "ImageViewer.h"

/************************************************************************
*  static data								*
************************************************************************/
// Module specification
static const char* imageviewer_spec[] =
{
    "implementation_id",	"ImageViewer",
    "type_name",		"ImageViewer",
    "description",		"Viewing image streams",
    "version",			"1.0.0",
    "vendor",			"t.ueshiba@aist.go.jp",
    "category",			"viewer",
    "activity_type",		"SPORADIC",
    "kind",			"DataFlowComponent",
    "max_instance",		"0",
    "language",			"C++",
    "lang_type",		"compile",
    "exec_cxt.periodic.rate",	"30.0",
    ""
};

namespace TU
{
/************************************************************************
*  global functions							*
************************************************************************/
template <> ImageViewerRTC<Img::TimedCameraImage>*
createImageViewerRTC<Img::TimedCameraImage>()
{
    using rtc_type = ImageViewerRTC<Img::TimedCameraImage>;
    
  // OpenRTMPluginによって立てられた既存のRTCマネージャを獲得
    RTC::Manager&	manager = RTC::Manager::instance();

  // ImageViewerコンポーネントを立ち上げ
    coil::Properties	profile(imageviewer_spec);
    manager.registerFactory(profile,
			    RTC::Create<rtc_type>, RTC::Delete<rtc_type>);

    return dynamic_cast<rtc_type*>(manager.createComponent("ImageViewer"));
}

/************************************************************************
*  class ImageViewerRTC<IMAGES>						*
************************************************************************/
template <> template <> bool
ImageViewerRTC<Img::TimedCameraImage>
::setImages(ImageViewer<Img::TimedCameraImage>& imageViewer) const
{
    std::unique_lock<std::mutex>	lock(_mutex);

    if (!_ready)		// 新データが到着していなければ...
  	return false;		// falseを返す.

    imageViewer.resize(1);
    
    const auto&	imageData = _images.data.image;
    imageViewer[0]->setImage(imageData.format,
			     imageData.width, imageData.height,
			     imageData.raw_data.get_buffer());

    _ready = false;	// 新データは既にセットしてしまったので，falseにする．
    return true;	// 新データがセットされたことを呼出側に知らせる．
}

}

