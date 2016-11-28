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
			  0, y++, 1, 1, 0, CmdDefs(),
			  0, 0, 0, 1));
    size_t	minU0, minV0, maxWidth, maxHeight;
    camera.getROILimits(minU0, minV0, maxWidth, maxHeight);
    auto&	roiCmds = cmds.back().subcmds;
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_U0, "u0",
			     0, 0, 1, 1, 0, CmdDefs(),
			     minU0, maxWidth - 1));
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_V0, "v0",
			     0, 1, 1, 1, 0, CmdDefs(),
			     minV0, maxHeight - 1));
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_Width, "width",
			     0, 2, 1, 1, 0, CmdDefs(),
			     0, maxWidth));
    roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_Height, "height",
			     0, 3, 1, 1, 0, CmdDefs(),
			     0, maxHeight));
    
  // featureを指定するコマンドを作る．
    BOOST_FOREACH (auto feature, camera.availableFeatures())
    {
	cmds.push_back(CmdDef());
	auto&	cmd = cmds.back();
	
	cmd.name  = camera.getName(feature);
	cmd.id	  = feature;
	cmd.gridy = y++;
	
	const auto	menuItems = camera.availableMenuItems(feature);

	if (menuItems.first == menuItems.second)
	{
	    int	min, max, step;

	    camera.getMinMaxStep(feature, min, max, step);
	    cmd.min  = min;
	    cmd.max  = max;
	    cmd.step = step;

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
CmdSVC_impl<V4L2CameraArray>::getFormat(const Cmd::Values& ids) const
{
    Cmd::Values	vals;
    
    if (size(_rtc.cameras()) == 0)
	return vals;

    const auto&	camera = *std::begin(_rtc.cameras());
    const auto	pixelFormat = camera.pixelFormat();
    int		i = 0;
    BOOST_FOREACH (const auto& frameSize,
		   camera.availableFrameSizes(pixelFormat))
    {
	if (frameSize.width.involves(camera.width()) &&
	    frameSize.height.involves(camera.height()))
	{
	    vals.length(2);
	    vals[0] = pixelFormat;
	    vals[1] = i;

	    break;
	}

	++i;
    }

    return vals;
}

template <> bool
CmdSVC_impl<V4L2CameraArray>::setFeature(const Cmd::Values& vals)
{
    _rtc.setFeature(vals, _n, _all);

    return false;	// GUIの更新が必要な属性はなし
}

template <> Cmd::Values
CmdSVC_impl<V4L2CameraArray>::getFeature(const Cmd::Values& ids) const
{
    Cmd::Values	vals;

    if (size(_rtc.cameras()) == 0)
	return vals;

    auto	camera = std::begin(_rtc.cameras());

    if (ids[0] == V4L2Camera::UNKNOWN_PIXEL_FORMAT)
    {
	size_t	u0, v0, width, height;
	camera->getROI(u0, v0, width, height);

	vals.length(4);
	vals[0] = u0;
	vals[1] = v0;
	vals[2] = width;
	vals[3] = height;
    }
    else
    {
	std::advance(camera, _n);

	int	val;
	TU::getFeature(*camera, ids[0], val);
	vals.length(1);
	vals[0] = val;
    }

    return vals;
}

}
}
