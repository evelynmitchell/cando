/*
    File: statusTracker.h
*/
/*
Open Source License
Copyright (c) 2016, Christian E. Schafmeister
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 
This is an open source license for the CANDO software from Temple University, but it is not the only one. Contact Temple University at mailto:techtransfer@temple.edu if you would like a different license.
*/
/* -^- */
       
       

#ifndef	StatusTracker_H //[
#define StatusTracker_H



#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <clasp/core/common.h>


#include <cando/chem/chemPackage.h>


namespace chem {


SMART(StatusTracker);

SMART(StatusTracker);
SMART(StatusTracker );
class StatusTracker_O : public core::CxxObject_O
{
    LISP_CLASS(chem,ChemPkg,StatusTracker_O,"StatusTracker",core::CxxObject_O);

public:
	void initialize();
public:
private:
	stringstream	_MessageStream;
	bool		_HasError;
	stringstream	_ErrorStream;
public:

	void	addMessage(const string& msg);
	void	addError(const string& err);
	bool	hasError() { return this->_HasError; };
	void	reset();

	string	getStatus();

	StatusTracker_sp copy();


	StatusTracker_O( const StatusTracker_O& ss ); //!< Copy constructor


	DEFAULT_CTOR_DTOR(StatusTracker_O);
};



};
TRANSLATE(chem::StatusTracker_O);
#endif //]
