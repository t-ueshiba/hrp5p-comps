/*
 *  $Id$
 */
#include <QVBoxLayout>
#include <QScrollArea>
#include <cnoid/ViewArea>
#include <functional>
#include "ControlPanelView.h"

namespace TU
{
extern "C"
{
void	ControlPanelRTCInit(RTC::Manager* manager)			;
}

/************************************************************************
*  static functions							*
************************************************************************/
static ControlPanelRTC*
createControlPanelRTC()
{
  // OpenRTMPluginによって立てられた既存のRTCマネージャを獲得
    auto&	manager = RTC::Manager::instance();

  // ControlPanelコンポーネントを立ち上げ
    ControlPanelRTCInit(&manager);
    return dynamic_cast<ControlPanelRTC*>(manager
					  .createComponent("ControlPanel"));
}
    
namespace v
{
QWidget*	createCmd(QWidget* parent, const CmdDef& cmd)		;

/************************************************************************
*  class CmdPane							*
************************************************************************/
CmdPane::CmdPane(QWidget* parent)
    :QWidget(parent),
     _ready(false),
     _rtc(createControlPanelRTC()),
     _timer(),
     _layout(new QGridLayout(this))
{
    if (!_rtc)
	throw std::runtime_error("CmdPane::CmdPane(): failed to create RTC!");

  // ビュー更新用タイマにコールバックを登録してスタート
    _timer.sigTimeout().connect(std::bind(&CmdPane::onTimeout, this));
    _timer.start(100);

    _layout->setHorizontalSpacing(2);
    _layout->setVerticalSpacing(2);
}
    
CmdPane::~CmdPane()
{
    _timer.stop();

    if (_rtc)
    {
	_rtc->exit();
	RTC::Manager::instance().cleanupComponents();
    }
}

const char*
CmdPane::getRTCName() const
{
    return _rtc->getInstanceName();
}
    
void
CmdPane::addCmds(const CmdDefs& cmds)
{
    for (const auto& cmd : cmds)	// コマンドを作って_layoutに収める
    {
	const auto	child = createCmd(this, cmd);
	
	_layout->addWidget(child, cmd.gridy, cmd.gridx,
			   cmd.gridHeight, cmd.gridWidth);
	connect(this,  SIGNAL(refresh(CmdVals&)),
		child, SLOT(onRefresh(CmdVals&)));
    }
    _layout->setRowStretch(_layout->rowCount(), 1);

    CmdVals	ids;
    ids.push_back(CmdDef::c_RefreshAll);
    refresh(ids);
}

void
CmdPane::removeCmds()
{
    for (QWidget* child; (child = findChild<QWidget*>()); )
	delete child;
}

void
CmdPane::onTimeout()
{
    if (_rtc->isExiting())	// RTCが死んでいたら...
    {
	_rtc = nullptr;
	_timer.stop();		// タイマを止めてここの再実行を防ぐ
	_sigFinalize();		// ビューを削除する
    }
    else			// RTCが生きていたら...
    {
	if (!_ready)
	{
	    CmdDefs	cmds;
	    if (_rtc->getCmds(cmds))	// サーバからコマンド一覧を受け取る
	    {
#ifdef DEBUG
		std::cerr << cmds;
#endif
		addCmds(cmds);		// 全コマンドを生成する
		_ready = true;
	    }
	}
	else
	{
	    if (!_rtc->ready())
	    {
		removeCmds();		// 全コマンドを削除する
		_ready = false;
	    }
	}
    }
}

void
CmdPane::onSet(CmdVals& vals)
{
#ifdef DEBUG
    std::cerr << "CmdPane::onSet():      vals =" << vals;
#endif
  // valsに収められた値をサーバに送り，refreshすべきIDをvalsに受ける
    vals = _rtc->setValues(vals);
#ifdef DEBUG
    std::cerr <<  ", ids  =" << vals << std::endl;
#endif
    refresh(vals);				// refresh指令を子に送る
}
    
void
CmdPane::onGet(CmdVals& vals, bool range)
{
#ifdef DEBUG
    std::cerr << "CmdPane::onGet("
	      << (range ? "range" : "value") << "): ids  = " << vals;
#endif
  // valsに収められたIDをサーバに送り，その値をvalsに受ける    
    vals = _rtc->getValues(vals, range);
#ifdef DEBUG
    std::cerr <<  ", vals =" << vals << std::endl;
#endif
}
    
/************************************************************************
*  class ControlPanelView						*
************************************************************************/
ControlPanelView::ControlPanelView()
    :View(), _pane(new CmdPane(this))
{
  // このビューにRTCの名前を付ける
    setName(_pane->getRTCName());

  // _paneからのsigFinalizeシグナルをこのビューのonFinalize()に接続する
    _pane->sigFinalize().connect(std::bind(&ControlPanelView::onFinalize,
					   this));
    
  // _paneを収めるscrollAreaを作る
    const auto	scrollArea = new QScrollArea;
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidget(_pane);
    
  // このビューのレイアウトとなるvboxLayoutを作り，scrollAreaを収める
    const auto	vboxLayout = new QVBoxLayout(this);
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    vboxLayout->addWidget(scrollArea);
}

void
ControlPanelView::onFinalize()
{
    viewArea()->removeView(this);
}

}
}
