/*
 *  $Id$
 */
#include <rtm/Manager.h>
#include <iostream>
#include "MyCmdWindow.h"

extern "C"
{
DLL_EXPORT void	MultiImageViewerRTCInit(RTC::Manager* manager)		;
}

/************************************************************************
*  static data								*
************************************************************************/
static RTC::RtcBase*	rtc = nullptr;

/************************************************************************
*  static funtions							*
************************************************************************/
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
	const auto	manager = RTC::Manager::init(argc, argv);
	manager->setModuleInitProc(MyModuleInit);
	manager->activateManager();
	manager->runManager(true);

	v::App		vapp(argc, argv);
	v::MyCmdWindow	win(vapp, dynamic_cast<MultiImageViewerRTC*>(rtc));
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
