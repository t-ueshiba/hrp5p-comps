/*
 *  $Id$
 */
#ifndef __TU_V_LABELCMD_H
#define __TU_V_LABELCMD_H

#include <QLabel>
#include "CmdDef.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class LabelCmd							*
************************************************************************/
class LabelCmd : public QLabel
{
    Q_OBJECT
    
  public:
    LabelCmd(QWidget* parent, const CmdDef& cmd)			;

  private Q_SLOTS:
    void	onRefresh(CmdVals& ids)					;
    
  private:
    CmdDef::Id const	_id;
};

}
}
#endif
