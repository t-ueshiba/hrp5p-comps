// -*- C++ -*-
/*!
 * @file MultiImageViewerComp.cpp
 * @brief Standalone component
 * @date $Date$
 *
 * $Id$
 */

#include <rtm/Manager.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "MultiImageViewerRTC.h"

extern "C"
{
DLL_EXPORT void	MultiImageViewerRTCInit(RTC::Manager* manager)		;
}

static RTC::RtcBase*	comp;

void
MyModuleInit(RTC::Manager* manager)
{
    MultiImageViewerRTCInit(manager);

    comp = manager->createComponent("MultiImageViewerRTC");
    if (comp == 0)
    {
	std::cerr << "Component create failed." << std::endl;
	abort();
    }
}

int
main(int argc, char** argv)
{
    using namespace	TU;
    
    RTC::Manager*	manager = RTC::Manager::init(argc, argv);

  // Initialize manager
    manager->init(argc, argv);

  // Set module initialization proceduer
  // This procedure will be invoked in activateManager() function.
    manager->setModuleInitProc(MyModuleInit);

  // Activate manager and register to naming service
    manager->activateManager();

  // run the manager in blocking mode
  // runManager(false) is the default.
  // manager->runManager();

  // If you want to run the manager in non-blocking mode, do like this
    manager->runManager(true);

    v::App		vapp(argc, argv);
    v::MyCmdWindow	win(vapp, dynamic_cast<MultiImageViewerRTC*>(comp));
    vapp.run();

    return 0;
}
