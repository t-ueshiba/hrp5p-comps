/*
 *  $Id$
 */
#include <cnoid/Plugin>
#include <cnoid/ViewManager>
#include "ControlPanelView.h"

namespace TU
{
/************************************************************************
*  class ControlPanelPlugin						*
************************************************************************/
class ControlPanelPlugin : public cnoid::Plugin
{
  public:
    ControlPanelPlugin()						;

    virtual bool	initialize()					;
};

ControlPanelPlugin::ControlPanelPlugin()
    :Plugin("TU::ControlPanel")
{
    require("OpenRTM");
}

bool
ControlPanelPlugin::initialize()
{
    viewManager().registerClass<v::ControlPanelView>(
		      "TU::v::ControlPaneView",
		      "Control panel", cnoid::ViewManager::MULTI_OPTIONAL);
    
    return true;
}

}
    
CNOID_IMPLEMENT_PLUGIN_ENTRY(TU::ControlPanelPlugin);
