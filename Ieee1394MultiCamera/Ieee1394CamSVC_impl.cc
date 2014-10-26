/*
 *  $Id$
 */
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include "TU/Ieee1394CameraArray.h"
#include "CamSVC_impl.h"

namespace TU
{
namespace v
{
CmdDefs	createFormatCmds (const Ieee1394Camera& camera)			;
CmdDefs	createFeatureCmds(const Ieee1394Camera& camera)			;
}
}

/************************************************************************
*  class CamSVC_impl<TU::Ieee1394CameraArray>				*
************************************************************************/
template <>
CamSVC_impl<TU::Ieee1394CameraArray>::~CamSVC_impl()
{
}

template <> void
CamSVC_impl<TU::Ieee1394CameraArray>::setROI(CORBA::ULong& u0,
					     CORBA::ULong& v0,
					     CORBA::ULong& width,
					     CORBA::ULong& height)
{
    if (_cameras.size())
    {
	TU::Ieee1394Camera::Format	format = _cameras[0]->getFormat();
	
	switch (format)
	{
	  case TU::Ieee1394Camera::Format_7_0:
	  case TU::Ieee1394Camera::Format_7_1:
	  case TU::Ieee1394Camera::Format_7_2:
	  case TU::Ieee1394Camera::Format_7_3:
	  case TU::Ieee1394Camera::Format_7_4:
	  case TU::Ieee1394Camera::Format_7_5:
	  case TU::Ieee1394Camera::Format_7_6:
	  case TU::Ieee1394Camera::Format_7_7:
	  {
	    if (_n < _cameras.size())
		_cameras[_n]->setFormat_7_ROI(format, u0, v0, width, height);
	    else
		for (size_t i = 0; i < _cameras.size(); ++i)
		    _cameras[i]->setFormat_7_ROI(format, u0, v0, width, height);

	    TU::Ieee1394Camera::Format_7_Info
		info = _cameras[0]->getFormat_7_Info(format);
	    u0	   = info.u0;
	    v0	   = info.u0;
	    width  = info.width;
	    height = info.height;
	  }
	    break;

	  default:
	    u0 = v0 = 0;
	    width  = _cameras[0]->width();
	    height = _cameras[0]->height();
	    break;
	}
    }
    else
	u0 = v0 = width = height = 0;
}

template <> void
CamSVC_impl<TU::Ieee1394CameraArray>::getROI(CORBA::ULong& u0,
					     CORBA::ULong& v0,
					     CORBA::ULong& width,
					     CORBA::ULong& height)
{
    u0 = v0 = width = height = 0;

    if (_cameras.size())
    {
	width  = _cameras[0]->width();
	height = _cameras[0]->height();
	
	TU::Ieee1394Camera::Format	format = _cameras[0]->getFormat();
	switch (format)
	{
	  case TU::Ieee1394Camera::Format_7_0:
	  case TU::Ieee1394Camera::Format_7_1:
	  case TU::Ieee1394Camera::Format_7_2:
	  case TU::Ieee1394Camera::Format_7_3:
	  case TU::Ieee1394Camera::Format_7_4:
	  case TU::Ieee1394Camera::Format_7_5:
	  case TU::Ieee1394Camera::Format_7_6:
	  case TU::Ieee1394Camera::Format_7_7:
	  {
	    TU::Ieee1394Camera::Format_7_Info
		info = _cameras[0]->getFormat_7_Info(format);
	    u0	   = info.u0;
	    v0	   = info.u0;
	    width  = info.width;
	    height = info.height;
	  }
	    break;
	}
    }
}

template <> CORBA::ULong
CamSVC_impl<TU::Ieee1394CameraArray>::getFormat()
{
    return (_cameras.size() ? _cameras[0]->getFormat() : 0);
}

template <> CORBA::ULong
CamSVC_impl<TU::Ieee1394CameraArray>::setFormat(CORBA::ULong format)
{
    if (_cameras.size())
    {
    }
    
    return 0;
}

// End of example implementational code



