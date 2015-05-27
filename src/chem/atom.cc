 #define	DEBUG_LEVEL_FULL
//
// (C) 2004 Christian E. Schafmeister
//
#include <string.h>
#include <clasp/core/common.h>
#include <clasp/core/str.h>
#include <cando/chem/symbolTable.h>
#include <cando/chem/matter.h>
#include <cando/chem/residue.h>
#include <cando/chem/atom.h>
#include <cando/chem/bond.h>
#include <cando/chem/molecule.h>
#include <cando/chem/restraint.h>
//#include "core/serialize.h"
#include <clasp/core/numerics.h>
//#include "core/archive.h"
//#include "core/archiveNode.h"
#include <cando/geom/ovector3.h>
#include <clasp/core/sort.h>
#include <cando/geom/omatrix.h>
#include <clasp/core/numbers.h>
#include <cando/chem/elements.h>
#include <clasp/core/translators.h>
#include <cando/chem/constitutionAtoms.h>

#include <clasp/core/wrappers.h>


namespace chem
{
    SYMBOL_EXPORT_SC_(ChemPkg,_PLUS_configurationEnumConverter_PLUS_);
    SYMBOL_EXPORT_SC_(ChemPkg,_PLUS_stereochemistryTypeConverter_PLUS_);
    SYMBOL_EXPORT_SC_(ChemPkg,_PLUS_atomFlagSymbolConverter_PLUS_);

/* Important properties
 * used by RenderMatter to render atoms
 *
 * :colorByElement true/false - if true overrides color; 
 * 				if not set and color is set then use color
 * 				if not set and color not set then colorByElement
 *
 * :color type(GrColor)		- defines color  (default byelement)
 * :renderStyle			- one of :none, :line, :ballAndStick or :cpk (default :none if it has bonds and :ball if it doesn't)
 * :showLabel			- true or false (default false)
 * :label			- Label to show (default "")
 */


    uint globalUniqueAtomOrder = 1;
    uint nextUniqueAtomOrder()
    {
	globalUniqueAtomOrder++;
	if ( globalUniqueAtomOrder == UndefinedUnsignedInt )
	{
	    globalUniqueAtomOrder = 1;
	}
	return globalUniqueAtomOrder;
    }

    long	__AtomBuildCounter = 1;







    core::NullTerminatedEnumAssociation configurationEnum[] = {
	{ "UNDEFINED", undefinedConfiguration },
	{ "S", S_Configuration },
	{ "R", R_Configuration },
	{ "", -1 }
    };

    core::NullTerminatedEnumAssociation stereochemistryTypes[] = {
	{ "UNDEFINED", undefinedCenter },
	{ "CHIRAL", chiralCenter },
	{ "PROCHIRAL", prochiralCenter },
	{ "", -1 }
    };


    struct	VdwRadiiPairs
    {
	string	_Element;
	double	_VdwRadius;
    };



#if ATOMIC_ANCHOR
    void AnchorRestraint::archive( core::ArchiveP node )
    {
	node->archivePlainObject<Vector3>("anchorPos","AnchorPos",this->_AnchorPos);
    }
#endif



#define ARGS_Atom_O_make "(&key name element)"
#define DECL_Atom_O_make ""
#define DOCS_Atom_O_make "make Atom args: &key (name \"\") (element :C)"
    Atom_sp Atom_O::make(MatterName name, Element element)
    {_G();
	GC_ALLOCATE(Atom_O,atom);
	atom->setName(name);
	atom->setElement(element);
	return atom;
    };

    
    

    void Atom_O::exposeCando(core::Lisp_sp lisp)
    {
	core::class_<Atom_O>()
////	    .def_raw("core:__init__",&Atom_O::__init__,"(self &key name)")
	    .def("atomName",&Atom_O::atomName)
	    .def("getRelativePriority", &Atom_O::getRelativePriority)
	    .def("bondedAtomsAsList", &Atom_O::bondedAtomsAsList)
	    .def("getAtomicNumber", &Atom_O::getAtomicNumber)
	    .def("isAromatic",&Atom_O::isAromatic)
	    .def("setIsAromatic",&Atom_O::setIsAromatic)
	    .def("getIonization",&Atom_O::getIonization)
	    .def("setIonization",&Atom_O::setIonization)
	    .def("getBondedHydrogenCount", &Atom_O::getBondedHydrogenCount)
//	.def("getMatterType",&Matter_O::getMatterType)
	    .def("atLowerAddressThan",&Atom_O::atLowerAddressThan)
	    .def("getAlias",&Atom_O::getAlias)
	    .def("setAlias",&Atom_O::setAlias)
	    .def("getAtomId",&Atom_O::getAtomId)
	    .def("getMoeIndex",&Atom_O::getMoeIndex)
	    .def("setMoeIndex",&Atom_O::setMoeIndex)
	    .def("getTempInt",&Atom_O::getTempInt)
	    .def("setTempInt",&Atom_O::setTempInt)
	    .def("isConfigurable",&Atom_O::isConfigurable)
	    .def("setStereochemistryType",&Atom_O::setStereochemistryType)
	    .def("getStereochemistryType",&Atom_O::getStereochemistryType)
	    .def("setConfiguration",&Atom_O::setConfiguration)
	    .def("getConfiguration",&Atom_O::getConfiguration)
	    .def("getConfigurationAsString",&Atom_O::getConfigurationAsString)
	    .def("calculateStereochemicalConfiguration",&Atom_O::calculateStereochemicalConfiguration)
	    .def("getFlags",&Atom_O::getFlags)
	    .def("resetFlags",&Atom_O::resetFlags)
	    .def("turnOnFlags",&Atom_O::turnOnFlags)
	    .def("turnOffFlags",&Atom_O::turnOffFlags)
	    .def("testAllFlags",&Atom_O::testAllFlags)
	    .def("testAnyFlags",&Atom_O::testAnyFlags)
	    .def("isInRing",&Atom_O::isInRing)
	    .def("inRingSize",&Atom_O::inRingSize)
	    .def("getRingMembershipCount",&Atom_O::getRingMembershipCount)
	    .def("setRingMembershipCount",&Atom_O::setRingMembershipCount)
	    .def("getNameIndex",&Atom_O::getNameIndex)
	    .def("getType",&Atom_O::getType)
	    .def("setType",&Atom_O::setType)
	    .def("getMoeType",&Atom_O::getMoeType)
	    .def("setMoeType",&Atom_O::setMoeType)
	    .def("getHybridization",&Atom_O::getHybridization)
	    .def("setHybridization",&Atom_O::setHybridization)
	    .def("getElementAsString",&Atom_O::getElementAsString)
	    .def("getElement",&Atom_O::getElement,"","","",false)
	    .def("setElement",&Atom_O::setElement,"","","",false)
	    .def("getResidueContainedBy", &Atom_O::getResidueContainedBy )
	    .def("getSelected",&Atom_O::getSelected)
	    .def("setSelected",&Atom_O::setSelected)
	    .def("getPositionInNanometers",&Atom_O::getPositionInNanometers)
	    .def("getPosition",&Atom_O::getPosition,"","","",false)
	    .def("setPositionInNanometers",&Atom_O::setPositionInNanometers)
	    .def("setPosition",&Atom_O::setPosition,"","","",false)
	    .def("getTouched",&Atom_O::getTouched)
	    .def("setTouched",&Atom_O::setTouched)
	    .def("getCharge",&Atom_O::getCharge)
	    .def("setCharge",&Atom_O::setCharge)
#if ATOMIC_ANCHOR
	    .def("getAnchorPos",&Atom_O::getAnchorPos)
	    .def("setAnchorPos",&Atom_O::setAnchorPos)
#endif
	    .def("getOccupancy",&Atom_O::getOccupancy)
	    .def("setOccupancy",&Atom_O::setOccupancy)
	    .def("getTempFactor",&Atom_O::getTempFactor)
	    .def("setTempFactor",&Atom_O::setTempFactor)
	    .def("getVdwRadius",&Atom_O::getVdwRadius)
	    .def("setVdwRadius",&Atom_O::setVdwRadius)
	    .def("getCovalentRadius",&Atom_O::getCovalentRadius)
	    .def("setCovalentRadius",&Atom_O::setCovalentRadius)
	    .def("setMembershipAr1",&Atom_O::setMembershipAr1)
	    .def("getMembershipAr1",&Atom_O::getMembershipAr1)
	    .def("setMembershipAr2",&Atom_O::setMembershipAr2)
	    .def("getMembershipAr2",&Atom_O::getMembershipAr2)
	    .def("setMembershipAr3",&Atom_O::setMembershipAr3)
	    .def("getMembershipAr3",&Atom_O::getMembershipAr3)
	    .def("setMembershipAr4",&Atom_O::setMembershipAr4)
	    .def("getMembershipAr4",&Atom_O::getMembershipAr4)
	    .def("setMembershipAr5",&Atom_O::setMembershipAr5)
	    .def("getMembershipAr5",&Atom_O::getMembershipAr5)
	    .def("setSeenId",&Atom_O::setSeenId)
	    .def("getSeenId",&Atom_O::getSeenId)
	    .def("setBackCount",&Atom_O::setBackCount)
	    .def("getBackCount",&Atom_O::getBackCount)
	    .def("setBackSpan",&Atom_O::setBackSpan)
	    .def("getBackSpan",&Atom_O::getBackSpan)
	    .def("setNextSpan",&Atom_O::setNextSpan)
	    .def("getNextSpan",&Atom_O::getNextSpan)
	    .def("localSpanningTree",&Atom_O::localSpanningTree)
	    .def("getValence",&Atom_O::getValence)
	    .def("flagsSet",&Atom_O::flagsSet)
	    .def("applyTransformToAtoms",&Atom_O::applyTransformToAtoms)
	    .def("bondTo",&Atom_O::bondTo)
	    .def("bondToSingle",&Atom_O::bondToSingle)
//	.def("setAtomStorageId",&Atom_O::setAtomStorageId)
//	.def("getAtomStorageId",&Atom_O::getAtomStorageId)
	    .def("isBondedToAtomNamed",&Atom_O::isBondedToAtomNamed)
	    .def("bondedNeighborWithName",&Atom_O::bondedNeighborWithName)
	    .def("hasBondWithOrder",&Atom_O::hasBondWithOrder)
	    .def("isBondedToElementOrder",&Atom_O::isBondedToElementOrder)
	    .def("isBondedToElementHybridization",
		 &Atom_O::isBondedToElementHybridization)
	    .def("isBondedToElementHybridizationElementHybridization",
		 &Atom_O::isBondedToElementHybridizationElementHybridization)
	    .def("isBondedTo",&Atom_O::isBondedTo)
	    .def("isBondedToWithBondOrder",&Atom_O::isBondedToWithBondOrder)
	    .def("removeBondTo",&Atom_O::removeBondTo)
	    .def("removeAllBonds",&Atom_O::removeAllBonds)
	    .def("coordination",&Atom_O::coordination)
	    .def("bondedNeighbor",&Atom_O::bondedNeighbor)
	    .def("bondedOrder",&Atom_O::bondedOrder)
	    .def("totalBondOrder",&Atom_O::totalBondOrder)
//	    .def("copy",&Atom_O::copy,"","","",false)
	    .def("createImplicitHydrogenNamesOnCarbon",&Atom_O::createImplicitHydrogenNamesOnCarbon)
	    .def("testConsistancy",&Atom_O::testConsistancy)
//	.def("asXml",&Atom_O::asXml)
	    .def("numberOfBonds",&Atom_O::numberOfBonds)
	    .def("getBondList",&Atom_O::getBondList)
	    .def("bondsAsList",&Atom_O::bondsAsList)
	    .def("getHeavyAtomBondList",&Atom_O::getHeavyAtomBondList)
	    ;
	Defun_maker(ChemPkg,Atom);

	SYMBOL_EXPORT_SC_(ChemPkg,configurationToSymbolConverter);
	SYMBOL_EXPORT_SC_(ChemPkg,stereochemistryToSymbolConverter);

    }

