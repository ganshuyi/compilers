// 2019029089 GAN SHU YI

/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the C-minus compiler                         */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { // reserved words
    case IF: 
	fprintf(listing,
		"reserved word: %s\n", tokenString); 
	break;
    case ELSE: 
	fprintf(listing, 
		"reserved word: %s\n", tokenString); 
	break;
    case WHILE: 
	fprintf(listing, 
		"reserved word: %s\n", tokenString); 
	break;
    case RETURN: 
	fprintf(listing, 
		"reserved word: %s\n", tokenString); 
	break;
    case INT: 
	fprintf(listing, 
		"reserved word: %s\n", tokenString); 
	break;
    case VOID: 
	fprintf(listing, 
		"reserved word: %s\n", tokenString); 
	break;
    // special symbols
    case ASSIGN: fprintf(listing,"=\n"); break;
    case EQ: fprintf(listing,"==\n"); break;
    case NE: fprintf(listing, "!=\n"); break;
    case LT: fprintf(listing,"<\n"); break;
    case LE: fprintf(listing, "<=\n"); break;
    case GT: fprintf(listing, ">\n"); break;
    case GE: fprintf(listing, ">=\n"); break;
    case PLUS: fprintf(listing,"+\n"); break;
    case MINUS: fprintf(listing,"-\n"); break;
    case TIMES: fprintf(listing,"*\n"); break;
    case OVER: fprintf(listing,"/\n"); break;
    case LPAREN: fprintf(listing,"(\n"); break;
    case RPAREN: fprintf(listing,")\n"); break;
    case LBRACE: fprintf(listing, "[\n"); break;
    case RBRACE: fprintf(listing, "]\n"); break;
    case LCURLY: fprintf(listing, "{\n"); break;
    case RCURLY: fprintf(listing, "}\n"); break;
    case SEMI: fprintf(listing,";\n"); break;
    case COMMA: fprintf(listing, ",\n"); break;
    // book-keeping tokens
    case ENDFILE: fprintf(listing,"EOF\n"); break;
    case NUM:
      fprintf(listing,
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      fprintf(listing,
          "ID, name= %s\n",tokenString);
      break;
    case ERROR:
      fprintf(listing,
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    //t->type = Void;
  }
  return t;
}

/* Function newDeclNode creates a new declaration node
 * for syntax tree constuction */
TreeNode * newDeclNode(DeclKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t==NULL)
		fprintf(listing,"Out of memory error at line %d\n",lineno);
	else {
		 for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
		 t->sibling = NULL;
		 t->nodekind = DeclK;
    		 t->kind.decl = kind;
    		 t->lineno = lineno;
	}
	return t;
}

/* Function newParamNode creates a new parameter node
 * for syntax tree construction */
TreeNode * newParamNode(ParamKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
        int i;
        if (t==NULL)
                fprintf(listing,"Out of memory error at line %d\n",lineno);
        else {
                 for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
                 t->sibling = NULL;
                 t->nodekind = ParamK;
                 t->kind.type = kind;
                 t->lineno = lineno;
        }
        return t;
}

/* Function newTypeNode creates a new type node
 * for syntax tree construction */
TreeNode * newTypeNode(TypeKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
        int i;
        if (t==NULL)
                fprintf(listing,"Out of memory error at line %d\n",lineno);
        else {
                 for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
                 t->sibling = NULL;
                 t->nodekind = TypeK;
                 t->kind.type = kind;
                 t->lineno = lineno;
        }
        return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2


/* function printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    if (tree->nodekind==StmtK)
    { printSpaces();
      switch (tree->kind.stmt) {
        case CompoundK:
          fprintf(listing,"Compound statement:\n");
          break;
	case IfK:
	  fprintf(listing,"If Statement:\n");
	  break;
	case IfElseK:
	  fprintf(listing,"If-Else Statement:\n");
	  break;
        case WhileK:
          fprintf(listing,"While statement:\n");
          break;
        case ReturnK:
          fprintf(listing,"Return Statement:\n");
          break;
        default:
          fprintf(listing,"Unknown StmtNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK)
    { printSpaces();
      switch (tree->kind.exp) {
        case OpK:
          fprintf(listing,"Op: ");
          printToken(tree->attr.op,"\n");
          break;
        case ConstK:
          fprintf(listing,"Const: %d\n",tree->attr.val);
          break;
        case IdK:
          fprintf(listing,"Variable: name = %s\n",tree->attr.name);
          break;
	case ArrIdK:
	  fprintf(listing,"Variable: name = %s\n",tree->attr.name);
	  break;
	case CallK:
	  fprintf(listing,"Call: function name = %s\n",tree->attr.name);
	  break;
	case AssignK:
          fprintf(listing,"Assign:\n");
          break;
	default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==DeclK)
    { printSpaces();
      switch (tree->kind.decl) {
	case VarK:
	  fprintf(listing,"Variable declaration: name = %s, ",tree->attr.name);
	  break;
	case FunK:
	  fprintf(listing,"Function declaration: name = %s, return ",tree->attr.name);
          break;
	case ArrVarK:
	  fprintf(listing,"Variable declaration: name = %s, type = int[]\n",tree->attr.arr.name);
	  //fprintf(listing,"Array length: %d,",tree->attr.arr.size);
          break;
	default:
	  fprintf(listing,"Unknown DeclNode\n");
	  break;
	}
    }
    else if (tree->nodekind==ParamK)
    { printSpaces();
      switch (tree->kind.param) {
    	case ArrParamK:
	  fprintf(listing,"Array parameter: name = %s, ",tree->attr.name);
          break;
	case NonArrParamK:
	  fprintf(listing,"Parameter: name = %s, ",tree->attr.name);
          break;
       case VoidParamK:
          fprintf(listing,"Void Parameter\n");
          break;
       default:
          fprintf(listing,"Unknown ParamNode\n");
          break;
       }
    }
    else if (tree->nodekind==TypeK)
    { switch (tree->kind.type) {
	case TypeNameK: 
	  fprintf(listing,"type = ");
	  switch (tree->attr.type) {
	    case INT:
	      fprintf(listing,"int\n");
	      break;
	    case VOID:
	      fprintf(listing, "void\n");
	      break;
	  }
	  break;	 
	default:
	  fprintf(listing,"Unknown TypeNode\n");
	  break;
	}
    }
    else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
         printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}

