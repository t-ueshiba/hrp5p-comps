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
    typedef Cmd::IValues			IValues;
    typedef Cmd::IValues_out			IValues_out;
    typedef Cmd::FValues			FValues;
    typedef Cmd::FValues_out			FValues_out;
    
    enum
    {
	c_ContinuousShot, c_Format, c_CameraChoice,
    };
    
  public:
			CmdSVC_impl(CAMERAS& cameras)			;
    virtual		~CmdSVC_impl()					;

    char*		getCmds()					;
    void		getIValues(CORBA::ULong id, IValues_out vals)	;
    void		setIValues(CORBA::ULong id, IValues& vals)	;
    void		getFValues(CORBA::ULong id, FValues_out vals)	;
    void		setFValues(CORBA::ULong id, FValues& vals)	;

  private:
    CmdDefs		createCmds()				const	;
    static CmdDefs	createFormatCmds(const camera_type& camera)	;
    static void		addCmds(const camera_type& camera,
				CmdDefs& cmds)				;
    void		getFormat(IValues_out vals)		const	;
    void		setFormat(IValues& vals)			;
    void		getFeatureValue(IValues_out vals)	const	;
    void		setFeatureValue(IValues& vals)			;
    
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
CmdSVC_impl<CAMERAS>::getIValues(CORBA::ULong id, IValues_out vals)
{
    switch (id)
    {
      case c_ContinuousShot:
	vals[0] = (_cameras.size() ? _cameras[0]->inContinuousShot() : 0);
	break;
      case c_Format:
      //getFormat(vals);
	break;
      case c_CameraChoice:
	vals[0] = _n;
	break;
      default:
      //getFeatureValue(vals);
	break;
    }
}

template <class CAMERAS> void
CmdSVC_impl<CAMERAS>::setIValues(CORBA::ULong id, IValues& vals)
{
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
      //setFeatureValue(vals);
	break;
    }
}

template <class CAMERAS> void
CmdSVC_impl<CAMERAS>::getFValues(CORBA::ULong id, FValues_out vals)
{
}

template <class CAMERAS> void
CmdSVC_impl<CAMERAS>::setFValues(CORBA::ULong id, FValues& vals)
{
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
					  i, 0, noSub, 0, 1, 1, 1, CA_None,
					  i, 0, 1, 1, 0));
    cameraChoiceCmds.push_back(CmdDef(C_RadioButton, "all", _cameras.size(), 0,
				      noSub, 0, 1, 1, 1, CA_None,
				      _cameras.size(), 0, 1, 1, 0));

    CmdDefs	cmds =
    {
	{C_ToggleButton, "Continuous shot", c_ContinuousShot, 0, noSub,
	 0, 1, 1, 1, CA_None, 0, 0, 1, 1, 0},
	{C_MenuButton, "Format", c_Format, 0, createFormatCmds(*_cameras[0]),
	 0, 1, 1, 1, CA_None, 0, 1, 1, 1, 0},
	{C_ChoiceFrame,  "", c_CameraChoice, int(_cameras.size()),
	 cameraChoiceCmds, 0, int(_cameras.size()), 1, 1, CA_None,
	 0, 2, 1, 1, 0},
    };

    addCmds(*_cameras[0], cmds);

    return cmds;
}

}
}
#endif // CMDSVC_IMPL_H
