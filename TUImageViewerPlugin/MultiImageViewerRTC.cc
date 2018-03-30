/*
 *  $Id$
 */
#include "MultiImageViewer.h"
#include "Img.hh"

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

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
ImageViewerRTCInit(RTC::Manager* manager)
{
    using	rtc_type = TU::MultiImageViewerRTC<Img::TimedCameraImage>;
    
    coil::Properties	profile(imageviewer_spec);
    manager->registerFactory(profile,
                             RTC::Create<rtc_type>, RTC::Delete<rtc_type>);
}
};

namespace TU
{
template <> MultiImageViewerRTC<Img::TimedCameraImage>*
createMultiImageViewerRTC<Img::TimedCameraImage>()
{
  // OpenRTMPluginによって立てられた既存のRTCマネージャを獲得
    RTC::Manager&	manager = RTC::Manager::instance();

  // MultiImageViewerコンポーネントを立ち上げ
    ImageViewerRTCInit(&manager);
    return dynamic_cast<MultiImageViewerRTC<Img::TimedCameraImage>*>(
		manager.createComponent("ImageViewer"));
}

/************************************************************************
*  class ImageViewerRTC							*
************************************************************************/
template <> template <> bool
MultiImageViewerRTC<Img::TimedCameraImage>::
setImages(MultiImageViewer<Img::TimedCameraImage>* multiImageViewer)
{
    std::unique_lock<std::mutex>	lock(_mutex);

    if (!_ready)		// 新データが到着していなければ...
  	return false;		// falseを返す.

    multiImageViewer->resize(1);
    
    const auto&	imageData = _images.data.image;
    (*multiImageViewer)[0]->setImage(imageData.format,
				     imageData.width, imageData.height,
				     imageData.raw_data.get_buffer());

    _ready = false;	// 新データは既にセットしてしまったので，falseにする．
    return true;	// 新データがセットされたことを呼出側に知らせる．
}

}

