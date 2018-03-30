/*
 *  $Id$
 */
#ifndef __TU_V_TOGGLEBUTTONCMD_H
#define __TU_V_TOGGLEBUTTONCMD_H

#include <QPushButton>
#include "CmdDef.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class ToggleButtonCmd						*
************************************************************************/
class ToggleButtonCmd : public QPushButton
{
    Q_OBJECT
    
  public:
    ToggleButtonCmd(QWidget* parent, const CmdDef& cmd)			;

  Q_SIGNALS:
    void	set(CmdVals& vals)					;
    void	get(CmdVals& vals, bool range)				;
									
  private Q_SLOTS:
    void	onClicked(bool)						;
    void	onRefresh(CmdVals& ids)					;
    void	onRead(CmdVals& vals)					;
    void	onWrite(CmdVals& vals, bool range)			;
    
  private:
    const CmdDef::Id	_id;
};

}
}
#endif