    void Atom_O::exposePython(core::Lisp_sp lisp)
    {_G();
#ifdef USEBOOSTPYTHON
	PYTHON_CLASS(ChemPkg,Atom,"","",_lisp)
	    .def("atomName",&Atom_O::atomName)
	    .def("getRelativePriority", &Atom_O::getRelativePriority)
	    .def("bondedAtomsAsList", &Atom_O::bondedAtomsAsList)
	    .def("getAtomicNumber", &Atom_O::getAtomicNumber)
	    .def("isAromatic",&Atom_O::isAromatic)
	    .def("setIsAromatic",&Atom_O::setIsAromatic)
	    .def("getIonization",&Atom_O::getIonization)
	    .def("setIonization",&Atom_O::setIonization)
	    .def("getBondedHydrogenCount", &Atom_O::getBondedHydrogenCount)
//	.def("getMatterType",&Matter_O::getMatterType)
	    .def("atLowerAddressThan",&Atom_O::atLowerAddressThan)
	    .def("getAlias",&Atom_O::getAlias)
	    .def("setAlias",&Atom_O::setAlias)
	    .def("getAtomId",&Atom_O::getAtomId)
	    .def("getMoeIndex",&Atom_O::getMoeIndex)
	    .def("setMoeIndex",&Atom_O::setMoeIndex)
	    .def("getTempInt",&Atom_O::getTempInt)
	    .def("setTempInt",&Atom_O::setTempInt)
	    .def("isConfigurable",&Atom_O::isConfigurable)
	    .def("setStereochemistryType",&Atom_O::setStereochemistryType)
	    .def("getStereochemistryType",&Atom_O::getStereochemistryType)
	    .def("setConfiguration",&Atom_O::setConfiguration)
	    .def("getConfiguration",&Atom_O::getConfiguration)
	    .def("getConfigurationAsString",&Atom_O::getConfigurationAsString)
	    .def("calculateStereochemicalConfiguration",&Atom_O::calculateStereochemicalConfiguration)
	    .def("getFlags",&Atom_O::getFlags)
	    .def("resetFlags",&Atom_O::resetFlags)
	    .def("turnOnFlags",&Atom_O::turnOnFlags)
	    .def("turnOffFlags",&Atom_O::turnOffFlags)
	    .def("testAllFlags",&Atom_O::testAllFlags)
	    .def("testAnyFlags",&Atom_O::testAnyFlags)
	    .def("isInRing",&Atom_O::isInRing)
	    .def("inRingSize",&Atom_O::inRingSize)
	    .def("getRingMembershipCount",&Atom_O::getRingMembershipCount)
	    .def("setRingMembershipCount",&Atom_O::setRingMembershipCount)
	    .def("getNameIndex",&Atom_O::getNameIndex)
	    .def("getType",&Atom_O::getType)
	    .def("setType",&Atom_O::setType)
	    .def("getMoeType",&Atom_O::getMoeType)
	    .def("setMoeType",&Atom_O::setMoeType)
	    .def("getHybridization",&Atom_O::getHybridization)
	    .def("setHybridization",&Atom_O::setHybridization)
	    .def("getElementAsString",&Atom_O::getElementAsString)
	    .def("getElement",&Atom_O::getElement)
	    .def("setElement",&Atom_O::setElement)
	    .def("getResidueContainedBy", &Atom_O::getResidueContainedBy )
	    .def("getSelected",&Atom_O::getSelected)
	    .def("setSelected",&Atom_O::setSelected)
	    .def("getPositionInNanometers",&Atom_O::getPositionInNanometers)
	    .def("getPosition",&Atom_O::getPosition)
	    .def("setPositionInNanometers",&Atom_O::setPositionInNanometers)
	    .def("setPosition",&Atom_O::setPosition)
	    .def("getTouched",&Atom_O::getTouched)
	    .def("setTouched",&Atom_O::setTouched)
	    .def("getCharge",&Atom_O::getCharge)
	    .def("setCharge",&Atom_O::setCharge)
#if ATOMIC_ANCHOR
	    .def("getAnchorPos",&Atom_O::getAnchorPos)
	    .def("setAnchorPos",&Atom_O::setAnchorPos)
#endif
	    .def("getOccupancy",&Atom_O::getOccupancy)
	    .def("setOccupancy",&Atom_O::setOccupancy)
	    .def("getTempFactor",&Atom_O::getTempFactor)
	    .def("setTempFactor",&Atom_O::setTempFactor)
	    .def("getVdwRadius",&Atom_O::getVdwRadius)
	    .def("setVdwRadius",&Atom_O::setVdwRadius)
	    .def("getCovalentRadius",&Atom_O::getCovalentRadius)
	    .def("setCovalentRadius",&Atom_O::setCovalentRadius)
	    .def("setMembershipAr1",&Atom_O::setMembershipAr1)
	    .def("getMembershipAr1",&Atom_O::getMembershipAr1)
	    .def("setMembershipAr2",&Atom_O::setMembershipAr2)
	    .def("getMembershipAr2",&Atom_O::getMembershipAr2)
	    .def("setMembershipAr3",&Atom_O::setMembershipAr3)
	    .def("getMembershipAr3",&Atom_O::getMembershipAr3)
	    .def("setMembershipAr4",&Atom_O::setMembershipAr4)
	    .def("getMembershipAr4",&Atom_O::getMembershipAr4)
	    .def("setMembershipAr5",&Atom_O::setMembershipAr5)
	    .def("getMembershipAr5",&Atom_O::getMembershipAr5)
	    .def("setSeenId",&Atom_O::setSeenId)
	    .def("getSeenId",&Atom_O::getSeenId)
	    .def("setBackCount",&Atom_O::setBackCount)
	    .def("getBackCount",&Atom_O::getBackCount)
	    .def("setBackSpan",&Atom_O::setBackSpan)
	    .def("getBackSpan",&Atom_O::getBackSpan)
	    .def("setNextSpan",&Atom_O::setNextSpan)
	    .def("getNextSpan",&Atom_O::getNextSpan)
	    .def("localSpanningTree",&Atom_O::localSpanningTree)
	    .def("getValence",&Atom_O::getValence)
	    .def("flagsSet",&Atom_O::flagsSet)
	    .def("applyTransformToAtoms",&Atom_O::applyTransformToAtoms)
	    .def("bondTo",&Atom_O::bondTo)
	    .def("bondToSingle",&Atom_O::bondToSingle)
//	.def("setAtomStorageId",&Atom_O::setAtomStorageId)
//	.def("getAtomStorageId",&Atom_O::getAtomStorageId)
	    .def("isBondedToAtomNamed",&Atom_O::isBondedToAtomNamed)
	    .def("bondedNeighborWithName",&Atom_O::bondedNeighborWithName)
	    .def("hasBondWithOrder",&Atom_O::hasBondWithOrder)
	    .def("isBondedToElementOrder",&Atom_O::isBondedToElementOrder)
	    .def("isBondedToElementHybridization",
		 &Atom_O::isBondedToElementHybridization)
	    .def("isBondedToElementHybridizationElementHybridization",
		 &Atom_O::isBondedToElementHybridizationElementHybridization)
	    .def("isBondedTo",&Atom_O::isBondedTo)
	    .def("isBondedToWithBondOrder",&Atom_O::isBondedToWithBondOrder)
	    .def("removeBondTo",&Atom_O::removeBondTo)
	    .def("removeAllBonds",&Atom_O::removeAllBonds)
	    .def("coordination",&Atom_O::coordination)
	    .def("bondedNeighbor",&Atom_O::bondedNeighbor)
	    .def("bondedOrder",&Atom_O::bondedOrder)
	    .def("totalBondOrder",&Atom_O::totalBondOrder)
//	    .def("copy",&Atom_O::copy,"","","",false)
	    .def("createImplicitHydrogenNamesOnCarbon",&Atom_O::createImplicitHydrogenNamesOnCarbon)
	    .def("testConsistancy",&Atom_O::testConsistancy)
//	.def("asXml",&Atom_O::asXml)
	    .def("dump",&Atom_O::dump)
	    .def("numberOfBonds",&Atom_O::numberOfBonds)
	    .def("getBondList",&Atom_O::getBondList)
	    .def("bondsAsList",&Atom_O::bondsAsList)
	    .def("getHeavyAtomBondList",&Atom_O::getHeavyAtomBondList)
	    ;
#endif
    }











/*! Return +1 if priority(a)>priority(b)
 *  return -1 if priority(a)<priority(b)
 *  return 0 if priority(a) == priority(b)
 *  Priority is decided by comparing RelativePriority and if they are the same
 *  by comparing names.
 */
    int Atom_O::priorityOrder(Atom_sp a, Atom_sp b)
    {
	if ( a.nilp() && b.nilp() ) return 0;
	if ( b.nilp() ) return 1;
	if ( a.nilp() ) return -1;
	if ( a->getRelativePriority()>b->getRelativePriority() ) return 1;
	if ( a->getRelativePriority()<b->getRelativePriority() ) return -1;
	if ( a->getName().get() > b->getName().get() ) return 1;
	if ( a->getName().get() < b->getName().get() ) return -1;
	return 0;
    }



