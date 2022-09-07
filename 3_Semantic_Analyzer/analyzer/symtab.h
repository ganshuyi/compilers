/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the C-minus compiler  */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

/* SIZE is the size of the hash table */
#define SIZE 211

#define MAX_SCOPE 1000

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec
{
	int lineno;
	struct LineListRec * next;
} * LineList;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
{
	char * name;
	int scope;
	LineList lines;
	int memloc;
	struct BucketListRec * next;
	TreeNode * treeNode;
} * BucketList;


typedef struct ScopeListRec
{
	char * name;
	int nestedLvl;
	BucketList bucket[SIZE]; // hash table
	struct ScopeListRec * parent;
} * Scope;

// SCOPE

Scope globalScope;

static Scope scopeList[MAX_SCOPE];
static int listSize = 0;

static Scope scopeStack[MAX_SCOPE];
static int topScope = 0;
static int location[MAX_SCOPE];

Scope sc_new (char * scopeName);
void sc_pop (void);
void sc_push (Scope scope);
//void sc_insertLs (Scope scope);
Scope sc_top (void);
int addLoc (void);

// STACK

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int loc, TreeNode * treeNode);

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name );

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

int st_lookup_top (char * name);
BucketList st_bucket (char * name);
void st_add_lineno ( char * name, int lineno );

#endif
