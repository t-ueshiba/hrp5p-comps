/*
 *  $Id$
 */
#include <QRadioButton>
#include <QCheckBox>
#include "GroupBoxCmd_.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class GroupBoxCmd							*
************************************************************************/
GroupBoxCmd::GroupBoxCmd(QWidget* parent, const CmdDef& cmd)
    :QGroupBox(tr(cmd.name.c_str()), parent),
     _id(cmd.id), _subcmds(cmd.subcmds), _buttons()
{
    auto	layout = new QGridLayout(this);
    layout->setContentsMargins(11, 0, 11, 0);
    
    for (const auto& subcmd : _subcmds)
    {
	auto	button = (subcmd.type == CmdDef::C_RadioButton ?
			  static_cast<QAbstractButton*>(
			      new QRadioButton(tr(subcmd.name.c_str()), this)) :
			  static_cast<QAbstractButton*>(
			      new QCheckBox(tr(subcmd.name.c_str()), this)));
	_buttons.push_back(button);
	layout->addWidget(button, subcmd.gridy, subcmd.gridx,
			  subcmd.gridHeight, subcmd.gridWidth);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
    }

    connect(this,   SIGNAL(set(CmdVals&)),
	    parent, SLOT(onSet(CmdVals&)));
    connect(this,   SIGNAL(get(CmdVals&, bool)),
	    parent, SLOT(onGet(CmdVals&, bool)));
}

void
GroupBoxCmd::onClicked(bool)
{
    CmdVals	vals;
    onRead(vals);			// チェックが付いているボタンのidを集め
    vals.push_back(_id);		// 自分のidを追加して
    set(vals);				// 親ビューのonSet()を呼ぶ	
}

void
GroupBoxCmd::onRefresh(CmdVals& ids)
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
GroupBoxCmd::onRead(CmdVals& vals)
{
    auto	subcmd = _subcmds.cbegin();
    for (auto button : _buttons)
    {
	if (button->isChecked())	// ボタンにチェックが付いていたら
	    vals.push_back(subcmd->id);	// そのidを追加
	++subcmd;
    }
}

void
GroupBoxCmd::onWrite(CmdVals& vals, bool range)
{
    if (range)
	return;

    for (; !vals.empty(); vals.pop_back())	// vals中の個々の値について...
    {				// それと同じ値のidを持つボタンにチェック付け
	auto	val    = vals.back();
	auto	subcmd = _subcmds.cbegin();
	for (auto button : _buttons)
	{
	    button->setChecked(subcmd->id == val);
	    ++subcmd;
	}
    }
}

}
}
