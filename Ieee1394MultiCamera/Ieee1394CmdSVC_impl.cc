/*
 *  $Id$
 */
#include "TU/Ieee1394CameraArray.h"
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
    Ieee1394Camera::Format	format;
    const char*			name;
};
static const Format formats[] =
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

struct FrameRate
{
    Ieee1394Camera::FrameRate	frameRate;
    const char*			name;
};
static const FrameRate frameRates[] =
{
    {Ieee1394Camera::FrameRate_1_875,	"1.875fps"},
    {Ieee1394Camera::FrameRate_3_75,	"3.75fps"},
    {Ieee1394Camera::FrameRate_7_5,	"7.5fps"},
    {Ieee1394Camera::FrameRate_15,	"15fps"},
    {Ieee1394Camera::FrameRate_30,	"30fps"},
    {Ieee1394Camera::FrameRate_60,	"60fps"},
    {Ieee1394Camera::FrameRate_x,	"custom frame rate"}
};

struct PixelFormat
{
    Ieee1394Camera::PixelFormat	pixelFormat;
    const char*			name;
};
static const PixelFormat pixelFormats[] =
{
    {Ieee1394Camera::MONO_8,		"Y(mono)"},
    {Ieee1394Camera::YUV_411,		"YUV(4:1:1)"},
    {Ieee1394Camera::YUV_422,		"YUV(4:2:2)"},
    {Ieee1394Camera::YUV_444,		"YUV(4:4:4)"},
    {Ieee1394Camera::RGB_24,		"RGB"},
    {Ieee1394Camera::MONO_16,		"Y(mono16)"},
    {Ieee1394Camera::RGB_48,		"RGB(color48)"},
    {Ieee1394Camera::SIGNED_MONO_16,	"Y(signed mono16)"},
    {Ieee1394Camera::SIGNED_RGB_48,	"RGB(signed color48)"},
    {Ieee1394Camera::RAW_8,		"RAW(raw8)"},
    {Ieee1394Camera::RAW_16,		"RAW(raw16)"}
};

struct TriggerMode
{
    Ieee1394Camera::TriggerMode	triggerMode;
    const char*			name;
};
static const TriggerMode triggerModes[] =
{
    {Ieee1394Camera::Trigger_Mode0,	"mode0"},
    {Ieee1394Camera::Trigger_Mode1,	"mode1"},
    {Ieee1394Camera::Trigger_Mode2,	"mode2"},
    {Ieee1394Camera::Trigger_Mode3,	"mode3"},
    {Ieee1394Camera::Trigger_Mode4,	"mode4"},
    {Ieee1394Camera::Trigger_Mode5,	"mode5"},
    {Ieee1394Camera::Trigger_Mode14,	"mode14"},
};

struct Feature
{
    Ieee1394Camera::Feature	id;
    const char*			name;
};
static const Feature features[] =
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

