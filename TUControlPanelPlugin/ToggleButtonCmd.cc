/*
 *  $Id$
 */
#include "ToggleButtonCmd_.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class ToggleButtonCmd						*
************************************************************************/
ToggleButtonCmd::ToggleButtonCmd(QWidget* parent, const CmdDef& cmd)
    :QPushButton(tr(cmd.name.c_str()), parent), _id(cmd.id)
{
    setCheckable(true);

    connect(this,   SIGNAL(clicked(bool)),
	    this,   SLOT(onClicked(bool)));
    connect(this,   SIGNAL(set(CmdVals&)),
	    parent, SLOT(onSet(CmdVals&)));
    connect(this,   SIGNAL(get(CmdVals&, bool)),
	    parent, SLOT(onGet(CmdVals&, bool)));
}
    
void
ToggleButtonCmd::onClicked(bool val)
{
    CmdVals	vals;
    vals.push_back(val);		// 自分の値と
    vals.push_back(_id);		// idを
    set(vals);				// 親ビューに渡してサーバに送信
}
    
void
ToggleButtonCmd::onRefresh(CmdVals& ids)
{
    if (ids.empty())
	return;
    else if (ids.back() == _id)
	ids.pop_back();
    else if (ids.back() != CmdDef::c_RefreshAll)
	return;
    
    CmdVals	vals;
    vals.push_back(_id);		// 親ビューに自分のidを渡して
    get(vals, false);			// valsを獲得
    onWrite(vals, false);		// 獲得した値をセット
}
    
void
ToggleButtonCmd::onRead(CmdVals& vals)
{
    vals.push_back(isChecked());	// 自分の値を追加
}
    
void
ToggleButtonCmd::onWrite(CmdVals& vals, bool range)
{
    if (vals.empty() || range)
	return;

    setChecked(vals.back());		// 自分の値をセットして
    vals.pop_back();			// 捨てる
}
    
}
}
