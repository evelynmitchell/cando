#define	DEBUG_LEVEL_FULL

//
// (C) 2004 Christian E. Schafmeister
//

#include <string.h>
#include <clasp/core/foundation.h>
#include <clasp/core/lisp.h>
#include <clasp/core/lispStream.h>
#include <cando/adapt/mySaxInterface.h>


namespace adapt {
////////////////////////////////////////////////////////////////
//
//
//	getAttribute
//
//

    string	MySaxHandlerDefault::getAttribute(char* str) {
	int		iCount;
	const XML_Char	**pos;
	string	res;
	iCount = 0;
	pos = this->attributes;
	while (*pos) {
	    if ( strcmp(*pos,str)==0 ) {
		res = *(pos+1);
		return res;
	    }
	    pos++;
	    pos++;
	}
	return "";
    }





//////////////////////////////////////////////////////////////
//
//
//	getAttributeCount
//
//

    int	MySaxHandlerDefault::getAttributeCount() {
	int		iCount;
	const XML_Char	**pos;
	iCount = 0;
	pos = this->attributes;
	while ((*pos)) {
	    pos++;
	    iCount++;
	}
	return iCount/2;
    }

//////////////////////////////////////////////////////////////
//
//
//	getAttributeAtIndex
//
//

    void	MySaxHandlerDefault::getAttributeAtIndex(int ind, string& attrName, string& attrVal )
    {
	const XML_Char	**pos;
	string	res;
	pos = this->attributes;
	while (*pos) {
	    if ( ind == 0 ) {
		attrName = *pos;
		attrVal = *(pos+1);
		return;
	    }
	    ind--;
	    pos++;
	    pos++;
	}
	attrName = "";
	attrVal = "";
	return;
    }













//
//
//    XML_SetStartElementHandler( this->parser, myExpatStartElement );
//
//
    static void	myExpatStartElement( 	void* 		userData,
					const	XML_Char	*name,
					const	XML_Char	**atts)
    {   
	MySaxParser*	parser = (MySaxParser*)userData;
	_G();
	LOG(BF("myExpatStartElement local name = %s") % (name ) );
	parser->handler->setLocalName(name);
	parser->handler->setLineNumber(XML_GetCurrentLineNumber(parser->getXMLParser()));
	parser->handler->setAttributes(atts);
	parser->handler->startElement();
	LOG(BF("myExpatStartElement done") );
    }


    static void	myExpatEndElement( 	void* 		userData,
					const	XML_Char	*name )
    {
	MySaxParser*	parser = (MySaxParser*)userData;
	_G();
	LOG(BF("myExpatEndElement local name = %s") % (name ) );
	parser->handler->setLocalName(name);
	parser->handler->setLineNumber(XML_GetCurrentLineNumber(parser->getXMLParser()));
	parser->handler->endElement();
    }


    static void	myExpatCharacterData( 	void* 		userData,
					const	XML_Char	*data,
					int			len)
    {
	MySaxParser*	parser = (MySaxParser*)userData;
	_G();
	char		*myBuffer;

	myBuffer = (char*)malloc(len+1);
	strncpy( myBuffer, data, len );
	myBuffer[len] = '\0';
	LOG(BF("myExpatCharacterDataHandler characters: |%s|") % (myBuffer ) );
	parser->handler->setData(myBuffer);
	parser->handler->setDataLength(len);
	parser->handler->characters();
	free(myBuffer);
    }




// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
//
//	MySaxParser
//
//
    MySaxParser::MySaxParser()
    {_G();
    }


