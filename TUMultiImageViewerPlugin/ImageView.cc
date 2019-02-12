/*
 *  $Id$
 */
#include <QScrollBar>
#include "ImageView.h"

namespace TU
{
/************************************************************************
*  class ImageView							*
************************************************************************/
ImageView::ImageView(QWidget* parent)
    :QGraphicsView(parent), _data(), _qimage(), _colors(256), _colorsF(256),
     _menu(new QMenu(this)), _fit(true), _scale(1)
{
    setBackgroundRole(QPalette::Dark);
    setViewportUpdateMode(FullViewportUpdate);

    for (int i = 0; i < _colors.size(); ++i)
	_colors[i] = 0xff000000 | (i << 16) | (i << 8) | i;

    for (int i = 0; i < 128; ++i)
    {
	_colorsF[128+i] = 0xff000000 | (i << 9);
	_colorsF[127-i] = 0xff000000 | ((i << 17) + 1);
    }

    auto	menuItems = new QActionGroup(this);
    auto	menuItem  = new MenuItem("fit", _menu, menuItems);
    menuItem->sigTriggered().connect(std::bind(&ImageView::setScale,
					       this, -1));
    menuItem = new MenuItem("x0.25", _menu, menuItems);
    menuItem->sigTriggered().connect(std::bind(&ImageView::setScale,
					       this, 0.25));
    menuItem = new MenuItem("x0.5", _menu, menuItems);
    menuItem->sigTriggered().connect(std::bind(&ImageView::setScale,
					       this, 0.5));
    menuItem = new MenuItem("x1", _menu, menuItems);
    menuItem->sigTriggered().connect(std::bind(&ImageView::setScale,
					       this, 1));
    menuItem = new MenuItem("x2", _menu, menuItems);
    menuItem->sigTriggered().connect(std::bind(&ImageView::setScale,
					       this, 2));
    menuItem = new MenuItem("x4", _menu, menuItems);
    menuItem->sigTriggered().connect(std::bind(&ImageView::setScale,
					       this, 4));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
	    this, SLOT(showContextMenu(const QPoint&)));
}

void
ImageView::paintEvent(QPaintEvent* event)
{
    if (_qimage.bits() == 0)
	return;

    if (_fit)
	_scale = std::min(qreal(viewport()->width())  / _qimage.width(),
			  qreal(viewport()->height()) / _qimage.height());

    QPainter	painter(viewport());
    painter.setTransform(QTransform(_scale, 0, 0, 0, _scale, 0,
				    -horizontalScrollBar()->sliderPosition(),
				    -verticalScrollBar()->sliderPosition()));
    painter.drawImage(0, 0, _qimage);
}

void
ImageView::setScale(qreal scale)
{
    if (scale < 0)
    {
	_fit = true;
	setSceneRect(QRectF());		// 空のSceneRectを設定
    }
    else
    {
	_fit = false;
	_scale = scale;

      // Sceneの範囲を設定することによって(必要ならば）ScrollBarを表示させる
	setSceneRect(0, 0, _scale*_qimage.width(), _scale*_qimage.height());
    }

    viewport()->update();
}

void
ImageView::showContextMenu(const QPoint& p)
{
  // メニューをexecすると_scaleやスクロールバーの位置が変わるので，現在値を保存
  // (left, top) は，viewport原点から見た現在表示されている領域の左上隅の位置
    const qreal	scale = _scale;
    const int	left  = horizontalScrollBar()->sliderPosition(),
		top   = verticalScrollBar()->sliderPosition();

  // pはviewport中の現在表示されている領域の左上隅を原点としたクリック位置なので，
  // ディスプレイ全体座標系での値に直してメニューをポップアップ
    _menu->exec(viewport()->mapToGlobal(p));

  // メニューを出した位置に対応する画像上の点をビューの中央にもってくるため，
  // メニューをポップアップした位置のviewport座標 (left + p.x(), top + p.u())
  // を新しいスケールに合わせて変換したものをビューの大きさの半分だけ左上にシフト
  // した位置が表示領域の左上隅になるようにする.
    horizontalScrollBar()
	->setSliderPosition(int(_scale/scale*(left + p.x())) - width()/2);
    verticalScrollBar()
	->setSliderPosition(int(_scale/scale*(top  + p.y())) - height()/2);
}

}
