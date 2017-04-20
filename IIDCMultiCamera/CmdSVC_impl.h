/*
 *  $Id$
 */
#ifndef CMDSVC_IMPL_H
#define CMDSVC_IMPL_H

#include <sstream>
#include <boost/archive/xml_oarchive.hpp>
#include "Cmd.hh"
#include "CmdDef.h"

namespace TU
{
template <class CAMERAS>	class MultiCameraRTC;

#ifdef DEBUG
inline std::ostream&
print(std::ostream& out, const Cmd::Values& vals)
{
    for (CORBA::ULong i = 0; i < vals.length(); ++i)
	out << " (" << vals[i].i << ',' << vals[i].f << ')';
    return out;
}
#endif

namespace v
{
/************************************************************************
*  class CmdSVC_impl<CAMERAS>						*
************************************************************************/
//! マルチカメラシステムを操作するためのコマンドサービスを表現するクラス
template <class CAMERAS>
class CmdSVC_impl : public virtual POA_Cmd::Controller,
		    public virtual PortableServer::RefCountServantBase
{
  private:
    typedef typename CAMERAS::value_type	camera_type;

  //! カメラ操作コマンド
    enum
    {
	c_RecordImages,		//!< 録画
	c_SaveConfigToFile,	//!< 設定ファイルへの保存
	c_SaveConfig,		//!< メモリへの設定の保存
	c_RestoreConfig,	//!< メモリからの設定の読み込み
	c_ContinuousShot,	//!< 画像ストリームの起動/停止
	c_Format,		//!< 画像フォーマットの選択
	c_CameraSelection,	//!< 操作対象カメラの選択
	c_AllCameras,		//!< 全カメラ一斉操作モード
	c_U0,			//!< ROIの左上隅横座標
	c_V0,			//!< ROIの左上隅縦座標
	c_Width,		//!< ROIの幅
	c_Height,		//!< ROIの高さ
	c_PacketSize,		//!< パケットサイズ
	c_PixelFormat,		//!< ROIの画素フォーマット
    };
    
  public:
			CmdSVC_impl(MultiCameraRTC<CAMERAS>& rtc)	;
    virtual		~CmdSVC_impl()					;

  // カメラを操作するコンポーネントとのコマンド通信
    char*		getCmds()					;
    Cmd::Values*	setValues(const Cmd::Values& vals)		;
    Cmd::Values*	getValues(const Cmd::Values& ids,
				  CORBA::Boolean range)			;
    
  private:
    CmdDefs		createCmds()					;
    static CmdDefs	createFormatItems(const camera_type& camera)	;
    static void		appendFeatureCmds(const camera_type& camera,
					  CmdDefs& cmds)		;
    bool		inRecordingImages()			const	;
    bool		inContinuousShot()			const	;
    Cmd::Values		getFormat(const Cmd::Values& ids,
				  CORBA::Boolean range)		const	;
    Cmd::Values		setFeature(const Cmd::Values& vals)		;
    Cmd::Values		getFeature(const Cmd::Values& ids,
				   CORBA::Boolean range)	const	;
    
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
template <class CAMERAS> Cmd::Values*
CmdSVC_impl<CAMERAS>::setValues(const Cmd::Values& vals)
{
#ifdef DEBUG
    print(std::cerr << "CmdSVC_impl<CAMERAS>::setValues(): vals =", vals);
#endif
    Cmd::Values	ids;
    
    switch (vals[0].i)
    {
      case c_RecordImages:		// ファイルへ画像列を記録
	_rtc.recordImages(vals[1].i);
	break;
	
      case c_SaveConfigToFile:		// ファイルへカメラ設定を保存
	_rtc.cameras().save();
	break;

      case c_SaveConfig:		// メモリへカメラ設定を保存
	_rtc.saveConfig();
	break;

      case c_RestoreConfig:		// メモリからカメラ設定を読み込み
	_rtc.restoreConfig();
	ids.length(1);
	ids[0] = {CORBA::Long(CmdDef::c_RefreshAll), 0};
	break;

      case c_ContinuousShot:
	_rtc.continuousShot(vals[1].i);	// カメラ撮影を起動/停止
	break;
	
      case c_Format:
      {
	const auto	cont = _rtc.inContinuousShot();
	_rtc.continuousShot(false);
	_rtc.setFormat(vals);		// 画像フォーマットを設定
	_rtc.continuousShot(cont);
	ids.length(1);
	ids[0] = {CORBA::Long(CmdDef::c_RefreshAll), 0};
      }
	break;
	
      case c_CameraSelection:
	_n = vals[1].i;			// 選択カメラを変更
	ids.length(1);
	ids[0] = {CORBA::Long(CmdDef::c_RefreshAll), 0};
	break;

      case c_AllCameras:
	_all = vals[1].i;		// 全カメラ一斉操作モードを有効化/無効化
	break;
	
      default:
	ids = setFeature(vals);		// カメラ属性を変更
	break;
    }
#ifdef DEBUG
    print(std::cerr << ", ids  =", ids) << std::endl;
#endif
    return new Cmd::Values(ids);
}

//! カメラ属性の値またはその値域を得る
/*!
  \param ids	操作コンポーネントからのコマンド
  \param range	falseならば値, trueならば値域
  \return	カメラ属性の値または値域
*/
template <class CAMERAS> Cmd::Values*
CmdSVC_impl<CAMERAS>::getValues(const Cmd::Values& ids, CORBA::Boolean range)
{
#ifdef DEBUG
    print(std::cerr << "CmdSVC_impl<CAMERAS>::getValues(): ids  =", ids);
#endif
    Cmd::Values	vals;
    
    switch (ids[0].i)
    {
      case c_RecordImages:
	vals.length(1);
	vals[0] = {CORBA::Long(inRecordingImages()), 0};
	break;
	
      case c_ContinuousShot:
	vals.length(1);
	vals[0] = {CORBA::Long(inContinuousShot()), 0};
	break;

      case c_Format:
	vals = getFormat(ids, range);
	break;

      case c_CameraSelection:
	vals.length(1);
	vals[0] = {CORBA::Long(_n), 0};
	break;

      case c_AllCameras:
	vals.length(1);
	vals[0] = {CORBA::Long(_all), 0};
	break;

      default:
	vals = getFeature(ids, range);
	break;
    }
#ifdef DEBUG
    print(std::cerr << ", vals =", vals) << std::endl;
#endif
    return new Cmd::Values(vals);
}

template <class CAMERAS> CmdDefs
CmdSVC_impl<CAMERAS>::createCmds()
{
    const auto	ncameras = std::size(_rtc.cameras());
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
			  "Format", 1, 0, 1, 1, 0, createFormatItems(camera)));

