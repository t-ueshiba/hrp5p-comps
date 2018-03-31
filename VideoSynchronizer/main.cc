/*
 *  $Id$
 */
#include <rtm/Manager.h>
#include <iostream>

extern "C"
{
DLL_EXPORT void	VideoSynchronizerRTCInit(RTC::Manager* manager)		;
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
    VideoSynchronizerRTCInit(manager);

    if (!(rtc = manager->createComponent("VideoSynchronizerRTC")))
	throw std::runtime_error("Failed to create component.");
}

/************************************************************************
*  global funtions							*
************************************************************************/
int
main(int argc, char** argv)
{
    try
    {
	const auto	manager = RTC::Manager::init(argc, argv);
	manager->setModuleInitProc(MyModuleInit);
	manager->activateManager();
	manager->runManager(false);
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
