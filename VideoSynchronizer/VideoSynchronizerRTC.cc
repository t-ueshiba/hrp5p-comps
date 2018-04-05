/*
 *  $Id$
 */
#include "Img.hh"
#include "SynchronizerRTC.h"

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_BUFFER_SIZE	"1000"

// Module specification
static const char* videosynchronizer_spec[] =
{
    "implementation_id",		"VideoSynchronizerRTC",
    "type_name",			"VideoSynchronizer",
    "description",			"Sync. image streams with robot poses",
    "version",				"1.0.0",
    "vendor",				"t.ueshiba@aist.go.jp",
    "category",				"sensor reader",
    "activity_type",			"PERIODIC",
    "kind",				"DataFlowComponent",
    "max_instance",			"10",
    "language",				"C++",
    "lang_type",			"compile",
    "conf.default.int_bufferSize",	DEFAULT_BUFFER_SIZE,
    ""				// <-- Important! End of Spec. mark
};

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
VideoSynchronizerRTCInit(RTC::Manager* manager)
{
    using	synchronizer_t = TU::SynchronizerRTC<Img::TimedCameraImage,
						     RTC::TimedDoubleSeq>;
    
    coil::Properties	profile(videosynchronizer_spec);
    manager->registerFactory(profile,
                             RTC::Create<synchronizer_t>,
			     RTC::Delete<synchronizer_t>);
}
}

namespace TU
{
#ifdef DEBUG
std::ostream&
operator <<(std::ostream& out, const RTC::TimedDoubleSeq& q)
{
    out << q.tm << '\t';
  /*
    for (size_t i = 0; i < q.data.length(); ++i)
	out << ' ' << q.data[i];
  */
    return out;
}
#endif

template <> void
SynchronizerRTC<Img::TimedCameraImage, RTC::TimedDoubleSeq>
::initializeConfigurations()
{
    bindParameter("int_bufferSize", _bufSize, DEFAULT_BUFFER_SIZE);
}
    
}
    
