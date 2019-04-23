/*
	Program: FlowForm OSL (FOSL)
	Module: help.cpp
	Author: Gary Conway <gary.conway@fireking.com>
	Created: 7-11-2013
	Updated: 
	Language: C,C++ (this module is C++ compilable and linkable with C libs)
	Version: 3.01
	Notice: Copyright 2018 FireKing Security


	This module handles retrieving the help file contents. The help files are
	XML files and contain <sections> that define the help for a particular window or task


	XML HELP file format, created by Gary Conway, June 2012
	This file is the help file for FlowForm OSL.
	Each section should be formatted as shown below


	<xx_window>
		<Page>
		<Title></Title>
		<Body></Body>
		<Footer></Footer>
		</Page>
	</xx_window>

	Note that each section has opening and closing tags.
	Within the main tags, are permitted as many <Page></Page> sections as required
	The <Title></Title> section holds the main title for that help page, displayed centered above the text
	The <Header></Header> section holds the main title for that help page, displayed left justified above the text
	The <Body></Body> section holds the main body of the help text
	The <Footer></Footer> section holds any footer information for that page

	Inside any of the tags where text will be displayed, you are allowed to use
	certain formatting tags.
	[b][/b] causes any text between the tags to be displayed in BOLD

	&#x0D;	 these can be inserted to create a new line when displaying the text
	&amp;	displays an ampersand
	&lt;	displays a less than sign
	&gt;	displays a greater than sign
	&quot;	displays a double quotation mark
	&apos;	displays a single quotation mark




*/


using namespace std;


#include <string>
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include "hdr/config.h"

// declarations
TiXmlDocument *help_doc;	// (char *filename);
int HELP_pagecount;

// FUNCTIONS
void SetHelpFileName(string filename);
void DumpHelp(void);
bool FindHelpSection(string section);
void CloseHelp(void);
void GetHelpContents(void);


void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 );
const char * getIndentAlt( unsigned int numIndents );
int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent);
const char * getIndent( unsigned int numIndents );


/*
	The following structure is included in config.h
	After we find the appropriate section in the XML help file
	we fill in this structure for subsequent use by FlowForm OSL

typedef struct
{
	string section;
	string page[10];	// room for 10 pages
	string title;
	string heading;
	string body;
	string footer;
} help_data;

*/


help_data * HELP;
string help_filename;

contents_data * CONTENTS;
int HELP_content_sections;	// number of sections in the file (contents count)



/*
	retrieve the version number from TinyXML class
*/

void GetXMLVersion(int * maj, int *min, int *patch)
{
	TiXmlDocument *help_doc  = new TiXmlDocument(help_filename.c_str() );
	help_doc->GetVersion( maj, min, patch );
	delete help_doc;
}


/*
	set the help file name, which is language dependent
	this must be called before any other calls into this module
*/

void SetHelpFileName(string filename)
{
	help_filename = filename;
}


/*
	Find the "section" in the help xml file and populate the HELP[] struc
	RETURNS:	TRUE on success
				FALSE on error
*/