    ConfigurationEnum Atom_O::calculateStereochemicalConfiguration()
    {_G();
	if ( this->numberOfBonds() != 4 ) return undefinedConfiguration;
	core::List_sp neighborsByPriority = this->getNeighborsByRelativePriority();
	Atom_sp a4 = core::oCar(neighborsByPriority).as<Atom_O>();
	Atom_sp a3 = core::oCadr(neighborsByPriority).as<Atom_O>();
	Atom_sp a2 = core::oCaddr(neighborsByPriority).as<Atom_O>();
	Atom_sp a1 = core::oCadddr(neighborsByPriority).as<Atom_O>();
	Vector3 vme = this->getPosition();
	Vector3 v4 = a4->getPosition().sub(vme);
	Vector3 v3 = a3->getPosition().sub(vme);
	Vector3 v2 = a2->getPosition().sub(vme);
	Vector3 v1 = a1->getPosition().sub(vme);
	double dir2 = v2.dotProduct(v4.crossProduct(v3)); // For R this should be <0
	double dir1 = v1.dotProduct(v4.crossProduct(v3)); // For R this should be >0
	if ( dir1 > 0.0 ) // R_Configuration
	{
	    if ( dir2 < 0.0 )
	    {
	    	// Everything is consistent and (R)
		return R_Configuration;
	    }

	} else if ( dir1 < 0.0 )
	{
	    if ( dir2 > 0.0 )
	    {
		// Everything is consistent and (S)
		return S_Configuration;
	    }
	}
	return undefinedConfiguration;
    }


    string Atom_O::calculateStereochemicalConfigurationAsString()
    {_G();
	string s;
	ConfigurationEnum config = this->calculateStereochemicalConfiguration();
	switch ( config )
	{
	case S_Configuration:
	    s = "S";
	    break;
	case R_Configuration:
	    s = "R";
	    break;
	default:
	    s = "undefinedConfiguration";
	    break;
	}
	return s;
    }



//
// Constructor
//


    void	Atom_O::initialize()
    {_G();
	this->Base::initialize();
	this->copyAtom = _Nil<Atom_O>();
	this->flags = ATOM_NEEDS_MINIMIZER;
	this->_Mask = 0;
	this->tempInt = 0;
	this->_HintLP = false;
	this->_Element = element_Undefined;
	this->_Hybridization = hybridization_undefined;
	this->seenId = 0;
	this->_Alias = _Nil<core::Symbol_O>();
	this->_Ionization = 0; // neutral
	this->_Force.set(0.0,0.0,0.0);
#if ATOMIC_ANCHOR
	this->_AnchorRestraint.setIsDefined(false);
	this->_AnchorRestraint._AnchorPos.set(0.0,0.0,0.0);
#endif
	this->charge= 0;
	this->_RingMembershipCount = 0;
	this->type= _Nil<core::Symbol_O>();
	this->invalidateBackSpan();
	this->invalidateNextSpan();
	this->_MembershipAr1 = 0;
	this->_MembershipAr2 = 0;
	this->_MembershipAr3 = 0;
	this->_MembershipAr4 = 0;
	this->_MembershipAr5 = 0;
	this->_RelativePriority = 0;
	this->_Configuration = undefinedConfiguration;
	this->_StereochemistryType = undefinedCenter;
    }



    bool Atom_O::equal(core::T_sp obj) const
    {_G();
	if ( this->eq(obj) ) goto T;
	if ( obj.isA<Atom_O>() )
	{
	    Atom_sp other = obj.as<Atom_O>();
	    if ( other->getName() != this->getName() )
	    {
		LOG(BF("Atom names[ this->getName()=%s  other->getName()=%s ]  don't match!") % this->getName() % other->getName() );
		goto F;
	    }
	    if ( other->numberOfBonds() != this->numberOfBonds() )
	    {
		LOG(BF("Numbers of bonds don't match this->numberOfBonds()=%d  other->numberOfBonds()=%d!") % this->numberOfBonds() % other->numberOfBonds() );
		goto F;
	    }
	    goto T;
	}
	LOG(BF("The other object is not an ATOM!!!!"));
    F:
	LOG(BF("atoms this(%s) =equal= other(%s)  equal==false") % this->description() % obj->description() );
	return false;
    T:
	LOG(BF("atoms this(%s) =equal= other(%s)  equal==true") % this->description() % obj->description() );
	return true;
    }

    void Atom_O::transferCoordinates(Matter_sp obj)
    {_OF();
	if ( !this->equal(obj) )
	{
	    SIMPLE_ERROR(BF("This atom(%s) is not equal to %s so you cannot transfer coordinates")%this->description() % obj->description() );
	}
	Atom_sp other = obj.as<Atom_O>();
	this->setPosition(other->getPosition());
    }


//
// Copy constructor
//
    Atom_O::Atom_O(const Atom_O& ss) :Matter_O(ss)
    {
	this->_Element = ss._Element;
	this->_Alias = ss._Alias;
	this->type = ss.type;
	this->_Hybridization = ss._Hybridization;
	this->_HintLP = ss._HintLP;
	this->_selected = ss._selected;
	this->flags = ss.flags;
	this->_Mask = ss._Mask;
	this->position = ss.position;
	this->_Force = ss._Force;
#if ATOMIC_ANCHOR
	this->_AnchorRestraint = ss._AnchorRestraint;
#endif
	this->touched = ss.touched;
	this->charge = ss.charge;
	this->_RingMembershipCount = ss._RingMembershipCount;
	this->occupancy = ss.occupancy;
	this->tempFactor = ss.tempFactor;
	this->vdwRadius = ss.vdwRadius;
	this->covalentRadius = ss.covalentRadius;
	this->copyAtom = Atom_wp();
	this->tempInt = ss.tempInt;
	this->moeIndex = ss.moeIndex;
	this->moeType = ss.moeType;
	this->_Ionization = ss._Ionization;
	this->_MembershipAr1 = ss._MembershipAr1;
	this->_MembershipAr2 = ss._MembershipAr2;
	this->_MembershipAr3 = ss._MembershipAr3;
	this->_MembershipAr4 = ss._MembershipAr4;
	this->_MembershipAr5 = ss._MembershipAr5;
	this->_RelativePriority = ss._RelativePriority;
	this->_Configuration = ss._Configuration;
	this->_StereochemistryType = ss._StereochemistryType;
	this->_BackSpan = _Nil<Atom_O>();		// Is this a bad idea to access env in copy?
	this->_NextSpan = _Nil<Atom_O>();		// Is this a bad idea to access env in copy?
    }

//
// Destructor
//
//	Remove all bonds to this atom
//


