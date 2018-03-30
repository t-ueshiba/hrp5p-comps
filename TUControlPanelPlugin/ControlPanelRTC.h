/*
 *  $Id$
 */
#ifndef __TU_CONTROLPANELRTC_H
#define __TU_CONTROLPANELRTC_H

#include <mutex>
#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include "Cmd.hh"
#include "CmdDef.h"

namespace TU
{
/************************************************************************
*  class ControlPanelRTC						*
************************************************************************/
class ControlPanelRTC : public RTC::DataFlowComponentBase
{
  public:
    ControlPanelRTC(RTC::Manager* manager)				;

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;
#ifdef DEBUG
    virtual RTC::ReturnCode_t	onFinalize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onStartup(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onShutdown(RTC::UniqueId ec_id)		;
#endif
    bool			isExiting()			const	;
    bool			ready()				const	;
    bool			getCmds(v::CmdDefs& cmds)	const	;
    v::CmdVals			setValues(const v::CmdVals& vals)	;
    v::CmdVals			getValues(const v::CmdVals& ids,
					  bool range)			;
    
  private:
    mutable std::mutex			_mutex;
    bool				_ready;
    v::CmdDefs				_cmds;
    RTC::CorbaConsumer<Cmd::Controller>	_command;	// service consumer
    RTC::CorbaPort			_commandPort;	// service port
};

inline bool
ControlPanelRTC::isExiting() const
{
    std::unique_lock<std::mutex>	lock(_mutex);
    return m_exiting;
}

inline bool
ControlPanelRTC::ready() const
{
    std::unique_lock<std::mutex>	lock(_mutex);
    return _ready;
}

inline bool
ControlPanelRTC::getCmds(v::CmdDefs& cmds) const
{
    std::unique_lock<std::mutex>	lock(_mutex);
    if (_ready)
	cmds = _cmds;
    
    return _ready;
}

}
#endif	// !__TU_CONTROLPANELRTC_H
