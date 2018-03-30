/*
 *  $Id$
 */
#include "LabelCmd_.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class LabelCmd							*
************************************************************************/
LabelCmd::LabelCmd(QWidget* parent, const CmdDef& cmd)
    :QLabel(tr(cmd.name.c_str()), parent), _id(cmd.id)
{
}

void
LabelCmd::onRefresh(CmdVals& ids)
{
}
    
}
}