    void Atom_O::setConfiguration(ConfigurationEnum conf)
    {_OF();
	this->_Configuration = conf;
	LOG(BF("Changed configuration of atom[%s] to [%s]")
	    % this->__repr__() % this->getConfigurationAsString() );
    }



    bool	Atom_O::atLowerAddressThan(Atom_sp b)
    {
	return this < b.get();
    }

    void Atom_O::setElementFromAtomName()
    {_G();
	Element element = elementFromAtomNameString(this->getName()->symbolName()->get());
	LOG(BF(" Resulting element= |%d|") %element );
	this->setElement(element);
    }

    void	Atom_O::setElementFromString(const string& str)
    {_G();
	Element element = elementFromAtomNameStringCaseInsensitive(str);
	LOG(BF(" Resulting element= |%d|") %element );
	this->setElement(element);
    }


    void Atom_O::setHybridizationFromString(const string& h)
    {_G();
        FIX_ME();
#if 0
	this->_Hybridization = hybridizationForString(h);
#endif
    }

    void Atom_O::_addHydrogenWithName(MatterName name)
    {_G();
	Atom_sp h = Atom_O::create();
	h->setName(name);
	h->setElement(element_H);
	Residue_sp res = this->getResidueContainedBy();
	res->addAtom(h);
	this->bondTo(h,singleBond);
    }

    Atom_sp Atom_O::bondedNeighbor(int i)
    {
	return this->bonds[i]->getOtherAtom(this->sharedThis<Atom_O>());
    }

    BondOrder Atom_O::bondedOrder(int i) 
    {
	return this->bonds[i]->getOrder();
    };

    int	Atom_O::numberOfBonds() const
    {
	return this->bonds.size();
    };

    int Atom_O::coordination() 
    {
	return this->bonds.size();
    };


/*! If this atom is a carbon then count the number of bonds and add
 * additional hydrogens to get the number of bonds to 4.
 * Only do this on neutral carbons.
 * Give the hydrogens names based on this atoms name.
 * eg:
 * This atom name :  Number of Hydrogens to add :  Hydrogen names
 *       CA                     1                       HA
 *       CB                     2                       HB1, HB2
 *       CC1                    3                       HC11, HC12, HC13 
 */ 
    core::List_sp Atom_O::createImplicitHydrogenNamesOnCarbon()
    {
	core::Cons_sp first, cons;
	first = core::Cons_O::create(_Nil<core::T_O>(),_Nil<core::T_O>());
	cons = first;
	if ( this->getElement() != element_C ) return _Nil<core::T_O>();
	if ( this->getIonization() != 0 ) return _Nil<core::T_O>();
	uint totalBondOrder = this->totalBondOrder();
	if ( totalBondOrder == 4 ) return _Nil<core::T_O>();
	uint addHydrogens = 4 - totalBondOrder;
	string nameSuffix = this->getName()->symbolName()->get().substr(1,9999);
	if ( addHydrogens == 1 )
	{
            MatterName hname = chemkw_intern("H"+nameSuffix);
	    core::Cons_sp one = core::Cons_O::create(hname,_Nil<core::T_O>());
	    cons->setCdr(one);
	    cons = one;
	    return oCdr(first);
	}
	for ( uint i=0; i<addHydrogens; i++ )
	{
	    stringstream ss;
	    ss << "H" << nameSuffix << (i+1);
            MatterName hname = chemkw_intern(ss.str());
	    core::Cons_sp one = core::Cons_O::create(hname,_Nil<core::T_O>());
	    cons->setCdr(one);
	    cons = one;
	}
	return oCdr(first);
    }




    void Atom_O::fillInImplicitHydrogensOnCarbon()
    {_G();
	core::List_sp names = this->createImplicitHydrogenNamesOnCarbon();
	if ( names.nilp() ) return;
	for ( auto cur : names ) {
	    this->_addHydrogenWithName(oCar(cur).as<MatterName::Type>());
	}
    }

//
//	randomizeAtomPositions
//
//	Set the coordinates of all atoms to random values
//		between 0 and 100 angstroms
//
//
    void	Atom_O::randomizeAtomPositions()
    {_G();
	double				x,y,z;
	Vector3				v;
	x = core::randomNumber01()*20.0;
	y = core::randomNumber01()*20.0;
	z = core::randomNumber01()*20.0;
	v.set(x,y,z);
	this->setPosition(v);
    }



//
//	perturbAtomPositions
//
//	Set the coordinates of all atoms to random values
//		between 0 and 100 angstroms
//
//
    void	Atom_O::perturbAtomPositions(double dist)
    {_G();
	double				xd,yd,zd;
	Vector3				v,vd;
	xd = (core::randomNumber01()*2.0-1.0)*dist;
	yd = (core::randomNumber01()*2.0-1.0)*dist;
	zd = (core::randomNumber01()*2.0-1.0)*dist;
	v = this->getPosition();
	vd.set(xd,yd,zd);
	v = v+vd;
	this->setPosition(v);
    }



gc::Nilable<Atom_sp> Atom_O::highestPriorityNeighborThatIsnt(gc::Nilable<Atom_sp> avoid)
    {_G();
	Atom_sp bestAtom = _Nil<Atom_O>();
	Atom_sp atom;
	VectorBond::iterator	b;
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ ) {
	    atom = (*b)->getOtherAtom(this->sharedThis<Atom_O>());
	    if ( avoid.notnilp() && atom == avoid ) continue;
	    if ( priorityOrder(atom,bestAtom)>0 )
	    {
		bestAtom = atom;
	    }
	}
	return bestAtom;
    }
    

gc::Nilable<Atom_sp> Atom_O::lowestPriorityNeighborThatIsnt(gc::Nilable<Atom_sp> avoid)
{
  gc::Nilable<Atom_sp> bestAtom = _Nil<core::T_O>();
  Atom_sp atom;
  VectorBond::iterator	b;
  for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ ) {
    atom = (*b)->getOtherAtom(this->sharedThis<Atom_O>());
    if ( avoid.notnilp() && atom == avoid ) continue;
    if ( bestAtom.nilp() ) {
      bestAtom = atom;
      continue;
    }
    if ( priorityOrder(atom,bestAtom)<0 ) bestAtom = atom;
  }
  return bestAtom;
}




    Bond_sp Atom_O::getBondTo(Atom_sp a)
    {_OF();
	VectorBond::iterator	b;
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ )
	{
	    if ( a.get() == (*b)->getOtherAtom(this->sharedThis<Atom_O>()).get() ) return *b;
	}
	SIMPLE_ERROR(BF("Could not find bond"));
    }

    void Atom_O::_addExistingBond(Bond_sp const& bond)
    {_OF();
	this->bonds.push_back(bond);
    }

    void Atom_O::addBond(Bond_sp bond)
    {_OF();
	this->bonds.push_back(bond);
    }

    Bond_sp Atom_O::bondTo( Atom_sp to, BondOrder o )
    {_OF();
	// Check if there is already a bond to this atom and
	// throw an exception if there is
	Atom_sp from = this->sharedThis<Atom_O>();
	VectorBond::iterator	b;
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ )
	{
	    if ( (*b)->getOtherAtom(from) == to )
	    {
		SIMPLE_ERROR(BF("You tried to form a bond from[%s]-to[%s] but there is already one there!!") 
				   % this->__repr__() % to->__repr__() );
	    }
	}
	Bond_sp bn = Bond_O::create(from,to,o);
	this->bonds.push_back(bn);
	to.get()->bonds.push_back(bn);
	return bn;
    }





    Bond_sp Atom_O::bondToSingle(  Atom_sp a )
    {_OF();
	return this->bondTo(a,singleBond);
    }



    string	Atom_O::getNameIndex()
    {
        FIX_ME();
#if 0
	string	s;
	string	sIndex;
	char	caIndex[255];

	s = this->getName()+":";
	sprintf( caIndex, "%d", this->moeIndex );
	s = s + caIndex;
	return(s);
#endif
    }


    class	OrderByPriorityAndName
    {
    public:
	bool operator() ( Atom_sp x, Atom_sp y )
	{
	    if ( x->getRelativePriority()<y->getRelativePriority() ) return true;
	    if ( x->getRelativePriority()>y->getRelativePriority() ) return false;
	    if ( x->getName()->symbolNameAsString()<y->getName()->symbolNameAsString() ) return true;
	    return false;
	}
    };

    core::List_sp	Atom_O::getNeighborsByRelativePriority()
    {_G();
	LOG(BF("Ordering neighbors around(%s) by priority and name") % this->getName()  );
	VectorAtom	reversedNeighbors;
	VectorBond::iterator	b;
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ ) {
	    reversedNeighbors.push_back((*b)->getOtherAtom(this->sharedThis<Atom_O>()));
	}
	OrderByPriorityAndName orderer;
	core::List_sp ncons = _Nil<core::T_O>();
        core::sort::quickSort(reversedNeighbors.begin(),reversedNeighbors.end(),orderer);
	// At this point the sorted in reverse order
	// 
	// Now copy them into a Cons list backwards to get the
	// proper order
	LOG(BF("The sorted neighbors in REVERSE order is:") );
	for ( VectorAtom::iterator ni=reversedNeighbors.begin();
	      ni!=reversedNeighbors.end(); ni++ )
	{
	    LOG(BF("    neighbor priority(%u) name(%s)") % (*ni)->getRelativePriority() % (*ni)->getName()  );
	    ncons = core::Cons_O::create(*ni,ncons);
	}
	return ncons;
    }





    void	Atom_O::modifyFlags(int op, ATOM_FLAGS fl ) {
	if ( op == ATOMFLAG_OR ) {
	    this->flags |= fl;
	} else if ( op==ATOMFLAG_AND ) {
	    this->flags &= fl;
	} else if ( op==ATOMFLAG_ON ) {
	    this->flags |= fl;
	} else if ( op==ATOMFLAG_OFF ) {
	    this->flags &= (~fl);
	}
    }




    bool	Atom_O::isBondedToAtomNamed(MatterName name)
    {
	VectorBond::iterator	b;

	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ ) {
	    if ( (*b)->getOtherAtom(this->sharedThis<Atom_O>())->getName() == name ) {
		return true;
	    }
	}
	return false;
    }

    Atom_sp	Atom_O::bondedNeighborWithName(MatterName name)
    {
	VectorBond::iterator	b;
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ ) {
	    if ( (*b)->getOtherAtom(this->sharedThis<Atom_O>())->getName() == name ) {
		return (*b)->getOtherAtom(this->sharedThis<Atom_O>());
	    }
	}
	Atom_sp a = _Nil<Atom_O>();
	return a;
    }


    bool	Atom_O::isBondedToElementOrder(Element el, BondOrder o)
    {
	VectorBond::iterator	b;

	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ ) 
	{
	    if ( (*b)->getOtherAtom(this->sharedThis<Atom_O>())->getElement() == el ) 
	    {
		if ( (*b)->getOrder() == o ) return true;
	    }
	}
	return false;
    }


    bool Atom_O::hasBondWithOrder(BondOrder o) const
    {
	VectorBond::const_iterator	b;
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ ) 
	{
	    if ( (*b)->getOrder() == o) return true;
	}
	return false;
    }



    bool	Atom_O::isBondedToElementHybridization(Element el, Hybridization hy)
    {
	VectorBond::iterator	b;

	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ ) 
	{
	    if ( (*b)->getOtherAtom(this->sharedThis<Atom_O>())->getElement() == el ) 
	    {
		if ( (*b)->getOtherAtom(this->sharedThis<Atom_O>())->getHybridization() == hy ) return true;
	    }
	}
	return false;
    }




    bool	Atom_O::isBondedToElementHybridizationElementHybridization(Element el1, Hybridization hy1, Element el2, Hybridization hy2)
    {_OF();
	IMPLEMENT_ME(); // Handle new way of handling bonds
#if 0
	VectorBond::iterator	b;
	VectorBond::iterator	b2;
	Atom_sp	a1;
	Atom_sp	a2;
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ ) 
	{
	    a1 = Atom_sp((*b)->getTo());
	    if ( a1->getElement() == el1 ) 
	    {
		if ( a1->getHybridization() == hy1 ) 
		{
		    for ( b2=a1->bonds.begin();b2!=a1->bonds.end();b2++) 
		    {
			a2 = Atom_sp((*b2)->getTo());
			if ( a2 == this->sharedThis<Atom_O>() ) continue;
			if ( a2->getElement() == el2 ) 
			{
			    if ( (*b2)->getTo()->getHybridization() == hy2 ) return true;
			}
		    }
		}
	    }
	}
	return false;
