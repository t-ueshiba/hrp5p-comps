/*
 *  $Id$
 */
#ifndef __TU_V_BUTTONCMD_H
#define __TU_V_BUTTONCMD_H

#include <QPushButton>
#include "Dialog_.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class ButtonCmd							*
************************************************************************/
class ButtonCmd : public QPushButton
{
    Q_OBJECT
    
  public:
    ButtonCmd(QWidget* parent, const CmdDef& cmd)			;

  Q_SIGNALS:
    void	set(CmdVals& vals)					;
    void	get(CmdVals& vals, bool range)				;
    void	refresh(CmdVals& ids)					;
    void	read(CmdVals& vals)					;
    void	write(CmdVals& vals, bool range)			;

  private Q_SLOTS:
    void	onPressed()						;
    void	onSet(CmdVals& vals)					;
    void	onGet(CmdVals& vals, bool range)			;
    void	onRefresh(CmdVals& ids)					;
    void	onRead(CmdVals& vals)					;
    void	onWrite(CmdVals& vals, bool range)			;

  private:
    CmdDef::Id const	_id;
    Dialog*		_dialog;
};

}
}
#endif
