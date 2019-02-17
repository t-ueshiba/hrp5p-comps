/*
 *  $Id$
 */
#include "Img.hh"
#include "LentiMarkDataType.hh"
#include "SynchronizerRTC.h"

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_BUFFER_SIZE	"100"
#define DEFAULT_VERBOSE		"0"

// Module specification
static const char* videosynchronizer_spec[] =
{
    "implementation_id",	"MarkerPoseSynchronizerRTC",
    "type_name",		"MarkerPoseSynchronizer",
    "description",		"Sync. image streams with robot poses",
    "version",			"1.0.0",
    "vendor",			"t.ueshiba@aist.go.jp",
    "category",			"sensor reader",
    "activity_type",		"PERIODIC",
    "kind",			"DataFlowComponent",
    "max_instance",		"10",
    "language",			"C++",
    "lang_type",		"compile",
    "conf.default.bufferSize",	DEFAULT_BUFFER_SIZE,
    "conf.default.verbose",	DEFAULT_VERBOSE,
    ""				// <-- Important! End of Spec. mark
};

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
MarkerPoseSynchronizerRTCInit(RTC::Manager* manager)
{
    using	synchronizer_t = TU::SynchronizerRTC<TimedMarkerPoseSeq,
						     RTC::TimedDoubleSeq>;
    
    coil::Properties	profile(videosynchronizer_spec);
    manager->registerFactory(profile,
                             RTC::Create<synchronizer_t>,
			     RTC::Delete<synchronizer_t>);
}
}

namespace TU
{
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

template <> void
SynchronizerRTC<TimedMarkerPoseSeq, RTC::TimedDoubleSeq>
::initializeConfigurations()
{
    bindParameter("bufferSize", _bufSize, DEFAULT_BUFFER_SIZE);
    bindParameter("verbose",    _verbose, DEFAULT_VERBOSE);
}
    
}
    
