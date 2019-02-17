/*
 *  $Id: CmdDef.h 1589 2014-07-11 04:00:54Z ueshiba $  
 */
#ifndef __TU_V_CMDDEF_H
#define __TU_V_CMDDEF_H

#include <string>
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>

namespace TU
{
namespace v
{
/************************************************************************
*  struct CmdVal							*
************************************************************************/
class CmdVal
{
  public:
    CmdVal()				:_i(0), _f(0)		{}
    CmdVal(int i, float f)		:_i(i), _f(f)		{}
    CmdVal(int i)			:_i(i), _f(0)		{}
    template <class T,
	      std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
    CmdVal(T f)				:_i(0), _f(f)		{}
    
		operator int()				const	{ return _i; }
    int		i()					const	{ return _i; }
    float	f()					const	{ return _f; }

  private:
    int		_i;
    float	_f;
};
typedef std::vector<CmdVal>	CmdVals;

inline std::ostream&
operator <<(std::ostream& out, const CmdVal& val)
{
    return out << '(' << val.i() << ',' << val.f() << ')';
}
    
inline std::ostream&
operator <<(std::ostream& out, const CmdVals& vals)
{
    for (const auto& val : vals)
	out << ' ' << val;
    return out;
}
    
/************************************************************************
*  struct CmdDef							*
************************************************************************/
class CmdDef;
typedef std::vector<CmdDef>	CmdDefs;
    
class CmdDef
{
  public:
    typedef float		value_type;

    enum Type
    {
	C_Label,		// Regular text label
	C_Button,		// Button
	C_ToggleButton,		// Toggle button
	C_GroupBox,		// Set of radio buttons or check boxes
	C_RadioButton,		// Radio button (exclusive)
	C_CheckBox,		// Check box (non-exclusive)
	C_MenuItem,		// Menu item
	C_Slider,		// Slider
	C_List,			// List of items (scrollable)
	C_TextIn,		// Text input field
    };

    enum Attribute
    {
	CA_None		 =      0,	// No special attributes
	CA_DefaultButton =   0x02,	// Special Default Button
	CA_Horizontal    =   0x20,	// Horizontal orientation
	CA_Vertical	 =   0x40,	// Vertical orientation
	CA_NoBorder	 =  0x400,	// No border(frames,status bar)
	CA_NoSpace	 =  0x800,	// No space between widgets
    };

    typedef u_int	Id;	// ID for each item command or menu
    
    constexpr static Id	c_RefreshAll	= ~0;

  // standard menu item definitions
    constexpr static Id	M_File		= 32000;
    constexpr static Id	M_Edit		= 32001;
    constexpr static Id	M_Search	= 32002;
    constexpr static Id	M_Help		= 32003;
    constexpr static Id	M_Window	= 32004;
    constexpr static Id	M_Options	= 32005;
    constexpr static Id	M_Tools		= 32006;
    constexpr static Id	M_Font		= 32007;
    constexpr static Id	M_View		= 32008;
    constexpr static Id	M_Format	= 32009;
    constexpr static Id	M_Insert	= 32010;
    constexpr static Id	M_Test		= 32011;

    constexpr static Id	M_Line		= 32090;

    constexpr static Id	M_New		= 32100;
    constexpr static Id	M_Open		= 32101;
    constexpr static Id	M_Close		= 32102;
    constexpr static Id	M_Save		= 32103;
    constexpr static Id	M_SaveAs	= 32104;
    constexpr static Id	M_Print		= 32105;
    constexpr static Id	M_PrintPreview	= 32106;
    constexpr static Id	M_About		= 32107;
    constexpr static Id	M_Exit		= 32108;
    constexpr static Id	M_CloseFile	= 32109;	// V:1.13

    constexpr static Id	M_Cut		= 32110;
    constexpr static Id	M_Copy		= 32111;
    constexpr static Id	M_Paste		= 32112;
    constexpr static Id	M_Delete	= 32113;
    constexpr static Id	M_Clear		= 32114;

    constexpr static Id	M_UnDo		= 32120;
    constexpr static Id	M_SetDebug	= 32121;

    constexpr static Id	M_Find		= 32130;
    constexpr static Id	M_FindAgain	= 32131;
    constexpr static Id	M_Replace	= 32132;

    constexpr static Id	M_Preferences	= 32140;
    constexpr static Id	M_FontSelect	= 32141;

    constexpr static Id	M_Cancel	= 32150;
    constexpr static Id	M_Done		= 32151;
    constexpr static Id	M_OK		= 32152;
    constexpr static Id	M_Yes		= 32153;
    constexpr static Id	M_No		= 32154;
    constexpr static Id	M_All		= 32155;
    constexpr static Id	M_None		= 32156;
    constexpr static Id	M_Default	= 32157;

  public:
    CmdDef(Type			type_=C_Label,
	   Id			id_=0,
	   const std::string&	name_="",
	   size_t		gridx_=0,
	   size_t		gridy_=0,
	   size_t		gridWidth_=1,
	   size_t		gridHeight_=1,
	   size_t		size_=0,
	   CmdDefs		subcmds_=CmdDefs(),
	   u_int		attrs_=CA_None)
	:type(type_), id(id_), name(name_),
	 gridx(gridx_), gridy(gridy_),
	 gridWidth(gridWidth_), gridHeight(gridHeight_),
	 size(size_), subcmds(subcmds_), attrs(attrs_)			{}

  private:
    friend	class boost::serialization::access;
    
    template <class ARCHIVE> void
    serialize(ARCHIVE& ar, const unsigned int file_version)
    {
	ar & BOOST_SERIALIZATION_NVP(type);
	ar & BOOST_SERIALIZATION_NVP(id);
	ar & BOOST_SERIALIZATION_NVP(name);
	ar & BOOST_SERIALIZATION_NVP(gridx);
	ar & BOOST_SERIALIZATION_NVP(gridy);
	ar & BOOST_SERIALIZATION_NVP(gridWidth);
	ar & BOOST_SERIALIZATION_NVP(gridHeight);
	ar & BOOST_SERIALIZATION_NVP(size);
	ar & BOOST_SERIALIZATION_NVP(attrs);
	ar & BOOST_SERIALIZATION_NVP(subcmds);
    }

  public:
    Type	type;
    Id		id;
    std::string	name;
    size_t	gridx;
    size_t	gridy;
    size_t	gridWidth;
    size_t	gridHeight;
    size_t	size;
    CmdDefs	subcmds;
    u_int	attrs;
};
    
/************************************************************************
*  global functions							*
************************************************************************/
std::ostream&	operator <<(std::ostream& out, const CmdDef& cmd)	;

inline std::ostream&
operator <<(std::ostream& out, const CmdDefs& cmds)
{
    for (const auto& cmd : cmds)
	out << cmd << std::endl;
    return out;
}
    
inline std::ostream&
operator <<(std::ostream& out, const CmdDef& cmd)
{
    return out << cmd.name << ": [" << cmd.id
	       << "]@" << cmd.gridWidth << 'x' << cmd.gridHeight
	       << '+'  << cmd.gridx	<< '+' << cmd.gridy
	       << ", subcmds=" << cmd.subcmds; 
}

}	// namespace v
}	// namespace TU
#endif	// ! __TU_V_CMDDEF_H
