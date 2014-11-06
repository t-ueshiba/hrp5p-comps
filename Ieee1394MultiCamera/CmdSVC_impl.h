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
    typedef Cmd::Values_out			Values_out;
    
    enum
    {
	c_ContinuousShot, c_Format, c_CameraChoice,
    };
    
  public:
			CmdSVC_impl(CAMERAS& cameras)			;
    virtual		~CmdSVC_impl()					;

    char*		getCmds()					;
    void		getValues(Values_out vals)			;
    void		setValues(Values& vals)				;

  private:
    CmdDefs		createCmds()				const	;
    static CmdDefs	createFormatCmds(const camera_type& camera)	;
    static void		addFeatureCmds(const camera_type& camera,
				       CmdDefs& cmds)			;
    void		getFormat(Values_out vals)		const	;
    void		setFormat(Values& vals)				;
    void		getFeatureValue(Values_out vals)	const	;
    void		setFeatureValue(Values& vals)			;
    
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
CmdSVC_impl<CAMERAS>::getValues(Values_out vals)
{
    switch (vals[0])
    {
      case c_ContinuousShot:
	vals[1] = (_cameras.size() ? _cameras[0]->inContinuousShot() : 0);
	break;
      case c_Format:
      //getFormat(vals);
	break;
      case c_CameraChoice:
	vals[1] = _n;
	break;
      default:
      //getFeatureValue(vals);
	break;
    }
}

template <class CAMERAS> void
CmdSVC_impl<CAMERAS>::setValues(Values& vals)
{
#ifdef DEBUG
    std::cerr << "CmdSVC_impl<CAMERAS>::setValues() called!" << std::endl;
#endif
    switch (vals[0])
    {
      case c_ContinuousShot:
	if (vals[1])
	    exec(_cameras, &camera_type::continuousShot);
	else
	    exec(_cameras, &camera_type::stopContinuousShot);
	break;
      case c_Format:
      //setFormat(vals);
	break;
      case c_CameraChoice:
	_n = vals[1];
	break;
      default:
	handleCameraFeatures(_cameras, vals[0], vals[1], _n);
	break;
    }
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

    addFeatureCmds(*_cameras[0], cmds);

    return cmds;
}

template <class CAMERAS> void
CmdSVC_impl<CAMERAS>::getFeatureValue(Values_out vals) const
{
}
    
}
}
#endif // CMDSVC_IMPL_H
