/*
 *  $Id$
 */
#include "MultiImageViewer.h"

/************************************************************************
*  static data								*
************************************************************************/
// Module specification
static const char* multiimageviewer_spec[] =
{
    "implementation_id",	"MultiImageViewer",
    "type_name",		"MultiImageViewer",
    "description",		"Viewing multiple image streams",
    "version",			"1.0.0",
    "vendor",			"t.ueshiba@aist.go.jp",
    "category",			"sensor",
    "activity_type",		"PERIODIC",
    "kind",			"DataFlowComponent",
    "max_instance",		"0",
    "language",			"C++",
    "lang_type",		"compile",
    ""					// <-- Important! End of Spec. mark
};

/************************************************************************
*  global functions							*
************************************************************************/
extern "C"
{
void
MultiImageViewerInit(RTC::Manager* manager)
{
    coil::Properties	profile(multiimageviewer_spec);
    manager->registerFactory(profile,
                             RTC::Create<MultiImageViewer>,
			     RTC::Delete<MultiImageViewer>);
}
};

/************************************************************************
*  class MultiImageViewer						*
************************************************************************/
MultiImageViewer::MultiImageViewer(RTC::Manager* manager)
    : RTC::DataFlowComponentBase(manager),
      _images(),
      _imagesIn("TimedImages", _images),
      _argc(0),
      _vapp(_argc, 0),
      _win(_vapp,
	   multiimageviewer_spec[1], TU::v::Colormap::RGBColor, 16, 0, 0),
      _kernels(0)
{
}

MultiImageViewer::~MultiImageViewer()
{
}

RTC::ReturnCode_t
MultiImageViewer::onInitialize()
{
    std::cerr << "MultiImageViewer::onInitialize" << std::endl;
    
    addInPort("TimedImages", _imagesIn);
    
    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewer::onActivated(RTC::UniqueId ec_id)
{
    std::cerr << "MultiImageViewer::onActivated" << std::endl;
  //_win.show();

  //_images.length(0);
    
    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewer::onDeactivated(RTC::UniqueId ec_id)
{
    std::cerr << "MultiImageViewer::onDeactivated" << std::endl;

    for (auto& kernel : _kernels)
	kernel = nullptr;
    _kernels.resize(0);
    
    _win.hide();
    
    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewer::onExecute(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    static int	n = 0;
    std::cerr << "MultiImageViewer::onExecute";
#endif
    if (_imagesIn.isNew())
    {
      // 1フレーム分の多視点画像を読み込む
      //_imagesIn.read();

	do {
	    _imagesIn.read();
	} while (_imagesIn.isNew());

      // 視点数とカーネル数を一致させる
	if (_kernels.size() == 0)
	    _kernels.resize(_images.headers.length());

      // 各視点毎に画像データを読み込んで表示
	bool	updated = false;
	
	for (size_t i = 0; i < _kernels.size(); ++i)
	{
	    const auto&	header = _images.headers[i];
	    auto&	kernel = _kernels[i];

	    if (kernel == nullptr || !kernel->conform(header))
	    {
		switch (header.format)
		{
		  case Img::MONO_8:
		    kernel.reset(new Kernel<u_char>(_win, header.format));
		    break;
		  case Img::YUV_411:
		    kernel.reset(new Kernel<TU::YUV411>(_win, header.format));
		    break;
		  case Img::YUV_422:
		    kernel.reset(new Kernel<TU::YUV422>(_win, header.format));
		    break;
		  case Img::YUV_444:
		    kernel.reset(new Kernel<TU::YUV444>(_win, header.format));
		    break;
		  case Img::RGB_24:
		    kernel.reset(new Kernel<TU::RGB>(_win, header.format));
		    break;
		  default:
		    std::cerr << "Unsupported format!!" << std::endl;
		    return RTC::RTC_ERROR;
		}

		kernel->place(i % 2, i / 2, 1, 1);
		updated = true;
	    }
	}

	if (updated)
	    _win.show();

	const u_char*	data = _images.data.get_buffer();
	for (size_t i = 0; i < _kernels.size(); ++i)
	{
	    _kernels[i]->display(_images.headers[i], data);
	    data += _images.headers[i].size;
	}

      //_vapp.step();
#ifdef DEBUG
	std::cerr << ": " << n++;
#endif
    }
#ifdef DEBUG
    std::cerr << std::endl;
#endif
    return RTC::RTC_OK;
}

#ifdef DEBUG
RTC::ReturnCode_t
MultiImageViewer::onFinalize()
{
    std::cerr << "MultiImageViewer::onFinalize" << std::endl;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewer::onStartup(RTC::UniqueId ec_id)
{
    std::cerr << "MultiImageViewer::onStartup" << std::endl;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t
MultiImageViewer::onShutdown(RTC::UniqueId ec_id)
{
    std::cerr << "MultiImageViewer::onShutdown" << std::endl;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t MultiImageViewer::onAborting(RTC::UniqueId ec_id)
{
    return RTC::RTC_OK;
}
#endif


