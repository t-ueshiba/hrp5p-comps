/*
 *  $Id$
 */
#ifndef TU_IMAGEVIEWER_H
#define TU_IMAGEVIEWER_H

#include <cnoid/View>
#include <cnoid/Timer>
#include <QGridLayout>
#include <cnoid/ViewArea>
#include <vector>
#include "ImageViewerRTC.h"
#include "ImageView.h"

namespace TU
{
template <class IMAGE>
ImageViewerRTC<IMAGE>*	createImageViewerRTC()		;

/************************************************************************
*  class ImageViewer<IMAGE>						*
************************************************************************/
template <class IMAGE>
class ImageViewer : public cnoid::View, public std::vector<ImageView*>
{
  private:
    using	array_type = std::vector<ImageView*>;
    
  public:
		ImageViewer()		;
    virtual	~ImageViewer()		;
    
    using	array_type::size;
    void	resize(size_t nviews)	;

  private:
    void	onTimeout()		;
     
  private:
    ImageViewerRTC<IMAGE>*	_rtc;
    cnoid::Timer		_timer;
    QGridLayout* const		_layout;
};

template <class IMAGE>
ImageViewer<IMAGE>::ImageViewer()
    :View(), array_type(0), _rtc(createImageViewerRTC<IMAGE>()),
     _timer(), _layout(new QGridLayout(this))
{
    if (!_rtc)
	throw std::runtime_error("ImageViewer::ImageViewer(): failed to create RTC!");

  // このビューにRTCの名前を付ける
    setName(_rtc->getInstanceName());

  // ビュー更新用タイマにコールバックを登録してスタート
    _timer.sigTimeout().connect(std::bind(&ImageViewer::onTimeout, this));
    _timer.start(100);

    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);
}

template <class IMAGE>
ImageViewer<IMAGE>::~ImageViewer()
{
    _timer.stop();

    if (_rtc)
    {
	_rtc->exit();
	RTC::Manager::instance().cleanupComponents();
    }
}
    
template <class IMAGE> void
ImageViewer<IMAGE>::resize(size_t nviews)
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
    
template <class IMAGE> void
ImageViewer<IMAGE>::onTimeout()
{
    if (_rtc->isExiting())		// RTCが死んでいたら...
    {
	_timer.stop();			// タイマを止めてここの再実行を防ぐ
	_rtc = nullptr;
	viewArea()->removeView(this);
    }
    else if (_rtc->setImage(*this))	// RTCが持っている画像を
    {					// 各ビューにセットできたら...
	for (size_t i = 0; i < size(); ++i)
	    (*this)[i]->viewport()->update();	// 各ビューを更新
    }
}
    
}
#endif	// !TU_IMAGEVIEWER_H
