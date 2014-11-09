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
*  class CmdSVC_impl<CAMERAS>						*
************************************************************************/
template <class CAMERAS>
class CmdSVC_impl : public virtual POA_Cmd::Controller,
		    public virtual PortableServer::RefCountServantBase
{
  private:
    typedef typename CAMERAS::camera_type	camera_type;
    typedef Cmd::Values				Values;
    
    enum
    {
	c_ContinuousShot, c_Format, c_CameraChoice,
    };
    
  public:
			CmdSVC_impl(CAMERAS& cameras)			;
    virtual		~CmdSVC_impl()					;

    char*		getCmds()					;
    void		setValues(CORBA::ULong id, const Values& vals)	;
    Cmd::Values*	getValues(CORBA::ULong id)			;

  private:
    CmdDefs		createCmds()				const	;
    static CmdDefs	createFormatCmds(const camera_type& camera)	;
    static void		addFeatureCmds(const camera_type& camera,
				       CmdDefs& cmds)			;
    void		getFormat(CmdId id, Values& vals)	const	;
    void		setFormat(CmdId id, const Values& vals)		;
    
  private:
    CAMERAS&	_cameras;
    size_t	_n;		// currently selected camera #
};

template <class CAMERAS>
CmdSVC_impl<CAMERAS>::CmdSVC_impl(CAMERAS& cameras)
    :_cameras(cameras), _n(_cameras.size())
{
}

template <class CAMERAS>
CmdSVC_impl<CAMERAS>::~CmdSVC_impl()
{
}

template <class CAMERAS> char*
CmdSVC_impl<CAMERAS>::getCmds()
{
#ifdef DEBUG
    std::cerr << "CmdSVC_impl<CAMERAS>::getCmds() called!" << std::endl;
#endif
    std::ostringstream			oss;
    boost::archive::xml_oarchive	oar(oss);
    CmdDefs				cmds = createCmds();
    oar << BOOST_SERIALIZATION_NVP(cmds);
    
    return CORBA::string_dup(oss.str().c_str());
}

template <class CAMERAS> void
CmdSVC_impl<CAMERAS>::setValues(CORBA::ULong id, const Values& vals)
{
#ifdef DEBUG
    std::cerr << "CmdSVC_impl<CAMERAS>::setValues(): id = " << id
	      << ", vals =";
    for (CORBA::ULong i = 0; i < vals.length(); ++i)
	std::cerr << ' ' << vals[i];
    std::cerr << std::endl;
#endif
    switch (id)
    {
      case c_ContinuousShot:
	if (vals[0])
	    exec(_cameras, &camera_type::continuousShot);
	else
	    exec(_cameras, &camera_type::stopContinuousShot);
	break;
      case c_Format:
      //setFormat(vals);
	break;
      case c_CameraChoice:
	_n = vals[0];
	break;
      default:
	setCameraFeatureValue(_cameras, id, vals[0], _n);
	break;
    }
}

template <class CAMERAS> Cmd::Values*
CmdSVC_impl<CAMERAS>::getValues(CORBA::ULong id)
{
#ifdef DEBUG
    std::cerr << "CmdSVC_impl<CAMERAS>::getValues(): id = " << id;
#endif
    Values	vals;
    
    switch (id)
    {
      case c_ContinuousShot:
	vals.length(1);
	vals[0] = (_cameras.size() ? _cameras[0]->inContinuousShot() : 0);
	break;
      case c_Format:
      //getFormat(id, vals);
	break;
      case c_CameraChoice:
	vals.length(1);
	vals[0] = _n;
	break;
      default:
	vals.length(1);
	vals[0] = getCameraFeatureValue(_cameras, id, _n);
	break;
    }
#ifdef DEBUG
    std::cerr << ", vals =";
    for (CORBA::ULong i = 0; i < vals.length(); ++i)
	std::cerr << ' ' << vals[i];
    std::cerr << std::endl;
#endif
    return new Values(vals);
}

template <class CAMERAS> typename TU::v::CmdDefs
CmdSVC_impl<CAMERAS>::createCmds() const
{
    if (!_cameras.size())
	return CmdDefs();

    CmdDefs	cameraChoiceCmds;
    for (size_t i = 0; i < _cameras.size(); ++i)
	cameraChoiceCmds.push_back(CmdDef(C_RadioButton,
					  std::string("cam-") + char('0' + i),
					  i, noSub, 0, 1, 1, 1, CA_None,
					  i, 0, 1, 1, 0));
    cameraChoiceCmds.push_back(CmdDef(C_RadioButton, "all", _cameras.size(),
				      noSub, 0, 1, 1, 1, CA_None,
				      _cameras.size(), 0, 1, 1, 0));

    CmdDefs	cmds =
    {
	{C_ToggleButton, "Continuous shot", c_ContinuousShot, noSub,
	 0, 1, 1, 1, CA_None, 0, 0, 1, 1, 0},
	{C_Button, "Format", c_Format, createFormatCmds(*_cameras[0]),
	 0, 1, 1, 1, CA_None, 0, 1, 1, 1, 0},
	{C_GroupBox,  "", c_CameraChoice, cameraChoiceCmds,
	 0, int(_cameras.size()), 1, 1, CA_None, 0, 2, 1, 1, 0},
    };

    addFeatureCmds(*_cameras[0], cmds);

    return cmds;
}
    
}
}
#endif // CMDSVC_IMPL_H
