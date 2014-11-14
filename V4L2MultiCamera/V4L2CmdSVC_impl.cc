/*
 *  $Id$
 */
#include <boost/foreach.hpp>
#include "TU/V4L2CameraArray.h"
#include "CmdSVC_impl.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class CmdSVC_impl<V4L2CameraArray>					*
************************************************************************/
template <> CmdDefs
CmdSVC_impl<V4L2CameraArray>::createFormatCmds(const camera_type& camera)
{
    CmdDefs	cmds;

    BOOST_FOREACH (V4L2Camera::PixelFormat pixelFormat,
		   camera.availablePixelFormats())
    {
      // この画素フォーマットに対応するメニュー項目を作る．
	cmds.push_back(CmdDef(C_MenuItem,
			      camera.getName(pixelFormat), pixelFormat));
	CmdDef&	cmd = cmds.back();

      // この画素フォーマットがサポートする各フレームサイズに対応するメニュー項目を作る．
	BOOST_FOREACH (const V4L2Camera::FrameSize& frameSize,
		       camera.availableFrameSizes(pixelFormat))
	{
	    std::ostringstream	s;
	    s << frameSize;
	    cmd.subcmds.push_back(CmdDef(C_MenuItem,
					 s.str(), cmd.subcmds.size()));
	}
    }

  // ROIを指定する項目を作る．
    cmds.push_back(CmdDef(C_MenuItem,
			  "set ROI", V4L2Camera::UNKNOWN_PIXEL_FORMAT));
    size_t	minU0, minV0, maxWidth, maxHeight;
    camera.getROILimits(minU0, minV0, maxWidth, maxHeight);
    CmdDefs&	roiCmds = cmds.back().subcmds;
    roiCmds.push_back(CmdDef(C_Slider,
			     "u0", c_U0, noSub, minU0, maxWidth  - minU0 - 1));
    roiCmds.push_back(CmdDef(C_Slider,
			     "v0", c_V0, noSub, minV0, maxHeight - minV0 - 1));
    roiCmds.push_back(CmdDef(C_Slider,
			     "width",  c_Width,  noSub, 0, maxWidth));
    roiCmds.push_back(CmdDef(C_Slider,
			     "height", c_Height, noSub, 0, maxHeight));
    
    return cmds;
}

template <> void
CmdSVC_impl<V4L2CameraArray>::addExtraCmds(const camera_type& camera,
					   CmdDefs& cmds)
{
    size_t	y = (cmds.empty() ? 0 : cmds.back().gridy + 1);

    BOOST_FOREACH (V4L2Camera::Feature feature, camera.availableFeatures())
    {
	cmds.push_back(CmdDef());
	CmdDef&	cmd = cmds.back();
	
	cmd.name  = camera.getName(feature);
	cmd.id	  = feature;
	cmd.gridy = y++;
	
	V4L2Camera::MenuItemRange
	    menuItems = camera.availableMenuItems(feature);

	if (menuItems.first == menuItems.second)
	{
	    int	min, max, step;

	    camera.getMinMaxStep(feature, min, max, step);
	    cmd.min  = min;
	    cmd.max  = max;
	    cmd.step = step;

	    if (min == 0 && max == 1)
		cmd.type = C_ToggleButton;
	    else
		cmd.type = C_Slider;
	}
	else
	{
	    cmd.type = C_Button;

	    BOOST_FOREACH (const V4L2Camera::MenuItem& menuItem, menuItems)
	    {
		cmd.subcmds.push_back(CmdDef(C_MenuItem,
					     menuItem.name, menuItem.index));
	    }
	}
    }
}
    
template <> void
CmdSVC_impl<V4L2CameraArray>::getFormat(const V4L2CameraArray& cameras,
					Values& vals)
{
    if (cameras.size() == 0)
    {
	vals.length(0);
	return;
    }

    const V4L2Camera&		camera = *cameras[0];
    V4L2Camera::PixelFormat	pixelFormat = camera.pixelFormat();
    int				i = 0;
    BOOST_FOREACH (const V4L2Camera::FrameSize& frameSize,
		   camera.availableFrameSizes(pixelFormat))
    {
	if (frameSize.width.involves(camera.width()) &&
	    frameSize.height.involves(camera.height()))
	{
	    vals.length(6);
	    vals[0] = pixelFormat;
	    vals[1] = i;

	    size_t	u0, v0, width, height;
	    camera.getROI(u0, v0, width, height);
	    vals[2] = u0;
	    vals[3] = v0;
	    vals[4] = width;
	    vals[5] = height;
	    
	    return;
	}

	++i;
    }
}

}
}
