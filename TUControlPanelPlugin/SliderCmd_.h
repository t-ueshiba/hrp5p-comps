/*
 *  $Id$
 */
#ifndef __TU_V_SLIDERCMD_H
#define __TU_V_SLIDERCMD_H

#include <QSlider>
#include <QDoubleSpinBox>
#include "CmdDef.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class SliderCmd							*
************************************************************************/
class SliderCmd : public QWidget
{
    Q_OBJECT
    
  public:
    SliderCmd(QWidget* parent, const CmdDef& cmd)			;

    void	setRange(CmdDef::value_type min,
			 CmdDef::value_type max,
			 CmdDef::value_type step)			;
	
  Q_SIGNALS:
    void	set(CmdVals& vals)					;
    void	get(CmdVals& vals, bool range)				;
									
  private Q_SLOTS:
    void	onRefresh(CmdVals& ids)					;
    void	onRead(CmdVals& vals)					;
    void	onWrite(CmdVals& vals, bool range)			;
    void	onSliderValueChanged(int val)				;
    void	onSpinBoxValueChanged(double val)			;
    
  private:
    double	sliderValToVal(int sliderVal)			const	;
    int		valToSliderVal(double val)			const	;
    
  private:
    CmdDef::Id		const	_id;
    QSlider*		const	_slider;
    QDoubleSpinBox*		_spinBox;
};

}
}
#endif
