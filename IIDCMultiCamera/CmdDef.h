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
	      class=typename std::enable_if<std::is_floating_point<T>::value>
				::type>
    CmdVal(T f)				:_i(0), _f(f)		{}
    
		operator int()				const	{ return _i; }
    int		i()					const	{ return _i; }
    float	f()					const	{ return _f; }

  private:
    int		_i;
    float	_f;
};
typedef std::vector<CmdVal>	CmdVals;
    
/************************************************************************
*  struct CmdDef							*
************************************************************************/
struct CmdDef;
typedef std::vector<CmdDef>	CmdDefs;
    
struct CmdDef
{
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
    
  // standard menu item definitions
    static constexpr Id	M_File		= 32000;
    static constexpr Id	M_Edit		= 32001;
    static constexpr Id	M_Search	= 32002;
    static constexpr Id	M_Help		= 32003;
    static constexpr Id	M_Window	= 32004;
    static constexpr Id	M_Options	= 32005;
    static constexpr Id	M_Tools		= 32006;
    static constexpr Id	M_Font		= 32007;
    static constexpr Id	M_View		= 32008;
    static constexpr Id	M_Format	= 32009;
    static constexpr Id	M_Insert	= 32010;
    static constexpr Id	M_Test		= 32011;

    static constexpr Id	M_Line		= 32090;

    static constexpr Id	M_New		= 32100;
    static constexpr Id	M_Open		= 32101;
    static constexpr Id	M_Close		= 32102;
    static constexpr Id	M_Save		= 32103;
    static constexpr Id	M_SaveAs	= 32104;
    static constexpr Id	M_Print		= 32105;
    static constexpr Id	M_PrintPreview	= 32106;
    static constexpr Id	M_About		= 32107;
    static constexpr Id	M_Exit		= 32108;
    static constexpr Id	M_CloseFile	= 32109;	// V:1.13

    static constexpr Id	M_Cut		= 32110;
    static constexpr Id	M_Copy		= 32111;
    static constexpr Id	M_Paste		= 32112;
    static constexpr Id	M_Delete	= 32113;
    static constexpr Id	M_Clear		= 32114;

    static constexpr Id	M_UnDo		= 32120;
    static constexpr Id	M_SetDebug	= 32121;

    static constexpr Id	M_Find		= 32130;
    static constexpr Id	M_FindAgain	= 32131;
    static constexpr Id	M_Replace	= 32132;

    static constexpr Id	M_Preferences	= 32140;
    static constexpr Id	M_FontSelect	= 32141;

    static constexpr Id	M_Cancel	= 32150;
    static constexpr Id	M_Done		= 32151;
    static constexpr Id	M_OK		= 32152;
    static constexpr Id	M_Yes		= 32153;
    static constexpr Id	M_No		= 32154;
    static constexpr Id	M_All		= 32155;
    static constexpr Id	M_None		= 32156;
    static constexpr Id	M_Default	= 32157;

    CmdDef(Type			type_=C_Label,
	   Id			id_=0,
	   const std::string&	name_="",
	   size_t		gridx_=0,
	   size_t		gridy_=0,
	   size_t		gridWidth_=1,
	   size_t		gridHeight_=1,
	   size_t		size_=0,
	   CmdDefs		subcmds_=CmdDefs(),
	   value_type		min_=0,
	   value_type		max_=1,
	   value_type		step_=1,
	   u_int		attrs_=CA_None)
	:type(type_), id(id_), name(name_), subcmds(subcmds_),
	 gridx(gridx_), gridy(gridy_),
	 gridWidth(gridWidth_), gridHeight(gridHeight_), size(size_),
	 min(min_), max(max_), step(step_), attrs(attrs_)		{}

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
	ar & BOOST_SERIALIZATION_NVP(min);
	ar & BOOST_SERIALIZATION_NVP(max);
	ar & BOOST_SERIALIZATION_NVP(step);
	ar & BOOST_SERIALIZATION_NVP(attrs);
	ar & BOOST_SERIALIZATION_NVP(subcmds);
    }
    
    Type	type;
    Id		id;
    std::string	name;
    size_t	gridx;
    size_t	gridy;
    size_t	gridWidth;
    size_t	gridHeight;
    size_t	size;
    CmdDefs	subcmds;
    value_type	min;
    value_type	max;
    value_type	step;
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
    return out << cmd.name << ": " << cmd.id << " ["
	       << cmd.min << ',' << cmd.max << ':' << cmd.step
	       << "]@" << cmd.gridWidth << 'x' << cmd.gridHeight
	       << '+'  << cmd.gridx	<< '+' << cmd.gridy
	       << ", subcmds=" << cmd.subcmds; 
}

}	// namespace v
}	// namespace TU
#endif	// ! __TU_V_CMDDEF_H
