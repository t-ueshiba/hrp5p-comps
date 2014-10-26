/*
 *  $Id$
 */
#include "TU/Ieee1394++.h"
#include "CmdDef.h"

namespace TU
{
namespace v
{
/************************************************************************
*  local data								*
************************************************************************/
struct Feature
{
    Ieee1394Camera::Feature	id;
    const char*			name;
    int				prop[3];
};
static Feature	features[] =
{
    {Ieee1394Camera::TRIGGER_MODE,	"Trigger mode"	 },
    {Ieee1394Camera::BRIGHTNESS,	"Brightness"	 },
    {Ieee1394Camera::AUTO_EXPOSURE,	"Auto exposure"	 },
    {Ieee1394Camera::SHARPNESS,		"Sharpness"	 },
    {Ieee1394Camera::WHITE_BALANCE,	"White bal.(U/B)"},
    {Ieee1394Camera::WHITE_BALANCE,	"White bal.(V/R)"},
    {Ieee1394Camera::HUE,		"Hue"		 },
    {Ieee1394Camera::SATURATION,	"Saturation"	 },
    {Ieee1394Camera::GAMMA,		"Gamma"		 },
    {Ieee1394Camera::SHUTTER,		"Shutter"	 },
    {Ieee1394Camera::GAIN,		"Gain"		 },
    {Ieee1394Camera::IRIS,		"Iris"		 },
    {Ieee1394Camera::FOCUS,		"Focus"		 },
    {Ieee1394Camera::TEMPERATURE,	"Temperature"	 },
    {Ieee1394Camera::ZOOM,		"Zoom"		 },
  //{Ieee1394Camera::PAN,		"Pan"		 },
  //{Ieee1394Camera::TILT,		"Tilt"		 },
};
static const size_t	NFEATURES = sizeof(features) / sizeof(features[0]);

/************************************************************************
*  global functions							*
************************************************************************/
CmdDefs
createFeatureCmds(const Ieee1394Camera& camera)
{
    CmdDefs	cmds;
    size_t	y = 0;
    
    for (size_t i = 0; i < NFEATURES; ++i)
    {
	Feature&	feature = features[i];
	const u_int	inq = camera.inquireFeatureFunction(feature.id);
	
	if (inq & Ieee1394Camera::Presence)
	{
	    cmds.push_back(CmdDef());
	    CmdDef&	cmd = cmds.back();
	    size_t	x = 1;
	    
	    if (inq & Ieee1394Camera::OnOff)
	    {
		cmd.subcmds.push_back(CmdDef());
		CmdDef&	onOffCmd = cmd.subcmds.back();
		
	      // Create toggle button for turning on/off this feature.
		onOffCmd.type  = C_ToggleButton;
		onOffCmd.name  = "On";
		onOffCmd.id    = feature.id + IEEE1394CAMERA_OFFSET_ONOFF;
		onOffCmd.val   = camera.isTurnedOn(feature.id);
		onOffCmd.gridx = x++;
		onOffCmd.gridy = y;
	    }

	    if (feature.id == Ieee1394Camera::TRIGGER_MODE)
	    {
		cmd.type  = C_Label;
		cmd.name  = feature.name;
		cmd.id	  = feature.id;
		cmd.max   = 0;
		cmd.step  = 0;
		cmd.attrs = CA_NoBorder;
		cmd.gridx = 0;
		cmd.gridy = y;
	    }
	    else if (inq & Ieee1394Camera::Manual)
	    {
		if (inq & Ieee1394Camera::Auto)
		{
		    cmd.subcmds.push_back(CmdDef());
		    CmdDef&	autoCmd = cmd.subcmds.back();
		    
		  // Create toggle button for setting manual/auto mode.
		    autoCmd.type  = C_ToggleButton;
		    autoCmd.name  = "Auto";
		    autoCmd.id	  = feature.id + IEEE1394CAMERA_OFFSET_AUTO;
		    autoCmd.val	  = camera.isAuto(feature.id);
		    autoCmd.gridx = x;
		    autoCmd.gridy = y;
		} // (inq & Ieee1394Camera::Auto)

		cmd.name  = feature.name;
		cmd.id	  = feature.id;
		cmd.gridx = 0;
		cmd.gridy = y;
		
		if (inq & Ieee1394Camera::ReadOut)
		{
		  // Create sliders for setting values.
		    cmd.type  = C_Slider;

		    u_int	min, max;
		    camera.getMinMax(feature.id, min, max);
		    cmd.min = min;
		    cmd.max = max;

		    if (feature.id == Ieee1394Camera::WHITE_BALANCE)
		    {
			u_int	ub, vr;
			camera.getWhiteBalance(ub, vr);
			cmd.val = ub;
			
			cmds.push_back(CmdDef());
			CmdDef&	vrCmd = cmds.back();
			++i;	// skip feature
			++y;
			
			vrCmd.type  = C_Slider;
			vrCmd.name  = features[i].name;
			vrCmd.id    = features[i].id + 0x02;
			vrCmd.val   = vr;
			vrCmd.min   = min;
			vrCmd.max   = max;
			vrCmd.gridx = 0;
			vrCmd.gridy = y;
		    }
		    else
			cmd.val = camera.getValue(feature.id);
		} // (inq & Ieee1394Camera::ReadOut)
		else
		{
		  // Create a label for setting on/off and manual/auto modes.
		    cmd.type  = C_Label;
		    cmd.attrs = CA_NoBorder;
		} // !(inq & Ieee1394Camera::ReadOut)
	    } // (inq & Ieee1394Camera::Manual)

	    ++y;
	} // (inq & Ieee1394Camera::Presence)
    }

    return cmds;
}

}
}
