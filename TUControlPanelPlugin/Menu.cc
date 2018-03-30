/*
 *  $Id$
 */
#include <QActionGroup>
#include "Menu_.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class Menu								*
************************************************************************/
Menu::Menu(QWidget* parent, const CmdDef& cmd)
    :QMenu(parent), _id(cmd.id)
{
    const auto	actionGroup = new QActionGroup(this);
    for (const auto& subcmd : cmd.subcmds)
    {
	const auto	child = new MenuItem(this, subcmd);
	addAction(child);
	actionGroup->addAction(child);

	connect(this,  SIGNAL(read(CmdVals&)),
		child, SLOT(onRead(CmdVals&)));
	connect(this,  SIGNAL(write(CmdVals&, bool)),
		child, SLOT(onWrite(CmdVals&, bool)));
    }

    connect(this,   SIGNAL(set(CmdVals&)),
	    parent, SLOT(onSet(CmdVals&)));
    connect(this,   SIGNAL(get(CmdVals&, bool)),
	    parent, SLOT(onGet(CmdVals&, bool)));
}

void
Menu::onSet(CmdVals& vals)
{
    if (auto action = menuAction())	// このメニューを起動したアクションに
	action->setChecked(true);	// チェックを付ける
    
    vals.push_back(_id);		// 自分のidを追加して
    set(vals);				// 親に転送
}

void
Menu::onGet(CmdVals& vals, bool range)
{
    vals.push_back(_id);		// 自分のidを追加して
    get(vals, range);			// 親に転送
}

void
Menu::onRefresh(CmdVals& ids)
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
Menu::onRead(CmdVals& vals)
{
    read(vals);				// 全ての子メニュー項目を探索
}
    
void
Menu::onWrite(CmdVals& vals, bool range)
{
    write(vals, range);			// 全ての子メニュー項目に値を転送
}

/************************************************************************
*  class MenuItem							*
************************************************************************/
MenuItem::MenuItem(QWidget* parent, const CmdDef& cmd)
    :QAction(tr(cmd.name.c_str()), parent), _id(cmd.id), _dialog(0)
{
    setCheckable(true);
    
    if (!cmd.subcmds.empty())
    {
	QWidget*	child = 0;

	if (cmd.subcmds[0].type == CmdDef::C_MenuItem)
	{
	    setMenu(new Menu(parent, cmd));
	    child = menu();
	}
	else
	{
	    _dialog = new Dialog(parent, cmd);
	    child   = _dialog;
	}

	connect(this,  SIGNAL(read(CmdVals&)),
		child, SLOT(onRead(CmdVals&)));
	connect(this,  SIGNAL(write(CmdVals&, bool)),
		child, SLOT(onWrite(CmdVals&, bool)));
    }

    connect(this,   SIGNAL(triggered()),
	    this,   SLOT(onTriggered()));
    connect(this, SIGNAL(set(CmdVals&)), parent, SLOT(onSet(CmdVals&)));
}
    
void
MenuItem::onTriggered()
{
    if (_dialog)
    {
	if (!_dialog->refreshAndExec())	// ダイアログの設定が取り消されたら
	    setChecked(false);		// チェックを消す
    }
    else
    {
	CmdVals	vals;
	vals.push_back(_id);		// 自分のidを
	set(vals);			// 親メニューに送信
    }
}

void
MenuItem::onRead(CmdVals& vals)
{
    if (isChecked())			// チェックが付いていたら...
    {
	vals.push_back(_id);		// 自分のidを追加

	if (menu() || _dialog)		// 子メニュー/ダイアログがあれば...
	    read(vals);			// 再帰的に値を読み込む
    }
}
    
void
MenuItem::onWrite(CmdVals& vals, bool range)
{
  // 末尾の値が自分のidに一致しなければ何もしない
    if (vals.empty() || vals.back() != _id)
	return;
    
    setChecked(true);			// 自分にチェックを付けて
    vals.pop_back();			// 捨てる

    if (menu() || _dialog)		// 子メニュー/ダイアログがあれば...
	write(vals, range);		// 再帰的に残りの値を書き込む
}
	
}
}
