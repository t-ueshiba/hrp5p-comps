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

/************************************************************************
*  global functions							*
************************************************************************/
CmdDefs
createFormatCmds(const Ieee1394Camera& camera)
{
    CmdDefs	fmtCmds;
    
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
	    fmtCmds.push_back(CmdDef());
	    CmdDef&	fmtCmd = fmtCmds.back();

	    fmtCmd.type    = C_MenuItem;
	    fmtCmd.name	   = formats[i].name;
	    fmtCmd.id	   = formats[i].format;
	    fmtCmd.subcmds = rateCmds;
	}
    }

    return fmtCmds;
}

}
}
