/****************************************************/
/* File: cminus.y                                   */
/* The C-minus Yacc/Bison specification file        */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static int savedNum;
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);

%}

%token INT VOID
%token COMMA SEMI
%token IF ELSE WHILE RETURN
%token ID NUM
%right ASSIGN
%token LT LE GT GE NE EQ
%left PLUS MINUS
%left TIMES OVER
%left LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY
%token ERROR 

%nonassoc LOWERELSE
%nonassoc ELSE

%% /* Grammar for C-minus */

program     : declarationLs
                 { savedTree = $1;
		 //  printf("in prog\n");
	    } 
            ;

declarationLs    : declarationLs decl
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
		//	printf("in decl list\n");
                 }
            | decl  { $$ = $1; 
		//printf("in decl list\n");
	    }
            ;

decl	: varDecl { $$ = $1; //printf("in decl\n");
     	}
	| funDecl { $$ = $1; //printf("in decl\n");
	}
	;

id		: ID
	 	{ savedName = copyString(tokenString);
		  savedLineNo = lineno;
 		//	printf("in ID\n");
		}
		;

num	        : NUM
                { savedNum = atoi(tokenString);
                  savedLineNo = lineno;
		//	printf("in num\n");
                }
                ;

varDecl	: typeSpec id SEMI
	{ $$ = newDeclNode(VarK);
	  $$->child[0] = $1;
	  $$->lineno = lineno;
	  $$->attr.name = savedName;
//	  printf("in vardecl\n");
	}
	| typeSpec id LBRACE arrSize RBRACE SEMI
	{ $$ = newDeclNode(ArrVarK);
	  //$$->child[0] = $1;
	  $$->child[1] = $4;
	  $$->lineno = lineno;
	  $$->attr.arr.name = savedName;
	  $$->type = IntegerArr;
	//  printf("in vardecl (arr)\n");
	}
	;


arrSize : num
	{ $$ = newExpNode(ConstK);
	  $$->lineno = lineno;
	  $$->attr.val = savedNum;
	//	printf("in arr size\n");
	}

typeSpec    : INT 
	    { $$ = newTypeNode(TypeNameK);
	      $$->attr.type = INT;
	    }
	//	printf("in typespec int\n");
	    | VOID
	    { $$ = newTypeNode(TypeNameK);
	      $$->type = Void;
	      $$->attr.type = VOID;
	//	printf("in typespec\n");
	    }
	    ;

funDecl	: typeSpec id
	  { $$ = newDeclNode(FunK);
	    $$->lineno = lineno;
	    $$->attr.name = savedName;
		//printf("in func decl\n");
	  }
	  LPAREN parameters RPAREN compStmt
	  { $$ = $3; 
	    $$->child[0] = $1;
	    $$->child[1] = $5;
	    $$->child[2] = $7;
	//	printf("in func decl\n");
	  }
	  ;

parameters	: paramLs { $$ = $1; //printf("in parameters\n");
	        }
	   	| VOID
		  { $$ = newParamNode(VoidParamK);
		    $$->attr.type = VOID;
	//		printf("in parameters\n");
		}
		;

paramLs	: paramLs COMMA param
	{ YYSTYPE t = $1;
          if (t != NULL)
             { while (t->sibling != NULL)
                     t = t->sibling;
               t->sibling = $3;
               $$ = $1; }
           else $$ = $3;
	//	printf("in param ls\n");
        }
        | param { $$ = $1;// printf("in param ls\n");
	}
        ;

param	: typeSpec id
      	  { $$ = newParamNode(NonArrParamK);
            $$->child[0] = $1;
            $$->attr.name = savedName;
	//	printf("in param\n");
	  }
	  | typeSpec id LBRACE RBRACE
	  { $$ = newParamNode(ArrParamK);
            $$->child[0] = $1;
            $$->attr.name = savedName;
	//	printf("in param\n");
	  }

compStmt	: LCURLY localDecl stmtLs RCURLY
	 	  { $$ = newStmtNode(CompoundK);
		    $$->child[0] = $2;
		    $$->child[1] = $3;
	//		printf("in compstmt\n");
		  }
		;

localDecl	: localDecl varDecl
	  	{ YYSTYPE t = $1;
	          if (t != NULL)
        	     { while (t->sibling != NULL)
                	     t = t->sibling;
              		t->sibling = $2;
	                $$ = $1; }
           	  else $$ = $2;
	//	printf("in local decl\n");
        	}
		| { $$ = NULL;// printf("in local decl\n");
		}
		;

stmtLs	: stmtLs stmt
        { YYSTYPE t = $1;
          if (t != NULL)
             { while (t->sibling != NULL)
                     t = t->sibling;
               t->sibling = $2;
               $$ = $1; }
           else $$ = $2;
		//printf("in stmt ls\n");
        }
	| { $$ = NULL;// printf("in stmt ls\n");
	}
	;


stmt        : exprStmt { $$ = $1;// printf("in stmt\n");
	    }
            | compStmt { $$ = $1; //printf("in stmt\n");
	    }
            | selectStmt { $$ = $1; //printf("in stmt\n");
	    }
            | iterStmt { $$ = $1; ///printf("in stmt\n");
	    }
            | returnStmt { $$ = $1; //printf("in stmt\n");
	    }
            ;

