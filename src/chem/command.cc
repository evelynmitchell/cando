       

#define	DEBUG_LEVEL_NONE

#include <cando/chem/command.h>
//#include "core/archiveNode.h"
//#include "core/archive.h"
#include <clasp/core/wrappers.h>




namespace chem {


    REGISTER_CLASS(chem,Command_O);



#ifdef XML_ARCHIVE
    void	Command_O::archiveBase(core::ArchiveP node)
{
    // do nothing
}
#endif






};


