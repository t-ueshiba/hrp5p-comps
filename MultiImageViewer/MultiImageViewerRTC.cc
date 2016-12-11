/*
 *  $Id$
 */
#include "MultiImageViewerRTC.h"

/************************************************************************
*  static data								*
************************************************************************/
// Module specification
static const char* multiimageviewer_spec[] =
{
    "implementation_id",	"MultiImageViewerRTC",
    "type_name",		"MultiImageViewer",
    "description",		"Viewing multiple image streams",
    "version",			"1.0.0",
    "vendor",			"t.ueshiba@aist.go.jp",
    "category",			"sensor",
    "activity_type",		"PERIODIC",
    "kind",			"DataFlowComponent",
    "max_instance",		"1",
    "language",			"C++",
    "lang_type",		"compile",
    ""				// <-- Important! End of Spec. mark
};

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
MultiImageViewerRTCInit(RTC::Manager* manager)
{
    coil::Properties	profile(multiimageviewer_spec);
    manager->registerFactory(profile,
                             RTC::Create<TU::MultiImageViewerRTC>,
			     RTC::Delete<TU::MultiImageViewerRTC>);
}
};

namespace TU
{
/************************************************************************
*  class MultiImageViewerRTC						*
************************************************************************/
MultiImageViewerRTC::MultiImageViewerRTC(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _mutex(),
     _ready(false),
     _images(),
     _imagesIn("TimedImages", _images)
{
}

RTC::ReturnCode_t
MultiImageViewerRTC::onInitialize()
{
#ifdef DEBUG
    std::cerr << "MultiImageViewerRTC::onInitialize" << std::endl;
#endif    
    addInPort("TimedImages", _imagesIn);
    
    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewerRTC::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiImageViewerRTC::onActivated" << std::endl;
#endif
    _ready = false;
    
    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewerRTC::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiImageViewerRTC::onDeactivated" << std::endl;
#endif
    _ready = false;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewerRTC::onExecute(RTC::UniqueId ec_id)
{
//#ifdef DEBUG
#if 0
    static int	n = 0;
    std::cerr << "MultiImageViewerRTC::onExecute " << n++;
#endif
    coil::Guard<coil::Mutex>	guard(_mutex);
    
    if (!_ready && _imagesIn.isNew())
    {
	do
	{
	    _imagesIn.read();
	}
	while (_imagesIn.isNew());

	_ready = true;
    }
//#ifdef DEBUG
#if 0
    std::cerr << std::endl;
#endif
    return RTC::RTC_OK;
}

#ifdef DEBUG
RTC::ReturnCode_t
MultiImageViewerRTC::onStartup(RTC::UniqueId ec_id)
{
    std::cerr << "MultiImageViewerRTC::onStartup" << std::endl;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewerRTC::onShutdown(RTC::UniqueId ec_id)
{
    std::cerr << "MultiImageViewerRTC::onShutdown" << std::endl;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t MultiImageViewerRTC::onAborting(RTC::UniqueId ec_id)
{
    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewerRTC::onFinalize()
{
    std::cerr << "MultiImageViewerRTC::onFinalize" << std::endl;

    return RTC::RTC_OK;
}
#endif

}	// namepsace TU
