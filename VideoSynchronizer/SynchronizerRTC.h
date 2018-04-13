/*
 *  $Id$
 */
#ifndef SYNCHRONIZERRTC_H
#define SYNCHRONIZERRTC_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataType.hh>
#include <boost/circular_buffer.hpp>
#include <chrono>
#include <mutex>
#ifdef DEBUG
#  include "RTCTime.h"
#endif

namespace TU
{
/************************************************************************
*  class SynchronizerRTC<PRIMARY, SECONDARY>				*
************************************************************************/
template <class PRIMARY, class SECONDARY>
class SynchronizerRTC : public RTC::DataFlowComponentBase
{
  public:
    using primary_type	 = PRIMARY;
    using secondary_type = SECONDARY;

  public:
    SynchronizerRTC(RTC::Manager* manager)				;

    virtual RTC::ReturnCode_t	onInitialize()				;
    virtual RTC::ReturnCode_t	onActivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onExecute(RTC::UniqueId ec_id)		;
    virtual RTC::ReturnCode_t	onDeactivated(RTC::UniqueId ec_id)	;
    virtual RTC::ReturnCode_t	onAborting(RTC::UniqueId ec_id)		;

  private:
    void	initializeConfigurations()				;
    bool	select(RTC::Time tm)					;
    static std::chrono::nanoseconds
		nsec(RTC::Time tm)
		{
		    return std::chrono::nanoseconds(1000000000*tm.sec +
						    tm.nsec);
		}
    static bool	comp(const secondary_type& q, RTC::Time tm)
		{
		    return nsec(q.tm) < nsec(tm);
		}
    
  private:
    primary_type				_p;
    RTC::InPort<primary_type>			_pIn;
    secondary_type				_q;
    RTC::InPort<secondary_type>			_qIn;
    size_t					_bufSize;
    boost::circular_buffer<secondary_type>	_qBuf;
    RTC::OutPort<primary_type>			_pOut;
    secondary_type				_qSelected;
    RTC::OutPort<secondary_type>		_qOut;
    mutable std::mutex				_mutex;
};

template <class PRIMARY, class SECONDARY>
SynchronizerRTC<PRIMARY, SECONDARY>::SynchronizerRTC(RTC::Manager* manager)
    :RTC::DataFlowComponentBase(manager),
     _p(),
     _pIn("primary", _p),
     _q(),
     _qIn("secondary", _q),
     _bufSize(100),
     _qBuf(_bufSize),
     _pOut("primaryOut", _p),
     _qSelected(),
     _qOut("secondaryOut", _qSelected),
     _mutex()
{
}

template <class PRIMARY, class SECONDARY> RTC::ReturnCode_t
SynchronizerRTC<PRIMARY, SECONDARY>::onInitialize()
{
#ifdef DEBUG
    std::cerr << "SynchronizerRTC<PRIMARY, SECONDARY>::onInitialize"
	      << std::endl;
#endif
    initializeConfigurations();
    
    addInPort("primary",       _pIn);
    addInPort("secondary",     _qIn);
    addOutPort("primaryOut",   _pOut);
    addOutPort("secondaryOut", _qOut);
    
    return RTC::RTC_OK;
}

template <class PRIMARY, class SECONDARY> RTC::ReturnCode_t
SynchronizerRTC<PRIMARY, SECONDARY>::onActivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "SynchronizerRTC<PRIMARY, SECONDARY>::onActivated"
	      << std::endl;
#endif
    _qBuf.resize(_bufSize);
    
    return RTC::RTC_OK;
}

template <class PRIMARY, class SECONDARY> RTC::ReturnCode_t
SynchronizerRTC<PRIMARY, SECONDARY>::onExecute(RTC::UniqueId ec_id)
{
    std::unique_lock<std::mutex>	lock(_mutex);

    if (_qIn.isNew())
    {
	_qIn.read();		// ポート _poseIn から _pose に関節角を読み込む
	_qBuf.push_back(_q);
#ifdef DEBUG
	static size_t	n = 0;
	if (n % 10 == 0)
	    std::cerr << "secondary: " << _q.tm << std::endl;
	++n;
#endif
    }

    if (_pIn.isNew())
    {
	_pIn.read();

	if (select(_p.tm))
	{
	    _pOut.write();
	    _qOut.write();
	}
    }

    return RTC::RTC_OK;
}

template <class PRIMARY, class SECONDARY>
RTC::ReturnCode_t
SynchronizerRTC<PRIMARY, SECONDARY>::onDeactivated(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "SynchronizerRTC<PRIMARY, SECONDARY>::onDeactivated"
	      << std::endl;
#endif
    return RTC::RTC_OK;
}

template <class PRIMARY, class SECONDARY>
RTC::ReturnCode_t
SynchronizerRTC<PRIMARY, SECONDARY>::onAborting(RTC::UniqueId ec_id)
{
#ifdef DEBUG
    std::cerr << "SynchronizerRTC<PRIMARY, SECONDARY>::onAborting"
	      << std::endl;
#endif
    return RTC::RTC_OK;
}

template <class PRIMARY, class SECONDARY> bool
SynchronizerRTC<PRIMARY, SECONDARY>::select(RTC::Time tm)
{
#ifdef DEBUG
    if (_qBuf.empty())
	return false;

    std::cerr <<   "Search     " << tm
	      << "\n       in [" << _qBuf.front().tm << ", " << _qBuf.back().tm
	      << "]"
	      << std::endl;
#endif
    const auto	q = std::lower_bound(_qBuf.begin(), _qBuf.end(), tm, comp);

    if (q == _qBuf.begin() || q == _qBuf.end())
    {
#ifdef DEBUG
	std::cerr << "Failed to sync.\n" << std::endl;
#endif
	return false;
    }

#ifdef DEBUG
    std::cerr << q->tm << "selected.\n" << std::endl;
#endif
    _qSelected = *q;
    return true;
}

}	// namespace TU
#endif	// SYNCHRONIZERRTC_H
