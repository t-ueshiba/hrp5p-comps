/*
 *  $Id$
 */
#ifndef __TU_MULTICAMERA_H
#define __TU_MULTICAMERA_H

#include "Img.hh"
#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include "CmdSVC_impl.h"
#include <mutex>
#include <fstream>
#include <ctime>	// for std::strftime()

namespace TU
{
void	saveImageHeader(std::ostream& out, const Img::Header& header)	;
    
#ifdef DEBUG
inline void
countTime()
{
    static int		nframes = 0;
    static timeval	start;
    
    if (nframes == 10)
    {
	timeval	end;
	gettimeofday(&end, NULL);
	double	interval = (end.tv_sec  - start.tv_sec) +
            (end.tv_usec - start.tv_usec) / 1.0e6;
	std::cerr << nframes / interval << " frames/sec" << std::endl;
	nframes = 0;
    }
    if (nframes++ == 0)
	gettimeofday(&start, NULL);
}
#endif
/************************************************************************
*  class MultiCameraRTC<CAMERAS>					*
************************************************************************/
template <class CAMERAS>
class MultiCameraRTC : public RTC::DataFlowComponentBase
{
  private:
    typedef typename CAMERAS::value_type	camera_type;
    
  public:
    MultiCameraRTC(RTC::Manager* manager)				;

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;

  // 外部からはカメラ状態の変更を許さない
    const CAMERAS&	cameras()				const	;

  // カメラ状態の変更を伴うコマンドのみ本クラスで扱い、
  // カメラ状態を取得するだけのコマンドは class CmdSVC_impl<CAMERAS> で扱う
    void		saveConfig()					;
    void		restoreConfig()					;
    bool		inRecordingImages()				;
    void		recordImages(bool enable)			;
    bool		inContinuousShot()				;
    void		continuousShot(bool enable)			;
    void		setFormat(const Cmd::Values& vals)		;
    bool		setFeature(const Cmd::Values& vals,
				   size_t n, bool all)			;
    
  private:
    void		initializeConfigurations()			;
    void		allocateImages()				;
    void		embedTimestamp(bool enable)			;
    static RTC::Time	getTimestamp(const camera_type& camera)		;
    static size_t	setImageFormat(const camera_type& camera,
				       Img::Header& header)		;
    void		snap()						;
    
  private:
    CAMERAS				_cameras;
    mutable std::mutex			_mutex;
    std::string				_cameraName;	// config var.
    std::string				_recFilePrefix;	// config var.
    int					_syncedSnap;	// config var.
    bool				_startWithFlow;	// config var.
    std::ofstream			_fout;
    Img::TimedImages			_images;	// out data
    RTC::OutPort<Img::TimedImages>	_imagesOut;	// out data port
    v::CmdSVC_impl<CAMERAS>		_command;	// service provider
    RTC::CorbaPort			_commandPort;	// service port
};

/*
 *  public member functions
 */
template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "MultiCameraRTC::onInitialize" << std::endl;
#endif
  // コンフィグレーションをセットアップ
    initializeConfigurations();
    
  // データポートをセットアップ
    addOutPort("TimedImages", _imagesOut);

