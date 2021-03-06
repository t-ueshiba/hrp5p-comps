/*
 *  $Id$
 */
#include "Img.hh"
#include "SynchronizerRTC.h"

/************************************************************************
*  static data								*
************************************************************************/
// Module specification
static const char* videosynchronizer_spec[] =
{
    "implementation_id",	"VideoSynchronizerRTC",
    "type_name",		"VideoSynchronizer",
    "description",		"Sync. image streams with robot poses",
    "version",			"1.0.0",
    "vendor",			"t.ueshiba@aist.go.jp",
    "category",			"sensor reader",
    "activity_type",		"PERIODIC",
    "kind",			"DataFlowComponent",
    "max_instance",		"10",
    "language",			"C++",
    "lang_type",		"compile",
    "conf.default.bufferSize",	SYNCHRONIZERRTC_DEFAULT_BUFFER_SIZE,
    "conf.default.verbose",	SYNCHRONIZERRTC_DEFAULT_VERBOSE,
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
    