#endif
    }

    void Atom_O::makeAllAtomNamesInEachResidueUnique()
    {_G();
	SIMPLE_ERROR(BF("This should never be called for an Atom_sp"));
    }


    void Atom_O::basicRemoveBondTo(Atom_sp a)
    {_OF();
	VectorBond::iterator	b;
	Bond_wp				bb;
	Atom_wp				aa;
	Atom_wp				wpa;
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ )
	{
	    if ( (*b)->getOtherAtom(this->sharedThis<Atom_O>()) == a )
	    {
		this->bonds.erase(b);
		return;
	    }
	}
	stringstream ss;
	ss << "Trying to remove bond from atom(" << this->description()
	   << ") but I can't find the to atom(" << a->description() << ")";
	SIMPLE_ERROR(BF("%s")%ss.str());
    }

    void	Atom_O::removeBondTo(Atom_sp a)
    {_OF();
	Atom_sp	atemp;
	LOG(BF("Bond_O::removeBondTo") );
	this->basicRemoveBondTo(a);
	atemp = this->sharedThis<Atom_O>();
	a->basicRemoveBondTo(atemp);
    }

    void	Atom_O::removeAllBonds()
    {_OF();
	VectorBond::iterator	b;
	Atom_sp				atemp;
	// Remove back bond
	LOG(BF("Bond_O::removeAllBonds for %s") % this->description() );
	atemp = this->sharedThis<Atom_O>();
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ )
	{
	    Atom_sp ato = (*b)->getOtherAtom(atemp);
	    ato->basicRemoveBondTo(atemp);
	}
	this->bonds.clear();
    }



//
//	applyTransformToAtoms
//
    void	Atom_O::applyTransformToAtoms( const Matrix& m )
    {
	this->position = (m)*this->position;
    }





    string Atom_O::getHybridizationAsString()
    {_G();
	return hybridizationSymbolFromHybridization(this->getHybridization())->symbolName()->get();
    }

    string Atom_O::getElementAsString()
    {_G();
	return atomicSymbolFromElement(this->getElement())->symbolName()->get();
    }

    core::Symbol_sp Atom_O::getElementAsSymbol() const
    {_G();
	return symbolFromElement(this->getElement());
    }


#ifdef OLD_SERIALIZE
    void	Atom_O::serialize(serialize::SNode snode)
    {_OF();
	this->Base::serialize(snode);
	snode->attributeIfNotDefault<ATOM_FLAGS>( "flags", this->flags, (unsigned long)0 );
	snode->attributeSymbolEnumHiddenConverter( "element", this->_Element, _sym__PLUS_elementToSymbolConverter_PLUS_);
	snode->attributeSymbolEnumHiddenConverterIfNotDefault( "hybrid", this->_Hybridization, _sym__PLUS_hybridizationToSymbolConverter_PLUS_,hybridization_sp3 );
	snode->attributeIfNotNil( "alias", this->_Alias );
	snode->attributeIfNotDefault<uint>( "priority", this->_RelativePriority, 0 );
	snode->attributeIfNotDefault<bool>( "hintLP", this->_HintLP,false );
	snode->attributeIfNotDefault<double>( "chg", this->charge, 0.0 );
	snode->attributeSymbolEnumHiddenConverterIfNotDefault( "configuration", this->_Configuration, _sym__PLUS_configurationEnumConverter_PLUS_, undefinedConfiguration  );
	snode->attributeSymbolEnumHiddenConverterIfNotDefault( "stereochemistryType", this->_StereochemistryType, _sym__PLUS_stereochemistryTypeConverter_PLUS_, undefinedCenter );
	snode->attributeIfNotDefault<int>( "ion", this->_Ionization, 0 );
	snode->attributeIfNotDefault<int>( "rings", this->_RingMembershipCount, 0 );
	snode->attributeIfNotDefault<int>( "tempInt", this->tempInt, 0 );
	snode->attributeIfNotDefault<string>( "type", this->typeString, "" );
	snode->attributeIfNotDefault<int>( "ar1", this->_MembershipAr1, 0 );
	snode->attributeIfNotDefault<int>( "ar2", this->_MembershipAr2, 0 );
	snode->attributeIfNotDefault<int>( "ar3", this->_MembershipAr3, 0 );
	snode->attributeIfNotDefault<int>( "ar4", this->_MembershipAr4, 0 );
	snode->attributeIfNotDefault<int>( "ar5", this->_MembershipAr5, 0 );
	snode->attributeIfNotDefault<uint>( "mask", this->_Mask, (unsigned int)(0) );
	snode->archivePlainObjectIfDefined<Vector3>( "pos","Vector3",
						    this->position.isDefined(), this->position );
	LOG(BF("After pos archived Atom position@%p = %s") % &(this->position) % this->position.asString()  );
	LOG(BF("Atom position = %s") % this->position.asString() );
    }
#endif





