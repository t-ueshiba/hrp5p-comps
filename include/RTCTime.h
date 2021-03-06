/*
 *  $Id$
 */
#if !defined(RTCTIME_H)
#define RTCTIME_H

#include <rtm/idl/BasicDataType.hh>
#include <iostream>
#include <chrono>

namespace TU
{
template <class REP> inline std::ostream&
operator <<(std::ostream& out, const std::chrono::duration<REP, std::micro>& d)
{
    const auto	usec =  d.count()	     % 1000;
    const auto	msec = (d.count() /    1000) % 1000;
    const auto	sec  =  d.count() / 1000000;
    return out << sec << '.' << msec << '.' << usec;
}

template <class REP> inline std::ostream&
operator <<(std::ostream& out, const std::chrono::duration<REP, std::nano>& d)
{
    const auto	nsec =  d.count()		% 1000;
    const auto	usec = (d.count() /	  1000) % 1000;
    const auto	msec = (d.count() /    1000000) % 1000;
    const auto	sec  =  d.count() / 1000000000;
    return out << sec << '.' << msec << '.' << usec << '.' << nsec;
}

template <class CLOCK, class DURATION> inline std::ostream&
operator <<(std::ostream& out,
	    const std::chrono::time_point<CLOCK, DURATION>& tp)
{
    return out << tp.time_since_epoch();
}
    
inline std::ostream&
operator <<(std::ostream& out, const timeval& tm)
{
    using	std::chrono::microseconds;
    
    return out << microseconds(1000000*microseconds::rep(tm.tv_sec)
				     + microseconds::rep(tm.tv_usec));
}
    
inline std::ostream&
operator <<(std::ostream& out, const timespec& tm)
{
    using	std::chrono::nanoseconds;
    
    return out << nanoseconds(1000000000*nanoseconds::rep(tm.tv_sec)
				       + nanoseconds::rep(tm.tv_nsec));
}
    
inline std::ostream&
operator <<(std::ostream& out, const RTC::Time tm)
{
    return out << timespec{tm.sec, tm.nsec};
}
    
}
#endif