bool FindHelpSection(string section)
{

	TiXmlDocument *help_doc  = new TiXmlDocument(help_filename.c_str() );
	TiXmlNode* node = 0;
	TiXmlElement* element;
	TiXmlElement*child;
	TiXmlText* pText;
	TiXmlElement* pageElement = 0;
//	TiXmlElement* textElement=0;

	int page_index=0;
	HELP_pagecount=0;

	bool loadOkay = help_doc->LoadFile();

	if ( !loadOkay )
	{
			printf( "Error loading help file '%s'. Error='%s'. \r\n", help_filename.c_str(), help_doc->ErrorDesc() );
			delete help_doc;
			return FALSE;
	}



	node = help_doc->FirstChild( section.c_str() );
//	printf("Section: %s\r\n", node->Value() );

	// count the pages
	for( element = node->FirstChildElement();
		 element;
		 element = element->NextSiblingElement() )
	{
		HELP_pagecount++;
	}


	if (HELP != NULL)
		delete[] HELP;

	HELP = new help_data[HELP_pagecount];

//printf("HELP pages: %d\r\n",HELP_pagecount);

//	printf("finding %s\r\n", section.c_str() );

	node = help_doc->FirstChild( section.c_str() );
//	printf("Section: %s\r\n", node->Value() );

	HELP[page_index].section = node->Value();
	// count the pages
	for( element = node->FirstChildElement();
		 element;
		 element = element->NextSiblingElement() )
	{
		// we are on a "Page" element
//		printf("Page: %s\r\n", element->Value());

		for( pageElement = element->FirstChildElement();
			 pageElement;
			 pageElement = pageElement->NextSiblingElement() )
		{
			// elements inside the <Page> tag

			// print the element tag <Title> <Body> etc.
//			printf("Element: [%s]\r\n", pageElement->Value() );

			TiXmlNode* pChild;
			for ( pChild = pageElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
			{
				// print the element text
				pText = pChild->ToText();
//				printf("%s\r\n", pText->Value() );

				if (strcmp( pageElement->Value(), "Title") == 0)
					HELP[page_index].title = string(pText->Value() );

				if (strcmp( pageElement->Value(), "Header") == 0)
					HELP[page_index].header = string(pText->Value() );

				if (strcmp( pageElement->Value(), "Body") == 0)
					HELP[page_index].body = string(pText->Value() );

				if (strcmp( pageElement->Value(), "Footer") == 0)
					HELP[page_index].footer = string(pText->Value() );

			}
			
		}		
		page_index++;
//		printf("page_index: %d\r\n",page_index);


	}


	delete help_doc;
	return TRUE;

}





/*
	get all the main sections and create a TOC
	populate the CONTENTS[] structure


typedef struct
{
	string section;
	string name;
	string page_title[20];
	int page_count;
} contents_data;


	page_title will contain the text within the <Title></Title> tags, up to 20 pages allowed
	page_count = number of pages for the <section>

*/


void GetHelpContents(void)
{
	TiXmlDocument *help_doc  = new TiXmlDocument(help_filename.c_str() );
	TiXmlElement* section;
	TiXmlElement* page;
	TiXmlElement* title;
	TiXmlText* pText;
	int pages;
	int content_index;

	bool loadOkay = help_doc->LoadFile();

	if ( !loadOkay )
	{
			printf( "Error loading help file '%s'. Error='%s'. \r\n", help_filename.c_str(), help_doc->ErrorDesc() );
			delete[] help_doc;
			return;
	}


	HELP_content_sections=0;
	// count the number of sections
	for( section = help_doc->FirstChildElement();
		 section;
		 section = section->NextSiblingElement() )
	{
		if ( strcmp(section->Value(), "comment") != 0) 
		{
			HELP_content_sections++;
		}
	}



	if (CONTENTS != NULL)
		delete[] CONTENTS;

	CONTENTS = new contents_data[HELP_content_sections];



	content_index=0;
	// get the sections and find the "name" attribute
	for( section = help_doc->FirstChildElement();
		 section;
		 section = section->NextSiblingElement() )
	{
		if ( strcmp(section->Value(), "comment") == 0) continue;
		// get the attribute for this tag, it will be name="screen_name"
		TiXmlAttribute* pAttrib=section->FirstAttribute();
		// c srings: pAttrib->Name(), pAttrib->Value()
		CONTENTS[content_index].section = string(section->Value() );
		CONTENTS[content_index].name = string(pAttrib->Value() ); 
		//printf("%d. %s\r\n",content_index, CONTENTS[content_index].c_str());

		// now get the <Page> tags for this section
		pages=0;
		for( page = section->FirstChildElement();
		 page;
		 page = page->NextSiblingElement() )
		{
//			printf("Section: %s Page: <%s>\r\n",section->Value(), page->Value() );

			// now get the <Title> tags for this <Page>
			for( title = page->FirstChildElement();
			 title;
			 title = title->NextSiblingElement() )
			{
				if (  strcmp(title->Value(), "Title") != 0) continue;
//				printf("Title: <%s>\r\n", title->Value() );

				TiXmlNode* pChild;
				for ( pChild = title->FirstChild(); 
				  pChild; 
				  pChild = pChild->NextSibling()) 
				{
					// print the element text
					pText = pChild->ToText();
//					printf("Title text: [%s]\r\n", pText->Value() );
					CONTENTS[content_index].page_title[pages] = string(pText->Value() );
					pages++;
//					printf("Pages: %d\r\n",pages);
				}//endfor (pChild)
			}// endfor (title)

		} // endfor (page)

		CONTENTS[content_index].page_count = pages;
		content_index++;
//		printf("content_index: %d\r\n",content_index);

	}// endfor (section)

}




// do our cleanup

void CloseHelp(void)
{
	if (HELP != NULL)
		delete[] HELP;

	// just in case
	if (help_doc != NULL)
		delete[] help_doc;

	if (CONTENTS != NULL)
		delete[] CONTENTS;
}



void DumpHelp(void)
{
	char filename[]="../locale/english_help.xml";
	TiXmlDocument doc(filename);
	doc.LoadFile();
	dump_to_stdout(&doc,0);
}







void dump_to_stdout( TiXmlNode* pParent, unsigned int indent  )
{
	if ( !pParent ) return;

	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf( "%s", getIndent(indent));
	int num;

	switch ( t )
	{
	case TiXmlNode::TINYXML_DOCUMENT:
		printf( "Document" );
		break;

	case TiXmlNode::TINYXML_ELEMENT:
		printf( "Element [%s]", pParent->Value() );
		num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
		switch(num)
		{
			case 0:  printf( " (No attributes)"); break;
			case 1:  printf( "%s1 attribute", getIndentAlt(indent)); break;
			default: printf( "%s%d attributes", getIndentAlt(indent), num); break;
		}
		break;

	case TiXmlNode::TINYXML_COMMENT:
		printf( "Comment: [%s]", pParent->Value());
		break;

	case TiXmlNode::TINYXML_UNKNOWN:
		printf( "Unknown" );
		break;

	case TiXmlNode::TINYXML_TEXT:
		pText = pParent->ToText();
		printf( "Text: [%s]", pText->Value() );
		break;

	case TiXmlNode::TINYXML_DECLARATION:
		printf( "Declaration" );
		break;
	default:
		break;
	}
	printf( "\n" );
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		dump_to_stdout( pChild, indent+1 );
	}
}





int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
{
	if ( !pElement ) return 0;

	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent=getIndent(indent);
	printf("\n");
	while (pAttrib)
	{
		printf( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());

		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    printf( " int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;	
}




const unsigned int NUM_INDENTS_PER_SPACE=2;

const char * getIndent( unsigned int numIndents )
{
	static const char * pINDENT="                                      + ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}



// same as getIndent but no "+" at the end
const char * getIndentAlt( unsigned int numIndents )
{
	static const char * pINDENT="                                        ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}














