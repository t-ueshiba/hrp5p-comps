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
*  class MultiImageViewerPlugin						*
************************************************************************/
class MultiImageViewerPlugin : public cnoid::Plugin
{
  public:
    MultiImageViewerPlugin()						;

    virtual bool	initialize()					;
};

MultiImageViewerPlugin::MultiImageViewerPlugin()
    :Plugin("TU::MultiImageViewer")
{
    require("OpenRTM");
}

bool
MultiImageViewerPlugin::initialize()
{
    using	viewer_type = ImageViewer<Img::TimedImages>;
    
    viewManager().registerClass<viewer_type>(
		      "TU::MultiImageViewer", "Multi image viewer",
		      cnoid::ViewManager::MULTI_OPTIONAL);
    
    return true;
}

}
    
CNOID_IMPLEMENT_PLUGIN_ENTRY(TU::MultiImageViewerPlugin);
