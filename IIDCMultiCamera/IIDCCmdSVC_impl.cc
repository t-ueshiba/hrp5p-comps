/*
 *  $Id$
 */
#include "TU/IIDCCameraArray.h"
#include "MultiCameraRTC.h"

namespace TU
{
namespace v
{
/************************************************************************
*  local data								*
************************************************************************/
struct Format
{
    IIDCCamera::Format	format;
    const char*			name;
};
static const Format formats[] =
{
    {IIDCCamera::YUV444_160x120,	"160x120-YUV(4:4:4)"},
    {IIDCCamera::YUV422_320x240,	"320x240-YUV(4:2:2)"},
    {IIDCCamera::YUV411_640x480,	"640x480-YUV(4:1:1)"},
    {IIDCCamera::YUV422_640x480,	"640x480-YUV(4:2:2)"},
    {IIDCCamera::RGB24_640x480,		"640x480-RGB"},
    {IIDCCamera::MONO8_640x480,		"640x480-Y(mono)"},
    {IIDCCamera::MONO16_640x480,	"640x480-Y(mono16)"},
    {IIDCCamera::YUV422_800x600,	"800x600-YUV(4:2:2)"},
    {IIDCCamera::RGB24_800x600,		"800x600-RGB"},
    {IIDCCamera::MONO8_800x600,		"800x600-Y(mono)"},
    {IIDCCamera::YUV422_1024x768,	"1024x768-YUV(4:2:2)"},
    {IIDCCamera::RGB24_1024x768,	"1024x768-RGB"},
    {IIDCCamera::MONO8_1024x768,	"1024x768-Y(mono)"},
    {IIDCCamera::MONO16_800x600,	"800x600-Y(mono16)"},
    {IIDCCamera::MONO16_1024x768,	"1024x768-Y(mono16)"},
    {IIDCCamera::YUV422_1280x960,	"1280x960-YUV(4:2:2)"},
    {IIDCCamera::RGB24_1280x960,	"1280x960-RGB"},
    {IIDCCamera::MONO8_1280x960,	"1280x960-Y(mono)"},
    {IIDCCamera::YUV422_1600x1200,	"1600x1200-YUV(4:2:2)"},
    {IIDCCamera::RGB24_1600x1200,	"1600x1200-RGB"},
    {IIDCCamera::MONO8_1600x1200,	"1600x1200-Y(mono)"},
    {IIDCCamera::MONO16_1280x960,	"1280x960-Y(mono16)"},
    {IIDCCamera::MONO16_1600x1200,	"1600x1200-Y(mono16)"},
    {IIDCCamera::Format_7_0,		"Format_7_0"},
    {IIDCCamera::Format_7_1,		"Format_7_1"},
    {IIDCCamera::Format_7_2,		"Format_7_2"},
    {IIDCCamera::Format_7_3,		"Format_7_3"},
    {IIDCCamera::Format_7_4,		"Format_7_4"},
    {IIDCCamera::Format_7_5,		"Format_7_5"},
    {IIDCCamera::Format_7_6,		"Format_7_6"},
    {IIDCCamera::Format_7_6,		"Format_7_7"}
};

struct FrameRate
{
    IIDCCamera::FrameRate	frameRate;
    const char*			name;
};
static const FrameRate frameRates[] =
{
    {IIDCCamera::FrameRate_1_875,	"1.875fps"},
    {IIDCCamera::FrameRate_3_75,	"3.75fps"},
    {IIDCCamera::FrameRate_7_5,		"7.5fps"},
    {IIDCCamera::FrameRate_15,		"15fps"},
    {IIDCCamera::FrameRate_30,		"30fps"},
    {IIDCCamera::FrameRate_60,		"60fps"},
    {IIDCCamera::FrameRate_x,		"custom frame rate"}
};

struct PixelFormat
{
    IIDCCamera::PixelFormat	pixelFormat;
    const char*			name;
};
static const PixelFormat pixelFormats[] =
{
    {IIDCCamera::MONO_8,		"Y(mono)"},
    {IIDCCamera::YUV_411,		"YUV(4:1:1)"},
    {IIDCCamera::YUV_422,		"YUV(4:2:2)"},
    {IIDCCamera::YUV_444,		"YUV(4:4:4)"},
    {IIDCCamera::RGB_24,		"RGB"},
    {IIDCCamera::MONO_16,		"Y(mono16)"},
    {IIDCCamera::RGB_48,		"RGB(color48)"},
    {IIDCCamera::SIGNED_MONO_16,	"Y(signed mono16)"},
    {IIDCCamera::SIGNED_RGB_48,		"RGB(signed color48)"},
    {IIDCCamera::RAW_8,			"RAW(raw8)"},
    {IIDCCamera::RAW_16,		"RAW(raw16)"}
};

struct TriggerMode
{
    IIDCCamera::TriggerMode	triggerMode;
    const char*			name;
};
static const TriggerMode triggerModes[] =
{
    {IIDCCamera::Trigger_Mode0,		"mode0"},
    {IIDCCamera::Trigger_Mode1,		"mode1"},
    {IIDCCamera::Trigger_Mode2,		"mode2"},
    {IIDCCamera::Trigger_Mode3,		"mode3"},
    {IIDCCamera::Trigger_Mode4,		"mode4"},
    {IIDCCamera::Trigger_Mode5,		"mode5"},
    {IIDCCamera::Trigger_Mode14,	"mode14"},
};

struct Feature
{
    IIDCCamera::Feature	id;
    const char*			name;
};
static const Feature features[] =
{
    {IIDCCamera::TRIGGER_MODE,		"Trigger mode"	 },
    {IIDCCamera::BRIGHTNESS,		"Brightness"	 },
    {IIDCCamera::AUTO_EXPOSURE,		"Auto exposure"	 },
    {IIDCCamera::SHARPNESS,		"Sharpness"	 },
    {IIDCCamera::WHITE_BALANCE,		"White bal.(U/B)"},
    {IIDCCamera::WHITE_BALANCE,		"White bal.(V/R)"},
    {IIDCCamera::HUE,			"Hue"		 },
    {IIDCCamera::SATURATION,		"Saturation"	 },
    {IIDCCamera::GAMMA,			"Gamma"		 },
    {IIDCCamera::SHUTTER,		"Shutter"	 },
    {IIDCCamera::GAIN,			"Gain"		 },
    {IIDCCamera::IRIS,			"Iris"		 },
    {IIDCCamera::FOCUS,			"Focus"		 },
    {IIDCCamera::TEMPERATURE,		"Temperature"	 },
    {IIDCCamera::ZOOM,			"Zoom"		 },
  //{IIDCCamera::PAN,			"Pan"		 },
  //{IIDCCamera::TILT,			"Tilt"		 },
};

/************************************************************************
*  class CmdSVC_impl<IIDCCameraArray>				*
************************************************************************/
template <> CmdDefs
CmdSVC_impl<IIDCCameraArray>::createFormatItems(camera_type& camera)
{
    CmdDefs	cmds;
    
    for (const auto& format : formats)
    {
      // Create submenu items for setting frame rate.
	u_int	inq = camera.inquireFrameRate(format.format);

	switch (format.format)
	{
	  case IIDCCamera::Format_7_0:
	  case IIDCCamera::Format_7_1:
	  case IIDCCamera::Format_7_2:
	  case IIDCCamera::Format_7_3:
	  case IIDCCamera::Format_7_4:
	  case IIDCCamera::Format_7_5:
	  case IIDCCamera::Format_7_6:
	  case IIDCCamera::Format_7_7:
	    if (inq & IIDCCamera::FrameRate_x)
	    {
		cmds.push_back(CmdDef(C_MenuItem, format.name, format.format));

		IIDCCamera::Format_7_Info
				info = camera.getFormat_7_Info(format.format);
		CmdDefs&	roiCmds = cmds.back().subcmds;
		roiCmds.push_back(CmdDef(C_Slider, "u0", c_U0, noSub,
					 0, info.maxWidth - 1, info.unitU0, 1,
					 CA_None, 0, 0));
		roiCmds.push_back(CmdDef(C_Slider, "v0", c_V0, noSub,
					 0, info.maxHeight - 1, info.unitV0, 1,
					 CA_None, 0, 1));
		roiCmds.push_back(CmdDef(C_Slider, "width", c_Width, noSub,
					 0, info.maxWidth, info.unitWidth, 1,
					 CA_None, 0, 2));
		roiCmds.push_back(CmdDef(C_Slider, "height", c_Height, noSub,
					 0, info.maxHeight, info.unitHeight, 1,
					 CA_None, 0, 3));
		roiCmds.push_back(CmdDef(C_Button, "pixel format",
					 c_PixelFormat, noSub,
					 0, 0, 0, 1, CA_None, 0, 4));

		CmdDefs&	pixFmtCmds = roiCmds.back().subcmds;
		for (const auto& pixelFormat : pixelFormats)
		    if (info.availablePixelFormats & pixelFormat.pixelFormat)
			pixFmtCmds.push_back(CmdDef(C_MenuItem,
						    pixelFormat.name,
						    pixelFormat.pixelFormat));
	    }
	    break;

	  default:
	  {
	    CmdDefs	rateCmds;
	    for (const auto& frameRate : frameRates)
		if (inq & frameRate.frameRate)
		    rateCmds.push_back(CmdDef(C_MenuItem, frameRate.name,
					      frameRate.frameRate));

	  // Create menu items for setting format.	
	    if (!rateCmds.empty())
		cmds.push_back(CmdDef(C_MenuItem, format.name,
				      format.format, rateCmds));
	  }
	    break;
	} // switch (format.format)
    }

    return cmds;
}

template <> void
CmdSVC_impl<IIDCCameraArray>::addOtherCmds(const camera_type& camera,
					   CmdDefs& cmds)
{
    const size_t	NFEATURES = sizeof(features) / sizeof(features[0]);
    size_t		y = (cmds.empty() ? 0 : cmds.back().gridy + 1);
    
    for (size_t i = 0; i < NFEATURES; ++i)
    {
	const Feature&	feature = features[i];
	const u_int	inq = camera.inquireFeatureFunction(feature.id);
	
	if (inq & IIDCCamera::Presence)
	{
	    size_t	x = 0;
	    
	    if (feature.id == IIDCCamera::TRIGGER_MODE)
	    {
		cmds.push_back(CmdDef(C_Button, feature.name, feature.id));
		CmdDef&	cmd = cmds.back();
	    
		cmd.gridx = x++;
		cmd.gridy = y;
		
		for (const auto& triggerMode : triggerModes)
		    if (inq & triggerMode.triggerMode)
			cmd.subcmds.push_back(CmdDef(C_MenuItem,
						     triggerMode.name,
						     triggerMode.triggerMode));
	    }
	    else if (inq & IIDCCamera::Manual)
	    {
		cmds.push_back(CmdDef());
		CmdDef&	cmd = cmds.back();

		cmd.name  = feature.name;
		cmd.id	  = feature.id;
		cmd.gridx = x++;
		cmd.gridy = y;
		
		if (inq & IIDCCamera::ReadOut)
		{
		  // Create sliders for setting values.
		    cmd.type  = C_Slider;

		    u_int	min, max;
		    camera.getMinMax(feature.id, min, max);
		    cmd.min = min;
		    cmd.max = max;
		    
		    if (feature.id == IIDCCamera::WHITE_BALANCE)
		    {
			cmds.push_back(CmdDef());
			CmdDef&	vrCmd = cmds.back();
			++i;	// skip feature
			++y;
			
			vrCmd.type  = C_Slider;
			vrCmd.name  = features[i].name;
			vrCmd.id    = features[i].id
				    + IIDCCAMERA_OFFSET_VR;
			vrCmd.min   = min;
			vrCmd.max   = max;
			vrCmd.gridx = 0;
			vrCmd.gridy = y;
		    }
		} // (inq & IIDCCamera::ReadOut)
		else
		{
		  // Create a label for setting on/off and manual/auto modes.
		    cmd.type  = C_Label;
		    cmd.attrs = CA_NoBorder;
		} // !(inq & IIDCCamera::ReadOut)
	    } // (inq & IIDCCamera::Manual)

	    if (inq & IIDCCamera::OnOff)
	    {
		cmds.push_back(CmdDef(C_ToggleButton, "On",
				      feature.id
				      + IIDCCAMERA_OFFSET_ONOFF));
		CmdDef&	onOffCmd = cmds.back();

		onOffCmd.gridx = x++;
		onOffCmd.gridy = y;
	    }

	    if (
		(inq & IIDCCamera::Auto))
	    {
		cmds.push_back(CmdDef(C_ToggleButton,
				      (feature.id ==
				       IIDCCamera::TRIGGER_MODE ?
				       "Polarity(+)" : "Auto"),
				      feature.id
				      + IIDCCAMERA_OFFSET_AUTO));
		CmdDef&	autoCmd = cmds.back();

		autoCmd.gridx = x;
		autoCmd.gridy = y;
	    }
		
	    ++y;
	} // (inq & IIDCCamera::Presence)
    }
}

template <> Cmd::Values
CmdSVC_impl<IIDCCameraArray>::getFormat(const Cmd::Values& ids) const
{
    const IIDCCameraArray&	cameras = _rtc.cameras();
    Cmd::Values			vals;
    
    if (cameras.size() == 0)
	return vals;

    if (ids.length() == 1)
    {
	vals.length(2);
	vals[0] = cameras[0]->getFormat();
	vals[1] = cameras[0]->getFrameRate();
    }
    else if (ids.length() > 1)
    {
	IIDCCamera::Format	format = IIDCCamera::uintToFormat(ids[1]);
	
	switch (format)
	{
	  case IIDCCamera::Format_7_0:
	  case IIDCCamera::Format_7_1:
	  case IIDCCamera::Format_7_2:
	  case IIDCCamera::Format_7_3:
	  case IIDCCamera::Format_7_4:
	  case IIDCCamera::Format_7_5:
	  case IIDCCamera::Format_7_6:
	  case IIDCCamera::Format_7_7:
	  {
	      IIDCCamera::Format_7_Info
		  info = cameras[0]->getFormat_7_Info(format);
	      vals.length(5);
	      vals[0] = info.u0;
	      vals[1] = info.v0;
	      vals[2] = info.width;
	      vals[3] = info.height;
	      vals[4] = info.pixelFormat;
	  }
	    break;

	  default:
	    vals.length(1);
	    vals[0] = cameras[0]->getFrameRate();
	    break;
	}
    }

    return vals;
}

template <> Cmd::Values
CmdSVC_impl<IIDCCameraArray>::getOtherValues(const Cmd::Values& ids) const
{
    Cmd::Values	vals;
    vals.length(1);
    vals[0] = TU::getFeatureValue(_rtc.cameras(), ids[0], _n);

    return vals;
}

}
}

