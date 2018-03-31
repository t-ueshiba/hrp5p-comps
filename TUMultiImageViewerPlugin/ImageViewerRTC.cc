/*
 *  $Id$
 */
#include "ImageViewer.h"
#include "Img.hh"

/************************************************************************
*  static data								*
************************************************************************/
// Module specification
static const char* multiimageviewer_spec[] =
{
    "implementation_id",	"MultiImageViewer",
    "type_name",		"MultiImageViewer",
    "description",		"Viewing multiple image streams",
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
template <> ImageViewerRTC<Img::TimedImages>*
createImageViewerRTC<Img::TimedImages>()
{
    using rtc_type = ImageViewerRTC<Img::TimedImages>;
    
  // OpenRTMPluginによって立てられた既存のRTCマネージャを獲得
    RTC::Manager&	manager = RTC::Manager::instance();

  // MultiImageViewerコンポーネントを立ち上げ
    coil::Properties	profile(multiimageviewer_spec);
    manager.registerFactory(profile,
			    RTC::Create<rtc_type>, RTC::Delete<rtc_type>);

    return dynamic_cast<rtc_type*>(
		manager.createComponent("MultiImageViewer"));
}

/************************************************************************
*  class ImageViewerRTC<IMAGES>						*
************************************************************************/
template <> template <> bool
ImageViewerRTC<Img::TimedImages>
::setImages(ImageViewer<Img::TimedImages>& imageViewer) const
{
    std::unique_lock<std::mutex>	lock(_mutex);

    if (!_ready)		// 新データが到着していなければ...
  	return false;		// falseを返す.
    
    imageViewer.resize(_images.headers.length());

    size_t	offset = 0;
    for (size_t i = 0; i < imageViewer.size(); ++i)
    {
	const auto&	header = _images.headers[i];
	
	imageViewer[i]->setImage(header.format,
				 header.width, header.height,
				 _images.data.get_buffer() + offset);
	offset += header.size;
    }

    _ready = false;	// 新データは既にセットしてしまったので，falseにする．
    return true;	// 新データがセットされたことを呼出側に知らせる．
}

}

