/*
 *  $Id$
 */
#ifndef __TU_V_CONTROLPANELVIEW_H
#define __TU_V_CONTROLPANELVIEW_H

#include <cnoid/View>
#include <cnoid/Timer>
#include <cnoid/Signal>
#include <QGridLayout>
#include "ControlPanelRTC.h"
#include "CmdDef.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class CmdPane							*
************************************************************************/
class CmdPane : public QWidget
{
    Q_OBJECT
    
  public:
		CmdPane(QWidget* parent)	;
    virtual	~CmdPane()			;

    const char*	getRTCName()		const	;
    cnoid::SignalProxy<void(void)>
		sigFinalize()			{ return _sigFinalize; }
	
  private:
    void	addCmds(const CmdDefs& cmds)	;
    void	removeCmds()			;
    void	onTimeout()			;
    
  Q_SIGNALS:
    void	refresh(CmdVals& ids)		;
						
  private Q_SLOTS:
    void	onSet(CmdVals& vals)		;
    void	onGet(CmdVals& vals, bool range);
    
  private:
    bool			_ready;
    ControlPanelRTC*		_rtc;
    cnoid::Timer		_timer;
    QGridLayout*	const	_layout;
    cnoid::Signal<void(void)>	_sigFinalize;
};
    
/************************************************************************
*  class ControlPanelView						*
************************************************************************/
class ControlPanelView : public cnoid::View
{
  public:
		ControlPanelView()		;

  private:
    void	onFinalize()			;
    
  private:
    CmdPane* const	_pane;
};
    
}
}
#endif
