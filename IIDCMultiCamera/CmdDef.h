/*
 *  $Id: CmdDef.h 1589 2014-07-11 04:00:54Z ueshiba $  
 */
#ifndef __TU_V_CMDDEF_H
#define __TU_V_CMDDEF_H

#include <string>
#include <iostream>
#include <iomanip>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>
#include "TU/types.h"

namespace TU
{
namespace v
{
enum CmdType
{
    C_Label,			// Regular text label
    C_Button,			// Button
    C_ToggleButton,		// A toggle button
    C_GroupBox,			// A set of radio buttons or check boxes
    C_RadioButton,		// Radio button (exclusive)
    C_CheckBox,			// Check box (non-exclusive)
    C_MenuItem,			// Menu item
    C_Slider,			// Slider to enter value
    C_List,			// List of items (scrollable)
    C_TextIn,			// Text input field
};

enum CmdAttributes
{
    CA_None	     =      0,	// No special attributes
    CA_DefaultButton =   0x02,	// Special Default Button
    CA_Horizontal    =   0x20,	// Horizontal orientation
    CA_Vertical	     =   0x40,	// Vertical orientation
    CA_NoBorder	     =  0x400,	// No border(frames,status bar)
    CA_NoSpace	     =  0x800,	// No space between widgets
};

typedef u_int	CmdId;		// ID for each item command or menu
    
// standard menu item definitions
const CmdId	M_File		= 32000;
const CmdId	M_Edit		= 32001;
const CmdId	M_Search	= 32002;
const CmdId	M_Help		= 32003;
const CmdId	M_Window	= 32004;
const CmdId	M_Options	= 32005;
const CmdId	M_Tools		= 32006;
const CmdId	M_Font		= 32007;
const CmdId	M_View		= 32008;
const CmdId	M_Format	= 32009;
const CmdId	M_Insert	= 32010;
const CmdId	M_Test		= 32011;

const CmdId	M_Line		= 32090;

const CmdId	M_New		= 32100;
const CmdId	M_Open		= 32101;
const CmdId	M_Close		= 32102;
const CmdId	M_Save		= 32103;
const CmdId	M_SaveAs	= 32104;
const CmdId	M_Print		= 32105;
const CmdId	M_PrintPreview	= 32106;
const CmdId	M_About		= 32107;
const CmdId	M_Exit		= 32108;
const CmdId	M_CloseFile	= 32109;	// V:1.13

const CmdId	M_Cut		= 32110;
const CmdId	M_Copy		= 32111;
const CmdId	M_Paste		= 32112;
const CmdId	M_Delete	= 32113;
const CmdId	M_Clear		= 32114;

const CmdId	M_UnDo		= 32120;
const CmdId	M_SetDebug	= 32121;

const CmdId	M_Find		= 32130;
const CmdId	M_FindAgain	= 32131;
const CmdId	M_Replace	= 32132;

const CmdId	M_Preferences	= 32140;
const CmdId	M_FontSelect	= 32141;

const CmdId	M_Cancel	= 32150;
const CmdId	M_Done		= 32151;
const CmdId	M_OK		= 32152;
const CmdId	M_Yes		= 32153;
const CmdId	M_No		= 32154;
const CmdId	M_All		= 32155;
const CmdId	M_None		= 32156;
const CmdId	M_Default	= 32157;

/************************************************************************
*  struct CmdDef							*
************************************************************************/
struct CmdDef;
typedef std::vector<CmdDef>	CmdDefs;
typedef std::vector<int>	Vals;
    
struct CmdDef
{
    typedef int		value_type;

    CmdDef(CmdType type_=C_Label, const std::string& name_="",
	   CmdId id_=0, CmdDefs subcmds_=CmdDefs(),
	   value_type min_=0, value_type max_=1,
	   value_type step_=1, u_int div_=1, u_int attrs_=CA_None,
	   size_t gridx_=0, size_t gridy_=0,
	   size_t gridWidth_=1, size_t gridHeight_=1, size_t size_=0)
	:type(type_), name(name_), id(id_), subcmds(subcmds_),
	 min(min_), max(max_), step(step_), div(div_), attrs(attrs_),
	 gridx(gridx_), gridy(gridy_),
	 gridWidth(gridWidth_), gridHeight(gridHeight_), size(size_)	{}

    template <class ARCHIVE> void
    serialize(ARCHIVE& ar, const unsigned int file_version)
    {
	ar & BOOST_SERIALIZATION_NVP(type);
	ar & BOOST_SERIALIZATION_NVP(name);
	ar & BOOST_SERIALIZATION_NVP(id);
	ar & BOOST_SERIALIZATION_NVP(min);
	ar & BOOST_SERIALIZATION_NVP(max);
	ar & BOOST_SERIALIZATION_NVP(step);
	ar & BOOST_SERIALIZATION_NVP(div);
	ar & BOOST_SERIALIZATION_NVP(attrs);
	ar & BOOST_SERIALIZATION_NVP(gridx);
	ar & BOOST_SERIALIZATION_NVP(gridy);
	ar & BOOST_SERIALIZATION_NVP(gridWidth);
	ar & BOOST_SERIALIZATION_NVP(gridHeight);
	ar & BOOST_SERIALIZATION_NVP(size);
	ar & BOOST_SERIALIZATION_NVP(subcmds);
    }
    
    CmdType	type;
    std::string	name;
    CmdId	id;
    CmdDefs	subcmds;
    value_type	min;
    value_type	max;
    value_type	step;
    u_int	div;
    u_int	attrs;
    size_t	gridx;
    size_t	gridy;
    size_t	gridWidth;
    size_t	gridHeight;
    size_t	size;
};

static CmdDefs	noSub;
    
/************************************************************************
*  global functions							*
************************************************************************/
std::ostream&	operator <<(std::ostream& out, const CmdDef& cmd)	;
    
inline std::ostream&
operator <<(std::ostream& out, const CmdDefs& cmds)
{
    for (CmdDefs::const_iterator cmd = cmds.begin(); cmd != cmds.end(); ++cmd)
	out << *cmd << std::endl;
    return out;
}
    
inline std::ostream&
operator <<(std::ostream& out, const CmdDef& cmd)
{
    out << cmd.name << ": " << cmd.id << " [";
    if (cmd.div == 1)
	out << cmd.min << ',' << cmd.max << ':' << cmd.step;
    else
	out << float(cmd.min) /float(cmd.div) << ','
	    << float(cmd.max) /float(cmd.div) << ':'
	    << float(cmd.step)/float(cmd.div);
    return out << "]@" << cmd.gridWidth << 'x' << cmd.gridHeight
	       << '+'  << cmd.gridx	<< '+' << cmd.gridy
	       << ", subcmds=" << cmd.subcmds; 
}

}	// namespace v
}	// namespace TU
#endif	// ! __TU_V_CMDDEF_H
