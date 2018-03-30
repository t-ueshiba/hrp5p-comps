/*
 *  $Id$
 */
#ifndef __TU_V_MENU_H
#define __TU_V_MENU_H

#include <QMenu>
#include "Dialog_.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class Menu								*
************************************************************************/
class Menu : public QMenu
{
    Q_OBJECT

  public:
    Menu(QWidget* parent, const CmdDef& cmd)				;

  Q_SIGNALS:
    void	set(CmdVals& vals)					;
    void	get(CmdVals& vals, bool range)				;
    void	refresh(CmdVals& ids)					;
    void	read(CmdVals& vals)					;
    void	write(CmdVals& vals, bool range)			;

  private Q_SLOTS:
    void	onSet(CmdVals& vals)					;
    void	onGet(CmdVals& vals, bool range)			;
    void	onRefresh(CmdVals& ids)					;
    void	onRead(CmdVals& vals)					;
    void	onWrite(CmdVals& vals, bool range)			;
    
  private:
    CmdDef::Id const	_id;
};
    
/************************************************************************
*  class MenuItem							*
************************************************************************/
class MenuItem : public QAction
{
    Q_OBJECT
	
  public:
    MenuItem(QWidget* parent, const CmdDef& cmd)			;

  Q_SIGNALS:
    void	set(CmdVals& vals)					;
    void	read(CmdVals& vals)					;
    void	write(CmdVals& vals, bool range)			;
								
  private Q_SLOTS:
    void	onTriggered()						;
    void	onRead(CmdVals& vals)					;
    void	onWrite(CmdVals& vals, bool range)			;
    
  private:
    CmdDef::Id const	_id;
    Dialog*		_dialog;
};
    
}
}
#endif
