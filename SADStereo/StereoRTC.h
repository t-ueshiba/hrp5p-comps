/*
 *  $Id$
 */
#ifndef __TU_STEREORTC_H
#define __TU_STEREORTC_H

#include "MultiImage.hh"
#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include <coil/Guard.h>
#include "TU/Rectify.h"
#include "CmdSVC_impl.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
template <class T> static MultiImage::PixelFormat
pixelFormat()
{
    return MultiImage::MONO_8;
}
template <> MultiImage::PixelFormat
pixelFormat<YUV411>()
{
    return MultiImage::YUV_411;
}
template <> MultiImage::PixelFormat
pixelFormat<YUV422>()
{
    return MultiImage::YUV_422;
}
template <> MultiImage::PixelFormat
pixelFormat<YUYV422>()
{
    return MultiImage::YUYV_422;
}
template <> MultiImage::PixelFormat
pixelFormat<YUV444>()
{
    return MultiImage::YUV_444;
}
template <> MultiImage::PixelFormat
pixelFormat<RGB>()
{
    return MultiImage::RGB_24;
}
template <> MultiImage::PixelFormat
pixelFormat<float>()
{
    return MultiImage::FLT;
}
	
template <class SRC, class DST> static void
copyCalib(const SRC& src, DST& dst)
{
    for (size_t i = 0; i < 3; ++i)
	for (size_t j = 0; j < 4; ++j)
	    dst.P[i][j] = src.P[i][j];
    dst.d1 = src.d1;
    dst.d2 = src.d2;
}

/************************************************************************
*  class StereoRTC<STEREO, PIXEL>					*
************************************************************************/
template <class STEREO, class PIXEL=u_char>
class StereoRTC : public RTC::DataFlowComponentBase
{
  public:
    typedef STEREO			stereo_type;
    typedef PIXEL			pixel_type;
    typedef typename STEREO::Parameters	params_type;

  public:
    StereoRTC(RTC::Manager* manager)					;
    ~StereoRTC()							{}

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
#ifdef DEBUG
    virtual RTC::ReturnCode_t	onStartup(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onShutdown(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onFinalize()				;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;
#endif
  //virtual RTC::ReturnCode_t	onError(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onReset(RTC::UniqueId ec_id)		;
  //virtual RTC::ReturnCode_t	onStateUpdate(RTC::UniqueId ec_id)	;
  //virtual RTC::ReturnCode_t	onRateChanged(RTC::UniqueId ec_id)	;

    void		setBinocular(bool binocular)			;
    bool		getBinocular()				const	;
    void		setParameters(const params_type& params,
				      bool needsInitialization=false)	;
    const params_type&	getParameters()				const	;
    void		initializeRectification()			;
    
  private:
    void		initializeConfigurations()			;
    bool		mapInImages()					;
    void		mapOutImages()					;
    void		stereoMatch()					;
    
  private:
    mutable coil::Mutex				_mutex;
    bool					_initialized;
    bool					_binocular;
    double					_scale;
    Rectify					_rectify;
    stereo_type					_stereo;
    Image<pixel_type>				_images[3];
    Image<pixel_type>				_rectifiedImages[3];
    Image<float>				_disparityMap;

    std::string					_stereoParams;	// config var.
    MultiImage::TimedImages			_inImages;
    RTC::InPort<MultiImage::TimedImages>	_inImagesPort;
    MultiImage::TimedImages			_outRectifiedImages;
    RTC::OutPort<MultiImage::TimedImages>	_outRectifiedImagesPort;
    MultiImage::TimedImages			_outDisparityMap;
    RTC::OutPort<MultiImage::TimedImages>	_outDisparityMapPort;
    v::CmdSVC_impl<StereoRTC>			_command;
    RTC::CorbaPort				_commandPort;
};

template <class STEREO, class PIXEL> RTC::ReturnCode_t
StereoRTC<STEREO, PIXEL>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "StereoRTC<STEREO, PIXEL>::onInitialize" << std::endl;
#endif
    initializeConfigurations();
    
    addInPort( "TimedImages", _inImagesPort);
    addOutPort("TimedImages", _outRectifiedImagesPort);
    addOutPort("TimedImages", _outDisparityMapPort);

    _commandPort.registerProvider("Command", "Cmd::Controller", _command);
    addPort(_commandPort);

    std::ifstream	in(_stereoParams.c_str());
    if (in)
    {
	params_type	params;
	params.get(in);
	_stereo.setParameters(params);
    }
    
    return RTC::RTC_OK;
}

template <class STEREO, class PIXEL> RTC::ReturnCode_t
StereoRTC<STEREO, PIXEL>::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "StereoRTC<STEREO, PIXEL>::onActivated" << std::endl;
#endif
    return RTC::RTC_OK;
}

