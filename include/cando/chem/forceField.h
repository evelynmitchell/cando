      
       
//
// (C) 2004 Christian E. Schafmeister
//


/*
 *	forceField.h
 *
 *	Maintains a parameter set for the Moe general force field
 *	from these parameters we can derive any force field parameters
 *
 */

#ifndef FORCEFIELD_H
#define	FORCEFIELD_H

#include <clasp/core/common.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <cando/chem/bond.h>
#include <cando/geom/vector3.h>
//#include	"conformation.h"
#include <cando/chem/atom.h>
#include <cando/chem/residue.h>
#include <cando/chem/ffBaseDb.h>
#include <cando/chem/ffTypesDb.h>
#include <cando/chem/ffStretchDb.h>
#include <cando/chem/ffAngleDb.h>
#include <cando/chem/ffItorDb.h>
#include <cando/chem/ffPtorDb.h>
#include <cando/chem/ffNonbondDb.h>
#include <cando/chem/ffVdwDb.h>



namespace       chem {


extern string	ForceField_XmlName;



typedef	enum	{ ffStr, ffAng, ffStb, ffOop, ffItor, ffTor, ffVdw, ffEle, ffSol } DisableEnum;


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
class	InfoDb_O;
    typedef	gctools::smart_ptr<InfoDb_O>	InfoDb_sp;
SMART(InfoDb);
class InfoDb_O : public core::T_O
{
    LISP_BASE1(core::T_O);
    LISP_CLASS(chem,ChemPkg,InfoDb_O,"InfoDb");

public:
    void	archiveBase(core::ArchiveP node);

public:
    adapt::SymbolMap<core::Str_O>	_database;
public:

    void	addInfo( core::Symbol_sp key, const string& data );
	DEFAULT_CTOR_DTOR(InfoDb_O);
};



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

SMART(ForceField);
class ForceField_O : public core::T_O
{
    LISP_BASE1(core::T_O);
    LISP_CLASS(chem,ChemPkg,ForceField_O,"ForceField");

public:
	void initialize();
public:
	void archiveBase(core::ArchiveP node);

public:
//		/*! Read the forceField from a file*/
//	static ForceField_sp	open_ForceField(const string& fn);

public:
	string				_Title;
	string				_Ref;
    gctools::Vec0<core::Symbol_sp>		_SingleBondMultiBondDistinctions;
	InfoDb_sp			_Info;
        gc::Nilable<FFTypesDb_sp>	_Types;
	FFStretchDb_sp			_Stretches;
	FFAngleDb_sp			_Angles;
	FFItorDb_sp			_Itors;
	FFPtorDb_sp			_Ptors;
	FFNonbondDb_sp			_Nonbonds;
	FFVdwDb_sp			_Vdws;
//	bool				_Disable[sizeof(DisableEnum)];
#if 0
	RPFFEquivalenceDb		Equivalences;
	RPFFOutOfPlaneDb		OutOfPlanes;
	FFStretch_spBendDb		StretchBends;
#endif

    void	addSingleBondMultiBondDistinctionType(core::Symbol_sp s) {
			this->_SingleBondMultiBondDistinctions.push_back(s);
		};

	FFTypesDb_sp	getTypes() { return this->_Types; };

	FFStretchDb_sp getStretchDb() { return this->_Stretches;};
	FFAngleDb_sp getAngleDb() { return this->_Angles;};
	FFItorDb_sp getItorDb() { return this->_Itors;};
	FFPtorDb_sp getPtorDb() { return this->_Ptors;};
	FFNonbondDb_sp	getNonbondDb() { return this->_Nonbonds; };
	FFVdwDb_sp getVdwDb() { return this->_Vdws;};
	
	void	assignTypes(Matter_sp matter);

	void	setTitle(const string& title);
	void	setInfoDb( InfoDb_sp Info );
	void	setFFTypeDb( FFTypesDb_sp Types);
	void	setFFStretchDb( FFStretchDb_sp Stretches);
	void	setFFAngleDb( FFAngleDb_sp Angles);
	void	setFFItorDb( FFItorDb_sp Itors);
	void	setFFPtorDb( FFPtorDb_sp Ptors);
	void	setFFNonbondDb(FFNonbondDb_sp Nonbonds );
	void	setFFVdwDb(FFVdwDb_sp Vdws );

//	void		parseFromMoeStream(istream in);
	void		parseFromMoeFormatFileName(string name);


	void	pointAllPartsToForceField(ForceField_sp ff);

	void	saveAs(const string& fileName);

	DEFAULT_CTOR_DTOR(ForceField_O);
};



//
// Some forcefield values may be undefined
// they will be set to UNDEFINED_NUMBER
// test this with isUndefined(x)
//
#define	UNDEFINED_NUMBER	9.0e99
#define	UNDEFINED_CUTOFF	9.0e98
inline	bool	isUndefined(double x) {return (x)>=UNDEFINED_CUTOFF;}



};


TRANSLATE(chem::InfoDb_O);
TRANSLATE(chem::ForceField_O);
#endif