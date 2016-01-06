#define	DEBUG_LEVEL_FULL

//
// (C) 2004 Christian E. Schafmeister
//



#include <clasp/core/common.h>
#include <clasp/core/record.h>
#include <cando/chem/molecule.h>
#include <cando/chem/loop.h>
#include <clasp/core/numerics.h>
//#include "core/serialize.h"
#include <cando/chem/restraint.h>
#include <cando/chem/bond.h>
#include <clasp/core/translators.h>
#include <cando/chem/atomIdMap.h>
#include <cando/chem/symbolTable.h>
#include <clasp/core/wrappers.h>



namespace chem {

void	Molecule_O::initialize()
{
  this->Base::initialize();
}

bool Molecule_O::applyPropertyToSlot(core::Symbol_sp prop, core::T_sp value ) {
  if ( this->Base::applyPropertyToSlot(prop,value) ) return true;
  return false;
}

string Molecule_O::__repr__() const
{
  return this->Base::__repr__();
}

void Molecule_O::fields(core::Record_sp node)
{
  Bond_sp	bond;
  Atom_sp		a;
  Atom_sp	a1, a2;
  BondOrder	o;
  Loop lb;
  this->Base::fields(node);
  switch (node->stage()) {
  case core::Record_O::saving: {
            // Accumulate intraresidue bonds into a vector
    _BLOCK_TRACE("Xmling inter-residue bonds");
    BondList_sp bondList = BondList_O::create();
    { _BLOCK_TRACE("Accumulating inter residue bond copies");
      LOG(BF("Serializing InterResidueBonds") );
      lb.loopTopMoleculeGoal( this->sharedThis<Molecule_O>(), BONDS );
      while ( lb.advanceLoopAndProcess() ) {
        a1 = lb.getBondA1();
        a2 = lb.getBondA2();
		    // LOG(BF("bond with atom1=%s") % a1->description().c_str()  );
		    // LOG(BF("bond with atom2=%s") % a2->description().c_str()  );
        o  = lb.getBondOrder();
        bond = Bond_O::create(a1,a2,o);
        if ( bond->isInterResidueBond() )
        {
          LOG(BF("Adding bond between atom1(%s) and atom2(%s)")
              % a1->description()
              % a2->description() );
          bondList->addBond(bond);
        }
      }
    }
    node->field( INTERN_(kw,bl),bondList);
  }
      break;
  case core::Record_O::initializing:
  case core::Record_O::loading: {
    	    // create the intraResidue bonds
    BondList_sp bondList;
    node->field( INTERN_(kw,bl),bondList);
    ASSERTNOTNULL(bondList);
    bondList->imposeYourself();
  }
      break;
  case core::Record_O::patching: {
  };
      break;
  }
}

Molecule_O::Molecule_O(const Molecule_O& mol)
  :Matter_O(mol)
{
}

//
// Destructor
//

//
//	addResidue
//
//	Add the residue to the end of the molecule
//
void Molecule_O::addResidue( Matter_sp r )
{_G();
  this->addMatter( r );
  LOG(BF("Added %s to %s") % r->description().c_str() % this->description().c_str()  );
}

//
//	addResidueRetainId
//
//	Add the residue to the end of the molecule
//
void Molecule_O::addResidueRetainId( Matter_sp r )
{
  LOG(BF("Adding %s to %s") % r->description().c_str() % this->description().c_str()  );
  this->addMatterRetainId( r );
}





//
//	removeResidue
//
//	Remove the residue
//
CL_LISPIFY_NAME("removeResidue");
CL_DEFMETHOD void Molecule_O::removeResidue( Matter_sp a )
{
  gctools::Vec0<Matter_sp>::iterator	it;
  for ( it=this->getContents().begin(); it!= this->getContents().end(); it++ ) {
    if ( downcast<Residue_O>(*it) == a ) {
      this->eraseContent(it);
      return;
    }
  }
  SIMPLE_ERROR(BF("removeResidue: Molecule does not contain residue: %s") % _rep_(a->getName()) );
}

