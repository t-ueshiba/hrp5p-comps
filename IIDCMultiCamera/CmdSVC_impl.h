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
template <class CAMERAS>	class MultiCameraRTC;

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
    typedef typename CAMERAS::value_type	camera_type;
    
    enum
    {
	c_ContinuousShot, c_Format, c_CameraSelection, c_AllCameras,
	c_U0, c_V0, c_Width, c_Height, c_PixelFormat,
    };
    
  public:
			CmdSVC_impl(MultiCameraRTC<CAMERAS>& rtc)	;
    virtual		~CmdSVC_impl()					;

  // カメラを操作するコンポーネントとのコマンド通信
    char*		getCmds()					;
    CORBA::Boolean	setValues(const Cmd::Values& vals)		;
    Cmd::Values*	getValues(const Cmd::Values& ids)		;
    
  private:
    CmdDefs		createCmds()					;
    static CmdDefs	createFormatItems(const camera_type& camera)	;
    static void		appendFeatureCmds(const camera_type& camera,
					  CmdDefs& cmds)		;
    bool		inContinuousShot()			const	;
    Cmd::Values		getFormat(const Cmd::Values& ids)	const	;
    Cmd::Values		getFeature(const Cmd::Values& ids)	const	;
    bool		setFeature(const Cmd::Values& vals)		;

  private:
    MultiCameraRTC<CAMERAS>&	_rtc;
    size_t			_n;	// currently selected camera #
    bool			_all;	// steel all cameras simultaneously
};

//! マルチカメラシステムを操作するためのコマンドサービスを生成する
/*
  \param rtc	マルチカメラシステムを管理するコンポーネント
*/
template <class CAMERAS>
CmdSVC_impl<CAMERAS>::CmdSVC_impl(MultiCameraRTC<CAMERAS>& rtc)
    :_rtc(rtc), _n(0), _all(true)
{
}

//! マルチカメラシステムを操作するためのコマンドサービスを破壊する
template <class CAMERAS>
CmdSVC_impl<CAMERAS>::~CmdSVC_impl()
{
}

//! 操作コンポーネントからのコマンド一覧要求に応える
/*!
  \return	このマルチカメラシステムが提供するコマンドの一覧
*/
template <class CAMERAS> char*
CmdSVC_impl<CAMERAS>::getCmds()
{
#ifdef DEBUG
    std::cerr << "CmdSVC_impl<CAMERAS>::getCmds() called!" << std::endl;
#endif
    std::ostringstream			oss;
    boost::archive::xml_oarchive	oar(oss);
    CmdDefs				cmds = createCmds();
    oar << BOOST_SERIALIZATION_NVP(cmds);
    
    return CORBA::string_dup(oss.str().c_str());
}

//! カメラを操作する
/*!
  \param vals	コマンドとパラメータの列
  \return	操作コンポーネントの状態更新が必要ならtrue, そうでなければfalse
*/
template <class CAMERAS> CORBA::Boolean
CmdSVC_impl<CAMERAS>::setValues(const Cmd::Values& vals)
{
#ifdef DEBUG
    using namespace	std;
    
    cerr << "CmdSVC_impl<CAMERAS>::setValues(): vals =";
    for (CORBA::ULong i = 0; i < vals.length(); ++i)
	cerr << ' ' << vals[i];
    cerr << endl;
#endif
    switch (vals[0])
    {
      case c_ContinuousShot:
	_rtc.continuousShot(vals[1]);	// カメラ撮影を起動/停止
	return false;
	
      case c_Format:
	_rtc.setFormat(vals);		// 画像フォーマットを設定
	return false;
	
      case c_CameraSelection:
	_n = vals[1];			// 選択カメラを変更
	return true;

      case c_AllCameras:
	_all = vals[1];			// 全カメラ一斉操作モードを有効化/無効化
	return false;
	
      default:
	break;
    }

    return setFeature(vals);		// カメラ属性を変更
}

//! カメラの状態を得る
/*!
  \param ids	操作コンポーネントからのコマンド
  \return
*/
template <class CAMERAS> Cmd::Values*
CmdSVC_impl<CAMERAS>::getValues(const Cmd::Values& ids)
{
#ifdef DEBUG
    using namespace	std;
    
    cerr << "CmdSVC_impl<CAMERAS>::getValues(): ids =";
    for (CORBA::ULong i = 0; i < ids.length(); ++i)
	cerr << ' ' << ids[i];
#endif
    Cmd::Values	vals;
    
    switch (ids[0])
    {
      case c_ContinuousShot:
	vals.length(1);
	vals[0] = inContinuousShot();
	break;

      case c_Format:
	vals = getFormat(ids);
	break;

      case c_CameraSelection:
	vals.length(1);
	vals[0] = _n;
	break;

      case c_AllCameras:
	vals.length(1);
	vals[0] = _all;
	break;

      default:
	vals = getFeature(ids);
	break;
    }
#ifdef DEBUG
    cerr << ", vals =";
    for (CORBA::ULong i = 0; i < vals.length(); ++i)
	cerr << ' ' << vals[i];
    cerr << endl;
#endif
    return new Cmd::Values(vals);
}

template <class CAMERAS> CmdDefs
CmdSVC_impl<CAMERAS>::createCmds()
{
    const auto	ncameras = size(_rtc.cameras());
    CmdDefs	cmds;
    
    if (ncameras == 0)
	return cmds;

    const auto&	camera = *std::begin(_rtc.cameras());	// 最初のカメラ
    _n = 0;						// 最初のカメラを選択
    _all = true;					// 全カメラ同時操作モード
    
  // カメラ画像出力on/offコマンドの生成
    cmds.push_back(CmdDef(CmdDef::C_ToggleButton, c_ContinuousShot,
			  "Continuous shot", 0, 0));

  // カメラ画像フォーマット選択コマンドの生成
    cmds.push_back(CmdDef(CmdDef::C_Button, c_Format,
			  "Format", 0, 1, 1, 1, 0, createFormatItems(camera)));

    if (ncameras > 1)
    {
      // 操作対象カメラ選択コマンドの生成
	CmdDefs	cameraSelectionCmds;
	for (size_t i = 0; i < ncameras; ++i)
	    cameraSelectionCmds.push_back(CmdDef(CmdDef::C_RadioButton, i,
						 std::string(1, char('0'+i)),
						 i));
	cmds.push_back(CmdDef(CmdDef::C_GroupBox, c_CameraSelection,
			      "Camera selection", 0, 2, 1, 1, 0,
			      cameraSelectionCmds, 0, ncameras - 1));
	cmds.push_back(CmdDef(CmdDef::C_ToggleButton, c_AllCameras,
			      "all", 1, 2));
    }

  // 属性操作コマンドの生成
    appendFeatureCmds(camera, cmds);

    return cmds;
}

template <class CAMERAS> inline bool
CmdSVC_impl<CAMERAS>::inContinuousShot() const
{
    return std::begin(_rtc.cameras())->inContinuousShot();
}

template <class CAMERAS> Cmd::Values
CmdSVC_impl<CAMERAS>::getFeature(const Cmd::Values& ids) const
{
    Cmd::Values	vals;
    
    if (size(_rtc.cameras()) == 0)
	return vals;

    auto	camera = std::begin(_rtc.cameras());
    std::advance(camera, _n);				// 選択カメラ

    u_int	val;
    float	fval;
    TU::getFeature(*camera, ids[0], val, fval);
    vals.length(1);
    vals[0] = val;

    return vals;
}

}
}
#endif // CMDSVC_IMPL_H