    bool	MySaxParser::initialize()
    {_OF();
	LOG(BF("MySaxParser::initialize starting") );
	LOG(BF("MySaxParser::initialize Creating expat parser") );
	this->parser= XML_ParserCreate( "UTF-8" );
	XML_SetElementHandler( this->parser, myExpatStartElement, myExpatEndElement );
	XML_SetCharacterDataHandler( this->parser, myExpatCharacterData );
	XML_SetUserData( this->parser, (void*)this );
	return true;

    }



////MySaxParser//////////////////////////////////////////////////////////
//
// shutDown
//
//
    void	MySaxParser::shutDown()
    { _G();
	XML_ParserFree(this->parser);
    }

////MySaxParser//////////////////////////////////////////////////////////
//
// setContentHandler
//
//
    void	MySaxParser::setContentHandler(MySaxHandlerDefault *aHandler )
    {_G();
	LOG(BF("MySaxParser::setContentHandler") );
	this->handler = aHandler;

    }



    int	xml_fread( char* buffer, int numBytes, core::T_sp fIn )
    {
	char* cp;
	int	c, c2;
	cp = buffer;
	int sz;
	sz = 0;
	int limBytes = numBytes;
	while ( (core::clasp_peek_char(fIn)!=EOF) && sz < limBytes )
	{
	    c = core::clasp_read_char(fIn);
	    if ( c == '&' )
	    {
                c = core::clasp_read_char(fIn);
                if ( c == EOF ) break;
		if ( c == '&' )
		{
		    *cp = '&';
		} else {
		    c2 = core::clasp_read_char(fIn);
                    if ( c2 == EOF ) break;
		    if ( c == 'g' && c2 == 't' )
		    {
			*cp = '>';
		    } else if ( c == 'l' && c2 == 't' )
		    {
			*cp = '<';
		    }
		}
	    } else
	    {
		*cp = c;
	    }
	    cp++;
	    sz++;
	}
	return sz;
    }




////MySaxParser//////////////////////////////////////////////////////////
//
//	parse
//
//	Parse the file, return true if everything was ok
//
    bool	MySaxParser::parse(core::T_sp fIn, const string& fileName)
    {_G();
	unsigned char	buffer[BUFSIZ];
	uint		sz;
	XML_Status		xmlStatus;
	XML_Error		err;
	int			done;
	long		filePos;

	LOG(BF("MySaxParser::expat:: opening file: %s") % (fileName.c_str() ) );

//    this->_Progress.setMax(fileSize);
//    this->_Progress.setSteps(fileSize/10000);
	filePos = 0;
	while ( core::clasp_peek_char(fIn) != EOF )
	{
	    LOG(BF("MySaxParser::expat:: reading/parsing block of data asking for buffer with up to %d bytes") % sizeof(buffer) );
            sz = core::clasp_read_byte8(fIn,buffer,sizeof(buffer));
	    filePos = filePos + sz;
	    done = sz < sizeof(buffer);
	    LOG(BF("Read line from buffer - hit eof=%d read[%d bytes]") % done % sz );
	    xmlStatus = XML_Parse( this->parser, (char*)buffer, sz, done );
	    if ( xmlStatus == XML_STATUS_ERROR )
	    {
		printf( "MySaxParser::expat:: parse failed for file(%s)!!!!\n", fileName.c_str() );
		err = XML_GetErrorCode(this->parser);
		printf( "MySaxParser::expat:: error = %s\n", XML_ErrorString(err) );
		printf( "MySaxParser::expat:: line = %d\n", (int)(XML_GetCurrentLineNumber(this->parser)) );
                core::cl_close(fIn);
		return false;
	    }
	}
//    this->_Progress.finish();
	LOG(BF("MySaxParser::expat:: parse succeeded!!!!") );
	return true;
    }


////MySaxParser//////////////////////////////////////////////////////////
//
//	parseString
//
//	Parse the string, return true if everything was ok
//
    bool	MySaxParser::parseString(const char* xmlContent)
    { _G();
	LOG(BF("MySaxParser::parseString") );
	XML_Status		xmlStatus;

	xmlStatus = XML_Parse( this->parser, xmlContent,
			       strlen(xmlContent), 1 );
	if ( xmlStatus != XML_STATUS_OK ) {
	    return false;
	}
	return true;

    }

};