/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "util.h"

static char * fxName;
static int keepLastScope = FALSE;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

static void popPostProc(TreeNode * t)
{
	if (t->nodekind == StmtK) {
		if (t->kind.stmt == CompoundK) {
			sc_pop();
		}
	}
	if (t == NULL) return;
	else return;
}

static void insertInputFx (void)
{
	TreeNode * fun_decl = newDeclNode(FunK);
	fun_decl->type = Integer;

	TreeNode * type_spec = newExpNode(TypeK);
	type_spec->attr.type = INT;

	TreeNode * comp_stmt = newStmtNode(CompoundK);
	comp_stmt->child[0] = NULL;
	comp_stmt->child[1] = NULL;

	fun_decl->lineno = 0;
	fun_decl->attr.name = "input";
	fun_decl->child[0] = type_spec;
	fun_decl->child[1] = NULL;
	fun_decl->child[2] = comp_stmt;

	st_insert("input", -1, addLoc(), fun_decl);
}

static void insertOutputFx (void)
{
	TreeNode * fun_decl = newDeclNode(FunK);
        fun_decl->type = Void;

        TreeNode * type_spec = newDeclNode(FunK);
        type_spec->attr.type = VOID;

        TreeNode * param = newDeclNode(ParamK);
        param->attr.name = "arg";
	param->child[0] = newExpNode(TypeK);
        param->child[0]->attr.type =  INT;

	TreeNode * comp_stmt = newStmtNode(CompoundK);
        comp_stmt->child[0] = NULL;
        comp_stmt->child[1] = NULL;

        fun_decl->lineno = 0;
        fun_decl->attr.name = "output";
        fun_decl->child[0] = type_spec;
        fun_decl->child[1] = param;
        fun_decl->child[2] = comp_stmt;

        st_insert("output", -1, addLoc(), fun_decl);

}

static void symbolErr (TreeNode * t, char * msg) {
	fprintf(listing, "Symbol table error at line %d: %s\n", t->lineno, msg);
	Error = TRUE;
	exit(-1);
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      { case CompoundK: {
		if (keepLastScope) {
			keepLastScope = FALSE;
		}
		else {
			Scope scope = sc_new(fxName);
			sc_push(scope);
		}
		t->attr.scope = sc_top();
		break;
	default:
		break;
	} // CompoundK
	}
      break;
    case ExpK:
      switch (t->kind.exp)
      { case IdK:
	case ArrIdK:
	case CallK:
          if (st_lookup(t->attr.name) == -1)
          /* not yet in table, so treat as new definition */
            //st_insert(t->attr.name,t->lineno,location++);
	    symbolErr(t, "undefined symbol");
          else {
          /* already in table, so ignore location, 
             add line number of use only */ 
            //st_insert(t->attr.name,t->lineno,0);
	    st_add_lineno(t->attr.name, t->lineno);
	    BucketList ls = st_lookup(t->attr.name);
	    //t->type = ls->type;
	  }
          break;
        default:
          break;
      } // ExpK
      break;
    case DeclK:
      switch (t->kind.decl)
      { case VarK:
	case ArrVarK:
	{
		char * name;
		if (t->child[0]->attr.type == VOID) {
			symbolErr(t, "variable cannot be of type void");
			break;
		}
		if (t->kind.decl == VarK) {
			name = t->attr.name;
			t->type = Integer;
		}
		else {
			name = t->attr.arr.name;
			t->type = IntegerArr;
		}
		if (st_lookup_top(name) < 0)
			st_insert(name, t->lineno, addLoc(), t);
		else
			symbolErr(t, "symbol already declared in current scope");
      } // ArrVarK
      break;
	case FunK:
      		fxName = t->attr.name;
		if (st_lookup_top(fxName) >= 0) {
			symbolErr(t, "function already declared");
		break;
		}
		st_insert(fxName, t->lineno, addLoc(), t);
		sc_push(sc_new(fxName));
		keepLastScope = TRUE;
		switch (t->child[0]->attr.type)
		{ case INT:
			t->type = Integer;
			break;
		  case VOID:
			t->type = VOID;
			break;
		}
	break;
	default:
	break;
      } // DeclK
      break;
    case ParamK: {
	if (t->child[0]->attr.type == VOID)
		symbolErr(t->child[0], "parameter of type void is not allowed");
	if (st_lookup(t->attr.name) == -1) {
		st_insert(t->attr.name, t->lineno, addLoc(), t);
		if (t->kind.param == NonArrParamK)
			t->type = Integer;
		else
			symbolErr(t, "symbol already declared in current scope");
	}
	}
	break;
    default:
      break;
  } // nodekind switch
} // insertNode

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ 
	globalScope = sc_new("global");
	sc_push(globalScope);
	insertInputFx();
	insertOutputFx();
	traverse(syntaxTree,insertNode,popPostProc);
	sc_pop();
  if (TraceAnalyze)
  { //fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode * t, char * msg)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno, msg);
  Error = TRUE;
  exit(-1);
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { case ExpK:
      switch (t->kind.exp)
      { case OpK:
          if ((t->child[0]->type != Integer) ||
              (t->child[1]->type != Integer))
            typeError(t,"Op applied to non-integer");
          if ((t->attr.op == EQ) || (t->attr.op == LT))
            t->type = Boolean;
          else
            t->type = Integer;
          break;
        case ConstK:
        case IdK:
          t->type = Integer;
          break;
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt)
      { /*case IfK:
          if (t->child[0]->type == Integer)
            typeError(t->child[0],"if test is not Boolean");
          break;*/
        case AssignK:
          if (t->child[0]->attr.type == IntegerArr)
            typeError(t->child[0],"assignment to integer array value");
	  else if (t->child[0]->attr.type == Void) 
		  typeError(t->child[0], "assignment to void value");
          break;
	/*
        case WriteK:
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"write of non-integer value");
          break;
        case RepeatK:
          if (t->child[1]->type == Integer)
            typeError(t->child[1],"repeat test is not Boolean");
          break;
	  */
	case ReturnK: 
	  {
		  const TreeNode * fun_decl;
		  const ExpType fxType = fun_decl->type;
		  const TreeNode * expr = t->child[0];

		  if (fxType == Void && (expr != NULL && expr->type != Void))
			  typeError(t, "expected no return value");
		  else if (fxType == Integer && (expr == NULL || expr->type == Void))
			  typeError(t, "expected a return value");
	  }
	  break;
        default:
          break;
      }
      break;
    default:
      break;

  }
} 

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
