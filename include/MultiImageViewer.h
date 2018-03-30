/*
 *  $Id$
 */
#ifndef TU_MULTIIMAGEVIEWER_H
#define TU_MULTIIMAGEVIEWER_H

#include <cnoid/View>
#include <cnoid/Timer>
#include <QGridLayout>
#include <cnoid/ViewArea>
#include <MultiImageViewerRTC.h>
#include <vector>
#include "ImageView.h"

namespace TU
{
template <class IMAGES>
MultiImageViewerRTC<IMAGES>*	createMultiImageViewerRTC()		;

/************************************************************************
*  class MultiImageViewer<IMAGES>					*
************************************************************************/
template <class IMAGES>
class MultiImageViewer : public cnoid::View, public std::vector<ImageView*>
{
  private:
    using	array_type = std::vector<ImageView*>;
    
  public:
		MultiImageViewer()		;
    virtual	~MultiImageViewer()		;
    
    using	array_type::size;
    void	resize(size_t nviews)		;

  private:
    void	onTimeout()			;
     
  private:
    MultiImageViewerRTC<IMAGES>*	_rtc;
    cnoid::Timer			_timer;
    QGridLayout* const			_layout;
};

template <class IMAGES>
MultiImageViewer<IMAGES>::MultiImageViewer()
    :View(), array_type(0), _rtc(createMultiImageViewerRTC<IMAGES>()),
     _timer(), _layout(new QGridLayout(this))
{
    if (!_rtc)
	throw std::runtime_error("MultiImageViewer::MultiImageViewer(): failed to create RTC!");

  // このビューにRTCの名前を付ける
    setName(_rtc->getInstanceName());

  // ビュー更新用タイマにコールバックを登録してスタート
    _timer.sigTimeout().connect(std::bind(&MultiImageViewer::onTimeout, this));
    _timer.start(100);

    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);
}

template <class IMAGES>
MultiImageViewer<IMAGES>::~MultiImageViewer()
{
    _timer.stop();

    if (_rtc)
    {
	_rtc->exit();
	RTC::Manager::instance().cleanupComponents();
    }
}
    
template <class IMAGES> void
MultiImageViewer<IMAGES>::resize(size_t nviews)
{
    if (nviews == size())
	return;
    
    for (size_t i = 0; i < size(); ++i)
    {
	_layout->removeWidget((*this)[i]);
	delete (*this)[i];
    }

    array_type::resize(nviews);

    for (size_t i = 0; i < size(); ++i)
    {
	(*this)[i] = new ImageView(this);
	_layout->addWidget((*this)[i], i/2, i%2);
    }

    setLayout(_layout);
}
    
template <class IMAGES> void
MultiImageViewer<IMAGES>::onTimeout()
{
    if (_rtc->isExiting())		// RTCが死んでいたら...
    {
	_rtc = nullptr;
	_timer.stop();			// タイマを止めてここの再実行を防ぐ
	viewArea()->removeView(this);
    }
    else if (_rtc->setImages(this))	// RTCが持っている画像を
    {					// 各ビューにセットできたら...
	for (size_t i = 0; i < size(); ++i)
	    (*this)[i]->viewport()->update();	// 各ビューを更新
    }
}
    
}
#endif	// !TU_MULTIIMAGEVIEWER_H
