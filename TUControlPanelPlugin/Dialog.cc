/*
 *  $Id$
 */
#include <QDialogButtonBox>
#include "Dialog_.h"

namespace TU
{
namespace v
{
QWidget*	createCmd(QWidget* parent, const CmdDef& cmd)		;

/************************************************************************
*  class Dialog								*
************************************************************************/
Dialog::Dialog(QWidget* parent, const CmdDef& cmd)
    :QDialog(parent), _id(cmd.id)
{
    const auto	layout = new QGridLayout(this);
    size_t	xmax = 1, ymax = 0;

    addChildren(cmd.subcmds.cbegin(), cmd.subcmds.cend(), layout, xmax, ymax);

    const auto	dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
						       QDialogButtonBox::Cancel,
						       Qt::Horizontal, this);
    layout->addWidget(dialogButtonBox, ymax, 0, 1, xmax);
    
    connect(this,   SIGNAL(set(CmdVals&)),
	    parent, SLOT(onSet(CmdVals&)));
    connect(this,   SIGNAL(get(CmdVals&, bool)),
	    parent, SLOT(onGet(CmdVals&, bool)));

    connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

int
Dialog::refreshAndExec()
{
    CmdVals	ids;
    ids.push_back(_id);
    onRefresh(ids);
    return exec();
}
    
void
Dialog::accept()
{
    CmdVals	vals;
    read(vals);			// 全ての子のonRead()を呼んでその値を獲得
    vals.push_back(_id);	// このDialog自身のidを追加
    set(vals);			// 親を介してサーバに送信

    QDialog::accept();		// Dialogを閉じる	
}

void
Dialog::onSet(CmdVals& vals)
{
  // setシグナルはこれ自身が発するので，子コマンドからのシグナルを遮断
}

void
Dialog::onGet(CmdVals& vals, bool range)
{
  // getシグナルはこれ自身が発するので，子コマンドからのシグナルを遮断
}

void
Dialog::onRefresh(CmdVals& ids)
{
    if (ids.empty())
	return;
    else if (ids.back() == _id)
	ids.pop_back();
    else if (ids.back() != CmdDef::c_RefreshAll)
	return;
    
    CmdVals	vals;
    vals.push_back(_id);	// 親に自分のidを渡して
    get(vals, true);		// パラメータの値域を獲得し
    write(vals, true);		// 全ての子に書き込む

    vals.push_back(_id);	// 親に自分のidを渡して
    get(vals, false);		// パラメータの値を獲得し
    write(vals, false);		// 全ての子に書き込む
}
    
void
Dialog::onRead(CmdVals& vals)
{
    read(vals);			// 全ての子を探索してその値を読む
}

void
Dialog::onWrite(CmdVals& vals, bool range)
{
    write(vals, range);		// 全ての子に値を転送して書き込む
}

void
Dialog::addChildren(CmdDefs::const_iterator cmd, CmdDefs::const_iterator end,
		    QGridLayout* layout, size_t& xmax, size_t& ymax)
{
    if (cmd == end)
	return;

    const auto	child = createCmd(this, *cmd);
	
    layout->addWidget(child, cmd->gridy, cmd->gridx,
		      cmd->gridHeight, cmd->gridWidth);
	    
    if (xmax < cmd->gridx + cmd->gridWidth)
	xmax = cmd->gridx + cmd->gridWidth;
    if (ymax < cmd->gridy + cmd->gridHeight)
	ymax = cmd->gridy + cmd->gridHeight;

  // writeシグナルは子を正順に発火させてvalsの末尾から値を書き込む
    connect(this,  SIGNAL(write(CmdVals&, bool)),
	    child, SLOT(onWrite(CmdVals&, bool)));
    
    addChildren(++cmd, end, layout, xmax, ymax);
    
  // readシグナルは子を逆順に発火させてvalsに値を積む
    connect(this, SIGNAL(read(CmdVals&)), child, SLOT(onRead(CmdVals&)));
}

}
}
