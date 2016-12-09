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
     _alive(true),
     _isNew(false),
     _images(),
     _imagesIn("TimedImages", _images)
{
    std::cerr << "MultiImageViewerRTC::MultiImageViewerRTC()" << std::endl;
}

MultiImageViewerRTC::~MultiImageViewerRTC()
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
    coil::Guard<coil::Mutex>	guard(_mutex);
    _alive = true;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewerRTC::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiImageViewerRTC::onDeactivated" << std::endl;
#endif
    coil::Guard<coil::Mutex>	guard(_mutex);
    _alive = false;
    _isNew = false;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewerRTC::onExecute(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    static int	n = 0;
    std::cerr << "MultiImageViewerRTC::onExecute " << n++;
#endif
    coil::Guard<coil::Mutex>	guard(_mutex);
    
    if (_imagesIn.isNew())
    {
	do
	{
	    _imagesIn.read();
	}
	while (_imagesIn.isNew());

	_isNew = true;
    }
#ifdef DEBUG
    std::cerr << std::endl;
#endif
    return RTC::RTC_OK;
}

bool
MultiImageViewerRTC::setImages(v::MyCmdWindow& win)
{
    coil::Guard<coil::Mutex>	guard(_mutex);

    if (!_isNew)
	return false;

    bool	resized = win.resize(_images.headers.length());
    size_t	offset = 0;
    for (size_t view = 0; view < _images.headers.length(); ++view)
    {
	const auto&	header = _images.headers[view];
	
	win.setImage(view, header, _images.data.get_buffer() + offset);
	offset += header.size;
    }

    if (resized)
	win.show();

    _isNew = false;
    
    return true;
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