template <class STEREO, class PIXEL> RTC::ReturnCode_t
StereoRTC<STEREO, PIXEL>::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "StereoRTC<STEREO, PIXEL>::onDeactivated" << std::endl;
#endif
    _initialized = false;

    return RTC::RTC_OK;
}

template <class STEREO, class PIXEL> RTC::ReturnCode_t
StereoRTC<STEREO, PIXEL>::onExecute(RTC::UniqueId ec_id)
{
    if (_inImagesPort.isNew())
    {
	coil::Guard<coil::Mutex>	guard(_mutex);

	do
	{
	    _inImagesPort.read();	// 画像データ読み込み
	}
	while (_inImagesPort.isNew());

	if (!mapInImages())
	    return RTC::RTC_ERROR;
    
 	if (!_initialized)		// 初期化されていなかったら...
	{
	    initializeRectification();	// 初期化する
	    mapOutImages();
	    _initialized = true;
	}

	stereoMatch();			// ステレオマッチング

	_outRectifiedImagesPort.write();
	_outDisparityMapPort.write();
    }

    return RTC::RTC_OK;
}

#ifdef DEBUG
template <class STEREO, class PIXEL> RTC::ReturnCode_t
StereoRTC<STEREO, PIXEL>::onStartup(RTC::UniqueId ec_id)
{
    std::cerr << "StereoRTC<STEREO, PIXEL>::onStartup" << std::endl;

    return RTC::RTC_OK;
}

template <class STEREO, class PIXEL> RTC::ReturnCode_t
StereoRTC<STEREO, PIXEL>::onShutdown(RTC::UniqueId ec_id)
{
    std::cerr << "StereoRTC<STEREO, PIXEL>::onShutdown" << std::endl;

    return RTC::RTC_OK;
}

template <class STEREO, class PIXEL> RTC::ReturnCode_t
StereoRTC<STEREO, PIXEL>::onFinalize()
{
    std::cerr << "StereoRTC<STEREO, PIXEL>::onFinalize" << std::endl;

    return RTC::RTC_OK;
}

template <class STEREO, class PIXEL> RTC::ReturnCode_t
StereoRTC<STEREO, PIXEL>::onAborting(RTC::UniqueId ec_id)
{
    std::cerr << "StereoRTC<STEREO, PIXEL>::onAborting" << std::endl;

    return RTC::RTC_OK;
}
#endif
    
template <class STEREO, class PIXEL> void
StereoRTC<STEREO, PIXEL>::setBinocular(bool binocular)
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    const auto	binocular_old = _binocular;
    _binocular = (binocular || _inImages.headers.length() <= 2);
    if (_binocular != binocular_old)
	_initialized = false;
}
    
template <class STEREO, class PIXEL> bool
StereoRTC<STEREO, PIXEL>::getBinocular() const
{
    return _binocular;
}
    
template <class STEREO, class PIXEL> void
StereoRTC<STEREO, PIXEL>::setParameters(const params_type& params,
					bool needsInitialization)
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    _stereo.setParameters(params);
    if (needsInitialization)
	_initialized = false;
}
    
template <class STEREO, class PIXEL>
const typename StereoRTC<STEREO, PIXEL>::params_type&
StereoRTC<STEREO, PIXEL>::getParameters() const
{
    return _stereo.getParameters();
}
    
