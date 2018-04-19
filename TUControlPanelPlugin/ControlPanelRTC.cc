/*
 *  $Id$
 */
#include <sstream>
#include <boost/archive/text_iarchive.hpp>
#include "ControlPanelRTC.h"

/************************************************************************
*  static data								*
************************************************************************/
// Module specification
static const char* controlpanel_spec[] =
{
    "implementation_id",	"ControlPanel",
    "type_name",		"ControlPanel",
    "description",		"generic device controller",
    "version",			"1.0.0",
    "vendor",			"t.ueshiba@aist.go.jp",
    "category",			"controller",
    "activity_type",		"SPORADIC",
    "kind",			"DataFlowComponent",
    "max_instance",		"0",
    "language",			"C++",
    "lang_type",		"compile",
    "exec_cxt.periodic.rate",	"30.0",
    ""
};

namespace TU
{
/************************************************************************
*  global functions							*
************************************************************************/
ControlPanelRTC*
createControlPanelRTC()
{
  // OpenRTMPluginによって立てられた既存のRTCマネージャを獲得
    auto&	manager = RTC::Manager::instance();

  // ControlPanelコンポーネントを立ち上げ
    coil::Properties	profile(controlpanel_spec);
    manager.registerFactory(profile,
			    RTC::Create<ControlPanelRTC>,
			    RTC::Delete<ControlPanelRTC>);

    return dynamic_cast<ControlPanelRTC*>(manager
					  .createComponent("ControlPanel"));
}
    
/************************************************************************
*  class ControlPanelRTC						*
************************************************************************/
ControlPanelRTC::ControlPanelRTC(RTC::Manager* manager)
    : RTC::DataFlowComponentBase(manager),
      _mutex(),
      _ready(false),
      _cmds(),
      _command(),
      _commandPort("Command")
{
}

RTC::ReturnCode_t
ControlPanelRTC::onInitialize()
{
#ifdef DEBUG
    std::cerr << "ControlPanelRTC::onInitialize" << std::endl;
#endif
    _commandPort.registerConsumer("Command", "Cmd::Controller", _command);
    addPort(_commandPort);

    return RTC::RTC_OK;
}

RTC::ReturnCode_t
ControlPanelRTC::onExecute(RTC::UniqueId ec_id)
{
  //#ifdef DEBUG
#if 0
    static int	n = 0;
    std::cerr << "ControlPanelRTC::onExecute" << ": " << n++ << std::endl;
#endif
    if (!_ready)
    {
	try
	{
	    std::lock_guard<std::mutex>	lock(_mutex);
	    CORBA::String_var			s = _command->getCmds();
	    std::istringstream			iss(s._retn());
	    boost::archive::text_iarchive	iar(iss);
	    iar >> BOOST_SERIALIZATION_NVP(_cmds);

	    _ready = true;
	}
	catch (const CORBA::SystemException& err)
	{
#ifdef DEBUG
	    std::cerr << "ControlPanelRTC::onExecute(): " << err._name()
		      << std::endl;
#endif
	}
	catch (...)
	{
	    std::cerr << "ControlPanelRTC::onExecute(): Unknown error."
		      << std::endl;

	    return RTC::RTC_ERROR;
	}
    }
    
    return RTC::RTC_OK;
}

RTC::ReturnCode_t
ControlPanelRTC::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "ControlPanelRTC::onDeactivated" << std::endl;
#endif
    std::lock_guard<std::mutex>	lock(_mutex);
    _ready = false;
    
    return RTC::RTC_OK;
}

RTC::ReturnCode_t
ControlPanelRTC::onAborting(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "ControlPanelRTC::onAborting" << std::endl;
#endif
    std::lock_guard<std::mutex>	lock(_mutex);
    _ready = false;
    
    return RTC::RTC_OK;
}

#ifdef DEBUG
RTC::ReturnCode_t
ControlPanelRTC::onFinalize()
{
    std::cerr << "ControlPanelRTC::onFinalize" << std::endl;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t
ControlPanelRTC::onActivated(RTC::UniqueId ec_id)
{
    std::cerr << "ControlPanelRTC::onActivated" << std::endl;
    
    return RTC::RTC_OK;
}

RTC::ReturnCode_t
ControlPanelRTC::onStartup(RTC::UniqueId ec_id)
{
    std::cerr << "ControlPanelRTC::onStartup" << std::endl;

    return RTC::RTC_OK;
}

RTC::ReturnCode_t
ControlPanelRTC::onShutdown(RTC::UniqueId ec_id)
{
    std::cerr << "ControlPanelRTC::onShutdown" << std::endl;

    return RTC::RTC_OK;
}
#endif

v::CmdVals
ControlPanelRTC::setValues(const v::CmdVals& vals)
{
    v::CmdVals	ids;
    try
    {
      // vals の各値を vals_ に逆順に収める
	Cmd::Values	vals_;
	vals_.length(vals.size());
	size_t	i = 0;
	for (auto val = vals.crbegin(); val != vals.crend(); ++val)
	    vals_[i++] = {val->i(), val->f()};

      // サーバに vals_ を送ってセットし，返されたIDを ids_ に受ける
	Cmd::Values_var	ids_ = _command->setValues(vals_);

      // ids_ の各IDを ids に逆順に収める
	for (size_t i = ids_->length(); i-- > 0; )
	    ids.push_back({ids_[i].i, ids_[i].f});
    }
    catch (const CORBA::SystemException& err)
    {
	std::cerr << "ControlPanelRTC::setValues(): " << err._name()
		  << std::endl;
    }
    catch (...)
    {
	std::cerr << "Unknown error." << std::endl;
    }

    return ids;
}

v::CmdVals
ControlPanelRTC::getValues(const v::CmdVals& ids, bool range)
{
    v::CmdVals	vals;
    try
    {
      // ids の各IDを ids_ に逆順に収める
	Cmd::Values	ids_;
	ids_.length(ids.size());
	size_t	i = 0;
	for (auto id = ids.crbegin(); id != ids.crend(); ++id)
	    ids_[i++] = {id->i(), id->f()};
    
      // サーバに ids_ を送ってその値を調べ，それを vals_ に受ける
	Cmd::Values_var	vals_ = _command->getValues(ids_, range);

      // vals_ の各値を vals に逆順に収める
	for (size_t i = vals_->length(); i-- > 0; )
	    vals.push_back({vals_[i].i, vals_[i].f});
    }
    catch (const CORBA::SystemException& err)
    {
	std::cerr << "ControlPanelRTC::getValues(): " << err._name()
		  << std::endl;
    }
    catch (...)
    {
	std::cerr << "Unknown error." << std::endl;
    }

    return vals;
}
    
}
