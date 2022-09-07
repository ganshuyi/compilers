/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the compiler     */
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* create new scope */
Scope sc_new (char * scopeName) 
{
	Scope newScope = (Scope) malloc(sizeof(struct ScopeListRec));
	newScope->name = scopeName;
	newScope->nestedLvl = topScope;
	newScope->parent = sc_top();
	scopeList[listSize++] = newScope;
	
	return newScope;
}

void sc_pop (void)
{
	--topScope;	
}

void sc_push (Scope scope)
{
	scopeStack[topScope] = scope;
	location[topScope++] = 0;
}

Scope sc_top (void)
{
	return scopeStack[topScope-1];
}

int addLoc (void)
{
	return location[topScope-1]++;
}


/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int loc, TreeNode * treeNode)
{ 
  int h = hash(name);
  Scope scope = sc_top();
  BucketList l =  scope->bucket[h];

  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = scope->bucket[h];
    l->treeNode = treeNode;
    scope->bucket[h] = l; 
  }
  else /* found in table, so just add line number */
  { LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name )
{ 
  BucketList l =  st_bucket(name);
  if (l == NULL) return -1;
  return l->memloc;
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i,j;
  
  fprintf(listing, "\n< Symbol Table >\n");
  fprintf(listing,"Variable Name  Variable Type  Scope Name  Location  Line Numbers\n");
  fprintf(listing,"-------------  -------------  ----------  --------  ------------\n");
  for (i=0;i<listSize;++i) {
	  Scope scope = scopeList[i];
	  BucketList * hashTable = scope->bucket;

	  if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { LineList t = l->lines;
        fprintf(listing,"%-14s ",l->name);
        fprintf(listing,"%-8d  ",l->memloc);
        while (t != NULL)
        { fprintf(listing,"%4d ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */

int st_lookup_top (char * name)
{
	int h = hash(name);
	Scope scope = sc_top();
	while (scope) {
		BucketList bl = scope->bucket[h];
		while ((bl != NULL) && (strcmp(name,bl->name) != 0))
    			bl = bl->next;
    		if (bl != NULL) return bl->memloc;
    		break;
    	}
    	return -1;
}

BucketList st_bucket (char * name) 
{
	int h = hash(name);
	Scope scope = sc_top();
	while (scope) {
		BucketList bl = scope->bucket[h];
		while ((bl != NULL) && (strcmp(name, bl->name) != 0))
			bl = bl->next;
		if (bl != NULL) return bl;
		scope = scope->parent;
	}
	return NULL;
}

void st_add_lineno (char * name, int lineno)
{
	BucketList bl = st_bucket(name);
	LineList ll = bl->lines;
	while (ll->next != NULL)
		ll = ll->next;
	ll->next = (LineList) malloc(sizeof(struct LineListRec));
	ll->next->lineno = lineno;
	ll->next->next = NULL;
}