#ifdef CONSPACK
//    SYMBOL_EXPORT_SC_(ChemArchivePkg,flags);
//    SYMBOL_EXPORT_SC_(ChemArchivePkg,element);
    void	Atom_O::archiveBase(core::ArchiveP node)
    {_G();
	this->Base::archiveBase(node);
	node->attributeIfNotDefault<ATOM_FLAGS>( "flags", this->flags, (unsigned long)0 );
	node->attributeSymbolEnumHiddenConverter( "element", this->_Element, _sym__PLUS_elementToSymbolConverter_PLUS_);
	node->attributeSymbolEnumHiddenConverterIfNotDefault( "hybrid", this->_Hybridization, _sym__PLUS_hybridizationToSymbolConverter_PLUS_,hybridization_sp3 );
	node->attributeIfNotNil( "alias", this->_Alias );
	node->attributeIfNotDefault<uint>( "priority", this->_RelativePriority, 0 );
	node->attributeIfNotDefault<bool>( "hintLP", this->_HintLP,false );
	node->attributeIfNotDefault<double>( "chg", this->charge, 0.0 );
	node->attributeIfNotDefault<int>( "ion", this->_Ionization, 0 );
	node->attributeIfNotDefault<int>( "rings", this->_RingMembershipCount, 0 );
	node->attributeIfNotDefault<int>( "tempInt", this->tempInt, 0 );
	node->attributeIfNotDefault<string>( "type", this->typeString, "" );
	node->attributeIfNotDefault<int>( "ar1", this->_MembershipAr1, 0 );
	node->attributeIfNotDefault<int>( "ar2", this->_MembershipAr2, 0 );
	node->attributeIfNotDefault<int>( "ar3", this->_MembershipAr3, 0 );
	node->attributeIfNotDefault<int>( "ar4", this->_MembershipAr4, 0 );
	node->attributeIfNotDefault<int>( "ar5", this->_MembershipAr5, 0 );
	node->attributeIfNotDefault<uint>( "mask", this->_Mask, (uint)(0) );
	node->attributeSymbolEnumHiddenConverterIfNotDefault( "configuration", this->_Configuration, _sym__PLUS_configurationEnumConverter_PLUS_, undefinedConfiguration  );
	node->attributeSymbolEnumHiddenConverterIfNotDefault( "stereochemistryType", this->_StereochemistryType, _sym__PLUS_stereochemistryTypeConverter_PLUS_, undefinedCenter );
	node->attributePODIfDefined<Vector3>( "pos","Vector3",
					      this->position.isDefined(),
				     this->position );
	LOG(BF("After pos archived Atom position@%p = %s") % &(this->position) % this->position.asString()  );
	LOG(BF("Atom position = %s") % this->position.asString() );
    }
#endif


    void Atom_O::setPositionInNanometers(Vector3 o)
    {_OF();
	Vector3 angpos = o.multiplyByScalar(10.0);
	this->position = angpos;
    }


    void Atom_O::setTempInt(int i)
    {_G();
	LOG(BF("Setting tempInt to %d") % i  );
	this->tempInt = i;
    }

    int Atom_O::getTempInt()
    {_G();
	return this->tempInt;
    }

    string	Atom_O::getConfigurationAsString()
    {
	string config, stereo;
	if ( this->_Configuration == undefinedConfiguration )
	{
	    config = "UndefinedConfiguration";
	} else if ( this->_Configuration == S_Configuration )
	{
	    config = "(S)";
	} else if ( this->_Configuration == R_Configuration )
	{
	    config = "(R)";
	} else
	{
	    config = "???";
	}
	if ( this->_StereochemistryType == undefinedCenter )
	{
	    stereo = "UndefinedCenter";
	} else if ( this->_StereochemistryType == chiralCenter )
	{
	    stereo = "ChiralCenter";
	} else if ( this->_StereochemistryType == prochiralCenter )
	{
	    stereo = "ProchiralCenter";
	} else
	{
	    stereo = "???";
	}
	return config+"/"+stereo;
    }






//
// writeUniqueBondsToStream
//
// Stream the atoms bonds description to the output
//
#if 0
    void Atom_O::writeUniqueBondsToStream(string prefix, std::ostream& out )
    {
	VectorBond	bonds;
	VectorBond::iterator	b;
	bonds = this->getBonds();
	Atom_sp me = this->sharedThis<Atom_O>();
	for ( b=bonds.begin();b!=bonds.end() ; b++ ) {
	    if ( me->atLowerAddressThan((*b)->getOtherAtom(me)))
	    {
		out << prefix << "bond ";
		switch ( (*b)->getOrder() ) {
		case singleBond:
		    out << "single";
		    break;
		case doubleBond:
		    out << "double";
		    break;
		case tripleBond:
		    out << "triple";
		    break;
		case aromaticBond:
		    out << "aromatic";
		    break;
		default:
		    out << "--unknown--";
		}
		out << " " << me->getId() << " ";
		out << (*b)->getOtherAtom(me)->getId() << " ;" << std::endl;
	    }
	}
    }
#endif



    string	Atom_O::__repr__() const
    {
	stringstream ss;
	if ( this->containedByValid() )
	{
	    Residue_sp res = _Nil<Residue_O>();
	    Molecule_sp mol = _Nil<Molecule_O>();
	    if ( this->containedByValid() )
	    {
		res = this->containedBy().as<Residue_O>();
		if ( res->containedByValid() )
		{
		    mol = res->containedBy().as<Molecule_O>();
		}
	    }
	    string molName = (mol.nilp())?"":mol->getName()->symbolName()->get();
	    string resName = (res.nilp())?"":res->getName()->symbolName()->get();
	    ss << "#<" << this->className() << ":" << molName << ":" << resName << "@" << this->name << "@" << (void*)this << ">";
	} else
	{
          ss << "#<" << this->className() << " :invalidContainer! :name \"" << this->name << "@" << (void*)this << ">";
	}
	return ss.str();
    }

    string	Atom_O::description() const
    {
	stringstream ss;
	ss << this->className() << "("<<this->getName();
	if ( this->containerContainedBy.notnilp() ) {
          ss <<"["<<this->getResidueContainedBy_const()->description()<<"]";
	} else {
	    ss << "[residue-NULL-]";
	}
	ss << "bonds[" << this->numberOfBonds() << "]";
	ss <<")";
	return ss.str();
    }


/*!
 * Add all unique intra residue bonds to the BondList_sp
 */
    void	Atom_O::addUniqueIntraResidueBondCopiesToBondList(BondList_sp list)
    {_OF();
	VectorBond			bonds;
	VectorBond::iterator	b;
	ASSERTNOTNULL(list);
	bonds = this->getBonds();
	Atom_sp me = this->sharedThis<Atom_O>();
	for ( b=bonds.begin();b!=bonds.end() ; b++ )
	{
	    if ( me->atLowerAddressThan( (*b)->getOtherAtom(me)))
	    {
		if ( !(*b)->isInterResidueBond() ) {
		    LOG(BF("Original bond: %s") % (*b)->description() );
		    GC_COPY(Bond_O,bondCopy,*(b->get()));
		    LOG(BF("Copy bond: %s") % bondCopy->description() );
		    list->addBond(bondCopy);
		}
	    }
	}
    }

/*!
 * Add all unique inter residue bonds to the BondList_sp
 */
    void Atom_O::addUniqueInterResidueBondCopiesToBondList(BondList_sp list)
    {_OF();
	VectorBond			bonds;
	VectorBond::iterator	b;
	ASSERTNOTNULL(list);
	bonds = this->getBonds();
	Atom_sp me = this->sharedThis<Atom_O>();
	for ( b=bonds.begin();b!=bonds.end() ; b++ )
	{
	    if ( me->atLowerAddressThan( (*b)->getOtherAtom(me)))
	    {
		if ( (*b)->isInterResidueBond() ) {
		    GC_COPY(Bond_O, bondCopy, *(b->get()) ); // = RP_Copy<Bond_O>(*b);
		    list->addBond(bondCopy);
		}
	    }
	}
    }

    void Atom_O::addInterResidueBondsToBondList(BondList_sp bondlist)
    {_G();
	VectorBond::iterator	b;
	bonds = this->getBonds();
	for ( b=bonds.begin();b!=bonds.end() ; b++ )
	{
	    if ( (*b)->isInterResidueBond() ) {
                bondlist->append(*b);
	    }
	}
    }


//
// isBondedTo
//
// Return true if we are bonded to the atom described by the required
// bond order
    bool Atom_O::isBondedToWithBondOrder( Atom_sp aTarget, BondOrder o )
    {_OF();
	VectorBond::iterator	b;
	Atom_sp				a2;
	Atom_sp me = this->sharedThis<Atom_O>();
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ )
	{
	    a2 = (*b)->getOtherAtom(me);
	    LOG(BF("   and 0x%08x is bonded to: 0x%08x") % this % a2.get()  );
	    if ( a2 == aTarget ) {
		if ( o == (*b)->getOrder() ) {
		    return true;
		}
	    }
	}
	return false;
    }

//
// isBondedTo
//
// Return true if we are bonded to the atom described by the required
// bond order
    bool	Atom_O::isBondedTo( Atom_sp aTarget)
    {_G();
	VectorBond::iterator	b;
	Atom_sp				a2;
	LOG(BF("Looking at atom(%s) for bonded atom(%s)")
	    % this->getName()
	    % aTarget->getName() );
	Atom_sp me = this->sharedThis<Atom_O>();
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ )
	{
	    a2 = (*b)->getOtherAtom(me);
	    LOG(BF("  looking at atom(%s)") % a2->getName() );
	    if ( a2 == aTarget )
	    {
		LOG(BF("Found match") );
		return true;
	    }
	}
	LOG(BF("No match") );
	return false;
    }
//
// bondOrderTo
//
// Return the bond order by which we are bonded to the atom.
// Return noBond if there is no bond.
    BondOrder	Atom_O::bondOrderTo( Atom_sp aTarget )
    {
	VectorBond::iterator	b;
	Atom_sp				a2;
	Atom_sp me = this->sharedThis<Atom_O>();
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ )
	{
	    a2 = (*b)->getOtherAtom(me);
	    if ( a2 == aTarget )
	    {
		return (*b)->getOrder();
	    }
	}
	return noBond;
    }


