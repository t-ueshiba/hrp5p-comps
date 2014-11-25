/*
 *  $Id$
 */
#include "StereoRTC.h"
#include "TU/GFStereo.h"

/************************************************************************
*  static data								*
************************************************************************/
#define DEFAULT_STEREO_PARAMS	"/usr/local/etc/cameras/stereo.params"

// Module specification
static const char* sadstereo_spec[] =
{
    "implementation_id",		"GFStereoRTC",
    "type_name",			"GFStereoRTC",
    "description",			"Stereo matching using Guided Filter",
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
typedef GFStereo<float, u_short>	GFStereoType;
#else
typedef GFStereo<float, u_char>		GFStereoType;
#endif
}

extern "C"
{
void
GFStereoRTCInit(RTC::Manager* manager)
{
    coil::Properties	profile(sadstereo_spec);
    manager->registerFactory(profile,
                             RTC::Create<TU::StereoRTC<TU::GFStereoType> >,
			     RTC::Delete<TU::StereoRTC<TU::GFStereoType> >);
}
};

namespace TU
{
/************************************************************************
*  class StereoRTC<GFStereoType>					*
************************************************************************/
template <>
StereoRTC<GFStereoType>::StereoRTC(RTC::Manager* manager)
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
StereoRTC<GFStereoType>::initializeConfigurations()
{
    bindParameter("str_stereoParams", _stereoParams, DEFAULT_STEREO_PARAMS);
}

namespace v
{
/************************************************************************
*  class CmdSVC_impl<StereoRTC<GFStereoType> >				*
************************************************************************/
template <> void
CmdSVC_impl<StereoRTC<GFStereoType> >::addOtherCmds(CmdDefs& cmds)
{
    size_t	y = (cmds.empty() ? 0 : cmds.back().gridy + 1);

    cmds.push_back(CmdDef(C_Slider, "Regularization", c_Regularization, noSub,
			  1, 255, 1, 1, CA_None, 0, y));
}
    
template <> CORBA::Boolean
CmdSVC_impl<StereoRTC<GFStereoType> >::setOtherValues(const Cmd::Values& vals)
{
    params_type	params = _rtc.getParameters();

    switch (vals[0])
    {
      case c_Regularization:
	params.epsilon = vals[1];
	_rtc.setParameters(params);
	break;
    }
    
    return false;
}

template <> CORBA::Long
CmdSVC_impl<StereoRTC<GFStereoType> >::getOtherValues(
					   const Cmd::Values& ids) const
{
    switch (ids[0])
    {
      case c_Regularization:
	return _rtc.getParameters().epsilon;
    }
    
    return 0;
}

}
}


