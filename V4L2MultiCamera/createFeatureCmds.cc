/*
 *  $Id$
 */
#include <boost/foreach.hpp>
#include "TU/V4L2++.h"
#include "CmdDef.h"

namespace TU
{
namespace v
{
/************************************************************************
*  global functions							*
************************************************************************/
CmdDefs
createFeatureCmds(const V4L2Camera& camera)
{
    CmdDefs	cmds;
    size_t	y = 0;
    
    BOOST_FOREACH (V4L2Camera::Feature feature, camera.availableFeatures())
    {
	cmds.push_back(CmdDef());
	CmdDef&	cmd = cmds.back();
	
	cmd.name  = camera.getName(feature).c_str();
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

		choiceCmd.name = menuItem.name.c_str();
		choiceCmd.id   = menuItem.index;
		choiceCmd.val  = (i == cmd.val);

		++i;
	    }
	}

	++y;
    }

    return cmds;
}

}
}