//
// testConsistancy
//
//	Make sure that all of the bonds are consistant
    bool Atom_O::testConsistancy(Matter_sp parentShouldBe)
    {_OF();
	VectorBond::iterator	b;
	Atom_sp				a2,atemp;
	LOG(BF("bonds array start = 0x%08x") % ((void*)&(this->bonds[0]))  );
	Atom_sp me = this->sharedThis<Atom_O>();
	for ( b=this->bonds.begin();b!=this->bonds.end() ; b++ )
	{
	    a2 = (*b)->getOtherAtom(me);
	    LOG(BF("Testing consistancy of atom: 0x%08x should be bonded to: 0x%08x")
		% (this)
		% a2.get() );
	    if ( !(a2->isBondedToWithBondOrder(me,(*b)->getOrder())) )
	    {
		_lisp->print(BF( "Atom_O::testConsistancy failed" ));
		return false;
	    }
	}
	return true;
    }



    Matter_sp Atom_O::copy()
    {_OF();
	SIMPLE_ERROR(BF("Don't copy single atoms - bonds will be messed up"));
    }




//
//	copy
//
//	Allocate a copy of this atom and return it.
//	Keep track of the new atom so that we can redirect pointers to the copy.
//
    Matter_sp Atom_O::copyDontRedirectAtoms()
    {_OF();
	LOG(BF("Copying atom @%p") % this );
	Atom_sp myself = this->sharedThis<Atom_O>();
	GC_COPY(Atom_O, aNew , *this); // = RP_Copy<Atom_O>(this);
	for ( VectorBond::const_iterator b=bonds.begin();b!=bonds.end() ; b++ )
	{
	    /* Only copy Bond object if we are the Atom1
	     This is to ensure that bonds aren't duplicated*/
	    if ( (*b)->isAtom1(myself) )
	    {
		Bond_sp bNew = (*b)->copyDontRedirectAtoms();
		aNew->bonds.push_back(bNew);
	    }
	}
	this->copyAtom = aNew;
	aNew->copyRestraintsDontRedirectAtoms(this);
	LOG(BF("    copy atom== %s") % aNew->description());
	return(aNew);
    }



//
//	redirectAtom
//
//	Redirect the bonds of this atom to their copies.
    void Atom_O::redirectAtoms()
    {_G();
	VectorBond::iterator	b;
	LOG(BF("Redirecting bonds for %s") % this->description());
	for ( b=bonds.begin();b!=bonds.end() ; b++ )
	{
	    (*b)->redirectAtoms();
	}
	this->redirectRestraintAtoms();
    }




    VectorAtom	Atom_O::getBondedAtoms()
    {
	VectorBond::iterator	b;
	VectorAtom			vAtoms;
	Atom_sp me = this->sharedThis<Atom_O>();
	for (b=this->bonds.begin();b!=this->bonds.end(); b++ )
	{
	    vAtoms.push_back((*b)->getOtherAtom(me));
	}
	return vAtoms;
    }

    core::List_sp Atom_O::bondedAtomsAsList()
    {
	core::List_sp list = _Nil<core::T_O>();
	VectorBond::iterator	b;
	Atom_sp me = this->sharedThis<Atom_O>();
	for (b=this->bonds.begin();b!=this->bonds.end(); b++ )
	{
	    list = core::Cons_O::create((*b)->getOtherAtom(me),list);
	}
	return list;
    }

    void	advanceAtomBuildCounter()
    {
	__AtomBuildCounter++;
	if (__AtomBuildCounter==0) __AtomBuildCounter++;
    };

    long	getAtomBuildCounter()
    {
	return __AtomBuildCounter;
    };


    BondList_sp	Atom_O::getBondList()
    {
	VectorBond::iterator	b;
	GC_ALLOCATE(BondList_O, bl );
	for (b=this->bonds.begin();b!=this->bonds.end(); b++ )
	{
	    bl->addBond(*b);
	}
	return bl;
    }

    uint Atom_O::maxTotalBondOrder()
    {
	return chem::maxTotalBondOrderForElement(this->getElement());
    }


    uint Atom_O::totalBondOrder()
    {_OF();
	VectorBond::iterator	b;
	core::List_sp		list;
	uint twice = 0;
	for (b=this->bonds.begin();b!=this->bonds.end(); b++ ) 
	{
	    if ( (*b)->getOrder() == singleBond ) twice += 2;
	    else if ((*b)->getOrder() == doubleBond ) twice += 4;
	    else if ((*b)->getOrder() == aromaticBond ) twice += 3;
	    else if ((*b)->getOrder() == tripleBond ) twice += 6;
	    else if ((*b)->getOrder() == dashedSingleBond ) twice += 2;
	    else if ((*b)->getOrder() == dashedDoubleBond ) twice += 4;
	    else twice += 2;
	}
	if ( (twice & 1) != 0 )
	{
	    SIMPLE_ERROR(BF("The total bond order for "+this->description()+" will not be a whole number"));
	}
	return twice/2;
    }


    core::List_sp Atom_O::bondsAsList()
    {
	VectorBond::iterator	b;
	core::List_sp		list;
	list = _Nil<core::T_O>();
	for (b=this->bonds.begin();b!=this->bonds.end(); b++ ) {
	    list = core::Cons_O::create(*b,list);
	}
	return list;
    }

    bool	Atom_O::isHeavyAtom()
    {_G();
	return ((this->_Element!=element_H) && (this->_Element!=element_D) && (this->_Element!=element_T));
    }


    BondList_sp	Atom_O::getHeavyAtomBondList()
    {
	VectorBond::iterator	b;
	GC_ALLOCATE(BondList_O, bl );
	Atom_sp me = this->sharedThis<Atom_O>();
	for (b=this->bonds.begin();b!=this->bonds.end(); b++ )
	{
	    if ( (*b)->getOtherAtom(me)->getElement() != element_H )
	    {
		bl->addBond(*b);
	    }
	}
	return bl;
    }




    int     Atom_O::getAtomicNumber()
    {
	return atomicNumberForElement(this->getElement());

    }

    double	Atom_O::getAtomicWeight()
    {
	return atomicWeightForElement(this->getElement());
    }


    int     Atom_O::getBondedHydrogenCount()
    {_OF();
	VectorBond::iterator	b;
	int                             hs;
	Atom_wp				weakAtomTo;
	hs=0;
	Atom_sp me = this->sharedThis<Atom_O>();
	for (b=this->bonds.begin();b!=this->bonds.end(); b++ )
	{
	    Atom_sp other = (*b)->getOtherAtom(me);
	    if ( other->getElement() == element_H ) hs++;
	}
	return hs;
    }


    bool    Atom_O::inRingSize(int r) const
    {_OF();
	bool result = false;
	switch (r)
	{
        case 3:
            result = this->testAnyFlags(in3MemberRing);
	    break;
        case 4:
            result = this->testAnyFlags(in4MemberRing);
	    break;
        case 5:
            result = this->testAnyFlags(in5MemberRing);
	    break;
        case 6:
            result = this->testAnyFlags(in6MemberRing);
	    break;
        case 7:
            result = this->testAnyFlags(in7MemberRing);
	    break;
        case 8:
            result = this->testAnyFlags(in8MemberRing);
	    break;
        default:
	    result = false;
	    break;
	}
	LOG(BF("Tested ring membership[%s]=%d of %s") % r % result % this->description() );
	return result;
    }


    void Atom_O::clearAllRingMembershipFlags()
    {_G();
	this->turnOffFlags(InRing);
    }


    void Atom_O::setInRingOfSize(int r)
    {_OF();
	LOG(BF("Setting %s in ring of size[%d]") % this->description() % r );
	switch (r) {
        case 3:
            this->turnOnFlags(in3MemberRing);
	    break;
        case 4:
            this->turnOnFlags(in4MemberRing);
	    break;
        case 5:
            this->turnOnFlags(in5MemberRing);
	    break;
        case 6:
            this->turnOnFlags(in6MemberRing);
	    break;
        case 7:
            this->turnOnFlags(in7MemberRing);
	    break;
        case 8:
            this->turnOnFlags(in8MemberRing);
	    break;
	default:
	    // We ignore larger rings
	    break;
	}
    }



    int     Atom_O::getValence()
    {
	VectorBond::iterator	b;
	int             valence;
	BondOrder       bo;
	valence = 0;
	for (b=this->bonds.begin();b!=this->bonds.end(); b++ )
	{
	    bo = (*b)->getOrder();
	    switch ( bo ) {
            case singleBond:
                valence += 1;
                break;
            case doubleBond:
            case aromaticBond:
                valence += 2;
                break;
            case tripleBond:
                valence += 3;
                break;
	    default:
	        valence += 0;
	    }
	}
	return valence;
    }



    Atom_sp Atom_O::getBackSpan()
    {_G();
      ASSERT(this->_BackSpan);
      return this->_BackSpan;
    }


    Atom_sp Atom_O::getNextSpan()
    {_G();
      ASSERT(this->_NextSpan);
      return this->_NextSpan;
    }


    core::List_sp Atom_O::_expandLocalSpanningTree(Atom_sp avoidAtom, Bond_sp incomingBond, uint depth)
    {
	core::Cons_sp localTree = core::Cons_O::create(incomingBond);
	if ( depth <= 0 ) return localTree;
	core::List_sp tail = localTree;
	for ( VectorBond::iterator bi=this->bonds.begin(); bi!=this->bonds.end(); bi++ )
	{
	    Atom_sp neighborAtom = (*bi)->getOtherAtom(this->sharedThis<Atom_O>());
	    if ( neighborAtom == avoidAtom ) continue;
	    core::List_sp one = neighborAtom->_expandLocalSpanningTree(this->sharedThis<Atom_O>(),*bi,depth-1);
	    tail.asCons()->setCdr(one);
	    tail = one;
	}
	return localTree;
    }

    core::List_sp Atom_O::localSpanningTree(uint depth)
    {_G();
	core::Cons_sp localTree = core::Cons_O::create(this->sharedThis<Atom_O>());
	if ( depth <= 0 ) return localTree;
	core::List_sp tail = localTree;
	for ( VectorBond::iterator bi=this->bonds.begin(); bi!=this->bonds.end(); bi++ )
	{
	    Atom_sp neighborAtom = (*bi)->getOtherAtom(this->sharedThis<Atom_O>());
	    core::List_sp one = neighborAtom->_expandLocalSpanningTree(this->sharedThis<Atom_O>(),*bi,depth-1);
	    tail.asCons()->setCdr(one);
	    tail = one;
	}
	return localTree;
    }


    bool Atom_O::isConfigurable()
    {
	return this->_StereochemistryType != undefinedCenter;
    }


    void Atom_O::invertStructureAndRestraints()
    {_G();
	/*! Invert the Z coordinate */
	Vector3& pos=this->position;
	this->position.set(pos.getX(),pos.getY(),pos.getZ()*-1.0);
#if ATOMIC_ANCHOR
	Vector3& anchor = this->_AnchorRestraint._AnchorPos;
	anchor.set(anchor.getX(),anchor.getY(),anchor.getZ()*-1.0);
#else
	this->invertStereochemistryOfRestraints();
#endif
	
	ConfigurationEnum config = this->_Configuration;
	if ( config == S_Configuration )
	{
	    this->_Configuration = R_Configuration;
	} else if ( config == R_Configuration )
	{
	    this->_Configuration = S_Configuration;
	}
    }