template <class STEREO, class PIXEL> void
StereoRTC<STEREO, PIXEL>::initializeRectification()
{
  // rectificationを初期化
    if (_binocular)
    {
	_rectify.initialize(_images[0], _images[1],
			    _scale,
			    _stereo.getParameters().disparitySearchWidth,
			    _stereo.getParameters().disparityMax);
    }
    else
    {
	_rectify.initialize(_images[0], _images[1], _images[2],
			    _scale,
			    _stereo.getParameters().disparitySearchWidth,
			    _stereo.getParameters().disparityMax);
	
      // rectifyされた上画像のカメラ行列をセット
	_rectifiedImages[2].P  = _rectify.H(2) * _images[2].P;
	_rectifiedImages[2].P /= _rectifiedImages[2].P[2](0, 3).length();
    }

  // rectifyされた左・右画像および視差マップのカメラ行列をセット
    _rectifiedImages[0].P  = _rectify.H(0) * _images[0].P;
    _rectifiedImages[0].P /= _rectifiedImages[0].P[2](0, 3).length();
    _rectifiedImages[1].P  = _rectify.H(1) * _images[1].P;
    _rectifiedImages[1].P /= _rectifiedImages[1].P[2](0, 3).length();
    _disparityMap.P	   = _rectifiedImages[0].P;
}

template <class STEREO, class PIXEL> bool
StereoRTC<STEREO, PIXEL>::mapInImages()
{
    switch (_inImages.headers.length())	// 画像数をチェック
    {
      case 0:
      case 1:
	std::cerr << "Two or more images required!" << std::endl;
	return false;
      case 2:
	_binocular = true;
	break;
      default:
	_binocular = false;
	break;
    }
    
    const size_t	nimages = (_binocular ? 2 : 3);
    auto		data = _inImages.data.get_buffer();
    for (size_t i = 0; i < nimages; ++i)
    {
	const auto&	header = _inImages.headers[i];

	if (header.format != pixelFormat<pixel_type>())
	    throw std::runtime_error("Mismatched output image pixel format!");
	copyCalib(header, _images[i]);
	_images[i].resize((pixel_type*)data, header.height, header.width);
	data += header.size;
    }

    return true;
}

template <class STEREO, class PIXEL> void
StereoRTC<STEREO, PIXEL>::mapOutImages()
{
  // rectifyされた出力画像の枚数を設定
    const size_t	nimages = (_binocular ? 2 : 3);
    _outRectifiedImages.headers.length(nimages);	// ヘッダ数を設定
    
  // rectifyされた出力画像のヘッダを設定し，データ領域を確保
    size_t	total_size = 0;
    for (size_t i = 0; i < nimages; ++i)
    {
	auto&	header = _outRectifiedImages.headers[i];
	header.format = pixelFormat<pixel_type>();
	header.width  = _rectify.width(i);
	header.height = _rectify.height(i);
	header.size   = header.width * header.size * sizeof(pixel_type);
	copyCalib(_rectifiedImages[i], header);

	total_size += header.size;
    }
    _outRectifiedImages.data.length(total_size);	// データ領域を確保

  // rectifyされた出力画像のデータ領域を内部画像にマップ
    auto	data = _outRectifiedImages.data.get_buffer(false);
    for (size_t i = 0; i < nimages; ++i)
    {
	const auto&	header = _outRectifiedImages.headers[i];

	_rectifiedImages[i].resize((pixel_type*)data,
				   header.height, header.width);
	data += header.size;
    }

  // 視差画像のヘッダをrectify出力画像からコピーし，データ領域を確保して内部画像にマップ
    _outDisparityMap.headers.length(1);
    auto&	header = _outRectifiedImages.headers[0];
    header = _outRectifiedImages.headers[0];
    _outDisparityMap.data.length(header.width * header.height * sizeof(float));
    _disparityMap.resize((float*)_outDisparityMap.data.get_buffer(false),
			 header.height, header.width);
}

template <class STEREO, class PIXEL> void
StereoRTC<STEREO, PIXEL>::stereoMatch()
{
    if (_binocular)
    {
	_rectify(_images[0], _images[1],
		 _rectifiedImages[0], _rectifiedImages[1]);
	_stereo(_rectifiedImages[0].cbegin(), _rectifiedImages[0].cend(),
		_rectifiedImages[1].cbegin(), _disparityMap.begin());
    }
    else
    {
	_rectify(_images[0], _images[1], _images[2],
		 _rectifiedImages[0], _rectifiedImages[1], _rectifiedImages[2]);
	_stereo(_rectifiedImages[0].cbegin(), _rectifiedImages[0].cend(),
		_rectifiedImages[0].cend(),   _rectifiedImages[1].cbegin(),
		_rectifiedImages[2].cbegin(), _disparityMap.begin());
    }
}

}	// namespace TU
#endif	// !__TU_STEREOTC_H