  // ファイルへの録画コマンドの生成
    cmds.push_back(CmdDef(CmdDef::C_ToggleButton, c_RecordImages,
			  "Rec.", 2, 0));

  // ファイルへのカメラ設定保存コマンドの生成
    cmds.push_back(CmdDef(CmdDef::C_Button, c_SaveConfigToFile,
			  "Save conf.", 3, 0));

    if (ncameras > 1)
    {
      // 操作対象カメラ選択コマンドの生成
	CmdDefs	cameraSelectionCmds;
	for (size_t i = 0; i < ncameras; ++i)
	    cameraSelectionCmds.push_back(CmdDef(CmdDef::C_RadioButton, i,
						 std::string(1, char('0'+i)),
						 i));
	cmds.push_back(CmdDef(CmdDef::C_GroupBox, c_CameraSelection,
			      "Camera selection", 0, 1, 1, 1, 0,
			      cameraSelectionCmds));
	cmds.push_back(CmdDef(CmdDef::C_ToggleButton, c_AllCameras,
			      "all", 1, 1));
    }

  // 属性操作コマンドの生成
    appendFeatureCmds(camera, cmds);

    return cmds;
}

template <class CAMERAS> inline bool
CmdSVC_impl<CAMERAS>::inRecordingImages() const
{
    return _rtc.inRecordingImages();
}

template <class CAMERAS> inline bool
CmdSVC_impl<CAMERAS>::inContinuousShot() const
{
    return std::begin(_rtc.cameras())->inContinuousShot();
}

}
}
#endif // CMDSVC_IMPL_H