  // サービスプロバイダとサービスポートをセットアップ
    _commandPort.registerProvider("Command", "Cmd::Controller", _command);
    addPort(_commandPort);
    
    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiCameraRTC::onActivated" << std::endl;
#endif
    try
    {
      // 設定ファイルを読み込んでカメラを生成・セットアップ
	_cameras.restore(_cameraName.c_str());

      // カメラ数分の画像ヘッダを確保
	_images.headers.length(_cameras.size());

      // キャリブレーションを読み込み
	std::ifstream	in(_cameras.calibFile().c_str());
	if (!in)
	    throw std::runtime_error("MultiCameraRTC<CAMERAS>::onActivated(): failed to open " + _cameras.calibFile() + " !");
	for (size_t i = 0; i < _images.headers.length(); ++i)
	{
	    auto&	header = _images.headers[i];
	    in >> header.P[0][0] >> header.P[0][1]
	       >> header.P[0][2] >> header.P[0][3]
	       >> header.P[1][0] >> header.P[1][1]
	       >> header.P[1][2] >> header.P[1][3]		
	       >> header.P[2][0] >> header.P[2][1]
	       >> header.P[2][2] >> header.P[2][3]		
	       >> header.d1 >> header.d2;
	}

	allocateImages();		// 画像データ領域を確保
	embedTimestamp(true);		// 撮影時刻の記録を初期化
	continuousShot(_startWithFlow);	// 連続撮影/停止のいずれかで開始
    }
    catch (std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	
	return RTC::RTC_ERROR;
    }

    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onExecute(RTC::UniqueId ec_id)
{
    std::unique_lock<std::mutex>	lock(_mutex);

    if (inContinuousShot())
    {
#ifdef DEBUG
	countTime();
#endif
	if (_syncedSnap)
	    syncedSnap(_cameras, std::chrono::microseconds(_syncedSnap));
	else
	    std::for_each(std::begin(_cameras), std::end(_cameras),
			  [](auto& camera){ camera.snap(); });
	
	size_t	i = 0;
	auto	data = _images.data.get_buffer(false);
	for (const auto& camera : _cameras)
	{
	    camera.captureRaw(data);
	    _images.headers[i].tm = getTimestamp(camera);
	    data += _images.headers[i].size;
	    ++i;
	}
	_imagesOut.write();

	if (inRecordingImages())
	    _fout.write(reinterpret_cast<const char*>(
			    _images.data.get_buffer()),
			_images.data.length());
    }

    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiCameraRTC::onDeactivated" << std::endl;
#endif
    continuousShot(false);		// 連続撮影を終了

  // デバイスを終了
    std::for_each(std::begin(_cameras), std::end(_cameras),
		  [](auto& camera){ camera.terminate(); });

    return RTC::RTC_OK;
}

template <class CAMERAS> RTC::ReturnCode_t
MultiCameraRTC<CAMERAS>::onAborting(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "MultiCameraRTC::onAborting" << std::endl;
#endif
  // デバイスを終了
    std::for_each(std::begin(_cameras), std::end(_cameras),
		  [](auto& camera){ camera.terminate(); });

    return RTC::RTC_OK;
}

template <class CAMERAS> inline const CAMERAS&
MultiCameraRTC<CAMERAS>::cameras() const
{
    return _cameras;
}

template <class CAMERAS> void
MultiCameraRTC<CAMERAS>::saveConfig()
{
}

template <class CAMERAS> void
MultiCameraRTC<CAMERAS>::restoreConfig()
{
}

template <class CAMERAS> bool
MultiCameraRTC<CAMERAS>::inRecordingImages()
{
    return _fout.is_open();
}

template <class CAMERAS> void
MultiCameraRTC<CAMERAS>::recordImages(bool enable)
{
    if (enable == inRecordingImages())
	return;

    std::unique_lock<std::mutex>	lock(_mutex);

    if (enable)
    {
	using	std::operator +;
	
	char		recFileSuffix[128];
	const auto	t = std::time(NULL);
	std::strftime(recFileSuffix, sizeof(recFileSuffix),
		      "-%F-%T.epbms", std::localtime(&t));
	const auto	recFileName = _recFilePrefix
				    + std::string(recFileSuffix);
	_fout.open(recFileName.c_str(),
		   std::ofstream::out | std::ofstream::binary);
	if (!_fout)
	    throw std::runtime_error("MultiCameraRTC<CAMERAS>::recordImages(): failed to open " + recFileName + " !");
	
	_fout << 'M' << _images.headers.length() << std::endl;
	for (size_t i = 0; i < _images.headers.length(); ++i)
	    saveImageHeader(_fout, _images.headers[i]);
    }
    else
    {
	_fout.close();
    }
}

template <class CAMERAS> bool
MultiCameraRTC<CAMERAS>::inContinuousShot()
{
    for (const auto& camera : _cameras)
	if (!camera.inContinuousShot())
	    return false;
    return true;
}

template <class CAMERAS> void
MultiCameraRTC<CAMERAS>::continuousShot(bool enable)
{
    std::unique_lock<std::mutex>	lock(_mutex);

    std::for_each(std::begin(_cameras), std::end(_cameras),
		  [enable](auto& camera){ camera.continuousShot(enable); });
}

template <class CAMERAS> void
MultiCameraRTC<CAMERAS>::allocateImages()
{
    size_t	i = 0, total_size = 0;
    for (const auto& camera : _cameras)
	total_size += setImageFormat(camera, _images.headers[i++]);
    _images.data.length(total_size);
}
    
template <class CAMERAS> RTC::Time
MultiCameraRTC<CAMERAS>::getTimestamp(const camera_type& camera)
{
    const std::chrono::nanoseconds
		nsec = camera.getTimestamp().time_since_epoch();
    return {CORBA::ULong(nsec.count() / 1000000000),
	    CORBA::ULong(nsec.count() % 1000000000)};
}

}
#endif	// !__TU_MULTICAMERA_H
