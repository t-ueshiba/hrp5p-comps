/*
 *  $Id$
 */
#ifndef __TU_V_GROUPBOXCMD_H
#define __TU_V_GROUPBOXCMD_H

#include <QGroupBox>
#include <QGridLayout>
#include <QAbstractButton>
#include "CmdDef.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class GroupBoxCmd							*
************************************************************************/
class GroupBoxCmd : public QGroupBox
{
    Q_OBJECT
    
  public:
    GroupBoxCmd(QWidget* parent, const CmdDef& cmd)			;

  Q_SIGNALS:
    void	set(CmdVals& vals)					;
    void	get(CmdVals& vals, bool range)				;
									
  private Q_SLOTS:
    void	onClicked(bool)						;
    void	onRefresh(CmdVals& ids)					;
    void	onRead(CmdVals& vals)					;
    void	onWrite(CmdVals& vals, bool range)			;
    
  private:
    CmdDef::Id			const	_id;
    CmdDefs			const	_subcmds;
    std::vector<QAbstractButton*>	_buttons;
};

}
}
#endif
