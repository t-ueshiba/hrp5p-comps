/*
 *  $Id$
 */
#include <sstream>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include "BasicDataType.hh"
#include "Cam.hh"
#include "CmdDef.h"

#ifndef CAMSVC_IMPL_H
#define CAMSVC_IMPL_H
 
/************************************************************************
*  class CamSVC_impl<CAMERAS>						*
************************************************************************/
template <class CAMERAS>
class CamSVC_impl : public virtual POA_Cam::Controller,
		    public virtual PortableServer::RefCountServantBase
{
  public:
    typedef typename CAMERAS::camera_type	camera_type;
    
  public:
			CamSVC_impl(CAMERAS& cameras)
			    :_cameras(cameras), _n(_cameras.size())	{}
    virtual		~CamSVC_impl()					;

   // attributes and operations
    CORBA::ULong	ncameras()					;
    void		selectCamera(CORBA::ULong n)			;
    CORBA::ULong	selectedCamera()				;
    void		setROI(CORBA::ULong& u0,
			       CORBA::ULong& v0,
			       CORBA::ULong& width,
			       CORBA::ULong& height)			;
    void		getROI(CORBA::ULong& u0,
			       CORBA::ULong& v0,
			       CORBA::ULong& width,
			       CORBA::ULong& height)			;
    char*		availableFormats()				;
    CORBA::ULong	getFormat()					;
    CORBA::ULong	setFormat(CORBA::ULong format)			;
    char*		availableFeatures()				;
    CORBA::Long		getValue(CORBA::ULong feature)			;
    CORBA::Long		setValue(CORBA::ULong feature, CORBA::Long val)	;

  private:
    CAMERAS&	_cameras;
    size_t	_n;		// currently selected camera #
};

template <class CAMERAS> CORBA::ULong
CamSVC_impl<CAMERAS>::ncameras()
{
    return _cameras.size();
}

template <class CAMERAS> void
CamSVC_impl<CAMERAS>::selectCamera(CORBA::ULong n)
{
    _n = n;
}

template <class CAMERAS> CORBA::ULong
CamSVC_impl<CAMERAS>::selectedCamera()
{
    return _n;
}

template <class CAMERAS> char*
CamSVC_impl<CAMERAS>::availableFormats()
{
    using namespace	TU::v;

    CmdDefs	cmds;
    if (_cameras.size())
	cmds = createFormatCmds(*_cameras[0]);

    std::ostringstream			oss;
    boost::archive::xml_oarchive	oar(oss);
    oar << BOOST_SERIALIZATION_NVP(cmds);
  //oss.flush();
    
    return CORBA::string_dup(oss.str().c_str());
}

template <class CAMERAS> char*
CamSVC_impl<CAMERAS>::availableFeatures()
{
    using namespace	TU::v;

    CmdDefs	cmds;
    if (_cameras.size())
	cmds = createFeatureCmds(*_cameras[0]);

    std::ostringstream			oss;
    boost::archive::xml_oarchive	oar(oss);
    oar << BOOST_SERIALIZATION_NVP(cmds);
  //oss.flush();
    
    return CORBA::string_dup(oss.str().c_str());
}

template <class CAMERAS> CORBA::Long
CamSVC_impl<CAMERAS>::getValue(CORBA::ULong feature)
{
    return _cameras[_n < _cameras.size() ? _n : 0]
		->getValue(camera_type::uintToFeature(feature));
}

template <class CAMERAS> CORBA::Long
CamSVC_impl<CAMERAS>::setValue(CORBA::ULong feature, CORBA::Long val)
{
    handleCameraFeatures(_cameras, feature, val, _n);
    return getValue(feature);
}

#endif // CAMSVC_IMPL_H
