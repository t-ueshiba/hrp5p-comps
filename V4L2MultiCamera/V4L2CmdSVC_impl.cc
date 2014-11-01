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
*  class CamSVC_impl<V4L2CameraArray>					*
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
	    cmd.subcmds.push_back(CmdDef(C_MenuItem));
	    CmdDef&	fsizeCmd = cmd.subcmds.back();

	    std::ostringstream	s;
	    s << frameSize;
	    fsizeCmd.name = s.str();
	    fsizeCmd.id   = cmd.subcmds.size() - 1;
	    fsizeCmd.val  = (camera.pixelFormat() == pixelFormat	&&
			     frameSize.width .involves(camera.width())  &&
			     frameSize.height.involves(camera.height()));
	}
    }

  // ROIを指定する項目を作る．
    cmds.push_back(CmdDef(C_MenuItem,
			  "set ROI", V4L2Camera::UNKNOWN_PIXEL_FORMAT));

    return cmds;
}

template <> void
CmdSVC_impl<V4L2CameraArray>::addCmds(const camera_type& camera, CmdDefs& cmds)
{
    size_t	y = (cmds.empty() ? 0 : cmds.back().gridy + 1);

    BOOST_FOREACH (V4L2Camera::Feature feature, camera.availableFeatures())
    {
	cmds.push_back(CmdDef());
	CmdDef&	cmd = cmds.back();
	
	cmd.name  = camera.getName(feature);
	cmd.id	  = feature;
	cmd.val   = camera.getValue(feature);
	cmd.gridy = y;
	
	V4L2Camera::MenuItemRange
	    menuItems = camera.availableMenuItems(feature);

	if (menuItems.first == menuItems.second)
	{
	    int	min, max, step;
	    camera.getMinMaxStep(feature, min, max, step);

	    if (min == 0 && max == 1)
		cmd.type = C_ToggleButton;
	    else
	    {
		cmd.type = C_Slider;
		cmd.min  = min;
		cmd.max  = max;
		cmd.step = step;
	    }
	}
	else
	{
	    cmd.type = C_ChoiceMenuButton;

	    int	i = 0;
	    BOOST_FOREACH (const V4L2Camera::MenuItem& menuItem, menuItems)
	    {
		cmd.subcmds.push_back(CmdDef());
		CmdDef&	choiceCmd = cmd.subcmds.back();

		choiceCmd.name = menuItem.name;
		choiceCmd.id   = menuItem.index;
		choiceCmd.val  = (i == cmd.val);

		++i;
	    }
	}

	++y;
    }
}
    
}
}
