/*
 # Copyright (C) 2000-2002 The ViewCVS Group. All Rights Reserved.
 # This file has been rewritten in C++ from the rcsparse.py file by
 # Lucas Bruand <lucas.bruand@ecl2002.ec-lyon.fr>
 #
 # By using this file, you agree to the terms and conditions set forth in
 # the LICENSE.html file which can be found at the top level of the ViewCVS
 # distribution or at http://viewcvs.sourceforge.net/license-1.html.
 #
 # Contact information:
 #   Greg Stein, PO Box 760, Palo Alto, CA, 94302
 #   gstein@lyra.org, http://viewcvs.sourceforge.net/
 #
 # -----------------------------------------------------------------------
 #
 # This software is being maintained as part of the ViewCVS project.
 # Information is available at:
 #    http://viewcvs.sourceforge.net/
 #
 # This file was originally based on portions of the blame.py script by
 # Curt Hagenlocher.
 #
 # -----------------------------------------------------------------------
 #
 */
 
/*
	This C++ library offers an API to a performance-oriented RCSFILE parser.
	It does little syntax checking.
	
	Version: $Id$
 */
#define CHUNK_SIZE 30000
#ifndef __PARSE_H
#define __PARSE_H
#include <iostream.h>
#include <strstream.h>
#include <stdio.h>
#include <fstream.h>
#include <string.h>
#include <stdlib.h>
#define delstr(a) if (a!=NULL) {delete [] a;a=NULL;};


/* This class represents a exception that occured during the parsing of a file */
class tparseException {
	
	char *value;
	public:
	tparseException(char *myvalue) { value=myvalue; };
	char *getvalue() { return value; };
};

/* This class is used to stored a list of the branches of a revision */
class Branche {
	public:
	char *name;
	Branche *next;
	Branche(char *myname, Branche *mynext) {
		name=myname;
		next=mynext;
	};
	~Branche() {
		delstr(name);
		name=NULL;
		if (next!=NULL) delete next;
		next=NULL;
	};
};
/* This class is a handler that receive the event generated by the parser 
   i.e.: When we reach the head revision tag, etc... */
class Sink {
	public:
	Sink() {};
	virtual int set_head_revision(char * revision) {
		cout<<" set head revision : "<<revision<<endl;
		delstr(revision);
		return 0;
	};
	virtual int set_principal_branch(char *branch_name) {
		cout<<" set principal branch : "<<branch_name<<endl;
		delstr(branch_name);
		return 0;
	};
	virtual int define_tag(char *name, char *revision) {
		cout<< " Tag: name="<<name<<" revision: "<<revision<<endl;
		delstr(name);
		return 0;
	};
	virtual int set_comment(char *comment) {
		cout<<" Comment: "<<comment<<endl;
		delstr(comment);
		return 0;
	};
	virtual int set_description(char *description) {
		cout<<"description :"<<description<<endl;
		delstr(description);
		return 0;
	};
	virtual int define_revision(char *revision, long timestamp, char *author, char *state, Branche *branches, char *next) {
		Branche *move;
		Branche *anc;
		cout<<"  Define_revision :"<<endl;
		cout<<"  |-revision = "<<revision<<endl;delstr(revision);
		cout<<"  |-timestamp= "<<timestamp<<endl;
		cout<<"  |-author   = "<<author<<endl;delstr( author);
		cout<<"  |-state    = "<<state<<endl;delstr(state);
		cout<<"  |-branches = ";
		move=branches;
		while (move!=NULL) {
			cout<<move->name<<", "; 
			anc=move;
			move=move->next; 
		};
		if (branches!=NULL) delete branches;
		cout<<endl;
		cout<<"  |-next     = "<<next<<endl<<endl;delstr(next);
		return 0;
	};
	virtual int set_revision_info(char *revision, char *log, char *text) 
	{
		cout << "set revision info :"<<revision<<endl;
		cout << "log :"<<log<<endl;
		cout <<"----text----"<<endl;
		cout << text;
		cout <<"----text----"<<endl;
		delstr(log);
		delstr(text);
		delstr(revision);
		return 0;
	};
  	virtual int tree_completed() { cout <<" tree completed"<<endl;return 0;};
  	virtual int parse_completed() {cout <<" parse completed"<<endl;return 0;};
};

/* The class is used to get one by one every token in the file. */
class TokenParser {
	private:
	istream *input;
	char buf[CHUNK_SIZE];
	int buflength;
	int idx;
	char *backget;
	public:
	char *semicol;
	char *get();
	void unget(char *token);
	int eof() {
		return (input->gcount()==0);
	};
	void matchsemicol() {
		char *ptr=get();
		if (ptr!=semicol) throw tparseException(" Incorrect syntax in the RCSFILE parsed!");
	};
	void match(char *token) {
		char *ptr;
		if (strcmp(ptr=get(),token)!=0) throw tparseException(" Incorrect syntax in the RCSFILE parsed!");
		delstr( ptr);
	};
	
	TokenParser(istream *myinput) {
		input=myinput;
		backget=NULL;
		idx=0;semicol=";";
		input->read(buf,CHUNK_SIZE);
		if ( (buflength=input->gcount())==0 )
			throw tparseException("Non-existing file or empty file");
	};
	
	
	~TokenParser() {
		if (input!=NULL) { delete input;input=NULL; };
	};
};

/* this is the class that does the actual job:
   by reading each part of the file and thus generate events to a sink event-handler*/
class tparseParser {
	private:
	TokenParser *tokenstream;
	Sink *sink;
	int parse_rcs_admin();
	int parse_rcs_tree();
	int parse_rcs_description();
	int parse_rcs_deltatext();
	public:
	tparseParser(ifstream *myinput,Sink* mysink) {
		sink=mysink;
		tokenstream= new TokenParser(myinput);
		
    		if (parse_rcs_admin()) return;
    		if (parse_rcs_tree()) return;

    		// many sinks want to know when the tree has been completed so they can
    		// do some work to prepare for the arrival of the deltatext
    		if (sink->tree_completed()) return;

    		if (parse_rcs_description()) return;
    		if (parse_rcs_deltatext()) return;

    		// easiest for us to tell the sink it is done, rather than worry about
    		// higher level software doing it.
    		if (sink->parse_completed()) return;
	}
	~tparseParser() {
		delete tokenstream;
    		delete sink;
	}
};

#endif
