/*
 *  $Id$
 */
#include <boost/foreach.hpp>
#include "TU/V4L2CameraArray.h"
#include "Img.hh"
#include "CameraRTC.h"

namespace TU
{
/************************************************************************
*  class CameraRTCBase<V4L2CameraArray>					*
************************************************************************/
template <> void
CameraRTCBase<V4L2CameraArray>::setFormat(const Cmd::Values& vals)
{
    std::lock_guard<std::mutex>	lock(_mutex);

    TU::setFormat(_cameras, vals[1].i, vals[2].i);
    allocateImages();
}
    
template <> bool
CameraRTCBase<V4L2CameraArray>::setFeature(const Cmd::Values& vals,
					   size_t n, bool all)
{
    if (vals[0].i == V4L2Camera::UNKNOWN_PIXEL_FORMAT)
    {
	std::lock_guard<std::mutex>	lock(_mutex);
    
	for (auto& camera : _cameras)
	    camera.setROI(vals[1].i, vals[2].i, vals[3].i, vals[4].i);
	allocateImages();

	return true;
    }
    else if (all)
	return TU::setFeature(_cameras, vals[0].i, vals[1].i);
    else
	return TU::setFeature(_cameras[n], vals[0].i, vals[1].i);
}

namespace v
{
/************************************************************************
*  class CmdSVC_impl<V4L2CameraArray, Img::TimedImages>			*
************************************************************************/
template <> CmdDefs
CmdSVC_impl<V4L2CameraArray>::createFormatItems() const
{
    const auto	camera = _rtc.cameras().begin();
    CmdDefs	cmds;

    BOOST_FOREACH (auto pixelFormat, camera->availablePixelFormats())
    {
      // この画素フォーマットに対応するメニュー項目を作る．
	cmds.push_back(CmdDef(CmdDef::C_MenuItem,
			      pixelFormat, camera->getName(pixelFormat)));

      // この画素フォーマットがサポートする各フレームサイズに対応するメニュー項目を作る．
	auto&	subcmds = cmds.back().subcmds;
	BOOST_FOREACH (const auto& frameSize,
		       camera->availableFrameSizes(pixelFormat))
	{
	    std::ostringstream	s;
	    s << frameSize;
	    subcmds.push_back(CmdDef(CmdDef::C_MenuItem,
				     subcmds.size(), s.str()));
	}
    }

    return cmds;
}

template <> void
CmdSVC_impl<V4L2CameraArray>::appendFeatureCmds(CmdDefs& cmds) const
{
    size_t	y = (cmds.empty() ? 0 : cmds.back().gridy + 1);

  // ROIを指定するコマンドを作る．
    cmds.push_back(CmdDef(CmdDef::C_Button,
			  V4L2Camera::UNKNOWN_PIXEL_FORMAT, "set ROI",
			  0, y++, 2, 1));
    auto&	roiCmds = cmds.back().subcmds;
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_U0,	 "u0",	   0, 0));
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_V0,	 "v0",	   0, 1));
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_Width,	 "width",  0, 2));
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_Height, "height", 0, 3));
    
  // featureを指定するコマンドを作る．
    const auto&	camera = _rtc.cameras()[0];
    BOOST_FOREACH (auto feature, camera.availableFeatures())
    {
	cmds.push_back(CmdDef());
	auto&	cmd = cmds.back();
	
	cmd.name      = camera.getName(feature);
	cmd.id	      = feature;
	cmd.gridy     = y++;
	cmd.gridWidth = 2;
	
	const auto	menuItems = camera.availableMenuItems(feature);

	if (menuItems.first == menuItems.second)
	{
	    int	min, max, step;

	    camera.getMinMaxStep(feature, min, max, step);

	    if (min == 0 && max == 1)
		cmd.type = CmdDef::C_ToggleButton;
	    else
		cmd.type = CmdDef::C_Slider;
	}
	else
	{
	    cmd.type = CmdDef::C_Button;

	    BOOST_FOREACH (const auto& menuItem, menuItems)
	    {
		cmd.subcmds.push_back(CmdDef(CmdDef::C_MenuItem,
					     menuItem.index, menuItem.name));
	    }
	}
    }
}
    
template <> Cmd::Values
CmdSVC_impl<V4L2CameraArray>::getFormat(const Cmd::Values& ids,
					CORBA::Boolean range) const
{
    Cmd::Values	vals;

    if (!range)
    {
	const auto&	camera = _rtc.cameras()[0];
	const auto	pixelFormat = camera.pixelFormat();
	int		i = 0;
	BOOST_FOREACH (const auto& frameSize,
		       camera.availableFrameSizes(pixelFormat))
	{
	    if (frameSize.width.involves(camera.width()) &&
		frameSize.height.involves(camera.height()))
	    {
		vals.length(2);
		vals[0].i = pixelFormat;
		vals[1].i = i;

		break;
	    }

	    ++i;
	}
    }

    return vals;
}

template <> Cmd::Values
CmdSVC_impl<V4L2CameraArray>::setFeature(const Cmd::Values& vals)
{
    _rtc.setFeature(vals, _n, _all);
    return Cmd::Values();	// GUIの更新が必要な属性はなし
}

template <> Cmd::Values
CmdSVC_impl<V4L2CameraArray>::getFeature(const Cmd::Values& ids,
					 CORBA::Boolean range) const
{
    const auto&	camera = _rtc.cameras()[_n];
    Cmd::Values	vals;

    if (ids[0].i == V4L2Camera::UNKNOWN_PIXEL_FORMAT)
    {
	if (range)
	{
	    size_t	minU0, minV0, maxWidth, maxHeight;
	    camera.getROILimits(minU0, minV0, maxWidth, maxHeight);

	    vals.length(12);
	    vals[ 0] = {0, float(minU0)};
	    vals[ 1] = {0, float(maxWidth - 1)};
	    vals[ 2] = {0, 1};
	    vals[ 3] = {0, float(minV0)};
	    vals[ 4] = {0, float(maxHeight - 1)};
	    vals[ 5] = {0, 1};
	    vals[ 6] = {0, 0};
	    vals[ 7] = {0, float(maxWidth)};
	    vals[ 8] = {0, 1};
	    vals[ 9] = {0, 0};
	    vals[10] = {0, float(maxHeight)};
	    vals[11] = {0, 1};
	}
	else
	{
	    size_t	u0, v0, width, height;
	    camera.getROI(u0, v0, width, height);

	    vals.length(4);
	    vals[0] = {0, float(u0)};
	    vals[1] = {0, float(v0)};
	    vals[2] = {0, float(width)};
	    vals[3] = {0, float(height)};
	}
    }
    else
    {
	const auto	feature = V4L2Camera::uintToFeature(ids[0].i);
	
	if (range)
	{
	    int	min, max, step;
	    camera.getMinMaxStep(feature, min, max, step);

	    vals.length(3);
	    vals[0] = {0, float(min)};
	    vals[1] = {0, float(max)};
	    vals[2] = {0, float(step)};
	}
	else
	{
	    vals.length(1);
	    vals[0] = {0, float(camera.getValue(feature))};
	}
    }

    return vals;
}
    
}
}