/************************************************************************
*  class CmdSVC_impl<Ieee1394CameraArray>				*
************************************************************************/
template <> CmdDefs
CmdSVC_impl<Ieee1394CameraArray>::createFormatItems(camera_type& camera)
{
    CmdDefs	cmds;
    
    for (const auto& format : formats)
    {
      // Create submenu items for setting frame rate.
	u_int	inq = camera.inquireFrameRate(format.format);

	switch (format.format)
	{
	  case Ieee1394Camera::Format_7_0:
	  case Ieee1394Camera::Format_7_1:
	  case Ieee1394Camera::Format_7_2:
	  case Ieee1394Camera::Format_7_3:
	  case Ieee1394Camera::Format_7_4:
	  case Ieee1394Camera::Format_7_5:
	  case Ieee1394Camera::Format_7_6:
	  case Ieee1394Camera::Format_7_7:
	    if (inq & Ieee1394Camera::FrameRate_x)
	    {
		cmds.push_back(CmdDef(C_MenuItem, format.name, format.format));

		Ieee1394Camera::Format_7_Info
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
CmdSVC_impl<Ieee1394CameraArray>::addOtherCmds(const camera_type& camera,
					       CmdDefs& cmds)
{
    const size_t	NFEATURES = sizeof(features) / sizeof(features[0]);
    size_t		y = (cmds.empty() ? 0 : cmds.back().gridy + 1);
    
    for (size_t i = 0; i < NFEATURES; ++i)
    {
	const Feature&	feature = features[i];
	const u_int	inq = camera.inquireFeatureFunction(feature.id);
	
	if (inq & Ieee1394Camera::Presence)
	{
	    size_t	x = 0;
	    
	    if (feature.id == Ieee1394Camera::TRIGGER_MODE)
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
	    else if (inq & Ieee1394Camera::Manual)
	    {
		cmds.push_back(CmdDef());
		CmdDef&	cmd = cmds.back();

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
		    cmd.min = min;
		    cmd.max = max;
		    
		    if (feature.id == Ieee1394Camera::WHITE_BALANCE)
		    {
			cmds.push_back(CmdDef());
			CmdDef&	vrCmd = cmds.back();
			++i;	// skip feature
			++y;
			
			vrCmd.type  = C_Slider;
			vrCmd.name  = features[i].name;
			vrCmd.id    = features[i].id
				    + IEEE1394CAMERA_OFFSET_VR;
			vrCmd.min   = min;
			vrCmd.max   = max;
			vrCmd.gridx = 0;
			vrCmd.gridy = y;
		    }
		} // (inq & Ieee1394Camera::ReadOut)
		else
		{
		  // Create a label for setting on/off and manual/auto modes.
		    cmd.type  = C_Label;
		    cmd.attrs = CA_NoBorder;
		} // !(inq & Ieee1394Camera::ReadOut)
	    } // (inq & Ieee1394Camera::Manual)

	    if (inq & Ieee1394Camera::OnOff)
	    {
		cmds.push_back(CmdDef(C_ToggleButton, "On",
				      feature.id
				      + IEEE1394CAMERA_OFFSET_ONOFF));
		CmdDef&	onOffCmd = cmds.back();

		onOffCmd.gridx = x++;
		onOffCmd.gridy = y;
	    }

	    if (
		(inq & Ieee1394Camera::Auto))
	    {
		cmds.push_back(CmdDef(C_ToggleButton,
				      (feature.id ==
				       Ieee1394Camera::TRIGGER_MODE ?
				       "Polarity(+)" : "Auto"),
				      feature.id
				      + IEEE1394CAMERA_OFFSET_AUTO));
		CmdDef&	autoCmd = cmds.back();

		autoCmd.gridx = x;
		autoCmd.gridy = y;
	    }
		
	    ++y;
	} // (inq & Ieee1394Camera::Presence)
    }
}

template <> Cmd::Values
CmdSVC_impl<Ieee1394CameraArray>::getFormat(const Values& ids) const
{
    const Ieee1394CameraArray&	cameras = _rtc.cameras();
    Values			vals;
    
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
	Ieee1394Camera::Format	format = Ieee1394Camera::uintToFormat(ids[1]);
	
	switch (format)
	{
	  case Ieee1394Camera::Format_7_0:
	  case Ieee1394Camera::Format_7_1:
	  case Ieee1394Camera::Format_7_2:
	  case Ieee1394Camera::Format_7_3:
	  case Ieee1394Camera::Format_7_4:
	  case Ieee1394Camera::Format_7_5:
	  case Ieee1394Camera::Format_7_6:
	  case Ieee1394Camera::Format_7_7:
	  {
	      Ieee1394Camera::Format_7_Info
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
CmdSVC_impl<Ieee1394CameraArray>::getOtherValues(const Values& ids) const
{
    Values	vals;
    vals.length(1);
    vals[0] = TU::getFeatureValue(_rtc.cameras(), ids[0], _n);

    return vals;
}

}
}

