/*
 *  $Id$
 */
#include <QBoxLayout>
#include <QLabel>
#include <cmath>	// for std::log10()
#include "SliderCmd_.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class SliderCmd							*
************************************************************************/
SliderCmd::SliderCmd(QWidget* parent, const CmdDef& cmd)
    :QWidget(parent), _id(cmd.id),
     _slider(new QSlider((cmd.attrs & CmdDef::CA_Vertical ? Qt::Vertical
							  : Qt::Horizontal),
			 this)),
     _spinBox(new QDoubleSpinBox(this))
{
    const auto	layout = new QBoxLayout((cmd.attrs & CmdDef::CA_Vertical ?
					 QBoxLayout::BottomToTop :
					 QBoxLayout::LeftToRight), this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(tr(cmd.name.c_str()), this));
    layout->addWidget(_slider);
    layout->addWidget(_spinBox);

    connect(_slider,  SIGNAL(valueChanged(int)),
	    this,     SLOT(onSliderValueChanged(int)));
    connect(_spinBox, SIGNAL(valueChanged(double)),
	    this ,    SLOT(onSpinBoxValueChanged(double)));

    connect(this,   SIGNAL(set(CmdVals&)),
	    parent, SLOT(onSet(CmdVals&)));
    connect(this,   SIGNAL(get(CmdVals&, bool)),
	    parent, SLOT(onGet(CmdVals&, bool)));
}

void
SliderCmd::onRefresh(CmdVals& ids)
{
    if (ids.empty())
	return;
    else if (ids.back() == _id)
	ids.pop_back();
    else if (ids.back() != CmdDef::c_RefreshAll)
	return;
    
    CmdVals	vals;
    vals.push_back(_id);
    get(vals, true);
    onWrite(vals, true);

    vals.clear();
    vals.push_back(_id);
    get(vals, false);		// 親ビューに自分のidを渡してvalsを獲得
    onWrite(vals, false);	// 獲得した値をセット
}

void
SliderCmd::onRead(CmdVals& vals)
{
  // 自分の値を追加
    const auto	val = _spinBox->value();
    vals.push_back(CmdVal(int(val), float(val)));
}
    
void
SliderCmd::onWrite(CmdVals& vals, bool range)
{
    if (vals.empty())
	return;

    if (range)
    {
      // setRange() がsiganalを発生する可能性があるので，一時的にslotを遮断
	disconnect(_spinBox, SIGNAL(valueChanged(double)),
		   this ,    SLOT(onSpinBoxValueChanged(double)));
	
	const auto	min = vals.back().f();
	vals.pop_back();
	const auto	max = vals.back().f();
	vals.pop_back();
	auto		step = vals.back().f();
	vals.pop_back();
	
	_slider ->setSingleStep(1);
	_spinBox->setRange(min, max);

	if (min == std::floor(min) &&
	    max == std::floor(max) && step == std::floor(step))
	{
	    if (step == 0)
		step = 1;
	    
	    _slider ->setRange(0, (int(max) - int(min))/int(step));
	    _spinBox->setSingleStep(step);
	    _spinBox->setDecimals(0);
	}
	else
	{
	    if (step == 0)
		step = (max - min) / 1000;

	    _slider ->setRange(0, 1000);
	    _spinBox->setSingleStep(step);
	    const auto	decimal = std::max(0, 3 + int(-std::log10(max - min)));
	    _spinBox->setDecimals(decimal);
	}

      // slotを再接続
	connect(_spinBox, SIGNAL(valueChanged(double)),
		this ,    SLOT(onSpinBoxValueChanged(double)));
    }
    else
    {
	_spinBox->setValue(vals.back().f());	// 渡された値をセットして
	vals.pop_back();			// 捨てる
    }
}

void
SliderCmd::onSliderValueChanged(int sliderVal)
{
    const auto	val = sliderValToVal(sliderVal);
    
    if (sliderVal != valToSliderVal(_spinBox->value()))
	_spinBox->setValue(val);
}
    
void
SliderCmd::onSpinBoxValueChanged(double val)
{
    _slider->setValue(valToSliderVal(val));

    CmdVals	vals;
    vals.push_back(CmdVal(int(val), float(val)));	// 自分の値と
    vals.push_back(_id);				// idを
    set(vals);						// サーバに送信
}

double
SliderCmd::sliderValToVal(int sliderVal) const
{
    const auto	max  = _spinBox->maximum();
    const auto	min  = _spinBox->minimum();
    const auto	step = _spinBox->singleStep();
    
    if (min == std::floor(min) &&
	max == std::floor(max) && step == std::floor(step))
    {
	return min + sliderVal*step;
    }
    else
    {
	return (step > 0 ?
		min + int(sliderVal*(max - min)/(1000*step))*step :
		min + sliderVal*(max - min)/1000);
    }
}
	     
int
SliderCmd::valToSliderVal(double val) const
{
    const auto	max  = _spinBox->maximum();
    const auto	min  = _spinBox->minimum();
    const auto	step = _spinBox->singleStep();
    
    if (min == std::floor(min) &&
	max == std::floor(max) && step == std::floor(step))
    {
	return (int(val) - int(min))/int(step);
    }
    else
    {
	return int(1000*(val - min)/(max - min));
    }
}
    
}
}
