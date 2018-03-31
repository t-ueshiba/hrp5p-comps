/*
 *  $Id$
 */
#include <rtm/Manager.h>
#include <iostream>
#include <stdexcept>

extern "C"
{
DLL_EXPORT void	V4L2CameraRTCInit(RTC::Manager* manager)		;
}

/************************************************************************
*  static funtions							*
************************************************************************/
void
MyModuleInit(RTC::Manager* manager)
{
    V4L2CameraRTCInit(manager);

    const auto	rtc = manager->createComponent("V4L2CameraRTC");
    if (!rtc)
	throw std::runtime_error("Failed to create component.");
}

/************************************************************************
*  global funtions							*
************************************************************************/
int
main(int argc, char* argv[])
{
    try
    {
	const auto	manager = RTC::Manager::init(argc, argv);
	manager->init(argc, argv);
	manager->setModuleInitProc(MyModuleInit);
	manager->activateManager();
	manager->runManager();
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
