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
*  class CmdSVC_impl<IIDCCameraArray>					*
************************************************************************/
template <> CmdDefs
CmdSVC_impl<IIDCCameraArray>::createFormatItems(const camera_type& camera)
{
    CmdDefs	cmds;
    
    for (const auto& format : IIDCCamera::formatNames)
    {
      // Create submenu items for setting frame rate.
	const auto	inq = camera.inquireFrameRate(format.format);

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

		const auto	info = camera.getFormat_7_Info(format.format);
		auto&		roiCmds = cmds.back().subcmds;
		roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_U0, "u0", 0, 0));
		roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_V0, "v0", 0, 1));
		roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_Width, "width",
					 0, 2));
		roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_Height, "height",
					 0, 3));
		roiCmds.push_back(CmdDef(CmdDef::C_Slider, c_PacketSize,
					 "packet size",
					 0, 4));
		roiCmds.push_back(CmdDef(CmdDef::C_Button, c_PixelFormat,
					 "pixel format", 0, 5));

		auto&		pixFmtCmds = roiCmds.back().subcmds;
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
	const auto	inq = camera.inquireFeatureFunction(feature.feature);
	
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
	    cmds.push_back(CmdDef(CmdDef::C_Slider,
				  feature.feature + IIDCCAMERA_OFFSET_VR,
				  "White bal.(V/R)", 0, ++y, 2, 1));
	    break;

	  default:
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
CmdSVC_impl<IIDCCameraArray>::getFormat(const Cmd::Values& ids,
					CORBA::Boolean range) const
{
    Cmd::Values	vals;
    const auto	camera = std::begin(_rtc.cameras());
    
    if (ids.length() == 1)
    {
	vals.length(2);
	vals[0] = {CORBA::Long(camera->getFormat()), 0};
	vals[1] = {CORBA::Long(camera->getFrameRate()), 0};
    }
    else if (ids.length() > 1)
    {
	const auto	format = IIDCCamera::uintToFormat(ids[1].i);
	
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
	      const auto	info = camera->getFormat_7_Info(format);

	      if (range)
	      {
		  vals.length(15);
		  vals[ 0] = {0, 0};
		  vals[ 1] = {0, float(info.maxWidth)};
		  vals[ 2] = {0, float(info.unitU0)};
		  vals[ 3] = {0, 0};
		  vals[ 4] = {0, float(info.maxHeight)};
		  vals[ 5] = {0, float(info.unitV0)};
		  vals[ 6] = {0, 0};
		  vals[ 7] = {0, float(info.maxWidth)};
		  vals[ 8] = {0, float(info.unitWidth)};
		  vals[ 9] = {0, 0};
		  vals[10] = {0, float(info.maxHeight)};
		  vals[11] = {0, float(info.unitHeight)};
		  vals[12] = {0, float(info.unitBytePerPacket)};
		  vals[13] = {0, float(info.maxBytePerPacket)};
		  vals[14] = {0, float(info.unitBytePerPacket)};
	      }
	      else
	      {
		  vals.length(6);
		  vals[0] = {0, float(info.u0)};
		  vals[1] = {0, float(info.v0)};
		  vals[2] = {0, float(info.width)};
		  vals[3] = {0, float(info.height)};
		  vals[4] = {0, float(info.bytePerPacket)};
		  vals[5] = {CORBA::Long(info.pixelFormat), 0};
	      }
	  }
	    break;

	  default:
	    vals.length(1);
	    vals[0] = {CORBA::Long(camera->getFrameRate()), 0};
	    break;
	}
    }

    return vals;
}

template <> Cmd::Values
CmdSVC_impl<IIDCCameraArray>::setFeature(const Cmd::Values& vals)
{
    Cmd::Values	ids;
    
    if (_rtc.setFeature(vals, _n, _all) &&
	(vals[0].i >= IIDCCamera::BRIGHTNESS + IIDCCAMERA_OFFSET_ABS))
    {
	ids.length(1);
	ids[0] = {CORBA::Long(vals[0].i - IIDCCAMERA_OFFSET_ABS), 0};
    }

    return ids;
}

template <> Cmd::Values
CmdSVC_impl<IIDCCameraArray>::getFeature(const Cmd::Values& ids,
					 CORBA::Boolean range) const
{
    Cmd::Values	vals;
    auto	camera = std::begin(_rtc.cameras());
    std::advance(camera, _n);

    if (range)
    {
	const auto	feature = (ids[0].i == IIDCCamera::WHITE_BALANCE
					     + IIDCCAMERA_OFFSET_VR	?
				   IIDCCamera::WHITE_BALANCE		:
				   IIDCCamera::uintToFeature(ids[0].i));

	vals.length(3);
	if (camera->isAbsControl(feature))
	{
	    float	min, max;
	    camera->getMinMax(feature, min, max);
	    vals[0] = {0, min};
	    vals[1] = {0, max};
	    vals[2] = {0, 0};
	}
	else
	{
	    u_int	min, max;
	    camera->getMinMax(feature, min, max);
	    vals[0] = {0, float(min)};
	    vals[1] = {0, float(max)};
	    vals[2] = {0, 0};
	}
    }
    else
    {
	u_int	val;
	float	fval;
	TU::getFeature(*camera, ids[0].i, val, fval);

	vals.length(1);
	vals[0] = {CORBA::Long(val), fval};
    }

    return vals;
}
    
}
}

