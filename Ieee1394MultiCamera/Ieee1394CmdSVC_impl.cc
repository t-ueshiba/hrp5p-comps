/*
 *  $Id$
 */
#include "TU/Ieee1394CameraArray.h"
#include "CmdSVC_impl.h"

namespace TU
{
namespace v
{
/************************************************************************
*  local data								*
************************************************************************/
struct Format
{
    Ieee1394Camera::Format	format;
    const char*			name;
    
};
static Format formats[] =
{
    {Ieee1394Camera::YUV444_160x120,	"160x120-YUV(4:4:4)"},
    {Ieee1394Camera::YUV422_320x240,	"320x240-YUV(4:2:2)"},
    {Ieee1394Camera::YUV411_640x480,	"640x480-YUV(4:1:1)"},
    {Ieee1394Camera::YUV422_640x480,	"640x480-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_640x480,	"640x480-RGB"},
    {Ieee1394Camera::MONO8_640x480,	"640x480-Y(mono)"},
    {Ieee1394Camera::MONO16_640x480,	"640x480-Y(mono16)"},
    {Ieee1394Camera::YUV422_800x600,	"800x600-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_800x600,	"800x600-RGB"},
    {Ieee1394Camera::MONO8_800x600,	"800x600-Y(mono)"},
    {Ieee1394Camera::YUV422_1024x768,	"1024x768-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_1024x768,	"1024x768-RGB"},
    {Ieee1394Camera::MONO8_1024x768,	"1024x768-Y(mono)"},
    {Ieee1394Camera::MONO16_800x600,	"800x600-Y(mono16)"},
    {Ieee1394Camera::MONO16_1024x768,	"1024x768-Y(mono16)"},
    {Ieee1394Camera::YUV422_1280x960,	"1280x960-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_1280x960,	"1280x960-RGB"},
    {Ieee1394Camera::MONO8_1280x960,	"1280x960-Y(mono)"},
    {Ieee1394Camera::YUV422_1600x1200,	"1600x1200-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_1600x1200,	"1600x1200-RGB"},
    {Ieee1394Camera::MONO8_1600x1200,	"1600x1200-Y(mono)"},
    {Ieee1394Camera::MONO16_1280x960,	"1280x960-Y(mono16)"},
    {Ieee1394Camera::MONO16_1600x1200,	"1600x1200-Y(mono16)"},
    {Ieee1394Camera::Format_7_0,	"Format_7_0"},
    {Ieee1394Camera::Format_7_1,	"Format_7_1"},
    {Ieee1394Camera::Format_7_2,	"Format_7_2"},
    {Ieee1394Camera::Format_7_3,	"Format_7_3"},
    {Ieee1394Camera::Format_7_4,	"Format_7_4"},
    {Ieee1394Camera::Format_7_5,	"Format_7_5"},
    {Ieee1394Camera::Format_7_6,	"Format_7_6"},
    {Ieee1394Camera::Format_7_6,	"Format_7_7"}
};
static const size_t	NFORMATS = sizeof(formats)/sizeof(formats[0]);

struct FrameRate
{
    Ieee1394Camera::FrameRate	frameRate;
    const char*			name;
};
static FrameRate frameRates[] =
{
    {Ieee1394Camera::FrameRate_1_875,	"1.875fps"},
    {Ieee1394Camera::FrameRate_3_75,	"3.75fps"},
    {Ieee1394Camera::FrameRate_7_5,	"7.5fps"},
    {Ieee1394Camera::FrameRate_15,	"15fps"},
    {Ieee1394Camera::FrameRate_30,	"30fps"},
    {Ieee1394Camera::FrameRate_60,	"60fps"},
    {Ieee1394Camera::FrameRate_x,	"custom frame rate"}
};
static const size_t	NRATES = sizeof(frameRates)/sizeof(frameRates[0]);

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
*  class CmdSVC_impl<Ieee1394CameraArray>				*
************************************************************************/
template <> CmdDefs
CmdSVC_impl<Ieee1394CameraArray>::createFormatCmds(const camera_type& camera)
{
    CmdDefs	cmds;
    
    Ieee1394Camera::Format	current_format = camera.getFormat();
    Ieee1394Camera::FrameRate	current_rate   = camera.getFrameRate();

    for (size_t i = 0; i < NFORMATS; ++i)
    {
	u_int	inq = camera.inquireFrameRate(formats[i].format);
	CmdDefs	rateCmds;
	
	for (size_t j = 0; j < NRATES; ++j)
	{
	    if (inq & frameRates[j].frameRate)
	    {
	      // Create submenu items for setting frame rate.
		rateCmds.push_back(CmdDef());
		CmdDef&	rateCmd = rateCmds.back();
		
		rateCmd.type = C_MenuItem;
		rateCmd.name = frameRates[j].name;
		rateCmd.id   = frameRates[j].frameRate;
		rateCmd.val  = ((current_format == formats[i].format) &&
				(current_rate   == frameRates[j].frameRate));
	    }
	}
	
	if (!rateCmds.empty())
	{
	  // Create menu items for setting format.
	    cmds.push_back(CmdDef());
	    CmdDef&	cmd = cmds.back();

	    cmd.type	= C_MenuItem;
	    cmd.name	= formats[i].name;
	    cmd.id	= formats[i].format;
	    cmd.subcmds = rateCmds;
	}
    }

    return cmds;
}

template <> void
CmdSVC_impl<Ieee1394CameraArray>::addCmds(const camera_type& camera,
					  CmdDefs& cmds)
{
    size_t	y = (cmds.empty() ? 0 : cmds.back().gridy + 1);
    
    for (size_t i = 0; i < NFEATURES; ++i)
    {
	const Feature&	feature = features[i];
	const u_int	inq = camera.inquireFeatureFunction(feature.id);
	
	if (inq & Ieee1394Camera::Presence)
	{
	    if (feature.id == Ieee1394Camera::TRIGGER_MODE)
	    {
		cmds.push_back(CmdDef(C_Label, feature.name, feature.id));
		CmdDef&	cmd = cmds.back();
	    
		cmd.attrs = CA_NoBorder;
		cmd.gridx = 0;
		cmd.gridy = y;
	    }
	    else if (inq & Ieee1394Camera::Manual)
	    {
		cmds.push_back(CmdDef());
		CmdDef&	cmd = cmds.back();
		size_t	x   = 0;

		cmd.name  = feature.name;
		cmd.id	  = feature.id;
		cmd.gridx = x++;
		cmd.gridy = y;
		
		if (inq & Ieee1394Camera::ReadOut)
		{
		  // Create sliders for setting values.
		    cmd.type  = C_Slider;

		    u_int	min, max;
		    camera.getMinMax(feature.id, min, max);
		    cmd.min  = min;
		    cmd.max  = max;
		    
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

		if (inq & Ieee1394Camera::OnOff)
		{
		    cmds.push_back(CmdDef(C_ToggleButton, "On",
					  feature.id
					  + IEEE1394CAMERA_OFFSET_ONOFF,
					  camera.isTurnedOn(feature.id)));
		    CmdDef&	onOffCmd = cmds.back();

		    onOffCmd.gridx = x++;
		    onOffCmd.gridy = y;
		}

		if (inq & Ieee1394Camera::Auto)
		{
		    cmds.push_back(CmdDef(C_ToggleButton, "Auto",
					  feature.id
					  + IEEE1394CAMERA_OFFSET_AUTO,
					  camera.isAuto(feature.id)));
		    CmdDef&	autoCmd = cmds.back();

		    autoCmd.gridx = x;
		    autoCmd.gridy = y;
		}
	    } // (inq & Ieee1394Camera::Manual)

	    ++y;
	} // (inq & Ieee1394Camera::Presence)
    }
}

}
}

