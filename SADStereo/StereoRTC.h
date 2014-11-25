/*
 *  $Id$
 */
#ifndef __TU_STEREORTC_H
#define __TU_STEREORTC_H

#include <fstream>
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
#include "Img.hh"
#include "CmdSVC_impl.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
template <class T> static Img::PixelFormat
pixelFormat()
{
    return Img::MONO_8;
    
}
template <> Img::PixelFormat
pixelFormat<YUV411>()
{
    return Img::YUV_411;
}
template <> Img::PixelFormat
pixelFormat<YUV422>()
{
    return Img::YUV_422;
}
template <> Img::PixelFormat
pixelFormat<YUV444>()
{
    return Img::YUV_444;
}
template <> Img::PixelFormat
pixelFormat<RGB>()
{
    return Img::RGB_24;
}
template <> Img::PixelFormat
pixelFormat<float>()
{
    return Img::FLT;
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

template <class T> static void
mapImage(const Img::TimedImage& inImage, Image<T>& image)
{
    copyCalib(inImage, image);
    image.resize((T*)inImage.data.get_buffer(), inImage.height, inImage.width);
}

template <class T> static void
allocateImage(const RTC::Time& tm, size_t width, size_t height,
	      Image<T>& rectifiedImage, Img::TimedImage& outImage)
{
    outImage.tm	    = tm;
    outImage.format = pixelFormat<T>();
    outImage.width  = width;
    outImage.height = height;
    copyCalib(rectifiedImage, outImage);

    outImage.data.length(sizeof(T) * outImage.width * outImage.height);

    rectifiedImage.resize((T*)outImage.data.get_buffer(),
			  outImage.height, outImage.width);
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
				      bool initialization_needed=false)	;
    const params_type&	getParameters()				const	;
    void		initializeRectification()			;
    
  private:
    void		initializeConfigurations()			;
    void		stereoMatch()					;
    
  private:
    mutable coil::Mutex			_mutex;
    bool				_initialized;
    bool				_binocular;
    double				_scale;
    Rectify				_rectify;
    stereo_type				_stereo;
    Image<pixel_type>			_images[3];
    Image<pixel_type>			_rectifiedImages[3];
    Image<float>			_disparityMap;
    std::string				_stereoParams;	// config var.
    Img::TimedImages			_inImages;	// in data
    RTC::InPort<Img::TimedImages>	_inImagesPort;	// in data poprt
    Img::TimedImages			_outImages;	// out data
    RTC::OutPort<Img::TimedImages>	_outImagesPort;	// out data poprt
    v::CmdSVC_impl<StereoRTC>		_command;	// service provider
    RTC::CorbaPort			_commandPort;	// service port
};

template <class STEREO, class PIXEL> RTC::ReturnCode_t
StereoRTC<STEREO, PIXEL>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "StereoRTC<STEREO, PIXEL>::onInitialize" << std::endl;
#endif
    initializeConfigurations();
    
    addInPort( "TimedImages", _inImagesPort);
    addOutPort("TimedImages", _outImagesPort);

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
    using namespace	std;
    
    if (_inImagesPort.isNew())
    {
	coil::Guard<coil::Mutex>	guard(_mutex);

	do
	{
	    _inImagesPort.read();	// 画像データ読み込み
	}
	while (_inImagesPort.isNew());

	if (!_initialized)		// 初期化されていなかったら...
	{
	    if (_inImages.length() < 2)	// 画像数をチェック
	    {
		cerr << "Two or more images required!" << endl;
		return RTC::RTC_ERROR;
	    }
	    else if (_inImages.length() == 2)
		_binocular = true;
	    else
		_binocular = false;
    
	    initializeRectification();	// 初期化する
	    _initialized = true;
	}
	else
	{
	    _images[0].resize((pixel_type*)_inImages[0].data.get_buffer(),
			      _inImages[0].height, _inImages[0].width);
	    _images[1].resize((pixel_type*)_inImages[1].data.get_buffer(),
			      _inImages[1].height, _inImages[1].width);
	    if (!_binocular)
		_images[2].resize((pixel_type*)_inImages[2].data.get_buffer(),
				  _inImages[2].height, _inImages[2].width);
	}

	stereoMatch();			// ステレオマッチング

	_outImagesPort.write();
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
    bool	binocular_old = _binocular;
    _binocular = (binocular || _inImages.length() <= 2);
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
					bool initialization_needed)
{
    coil::Guard<coil::Mutex>	guard(_mutex);
    _stereo.setParameters(params);
    if (initialization_needed)
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
  // 入力画像を内部入力画像にマップ
    mapImage(_inImages[0], _images[0]);
    mapImage(_inImages[1], _images[1]);

  // rectificationを初期化
    if (_binocular)
    {
	_rectify.initialize(_images[0], _images[1],
			    _scale,
			    _stereo.getParameters().disparitySearchWidth,
			    _stereo.getParameters().disparityMax);

      //_outImages.length(3);
	_outImages.length(1);
    }
    else
    {
	mapImage(_inImages[2], _images[2]);

	_rectify.initialize(_images[0], _images[1], _images[2],
			    _scale,
			    _stereo.getParameters().disparitySearchWidth,
			    _stereo.getParameters().disparityMax);

      // rectifyされた上画像のカメラ行列をセット
	_rectifiedImages[2].P  = _rectify.H(2) * _images[2].P;
	_rectifiedImages[2].P /= _rectifiedImages[2].P[2](0, 3).length();

      // rectifyされた上出力画像を確保し，rectifyされた内部上画像にマップ
      /*
	allocateImage(_inImages[2].tm, _rectify.width(2), _rectify.height(2),
		      _rectifiedImages[2], _outImages[2]);
      */

      //_outImages.length(4);
	_outImages.length(1);
    }

  // rectifyされた左・右画像および視差マップのカメラ行列をセット
    _rectifiedImages[0].P  = _rectify.H(0) * _images[0].P;
    _rectifiedImages[0].P /= _rectifiedImages[0].P[2](0, 3).length();
    _rectifiedImages[1].P  = _rectify.H(1) * _images[1].P;
    _rectifiedImages[1].P /= _rectifiedImages[1].P[2](0, 3).length();
    _disparityMap.P	   = _rectifiedImages[0].P;

  // rectifyされた左・右出力画像を確保し，rectifyされた内部左・右画像にマップ
  /*
    allocateImage(_inImages[0].tm, _rectify.width(0), _rectify.height(0),
		  _rectifiedImages[0], _outImages[0]);
    allocateImage(_inImages[1].tm, _rectify.width(1), _rectify.height(1),
		  _rectifiedImages[1], _outImages[1]);
  */

  // 視差画像を確保し，内部視差画像にマップ
  /*
    allocateImage(_inImages[0].tm, _rectify.width(0), _rectify.height(0),
		  _disparityMap, _outImages[_binocular ? 2 : 3]);
  */
    allocateImage(_inImages[0].tm, _rectify.width(0), _rectify.height(0),
		  _disparityMap, _outImages[0]);
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

}
#endif	// !__TU_STEREOTC_H
