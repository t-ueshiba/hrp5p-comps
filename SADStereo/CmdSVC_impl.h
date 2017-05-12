/*
 *  $Id$
 */
#ifndef	CMDSVC_IMPL_H
#define	CMDSVC_IMPL_H

#include <sstream>
#include <boost/archive/xml_oarchive.hpp>
#include "Cmd.hh"
#include "CmdDef.h"

namespace TU
{
#ifdef DEBUG
inline std::ostream&
print(std::ostream& out, const Cmd::Values& vals)
{
    for (CORBA::ULong i = 0; i < vals.length(); ++i)
	out << " (" << vals[i].i << ',' << vals[i].f << ')';
    return out;
}
#endif
    
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
    Cmd::Values*	setValues(const Cmd::Values& vals)		;
    Cmd::Values*	getValues(const Cmd::Values& ids,
				  CORBA::Boolean range)			;
    
  private:
    static CmdDefs	createCmds()					;
    static void		addOtherCmds(CmdDefs& cmds)			;
    Cmd::Values		setOtherValues(const Cmd::Values& vals)		;
    Cmd::Values		getOtherValues(const Cmd::Values& ids)	const	;
    
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

template <class STEREO_RTC> Cmd::Values*
CmdSVC_impl<STEREO_RTC>::setValues(const Cmd::Values& vals)
{
#ifdef DEBUG
    print(std::cerr << "CmdSVC_impl<STEREO_RTC>::setValues(): vals =", vals)
	<< std::endl;
#endif
    params_type	params = _rtc.getParameters();
    Cmd::Values	ids;

    switch (vals[0].i)
    {
      case c_Binocular:
	_rtc.setBinocular(vals[1].i);
	break;
      case c_DisparitySearchWidth:
	params.disparitySearchWidth = vals[1].i;
	_rtc.setParameters(params, true);
	break;
      case c_DisparityMax:
	params.disparityMax = vals[1].i;
	_rtc.setParameters(params, true);
	break;
      case c_DisparityInconsistency:
	params.disparityInconsistency = vals[1].i;
	_rtc.setParameters(params);
	break;
      case c_grainSize:
	params.grainSize = vals[1].i;
	_rtc.setParameters(params);
	break;
      case c_WindowSize:
	params.windowSize = vals[1].i;
	_rtc.setParameters(params);
	break;
      case c_IntensityDiffMax:
	params.intensityDiffMax = vals[1].i;
	_rtc.setParameters(params);
	break;
      default:
	ids = setOtherValues(vals);
	break;
    }
    
    return new Cmd::Values(ids);
}

template <class STEREO_RTC> Cmd::Values*
CmdSVC_impl<STEREO_RTC>::getValues(const Cmd::Values& ids,
				   CORBA::Boolean range)
{
#ifdef DEBUG
    print(std::cerr << "CmdSVC_impl<STEREO_RTC>::getValues(): ids =", ids);
#endif
    Cmd::Values	vals;
    vals.length(1);
    
    switch (ids[0].i)
    {
      case c_Binocular:
	vals[0] = {CORBA::Long(_rtc.getBinocular()), 0};
	break;
      case c_DisparitySearchWidth:
	vals[0] = {CORBA::Long(_rtc.getParameters().disparitySearchWidth), 0};
	break;
      case c_DisparityMax:
	vals[0] = {CORBA::Long(_rtc.getParameters().disparityMax), 0};
	break;
      case c_DisparityInconsistency:
	vals[0] = {CORBA::Long(_rtc.getParameters().disparityInconsistency), 0};
	break;
      case c_grainSize:
	vals[0] = {CORBA::Long(_rtc.getParameters().grainSize), 0};
	break;
      case c_WindowSize:
	vals[0] = {CORBA::Long(_rtc.getParameters().windowSize), 0};
	break;
      case c_IntensityDiffMax:
	vals[0] = {CORBA::Long(_rtc.getParameters().intensityDiffMax), 0};
	break;
      default:
	vals = getOtherValues(ids);
	break;
    }
#ifdef DEBUG
    print(std::cerr << ", vals =", vals) << std::endl;
#endif
    return new Cmd::Values(vals);
}

template <class STEREO_RTC> CmdDefs
CmdSVC_impl<STEREO_RTC>::createCmds()
{
    static CmdDefs cmds =
    {
	{CmdDef::C_ToggleButton, c_Binocular,	"Binocular",		0, 0},
	{CmdDef::C_Slider, c_WindowSize,	"Window size",		0, 1},
	{CmdDef::C_Slider, c_DisparitySearchWidth, "Disparity search width",
	 0, 2},
	{CmdDef::C_Slider, c_DisparityMax,	"Max. disparity",	0, 3},
	{CmdDef::C_Slider, c_DisparityInconsistency, "Disparity inconsistency",
	 0, 4},
	{CmdDef::C_Slider, c_IntensityDiffMax,  "Max. intensity diff.",	0, 5},
    };

    addOtherCmds(cmds);
    
    return cmds;
}

template <class STEREO_RTC> void
CmdSVC_impl<STEREO_RTC>::addOtherCmds(CmdDefs& cmds)
{
}
    
template <class STEREO_RTC> Cmd::Values
CmdSVC_impl<STEREO_RTC>::setOtherValues(const Cmd::Values& vals)
{
    return Cmd::Values();
}

template <class STEREO_RTC> Cmd::Values
CmdSVC_impl<STEREO_RTC>::getOtherValues(const Cmd::Values& ids) const
{
    return Cmd::Values();
}

}	// namespace v
}	// namespace TU
#endif	// !CMDSVC_IMPL_H
