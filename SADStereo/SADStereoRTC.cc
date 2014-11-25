/*
 *  $Id$
 */
#include "StereoRTC.h"
#include "TU/SADStereo.h"

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_STEREO_PARAMS	"/usr/local/etc/cameras/stereo.params"

// Module specification
static const char* sadstereo_spec[] =
{
    "implementation_id",		"SADStereoRTC",
    "type_name",			"SADStereoRTC",
    "description",			"Stereo matching using SAD",
    "version",				"1.0.0",
    "vendor",				"t.ueshiba@aist.go.jp",
    "category",				"transformer",
    "activity_type",			"SPORADIC",
    "kind",				"DataFlowComponent",
    "max_instance",			"0",
    "language",				"C++",
    "lang_type",			"compile",
    "conf.default.str_stereoParams",	DEFAULT_STEREO_PARAMS,
    ""
};

/************************************************************************
*  global functions							*
************************************************************************/
namespace TU
{
#ifdef HUGE_IMAGE
typedef SADStereo<int, u_short>		SADStereoType;
#else
typedef SADStereo<short, u_char>	SADStereoType;
#endif
}

extern "C"
{
void
SADStereoRTCInit(RTC::Manager* manager)
{
    coil::Properties	profile(sadstereo_spec);
    manager->registerFactory(profile,
                             RTC::Create<TU::StereoRTC<TU::SADStereoType> >,
			     RTC::Delete<TU::StereoRTC<TU::SADStereoType> >);
}
};

namespace TU
{
/************************************************************************
*  class StereoRTC<SADStereoType>					*
************************************************************************/
template <>
StereoRTC<SADStereoType>::StereoRTC(RTC::Manager* manager)
    : RTC::DataFlowComponentBase(manager),
      _mutex(),
      _initialized(false),
      _binocular(false),
      _scale(1.0),
      _rectify(),
      _stereo(),
      _images(),
      _rectifiedImages(),
      _disparityMap(),
      _stereoParams(DEFAULT_STEREO_PARAMS),
      _inImages(),
      _inImagesPort("InTimedImages", _inImages),
      _outImages(),
      _outImagesPort("OutTimedImages", _outImages),
      _command(*this),
      _commandPort("Command")
{
}

template <> void
StereoRTC<SADStereoType>::initializeConfigurations()
{
    bindParameter("str_stereoParams", _stereoParams, DEFAULT_STEREO_PARAMS);
}

}

