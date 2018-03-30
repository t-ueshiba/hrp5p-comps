/*
 *  $Id$
 */
#include "LabelCmd_.h"
#include "ButtonCmd_.h"
#include "ToggleButtonCmd_.h"
#include "GroupBoxCmd_.h"
#include "SliderCmd_.h"

namespace TU
{
namespace v
{
QWidget*
createCmd(QWidget* parent, const CmdDef& cmd)
{
    switch (cmd.type)
    {
      case CmdDef::C_Label:
	return new LabelCmd(parent, cmd);
      case CmdDef::C_Button:
	return new ButtonCmd(parent, cmd);
      case CmdDef::C_ToggleButton:
	return new ToggleButtonCmd(parent, cmd);
      case CmdDef::C_GroupBox:
	return new GroupBoxCmd(parent, cmd);
      case CmdDef::C_Slider:
	return new SliderCmd(parent, cmd);
      default:
	break;
    }

    throw std::runtime_error("createCmd(): unknwon command type!");
    
    return nullptr;
}

}
}
