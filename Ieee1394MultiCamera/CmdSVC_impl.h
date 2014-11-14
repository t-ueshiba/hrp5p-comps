/*
 *  $Id$
 */
#include <sstream>
#include <boost/archive/xml_oarchive.hpp>
#include "Cmd.hh"
#include "CmdDef.h"

#ifndef CMDSVC_IMPL_H
#define CMDSVC_IMPL_H

namespace TU
{
template <class CAMERAS>	class MultiCamera;

namespace v
{
/************************************************************************
*  class CmdSVC_impl<CAMERAS>						*
************************************************************************/
template <class CAMERAS>
class CmdSVC_impl : public virtual POA_Cmd::Controller,
		    public virtual PortableServer::RefCountServantBase
{
  private:
    typedef typename CAMERAS::camera_type	camera_type;
    typedef Cmd::Values				Values;
    
    enum
    {
	c_ContinuousShot, c_Format, c_CameraSelection,
	c_U0, c_V0, c_Width, c_Height, c_PixelFormat,
    };
    
  public:
			CmdSVC_impl(MultiCamera<CAMERAS>& rtc)		;
    virtual		~CmdSVC_impl()					;

    char*		getCmds()					;
    CORBA::Boolean	setValues(const Values& vals)			;
    Cmd::Values*	getValues(const Values& ids)			;

  private:
    CmdDefs		createCmds(const CAMERAS& cameras)		;
    static CmdDefs	createFormatCmds(camera_type& camera)		;
    static void		addExtraCmds(const camera_type& camera,
				     CmdDefs& cmds)			;
    static void		getFormat(const CAMERAS& cameras,
				  const Values& ids, Values& vals)	;
    
  private:
    MultiCamera<CAMERAS>&	_rtc;
    size_t			_n;	// currently selected camera #
};

template <class CAMERAS>
CmdSVC_impl<CAMERAS>::CmdSVC_impl(MultiCamera<CAMERAS>& rtc)
    :_rtc(rtc), _n(0)
{
}

template <class CAMERAS>
CmdSVC_impl<CAMERAS>::~CmdSVC_impl()
{
}

template <class CAMERAS> char*
CmdSVC_impl<CAMERAS>::getCmds()
{
#ifdef DEBUG
    std::cerr << "CmdSVC_impl<CAMERAS>::getCmds() called!" << std::endl;
#endif
    std::ostringstream			oss;
    boost::archive::xml_oarchive	oar(oss);
    CmdDefs				cmds = createCmds(_rtc.cameras());
    oar << BOOST_SERIALIZATION_NVP(cmds);
    
    return CORBA::string_dup(oss.str().c_str());
}

template <class CAMERAS> CORBA::Boolean
CmdSVC_impl<CAMERAS>::setValues(const Values& vals)
{
#ifdef DEBUG
    std::cerr << "CmdSVC_impl<CAMERAS>::setValues(): vals =";
    for (CORBA::ULong i = 0; i < vals.length(); ++i)
	std::cerr << ' ' << vals[i];
    std::cerr << std::endl;
#endif
    CORBA::Boolean	refresh = false;
    
    switch (vals[0])
    {
      case c_ContinuousShot:
	if (vals[1])
	    _rtc.continuousShot();
	else
	    _rtc.stopContinuousShot();
	break;
      case c_Format:
	_rtc.setFormat(vals);
	break;
      case c_CameraSelection:
	_n = vals[1];
	refresh = true;
	break;
      default:
	_rtc.setFeatureValue(vals[0], vals[1], _n);
	break;
    }

    return refresh;
}

template <class CAMERAS> Cmd::Values*
CmdSVC_impl<CAMERAS>::getValues(const Values& ids)
{
#ifdef DEBUG
    std::cerr << "CmdSVC_impl<CAMERAS>::getValues(): ids =";
    for (CORBA::ULong i = 0; i < ids.length(); ++i)
	std::cerr << ' ' << ids[i];
#endif
    Values	vals;
    
    switch (ids[0])
    {
      case c_ContinuousShot:
	vals.length(1);
	vals[0] = exec(_rtc.cameras(), &camera_type::inContinuousShot);
	break;
      case c_Format:
	getFormat(_rtc.cameras(), ids, vals);
	break;
      case c_CameraSelection:
	vals.length(1);
	vals[0] = _n;
	break;
      default:
	vals.length(1);
	vals[0] = getFeatureValue(_rtc.cameras(), ids[0], _n);
	break;
    }
#ifdef DEBUG
    std::cerr << ", vals =";
    for (CORBA::ULong i = 0; i < vals.length(); ++i)
	std::cerr << ' ' << vals[i];
    std::cerr << std::endl;
#endif
    return new Values(vals);
}

template <class CAMERAS> typename TU::v::CmdDefs
CmdSVC_impl<CAMERAS>::createCmds(const CAMERAS& cameras)
{
    CmdDefs	cmds;
    
    if (!cameras.size())
	return cmds;

    _n = cameras.size();	// 全カメラ同時操作モード

  // カメラ画像出力のon/offコマンドの生成
    cmds.push_back(CmdDef(C_ToggleButton, "Continuous shot", c_ContinuousShot,
			  noSub, 0, 1, 1, 1, CA_None, 0, 0));

  // カメラ画像フォーマットの選択コマンドの生成
    cmds.push_back(CmdDef(C_Button, "Format", c_Format,
			  createFormatCmds(*cameras[0]),
			  0, 1, 1, 1, CA_None, 0, 1));

  // 操作対象カメラの選択コマンドの生成
    CmdDefs	cameraSelectionCmds;
    for (size_t i = 0; i < cameras.size(); ++i)
	cameraSelectionCmds.push_back(
				CmdDef(C_RadioButton,
				       std::string(1, char('0' + i)), i,
				       noSub, 0, 1, 1, 1, CA_None, i));
    cameraSelectionCmds.push_back(CmdDef(C_RadioButton, "all", cameras.size(),
					 noSub, 0, 1, 1, 1, CA_None,
					 cameras.size()));
    cmds.push_back(CmdDef(C_GroupBox, "Camera selection", c_CameraSelection,
			  cameraSelectionCmds,
			  0, int(cameras.size()), 1, 1, CA_None, 0, 2));

  // その他のコマンドの生成
    addExtraCmds(*cameras[0], cmds);

    return cmds;
}
    
}
}
#endif // CMDSVC_IMPL_H
