/*
 *  $Id$
 */
#include <boost/foreach.hpp>
#include "TU/V4L2CameraArray.h"
#include "MultiCameraRTC.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class CmdSVC_impl<V4L2CameraArray>					*
************************************************************************/
template <> CmdDefs
CmdSVC_impl<V4L2CameraArray>::createFormatItems(const camera_type& camera)
{
    CmdDefs	cmds;

    BOOST_FOREACH (auto pixelFormat, camera.availablePixelFormats())
    {
      // この画素フォーマットに対応するメニュー項目を作る．
	cmds.push_back(CmdDef(CmdDef::C_MenuItem,
			      pixelFormat, camera.getName(pixelFormat)));

      // この画素フォーマットがサポートする各フレームサイズに対応するメニュー項目を作る．
	auto&	subcmds = cmds.back().subcmds;
	BOOST_FOREACH (const auto& frameSize,
		       camera.availableFrameSizes(pixelFormat))
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
CmdSVC_impl<V4L2CameraArray>::appendFeatureCmds(const camera_type& camera,
						CmdDefs& cmds)
{
    size_t	y = (cmds.empty() ? 0 : cmds.back().gridy + 1);

  // ROIを指定するコマンドを作る．
    cmds.push_back(CmdDef(CmdDef::C_Button,
			  V4L2Camera::UNKNOWN_PIXEL_FORMAT, "set ROI",
			  0, y++, 2, 1));
    auto&	roiCmds = cmds.back().subcmds;
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_U0, "u0", 0, 0));
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_V0, "v0", 0, 1));
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_Width, "width", 0, 2));
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_Height, "height", 0, 3));
    
  // featureを指定するコマンドを作る．
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
    const auto	camera = std::begin(_rtc.cameras());
    const auto	pixelFormat = camera->pixelFormat();
    int		i = 0;
    BOOST_FOREACH (const auto& frameSize,
		   camera->availableFrameSizes(pixelFormat))
    {
	if (range)
	{
	}
	else
	{
	    if (frameSize.width.involves(camera->width()) &&
		frameSize.height.involves(camera->height()))
	    {
		vals.length(2);
		vals[0].i = pixelFormat;
		vals[1].i = i;

		break;
	    }
	}

	++i;
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
    Cmd::Values	vals;
    auto	camera = std::begin(_rtc.cameras());
    std::advance(camera, _n);

    if (range)
    {
	if (ids[0].i == V4L2Camera::UNKNOWN_PIXEL_FORMAT)
	{
	    size_t	minU0, minV0, maxWidth, maxHeight;
	    camera->getROILimits(minU0, minV0, maxWidth, maxHeight);

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
    }
    else
    {
	if (ids[0].i == V4L2Camera::UNKNOWN_PIXEL_FORMAT)
	{
	    size_t	u0, v0, width, height;
	    camera->getROI(u0, v0, width, height);

	    vals.length(4);
	    vals[0] = {0, float(u0)};
	    vals[1] = {0, float(v0)};
	    vals[2] = {0, float(width)};
	    vals[3] = {0, float(height)};
	}
	else
	{
	    int	val;
	    TU::getFeature(*camera, ids[0].i, val);

	    vals.length(1);
	    vals[0] = {CORBA::Long(val), 0};
	}
    }

    return vals;
}
    
}
}
