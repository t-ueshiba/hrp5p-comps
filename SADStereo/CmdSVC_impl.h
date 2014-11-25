/*
 *  $Id$
 */
#include <sstream>
#include <boost/archive/xml_oarchive.hpp>
#include "Cmd.hh"
#include "CmdDef.h"

#ifndef CMDSVC_IMPL_H
#define CMDSVC_IMPL_H

namespace TU
{
namespace v
{
/************************************************************************
*  class CmdSVC_impl<STEREO_RTC>					*
************************************************************************/
template <class STEREO_RTC>
class CmdSVC_impl : public virtual POA_Cmd::Controller,
		    public virtual PortableServer::RefCountServantBase
{
  private:
    typedef typename STEREO_RTC::params_type	params_type;
    
    enum
    {
	c_Binocular,
	c_DisparitySearchWidth, c_DisparityMax, c_DisparityInconsistency,
	c_grainSize, c_WindowSize, c_IntensityDiffMax, c_Regularization,
    };
    
  public:
			CmdSVC_impl(STEREO_RTC& rtc) :_rtc(rtc)		{}
    virtual		~CmdSVC_impl()					{}

    char*		getCmds()					;
    CORBA::Boolean	setValues(const Cmd::Values& vals)		;
    Cmd::Values*	getValues(const Cmd::Values& ids)		;
    
  private:
    static CmdDefs	createCmds()					;
    static void		addOtherCmds(CmdDefs& cmds)			;
    CORBA::Boolean	setOtherValues(const Cmd::Values& vals)		;
    CORBA::Long		getOtherValues(const Cmd::Values& ids)	const	;
    
  private:
    STEREO_RTC&		_rtc;
};

template <class STEREO_RTC> char*
CmdSVC_impl<STEREO_RTC>::getCmds()
{
#ifdef DEBUG
    std::cerr << "CmdSVC_impl<STEREO_RTC>::getCmds() called!" << std::endl;
#endif
    std::ostringstream			oss;
    boost::archive::xml_oarchive	oar(oss);
    CmdDefs				cmds = createCmds();
    oar << BOOST_SERIALIZATION_NVP(cmds);
    
    return CORBA::string_dup(oss.str().c_str());
}

template <class STEREO_RTC> CORBA::Boolean
CmdSVC_impl<STEREO_RTC>::setValues(const Cmd::Values& vals)
{
#ifdef DEBUG
    using namespace	std;
    
    cerr << "CmdSVC_impl<STEREO_RTC>::setValues(): vals =";
    for (CORBA::ULong i = 0; i < vals.length(); ++i)
	cerr << ' ' << vals[i];
    cerr << endl;
#endif
    params_type	params = _rtc.getParameters();

    switch (vals[0])
    {
      case c_Binocular:
	_rtc.setBinocular(vals[1]);
	break;
      case c_DisparitySearchWidth:
	params.disparitySearchWidth = vals[1];
	_rtc.setParameters(params, true);
	break;
      case c_DisparityMax:
	params.disparityMax = vals[1];
	_rtc.setParameters(params, true);
	break;
      case c_DisparityInconsistency:
	params.disparityInconsistency = vals[1];
	_rtc.setParameters(params);
	break;
      case c_grainSize:
	params.grainSize = vals[1];
	_rtc.setParameters(params);
	break;
      case c_WindowSize:
	params.windowSize = vals[1];
	_rtc.setParameters(params);
	break;
      case c_IntensityDiffMax:
	params.intensityDiffMax = vals[1];
	_rtc.setParameters(params);
	break;
      default:
	return setOtherValues(vals);
    }
    
    return true;
}

template <class STEREO_RTC> Cmd::Values*
CmdSVC_impl<STEREO_RTC>::getValues(const Cmd::Values& ids)
{
#ifdef DEBUG
    using namespace	std;
    
    cerr << "CmdSVC_impl<STEREO_RTC>::getValues(): ids =";
    for (CORBA::ULong i = 0; i < ids.length(); ++i)
	cerr << ' ' << ids[i];
#endif
    Cmd::Values	vals;
    vals.length(1);
    
    switch (ids[0])
    {
      case c_Binocular:
	vals[0] = _rtc.getBinocular();
	break;
      case c_DisparitySearchWidth:
	vals[0] = _rtc.getParameters().disparitySearchWidth;
	break;
      case c_DisparityMax:
	vals[0] = _rtc.getParameters().disparityMax;
	break;
      case c_DisparityInconsistency:
	vals[0] = _rtc.getParameters().disparityInconsistency;
	break;
      case c_grainSize:
	vals[0] = _rtc.getParameters().grainSize;
	break;
      case c_WindowSize:
	vals[0] = _rtc.getParameters().windowSize;
	break;
      case c_IntensityDiffMax:
	vals[0] = _rtc.getParameters().intensityDiffMax;
	break;
      default:
	vals[0] = getOtherValues(ids);
	break;
    }
#ifdef DEBUG
    cerr << ", vals =";
    for (CORBA::ULong i = 0; i < vals.length(); ++i)
	cerr << ' ' << vals[i];
    cerr << endl;
#endif
    return new Cmd::Values(vals);
}

template <class STEREO_RTC> CmdDefs
CmdSVC_impl<STEREO_RTC>::createCmds()
{
    static CmdDefs cmds =
    {
	{C_ToggleButton, "Binocular",	      c_Binocular,		noSub,
	 0, 1, 1, 1, CA_None, 0, 0},
	{C_Slider, "Window size",	      c_WindowSize,		noSub,
	 3, 81, 1, 1, CA_None, 0, 1},
	{C_Slider, "Disparity search width",  c_DisparitySearchWidth,	noSub,
	 16, 192, 1, 1, CA_None, 0, 2},
	{C_Slider, "Max. disparity",	      c_DisparityMax,		noSub,
	 48, 768, 1, 1, CA_None, 0, 3},
	{C_Slider, "Disparity inconsistency", c_DisparityInconsistency,	noSub,
	 0, 10, 1, 1, CA_None, 0, 4},
	{C_Slider, "Max. intensity diff.",    c_IntensityDiffMax,	noSub,
	 0, 10, 1, 1, CA_None, 0, 5},
    };

    addOtherCmds(cmds);
    
    return cmds;
}

template <class STEREO_RTC> void
CmdSVC_impl<STEREO_RTC>::addOtherCmds(CmdDefs& cmds)
{
}
    
template <class STEREO_RTC> CORBA::Boolean
CmdSVC_impl<STEREO_RTC>::setOtherValues(const Cmd::Values& vals)
{
    return false;
}

template <class STEREO_RTC> CORBA::Long
CmdSVC_impl<STEREO_RTC>::getOtherValues(const Cmd::Values& ids) const
{
    return 0;
}

}
}
#endif // CMDSVC_IMPL_H
