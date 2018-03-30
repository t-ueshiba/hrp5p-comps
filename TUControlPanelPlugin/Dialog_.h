/*
 *  $Id$
 */
#ifndef __TU_V_DIALOG_H
#define __TU_V_DIALOG_H

#include <QDialog>
#include <QGridLayout>
#include "CmdDef.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class Dialog								*
************************************************************************/
class Dialog : public QDialog
{
    Q_OBJECT
    
  public:
    Dialog(QWidget* parent, const CmdDef& cmd)				;

    int		refreshAndExec()					;
    
  Q_SIGNALS:
    void	set(CmdVals& vals)					;
    void	get(CmdVals& vals, bool range)				;
    void	refresh(CmdVals& ids)					;
    void	read(CmdVals& vals)					;
    void	write(CmdVals& vals, bool range)			;
									
  private Q_SLOTS:
    void	accept()						;
    void	onSet(CmdVals& vals)					;
    void	onGet(CmdVals& vals, bool range)			;
    void	onRefresh(CmdVals& ids)					;
    void	onRead(CmdVals& vals)					;
    void	onWrite(CmdVals& vals, bool range)			;

  private:
    void	addChildren(CmdDefs::const_iterator cmd,
			    CmdDefs::const_iterator end,
			    QGridLayout* layout,
			    size_t& xmax, size_t& ymax)			;
    
  private:
    CmdDef::Id const	_id;
};
    
}
}
#endif
