/*
 *  $Id$
 */
#ifndef __TU_IMAGEVIEW_H
#define __TU_IMAGEVIEW_H

#include <QImage>
#include <QGraphicsView>
#include <QMenu>
#include <cnoid/Signal>

namespace TU
{
/************************************************************************
*  class ImageView							*
************************************************************************/
class ImageView : public QGraphicsView
{
    Q_OBJECT
    
  public:
    ImageView(QWidget* parent)						;

    template <class FORMAT>
    void	setImage(FORMAT format,
			 size_t width, size_t height, const uchar* data);
    
  private:
    void	paintEvent(QPaintEvent* event)				;

  private Q_SLOTS:
    void	setScale(qreal scale)					;
    void	showContextMenu(const QPoint& p)			;
    
  private:
    QVector<uchar>	_data;
    QImage		_qimage;
    QVector<QRgb>	_colors;
    QVector<QRgb>	_colorsF;
    QMenu* const	_menu;
    bool		_fit;
    qreal		_scale;
};
    
/************************************************************************
*  class MenuItem							*
************************************************************************/
class MenuItem : public QAction
{
    Q_OBJECT
	
  public:
    MenuItem(const char* name, QMenu* menu, QActionGroup* menuItems)
	: QAction(name, menu)	{
				    setCheckable(true);
				    connect(this, SIGNAL(triggered()),
					    this, SLOT(onTriggered()));
				    menu->addAction(this);
				    menuItems->addAction(this);
				}

    cnoid::SignalProxy<void()>
		sigTriggered()	{ return _sigTriggered; }

  private Q_SLOTS:
    void	onTriggered()	{ _sigTriggered(); }
    
  private:
    cnoid::Signal<void()>	_sigTriggered;
};

}
#endif	// !__TU_IMAGEVIEW_H