#if 0
    ConfigurationEnum Atom_O::getConfiguration()
    {
	return this->_Configuration;
    }
    void	Atom_O::setConfigurationPriorityHighToLow(string highest, string high, string low, string lowest )
    {
	ASSERTP(this->isConfigurable(), "You cannot assign priorities to an unconfigurable atom");
	this->_ConfigurationInfo._HighestPriority = highest;
	this->_ConfigurationInfo._HighPriority = high;
	this->_ConfigurationInfo._LowPriority = low;
	this->_ConfigurationInfo._LowestPriority = lowest;
    }

    string	Atom_O::getConfigurationPriorityHighest()
    {
	ASSERTP(this->isConfigurable(), "You cannot assign priorities to an unconfigurable atom");
	return this->_ConfigurationInfo._HighestPriority;
    }

    string	Atom_O::getConfigurationPriorityHigh()
    {
	ASSERTP(this->isConfigurable(), "You cannot assign priorities to an unconfigurable atom");
	return this->_ConfigurationInfo._HighPriority;
    }

    string	Atom_O::getConfigurationPriorityLow()
    {
	ASSERTP(this->isConfigurable(), "You cannot assign priorities to an unconfigurable atom");
	return this->_ConfigurationInfo._LowPriority;
    }

    string	Atom_O::getConfigurationPriorityLowest()
    {
	ASSERTP(this->isConfigurable(), "You cannot assign priorities to an unconfigurable atom");
	return this->_ConfigurationInfo._LowestPriority;
    }
#endif

    Matter_wp	Atom_O::getMatterContainedBy()
    {_OF();
	Matter_wp	containedBy;
	Residue_wp	wpresidue;
	Residue_sp	residue;
	ASSERTNOTNULL(this->containedBy());
	containedBy = this->containedBy();
	if ( containedBy.nilp() )
	{
	    SIMPLE_ERROR(BF("This atom isnt contained by anything"));
	}
	return containedBy;

//    wpresidue = downcast <Residue_O> (containedBy);
//    residue = wpresidue;
//    return residue;
    }


    Residue_sp	Atom_O::getResidueContainedBy() 
    {
	return this->containedByLock().as<Residue_O>();
    }

    Residue_sp	Atom_O::getResidueContainedBy_const() const
    {
	return this->containedByLock_const().as<Residue_O>();
    }







    void Atom_O::_describeAtomRecursively(string prefix, Atom_sp parent, BondOrder order, int maxDepth, stringstream& ss) const
{_OF();
    if ( maxDepth == 0 ) return;
    ss << prefix;
    if ( order != noBond )
    {
	ss << bondOrderToString(order);
    } else
    {
	ss << " ";
    }
    ss << this->description() << std::endl;
    for ( VectorBond::const_iterator bi=this->bonds.begin(); bi!=this->bonds.end(); bi++ )
    {
	Atom_sp other = (*bi)->getOtherAtom(this->const_sharedThis<Atom_O>());
	if ( other!=parent)
	{
	    other->_describeAtomRecursively(prefix+"  ",this->const_sharedThis<Atom_O>(),(*bi)->getOrder(),maxDepth-1,ss);
	}
    }
}


string Atom_O::localEnvironment(int maxDepth) const
{_OF();
    stringstream ss;
    this->_describeAtomRecursively("",_Nil<Atom_O>(),noBond,maxDepth,ss);
    return ss.str();
}









    bool	Atom_O::invalid()
    {
	IMPLEMENT_ME();
    }



AtomIdToAtomMap_sp Atom_O::buildAtomIdMap() const
{_OF();
    SIMPLE_ERROR(BF("Atom should never buildAtomIdMap"));
};


Atom_sp Atom_O::atomWithAtomId(AtomId_sp atomId) const
{_OF();
    SIMPLE_ERROR(BF("Atom should never return atomWithAtomId"));
};




    void	Atom_O::bumpPosition(double dist)
    {_OF();
	double		x, y, z;
	x = core::randomNumber01()*dist*2.0-dist+this->position.getX();
	y = core::randomNumber01()*dist*2.0-dist+this->position.getY();
	z = core::randomNumber01()*dist*2.0-dist+this->position.getZ();
	this->position.set(x,y,z);
    }


    void	Atom_O::failIfInvalid()
    {_OF();
	if ( this->invalid() ) {
	    SIMPLE_ERROR(BF("INVALID %s")% this->description());
	}
    }




    void Atom_O::lisp_initGlobals(core::Lisp_sp lisp)
    {_G();
	core::enum_<AtomFlagEnum> en(_sym__PLUS_atomFlagSymbolConverter_PLUS_,"AtomFlags");
	en.value(_sym_atomNeedsMinimizer, atomNeedsMinimizer);
	SYMBOL_EXPORT_SC_(ChemPkg,atomNeedsMinimizer);
	en.value(_sym_selected, selected);
	SYMBOL_EXPORT_SC_(ChemPkg,selected);
	en.value(_sym_onGrid, onGrid);
	SYMBOL_EXPORT_SC_(ChemPkg,onGrid);
	en.value(_sym_atomFixed, atomFixed);
	SYMBOL_EXPORT_SC_(ChemPkg,atomFixed);
	en.value(_sym_in3MemberRing, in3MemberRing);
	SYMBOL_EXPORT_SC_(ChemPkg,in3MemberRing);
	en.value(_sym_in4MemberRing, in4MemberRing);
	SYMBOL_EXPORT_SC_(ChemPkg,in4MemberRing);
	en.value(_sym_in5MemberRing, in5MemberRing);
	SYMBOL_EXPORT_SC_(ChemPkg,in5MemberRing);
	en.value(_sym_in6MemberRing, in6MemberRing);
	SYMBOL_EXPORT_SC_(ChemPkg,in6MemberRing);
	en.value(_sym_in7MemberRing, in7MemberRing);
	SYMBOL_EXPORT_SC_(ChemPkg,in7MemberRing);
	en.value(_sym_in8MemberRing, in8MemberRing);
	SYMBOL_EXPORT_SC_(ChemPkg,in8MemberRing);
	en.value(_sym_inRing, inRing);
	SYMBOL_EXPORT_SC_(ChemPkg,inRing);
	    ;
    }

    ConstitutionAtom_sp Atom_O::asConstitutionAtom(ConstitutionAtomIndex0N index)
    {_OF();
	ConstitutionAtom_sp ca = ConstitutionAtom_O::create(this->getName(),this->_Element,index);
	return ca;
    }

    void	Atom_O::defineConstitutionAtomBonding(ConstitutionAtom_sp consAtom, MapAtomsToConstitutionAtomIndex0N atomMap)
    {_OF();
	Atom_O::bondIterator bi;
	Atom_sp me = this->sharedThis<Atom_O>();
	for ( bi=this->bonds.begin(); bi!=this->bonds.end(); bi++ )
	{
	    ConstitutionBond_sp cb = (*bi)->asConstitutionBond(me,atomMap);
	    consAtom->addConstitutionBond(cb);
	}
    }


EXPOSE_CLASS_AND_GLOBALS(chem, Atom_O );
}; // namespace chem




