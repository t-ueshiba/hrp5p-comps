/*
 *  $Id$
 */
#include <cnoid/Plugin>
#include <cnoid/ViewManager>
#include "Img.hh"
#include "ImageViewer.h"

namespace TU
{
/************************************************************************
*  class ImageViewerPlugin						*
************************************************************************/
class ImageViewerPlugin : public cnoid::Plugin
{
  public:
    ImageViewerPlugin()							;

    virtual bool	initialize()					;
};

ImageViewerPlugin::ImageViewerPlugin()
    :Plugin("TU::ImageViewer")
{
    require("OpenRTM");
}

bool
ImageViewerPlugin::initialize()
{
    using	viewer_type = ImageViewer<Img::TimedCameraImage>;
    
    viewManager().registerClass<viewer_type>(
		      "TU::ImageViewer", "Image viewer",
		      cnoid::ViewManager::MULTI_OPTIONAL);
    
    return true;
}

}
    
CNOID_IMPLEMENT_PLUGIN_ENTRY(TU::ImageViewerPlugin);
