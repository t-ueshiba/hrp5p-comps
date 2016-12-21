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
*  static functions							*
************************************************************************/
static void
setSliderCmd(const IIDCCamera& camera, CmdDef& cmd)
{
    const auto	feature = (cmd.id == IIDCCamera::WHITE_BALANCE
				   + IIDCCAMERA_OFFSET_VR	?
			   IIDCCamera::WHITE_BALANCE		:
			   IIDCCamera::uintToFeature(cmd.id));
    
    if (camera.isAbsControl(feature))
    {
	float	min, max;
	camera.getMinMax(feature, min, max);
	cmd.min  = min;
	cmd.max  = max;
	cmd.step = 0;
    }
    else
    {
	u_int	min, max;
	camera.getMinMax(feature, min, max);
	cmd.min  = min;
	cmd.max  = max;
	cmd.step = 1;
    }
}
    
/************************************************************************
*  class CmdSVC_impl<IIDCCameraArray>				*
************************************************************************/
template <> CmdDefs
CmdSVC_impl<IIDCCameraArray>::createFormatItems(const camera_type& camera)
{
    CmdDefs	cmds;
    
    for (const auto& format : IIDCCamera::formatNames)
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
		cmds.push_back(CmdDef(CmdDef::C_MenuItem,
				      format.format, format.name));

		IIDCCamera::Format_7_Info
				info = camera.getFormat_7_Info(format.format);
		CmdDefs&	roiCmds = cmds.back().subcmds;
		roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_U0, "u0",
					 0, 0, 1, 1, 0, CmdDefs(),
					 0, info.maxWidth - 1, info.unitU0));
		roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_V0, "v0",
					 0, 1, 1, 1, 0, CmdDefs(),
					 0, info.maxHeight - 1, info.unitV0));
		roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_Width, "width",
					 0, 2, 1, 1, 0, CmdDefs(),
					 0, info.maxWidth, info.unitWidth));
		roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_Height, "height",
					 0, 3, 1, 1, 0, CmdDefs(),
					 0, info.maxHeight, info.unitHeight));
		roiCmds.push_back(CmdDef(CmdDef::C_Button, c_PixelFormat,
					 "pixel format", 0, 4));

		CmdDefs&	pixFmtCmds = roiCmds.back().subcmds;
		for (const auto& pixelFormat : IIDCCamera::pixelFormatNames)
		    if (info.availablePixelFormats & pixelFormat.pixelFormat)
			pixFmtCmds.push_back(CmdDef(CmdDef::C_MenuItem,
						    pixelFormat.pixelFormat,
						    pixelFormat.name));
	    }
	    break;

	  default:
	  {
	    CmdDefs	rateCmds;
	    for (const auto& frameRate : IIDCCamera::frameRateNames)
		if (inq & frameRate.frameRate)
		    rateCmds.push_back(CmdDef(CmdDef::C_MenuItem,
					      frameRate.frameRate,
					      frameRate.name));

	  // Create menu items for setting format.	
	    if (!rateCmds.empty())
		cmds.push_back(CmdDef(CmdDef::C_MenuItem, format.format,
				      format.name, 0, 0, 1, 1, 0, rateCmds));
	  }
	    break;
	} // switch (format.format)
    }

    return cmds;
}

template <> void
CmdSVC_impl<IIDCCameraArray>::appendFeatureCmds(const camera_type& camera,
						CmdDefs& cmds)
{
    size_t	y = (cmds.empty() ? 0 : cmds.back().gridy + 1);
    
    for (const auto& feature : IIDCCamera::featureNames)
    {
	const u_int	inq = camera.inquireFeatureFunction(feature.feature);
	
	if (!((inq & IIDCCamera::Presence) &&
	      (inq & IIDCCamera::Manual)   &&
	      (inq & IIDCCamera::ReadOut)))
	    continue;


	cmds.push_back(CmdDef(CmdDef::C_Slider, feature.feature, feature.name,
			      0, y, 2, 1));
	CmdDef&	cmd = cmds.back();

	switch (feature.feature)
	{
	  case IIDCCamera::TRIGGER_MODE:
	    cmd.type = CmdDef::C_Button;
	    for (const auto& triggerMode : IIDCCamera::triggerModeNames)
		if (inq & triggerMode.triggerMode)
		    cmd.subcmds.push_back(CmdDef(CmdDef::C_MenuItem,
						 triggerMode.triggerMode,
						 triggerMode.name));
	    break;

	  case IIDCCamera::WHITE_BALANCE:
	    setSliderCmd(camera, cmd);
	    cmds.push_back(CmdDef(CmdDef::C_Slider,
				  feature.feature + IIDCCAMERA_OFFSET_VR,
				  "White bal.(V/R)", 0, ++y, 2, 1));
	    setSliderCmd(camera, cmds.back());
	    break;

	  default:
	    setSliderCmd(camera, cmd);
	    break;
	}	// switch (feature.feature)

	if (inq & IIDCCamera::OnOff)
	    cmds.push_back(CmdDef(CmdDef::C_ToggleButton,
				  feature.feature + IIDCCAMERA_OFFSET_ONOFF,
				  "On", 2, y));

	if (inq & IIDCCamera::Auto)
	    cmds.push_back(CmdDef(CmdDef::C_ToggleButton,
				  feature.feature + IIDCCAMERA_OFFSET_AUTO,
				  (feature.feature == IIDCCamera::TRIGGER_MODE ?
				   "(+)" : "Auto"), 3, y));

	if (inq & IIDCCamera::Abs_Control)
	    cmds.push_back(CmdDef(CmdDef::C_ToggleButton,
				  feature.feature + IIDCCAMERA_OFFSET_ABS,
				  "Abs.", 4, y));

	++y;
    }
}

template <> Cmd::Values
CmdSVC_impl<IIDCCameraArray>::getFormat(const Cmd::Values& ids) const
{
    Cmd::Values	vals;
    const auto	camera = std::begin(_rtc.cameras());
    
    if (camera == std::end(_rtc.cameras()))
	return vals;

    
    if (ids.length() == 1)
    {
	vals.length(2);
	vals[0].i = camera->getFormat();
	vals[1].i = camera->getFrameRate();
    }
    else if (ids.length() > 1)
    {
	IIDCCamera::Format	format = IIDCCamera::uintToFormat(ids[1].i);
	
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
	      IIDCCamera::Format_7_Info	info = camera->getFormat_7_Info(format);
	      vals.length(5);
	      vals[0].i = info.u0;
	      vals[1].i = info.v0;
	      vals[2].i = info.width;
	      vals[3].i = info.height;
	      vals[4].i = info.pixelFormat;
	  }
	    break;

	  default:
	    vals.length(1);
	    vals[0].i = camera->getFrameRate();
	    break;
	}
    }

    return vals;
}

template <> bool
CmdSVC_impl<IIDCCameraArray>::setFeature(const Cmd::Values& vals)
{
    return (_rtc.setFeature(vals, _n, _all) &&
	    (vals[0].i >= IIDCCamera::BRIGHTNESS + IIDCCAMERA_OFFSET_ABS));
}

template <> Cmd::Values
CmdSVC_impl<IIDCCameraArray>::getFeature(const Cmd::Values& ids) const
{
    Cmd::Values	vals;
    auto	camera = std::begin(_rtc.cameras());
    
    if (camera == std::end(_rtc.cameras()))
	return vals;

    std::advance(camera, _n);

    u_int	val;
    float	fval;
    TU::getFeature(*camera, ids[0].i, val, fval);
    vals.length(1);
    vals[0].i = val;
    vals[0].f = fval;

    return vals;
}
    
}
}

