/*
 *  $Id$
 */
#include <rtm/Manager.h>
#include <iostream>
#include "MultiImageViewerRTC.h"
#include "MyCmdWindow.h"

extern "C"
{
DLL_EXPORT void	MultiImageViewerRTCInit(RTC::Manager* manager)		;
}

static RTC::RtcBase*	rtc = nullptr;

void
MyModuleInit(RTC::Manager* manager)
{
    MultiImageViewerRTCInit(manager);

    if (!(rtc = manager->createComponent("MultiImageViewerRTC")))
    {
	std::cerr << "Component create failed." << std::endl;
	abort();
    }
}

int
main(int argc, char** argv)
{
    using namespace	TU;
    
    const auto	manager = RTC::Manager::init(argc, argv);
    manager->setModuleInitProc(MyModuleInit);
    manager->activateManager();
    manager->runManager(true);

    v::App		vapp(argc, argv);
    v::MyCmdWindow	win(vapp, dynamic_cast<MultiImageViewerRTC*>(rtc));
    vapp.run();

    return 0;
}