    bool	Molecule_O::equal(core::T_sp obj) const
    {_G();
      if ( this->eq(obj) ) return true;
      if ( Molecule_sp other = obj.asOrNull<Molecule_O>() ) {
	if ( other->getName() != this->getName() ) return false;
	if ( other->_contents.size() != this->_contents.size() ) return false;
	Matter_O::const_contentIterator tit = this->_contents.begin();
	Matter_O::const_contentIterator oit = this->_contents.begin();
	for ( ; tit!=this->_contents.end(); tit++, oit++ )
	{
          if ( ! (*tit)->equal(*oit) ) return false;
	}
	return true;
      }
      return false;
    }
    void Molecule_O::transferCoordinates(Matter_sp obj)
    {_G();
	if ( !obj.isA<Molecule_O>() ) 
	{
	    SIMPLE_ERROR(BF("You can only transfer coordinates to a Molecule from another Molecule"));
	}
	Molecule_sp other = obj.as<Molecule_O>();
	if ( other->_contents.size() != this->_contents.size() )
	{
	    SIMPLE_ERROR(BF("You can only transfer coordinates if the two Molecules have the same number of contents"));
	}
	Matter_O::contentIterator tit,oit;
	for ( tit=this->_contents.begin(), oit=other->_contents.begin();
	      tit!=this->_contents.end(); tit++, oit++ )
	{
	    (*tit)->transferCoordinates(*oit);
	}
    }
CL_LISPIFY_NAME("moveAllAtomsIntoFirstResidue");
CL_DEFMETHOD     void	Molecule_O::moveAllAtomsIntoFirstResidue()
    {
	contentIterator	a;
	contentIterator	r;
	contentIterator	rHead;
	contentIterator	rRest;
	rHead = this->getContents().begin();
	rRest = rHead+1;
	for ( r=rRest;r!=this->getContents().end(); ) {
	    for ( a = (*r)->getContents().begin(); a!= (*r)->getContents().end();) {
		(*a)->reparent(*rHead);
		a = (*r)->eraseContent(a);
	    }
//	delete (*r);
	    r = this->eraseContent(r);
	}
    }



    void	Molecule_O::duplicate(const Molecule_O* a )
    {
	*this = *a;
    }


Matter_sp	Molecule_O::copyDontRedirectAtoms()
{
  Residue_sp			res;
  GC_COPY(Molecule_O, newMol , *this); // = RP_Copy<Molecule_O>(this);
  for ( const_contentIterator a=this->begin_contents(); a!=this->end_contents(); a++ ) {
    res = (*a).as<Residue_O>();
    newMol->addMatter(res->copyDontRedirectAtoms());
  }
  newMol->copyRestraintsDontRedirectAtoms(this->asSmartPtr());
  return newMol;
}

void	Molecule_O::redirectAtoms()
    {_OF();
	LOG(BF("Molecule_O::redirectAtoms START") );
	for ( contentIterator a=this->getContents().begin(); a!=this->getContents().end(); a++ )
	{
	    Residue_sp res = downcast<Residue_O>(*a);
	    res->redirectAtoms();
	}
	this->redirectRestraintAtoms();
	LOG(BF("Molecule_O::redirectAtoms DONE") );
    }



CL_LISPIFY_NAME("copy");
CL_DEFMETHOD     Matter_sp Molecule_O::copy()
    {
	Molecule_sp	newMol;
	newMol = this->copyDontRedirectAtoms().as<Molecule_O>();
        Loop lbonds(this->asSmartPtr(),BONDS);
        while (lbonds.advanceLoopAndProcess()) {
          Bond_sp b = lbonds.getBond();
          Bond_sp bcopy = b->copyDontRedirectAtoms();
          bcopy->addYourselfToCopiedAtoms();
        }
	newMol->redirectAtoms();
	return newMol;
    }
#if 0 //[

    void	Molecule_O::dump()
    {
	contentIterator	rit;
	Residue_sp			res;
	printf( "Molecule: %s at: 0x%lx contains %d residues\n", this->getName().c_str(), this, this->getContents().size() );
	for ( rit=this->getContents().begin();
	      rit!=this->getContents().end(); rit++ ) {
	    res = downcast<Residue_O>(*rit);
	    res->dump();
	}
    }
#endif //]

VectorResidue	Molecule_O::getResiduesWithName(MatterName name ) {
	VectorResidue	result;
	Loop		lr;
	lr.loopTopMoleculeGoal( this->sharedThis<Molecule_O>(), RESIDUES );
	while ( lr.advanceLoopAndProcess() ) {
	    if ( lr.getResidue()->getName() == name ) {
		result.push_back(lr.getResidue());
	    }
	}
	return result;

    }


CL_LISPIFY_NAME("numberOfResiduesWithName");
CL_DEFMETHOD     int	Molecule_O::numberOfResiduesWithName( MatterName name )
    {
	VectorResidue	residues;
	residues = this->getResiduesWithName(name);
	return residues.size();
    }

CL_LISPIFY_NAME("getFirstResidueWithName");
CL_DEFMETHOD     Residue_sp	Molecule_O::getFirstResidueWithName(MatterName name)
    {_G();
	VectorResidue residues = this->getResiduesWithName(name);
	if ( residues.size() > 0 ) {
	    return *(residues.begin());
	}
	SIMPLE_ERROR(BF("getFirstResidueWithName: Molecule does not contain residues with name: %s")% _rep_(name) );
    }



#ifdef RENDER
    geom::Render_sp	Molecule_O::rendered(core::List_sp opts)
    {_G();
	GrPickableMatter_sp	rend;
	rend = GrPickableMatter_O::create();
	rend->setFromMatter(this->sharedThis<Molecule_O>());
	return rend;
    }
#endif