exprStmt    : expr SEMI { $$ = $1; //printf("in exp stmt\n");
	    }
	    | SEMI { $$ = NULL; //printf("in exp stmt\n");
	    }
	    ;

selectStmt  : IF LPAREN expr RPAREN stmt %prec LOWERELSE
	    { $$ = newStmtNode(IfK);
	      $$->child[0] = $3;
	      $$->child[1] = $5;
	      //printf("in select stmt\n");
	      //$$->child[2] = NULL;
	    }
	    | IF LPAREN expr RPAREN stmt ELSE stmt
	    { $$ = newStmtNode(IfElseK);
              $$->child[0] = $3;
              $$->child[1] = $5;
	      $$->child[2] = $7;
		//printf("in select stmt\n");
	    }
	    ;

iterStmt	: WHILE LPAREN expr RPAREN stmt
	 	  { $$ = newStmtNode(WhileK);
        	    $$->child[0] = $3;
	            $$->child[1] = $5;
		//	printf("in iter stmt\n");
		  }
		;

returnStmt	: RETURN SEMI
	   	  { $$ = newStmtNode(ReturnK);
        	    $$->attr.type = VOID;
		//	printf("in return stmt\n");
		  }
		| RETURN expr SEMI
		  { $$ = newStmtNode(ReturnK);
	            $$->child[0] = $2;
		//	printf("in return\n");
		  }
		;

expr	: var ASSIGN expr
     	  { $$ = newExpNode(AssignK);
            $$->child[0] = $1;
            $$->child[1] = $3;
		//printf("in expr\n");
	  }
	| simpleExpr { $$ = $1;// printf("in expr\n"); 
	}
	;

var	: id
    	  { $$ = newExpNode(IdK);
	    $$->attr.name = savedName;
	  }
    	| id 
	  { $$ = newExpNode(ArrIdK);
            $$->attr.name = savedName;
	  } LBRACE expr RBRACE
	  {
	    $$ = $2;
	    $$->child[0] = $4;
	  }
	;

simpleExpr	: addExpr LE addExpr
	   	  { $$ = newExpNode(OpK);
		    $$->child[0] = $1;
		    $$->child[1] = $3;
		    $$->attr.op = LE;
		  }
		|  addExpr LT addExpr
                  { $$ = newExpNode(OpK);
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                    $$->attr.op = LT;
                  }
		| addExpr GT addExpr
                  { $$ = newExpNode(OpK);
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                    $$->attr.op = GT;
                  }
		| addExpr GE addExpr
                  { $$ = newExpNode(OpK);
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                    $$->attr.op = GE;
                  }
		| addExpr EQ addExpr
                  { $$ = newExpNode(OpK);
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                    $$->attr.op = EQ;
                  }
		| addExpr NE addExpr
                  { $$ = newExpNode(OpK);
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                    $$->attr.op = NE;
                  }

	   	| addExpr
		  { $$ = $1; //printf("in simple expr\n");
		}
		;

addExpr	: addExpr PLUS term
	  { $$ = newExpNode(OpK);
            $$->child[0] = $1;
            $$->child[1] = $3;
	    $$->attr.type = PLUS;
	  }
	| addExpr MINUS term
          { $$ = newExpNode(OpK);
            $$->child[0] = $1;
            $$->child[1] = $3;
	    $$->attr.type = MINUS;
          }
	| term 
	  { $$ = $1;// printf("in addexpr\n");
	}
	;

term	: term TIMES factor
     	  { $$ = newExpNode(OpK);
            $$->child[0] = $1;
            $$->child[1] = $3;
            $$->attr.type = TIMES;
	} 
	|term OVER factor
          { $$ = newExpNode(OpK);
            $$->child[0] = $1;
            $$->child[1] = $3;
            $$->attr.type = OVER;
	} 
     	| factor
	  { $$ = $1; //printf("in term\n");
	}
	;

factor	: LPAREN expr RPAREN { $$ = $2;// printf("in factor\n");
        }
       	| var { $$ = $1; //printf("in factor\n");
	}
	| call { $$ = $1; //printf("in factor\n");
	}
	| num 
	  { $$ = newExpNode(ConstK);
	    $$->attr.val = savedNum;
	    //$$->attr.type = INT;
	//	printf("in factor\n");
	  }
	;

call	: id 
    	  { $$ = newExpNode(CallK);
	    $$->attr.name = savedName;
	  } LPAREN arg RPAREN
	  { $$ = $2;
	    $$->child[0] = $4;
	//	printf("in call\n");
	  }
	;

arg	: argLs { $$ = $1; //printf("in arg\n");
    	}
    	| { $$ = NULL; //printf("in arg\n");
	}
	;

argLs	: argLs COMMA expr
          { YYSTYPE t = $1;
          if (t != NULL)
             { while (t->sibling != NULL)
                     t = t->sibling;
               t->sibling = $3;
               $$ = $1; }
           else $$ = $3;
	//printf("in argls\n");
          }
        | expr { $$ = $1; //printf("in argls\n");
	}
        ;


%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

