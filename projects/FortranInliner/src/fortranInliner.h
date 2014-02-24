#ifndef _FORTRANINLINER_H
#define _FORTRANINLINER_H

#include "rose.h"
#include "sageBuilder.h"

using namespace std;
namespace fortranInliner 
{
  extern vector<SgStatement*> funcCallStmtList;
  /* Function Declaration for Fortran inliner. */
  void fInliner(SgProject* project);
  /* Checking if directive is given before function call*/
  SgStatement* verifyInlineCandidate(SgFunctionCallExp* funcCall);
  /* inline stmts from function body*/
  void inlineStmts (SgExprStatement* funcCallStmt, SgScopeStatement* scope);
}
#endif  //_FORTRANINLINER_H
