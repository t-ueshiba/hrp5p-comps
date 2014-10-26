/*
 *  $Id$
 */
#include <boost/archive/xml_oarchive.hpp>
#include "TU/V4L2CameraArray.h"
#include "CamSVC_impl.h"

namespace TU
{
namespace v
{
CmdDefs	createFormatCmds (const V4L2Camera& camera)			;
CmdDefs	createFeatureCmds(const V4L2Camera& camera)			;
}
}

/************************************************************************
*  class CamSVC_impl<TU::V4L2CameraArray>				*
************************************************************************/
template <>
CamSVC_impl<TU::V4L2CameraArray>::~CamSVC_impl()
{
}

template <> void
CamSVC_impl<TU::V4L2CameraArray>::setROI(CORBA::ULong& u0,
					 CORBA::ULong& v0,
					 CORBA::ULong& width,
					 CORBA::ULong& height)
{
    if (_cameras.size())
    {
	if (_n < _cameras.size())
	    _cameras[_n]->setROI(u0, v0, width, height);
	else
	    for (size_t i = 0; i < _cameras.size(); ++i)
		_cameras[i]->setROI(u0, v0, width, height);

	getROI(u0, v0, width, height);
    }
    else
	u0 = v0 = width = height = 0;
}

template <> void
CamSVC_impl<TU::V4L2CameraArray>::getROI(CORBA::ULong& u0,
					 CORBA::ULong& v0,
					 CORBA::ULong& width,
					 CORBA::ULong& height)
{
    u0 = v0 = width = height = 0;

    if (_cameras.size())
    {
	size_t	u0_, v0_, width_, height_;
	_cameras[0]->getROI(u0_, v0_, width_, height_);

	u0     = u0_;
	v0     = v0_;
	width  = width_;
	height = height_;
    }
}

template <> CORBA::ULong
CamSVC_impl<TU::V4L2CameraArray>::getFormat()
{
    return (_cameras.size() ? _cameras[0]->pixelFormat() : 0);
}

template <> CORBA::ULong
CamSVC_impl<TU::V4L2CameraArray>::setFormat(CORBA::ULong format)
{
    if (_cameras.size())
    {
    }
    
    return 0;
}

// End of example implementational code