    uint	Molecule_O::numberOfAtoms()
    {
	Loop				lb;
	Vector3				v,vd;
	int				numberOfAtoms;
	numberOfAtoms = 0;
	lb.loopTopMoleculeGoal( this->sharedThis<Molecule_O>(), RESIDUES);
	while ( lb.advanceLoopAndProcess() ) 
	{
	    numberOfAtoms += lb.getResidue()->numberOfAtoms();
	}
	return numberOfAtoms;
    }


    AtomIdToAtomMap_sp Molecule_O::buildAtomIdMap() const
    {_OF();
	AtomIdToAtomMap_sp atomIdMap = AtomIdToAtomMap_O::create();
	atomIdMap->resize(1);
	int mid = 0;
	int numResidues = this->_contents[mid]->_contents.size();
	atomIdMap->resize(mid,numResidues);
	for ( int rid =0; rid<numResidues; rid++ )
	{
	    int numAtoms = this->_contents[mid]->_contents[rid]->_contents.size();
	    atomIdMap->resize(mid,rid,numAtoms);
	    for ( int aid=0; aid<numAtoms; aid++ )
	    {
		AtomId atomId(mid,rid,aid);
		atomIdMap->set(atomId,this->_contents[mid]->_contents[rid]->_contents[aid].as<Atom_O>());
	    }
	}
	return atomIdMap;
    }

    Atom_sp Molecule_O::atomWithAtomId(AtomId_sp atomId) const
    {_OF();
	int resId = atomId->residueId();
	if ( resId >=0 && resId <=(int)this->_contents.size() )
	{
	    Residue_sp residue = this->_contents[resId].as<Residue_O>();
	    return residue->atomWithAtomId(atomId);
	}
	SIMPLE_ERROR(BF("Illegal residueId[%d] must be less than %d") % resId % this->_contents.size() );
    }




#define ARGS_Molecule_O_make "(&key (name \"\"))"
#define DECL_Molecule_O_make ""
#define DOCS_Molecule_O_make "make Molecule args: &key name"
CL_LAMBDA("(&key (name \"\"))");
CL_LISPIFY_NAME(make-molecule);
CL_DEFUN Molecule_sp Molecule_O::make(MatterName name)
{_G();
    GC_ALLOCATE(Molecule_O,me);
    me->setName(name);
    return me;
};



    void Molecule_O::exposeCando(core::Lisp_sp lisp)
    {
	core::class_<Molecule_O>()
//	    .def_raw("core:__init__",&Molecule_O::__init__,"(self &key name)")
	    .def("copy",&Molecule_O::copy,"","","",false)
	    .def("firstResidue",&Molecule_O::firstResidue)
	    .def("residueWithId",&Molecule_O::residueWithId)
	    .def("hasResidueWithId",&Molecule_O::hasResidueWithId)
	    .def("getResidue",&Molecule_O::getResidue)
          .def("removeResidue",&Molecule_O::removeResidue)
	    .def("residueCount",&Molecule_O::residueCount)
	    .def("testMoleculeConsistancy",&Molecule_O::testMoleculeConsistancy)
	    .def("moveAllAtomsIntoFirstResidue",&Molecule_O::moveAllAtomsIntoFirstResidue)
	    .def("numberOfResiduesWithName",&Molecule_O::numberOfResiduesWithName)
	    .def("getFirstResidueWithName",&Molecule_O::getFirstResidueWithName)
	    ;
//	Defun_maker(ChemPkg,Molecule);
    }

    void Molecule_O::exposePython(core::Lisp_sp lisp)
    {_G();
#ifdef USEBOOSTPYTHON
	PYTHON_CLASS(ChemPkg,Molecule,"","",_lisp)
	    .def("copy",&Molecule_O::copy)
	    .def("firstResidue",&Molecule_O::firstResidue)
	    .def("residueWithId",&Molecule_O::residueWithId)
	    .def("hasResidueWithId",&Molecule_O::hasResidueWithId)
	    .def("getResidue",&Molecule_O::getResidue)
	    .def("residueCount",&Molecule_O::residueCount)
	    .def("testMoleculeConsistancy",&Molecule_O::testMoleculeConsistancy)
	    .def("moveAllAtomsIntoFirstResidue",&Molecule_O::moveAllAtomsIntoFirstResidue)
	    .def("numberOfResiduesWithName",&Molecule_O::numberOfResiduesWithName)
	    .def("getFirstResidueWithName",&Molecule_O::getFirstResidueWithName)
	    ;
#endif
    }


    EXPOSE_CLASS(chem,Molecule_O);
}; // namespace chem

