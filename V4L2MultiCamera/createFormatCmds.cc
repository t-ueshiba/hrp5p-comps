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
createFormatCmds(const V4L2Camera& camera)
{
    CmdDefs	fmtCmds;
    static const char*	setROILabel = "set ROI";
    
    BOOST_FOREACH (V4L2Camera::PixelFormat pixelFormat,
		   camera.availablePixelFormats())
    {
      // この画素フォーマットに対応するメニュー項目を作る．
	fmtCmds.push_back(CmdDef(C_MenuItem));
	CmdDef&	fmtCmd = fmtCmds.back();

	fmtCmd.name = camera.getName(pixelFormat);
	fmtCmd.id   = pixelFormat;

      // この画素フォーマットがサポートする各フレームサイズに対応するメニュー項目を作る．
	BOOST_FOREACH (const V4L2Camera::FrameSize& frameSize,
		       camera.availableFrameSizes(pixelFormat))
	{
	    fmtCmd.subcmds.push_back(CmdDef(C_MenuItem));
	    CmdDef&	fsizeCmd = fmtCmd.subcmds.back();

	    std::ostringstream	s;
	    s << frameSize;
	    fsizeCmd.name = s.str();
	    fsizeCmd.id   = fmtCmd.subcmds.size() - 1;
	    fsizeCmd.val  = (camera.pixelFormat() == pixelFormat	&&
			     frameSize.width .involves(camera.width())  &&
			     frameSize.height.involves(camera.height()));
	}
    }

  // ROIを指定する項目を作る．
    fmtCmds.push_back(CmdDef(C_MenuItem));
    CmdDef&	setROICmd = fmtCmds.back();
    
    setROICmd.name = "set ROI";
    setROICmd.id   = V4L2Camera::UNKNOWN_PIXEL_FORMAT;
    
    return fmtCmds;
}
    
}
}
