/*
 *  $Id$
 */
#include "ButtonCmd_.h"
#include "Menu_.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class ButtonCmd							*
************************************************************************/
ButtonCmd::ButtonCmd(QWidget* parent, const CmdDef& cmd)
    :QPushButton(tr(cmd.name.c_str()), parent), _id(cmd.id), _dialog(nullptr)
{
    if (cmd.subcmds.empty())
    {
	connect(this, SIGNAL(pressed()), this, SLOT(onPressed()));
    }
    else
    {
	QWidget*	child = nullptr;
	
	if (cmd.subcmds[0].type == CmdDef::C_MenuItem)
	{
	    setMenu(new Menu(parent, cmd));
	    child = menu();
	}
	else
	{
	    _dialog = new Dialog(parent, cmd);
	    child = _dialog;

	    connect(this, SIGNAL(pressed()), this, SLOT(onPressed()));
	}

	connect(this,   SIGNAL(get(CmdVals&, bool)),
		parent, SLOT(onGet(CmdVals&, bool)));

	connect(this,   SIGNAL(refresh(CmdVals&)),
		child,  SLOT(onRefresh(CmdVals&)));
	connect(this,   SIGNAL(read(CmdVals&)),
		child,  SLOT(onRead(CmdVals&)));
	connect(this,   SIGNAL(write(CmdVals&, bool)),
		child,  SLOT(onWrite(CmdVals&, bool)));
    }

    connect(this,   SIGNAL(set(CmdVals&)),
	    parent, SLOT(onSet(CmdVals&)));
}

void
ButtonCmd::onPressed()
{
    if (_dialog)
	_dialog->refreshAndExec();
    else
    {
	CmdVals	vals;
	vals.push_back(_id);	// 自分のidを
	set(vals);		// 親に送信
    }
}

void
ButtonCmd::onSet(CmdVals& vals)
{
    set(vals);			// 子メニュー/ダイアログからのvalsを親に転送
}
    
void
ButtonCmd::onGet(CmdVals& vals, bool range)
{
    get(vals, range);		// 何も追加せずにそのまま親に転送
}
    
void
ButtonCmd::onRefresh(CmdVals& ids)
{
    refresh(ids);		// そのまま子に転送
}

void
ButtonCmd::onRead(CmdVals& vals)
{
    read(vals);			// 子メニュー/ダイアログからvalsを獲得
}
    
void
ButtonCmd::onWrite(CmdVals& vals, bool range)
{
    write(vals, range);		// 子メニュー/ダイアログにvalsをセット
}

}
}
