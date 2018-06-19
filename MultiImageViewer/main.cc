/*
 *  $Id$
 */
#include <rtm/Manager.h>
#include "MultiImage.hh"
#include "MyCmdWindow.h"

/************************************************************************
*  static data								*
************************************************************************/
static RTC::RtcBase*	rtc = nullptr;

// Module specification
static const char* multiimageviewer_spec[] =
{
    "implementation_id",	"MultiImageViewerRTC",
    "type_name",		"MultiImageViewer",
    "description",		"Viewing multiple image streams",
    "version",			"1.0.0",
    "vendor",			"t.ueshiba@aist.go.jp",
    "category",			"viewer",
    "activity_type",		"PERIODIC",
    "kind",			"DataFlowComponent",
    "max_instance",		"1",
    "language",			"C++",
    "lang_type",		"compile",
    ""				// <-- Important! End of Spec. mark
};

/************************************************************************
*  static funtions							*
************************************************************************/
extern "C"
{
static void
MultiImageViewerRTCInit(RTC::Manager* manager)
{
    using	rtc_type = TU::ImageViewerRTC<MultiImage::TimedImages>;
    
    coil::Properties	profile(multiimageviewer_spec);
    manager->registerFactory(profile,
			     RTC::Create<rtc_type>, RTC::Delete<rtc_type>);
}
};

static void
MyModuleInit(RTC::Manager* manager)
{
    MultiImageViewerRTCInit(manager);

    if (!(rtc = manager->createComponent("MultiImageViewerRTC")))
	throw std::runtime_error("Failed to create component.");
}

/************************************************************************
*  global funtions							*
************************************************************************/
int
main(int argc, char** argv)
{
    using namespace	TU;

    try
    {
	using image_t	= MultiImage::TimedImages;
	using format_t	= MultiImage::PixelFormat;
	
	const auto	manager = RTC::Manager::init(argc, argv);
	manager->setModuleInitProc(MyModuleInit);
	manager->activateManager();
	manager->runManager(true);

	v::App	vapp(argc, argv);
	v::MyCmdWindow<image_t, format_t>
		win(vapp, dynamic_cast<ImageViewerRTC<image_t>*>(rtc));
	vapp.run();
    }
    catch (const std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	return 1;
    }
    catch (...)
    {
	std::cerr << "Unknown error." << std::endl;
	return 1;
    }
    
    return 0;
}
