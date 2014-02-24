#include "fortranInliner.h"
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

namespace fortranInliner
{
  // vector of SgFunctionCallExp to store all AST nodes
  vector<SgStatement*> funcCallStmtList;
  
  // memory pool traversal for SgFunctionCallExp
  class functionCallTraversal : public ROSE_VisitorPattern
  {
    public:
      void visit(SgFunctionCallExp* funcCallExp);
  };
  
  void functionCallTraversal::visit(SgFunctionCallExp* funcCallExp)
  {
    // push found AST node into vector.  
    // Transformation will happen later
    SgStatement* stmt = NULL;
    if((stmt = verifyInlineCandidate(funcCallExp)))
      funcCallStmtList.push_back(stmt);
  }

  // Function definition for fInliner (main inline driver)  
  void fInliner(SgProject* project)
  {
    // Perform memory pool traversal
    functionCallTraversal inlineFuncCallExp;
    traverseMemoryPoolVisitorPattern(inlineFuncCallExp);
    // Iterate over the functionCallExp list for inlining
    for(vector<SgStatement*>::iterator funcCall=funcCallStmtList.begin(); funcCall!=funcCallStmtList.end(); ++funcCall)
    {
      SgExprStatement* funcCallStmt = isSgExprStatement(*funcCall);
      ROSE_ASSERT(funcCallStmt);
      cout << funcCallStmt << " found!!" << endl;

      // step 1 inline all statements 
      inlineStmts(funcCallStmt, getScope(funcCallStmt));
      //  removing funcCallStmt
      removeStatement(funcCallStmt,false);
         
    }
  }

  /* inline stmts from function body*/
  void inlineStmts(SgExprStatement* funcCallStmt, SgScopeStatement* scope)
  {
    SgFunctionCallExp* funcCallExp = isSgFunctionCallExp(funcCallStmt->get_expression());
    ROSE_ASSERT(funcCallExp);
    SgFunctionDeclaration* funcDecl = funcCallExp->getAssociatedFunctionDeclaration();
    ROSE_ASSERT(funcDecl);
    cout << "funcDecl=" << funcDecl << endl;
    SgFunctionDefinition* funcDef = funcDecl->get_definition();
    ROSE_ASSERT(funcDef);
    cout << "funcDef=" << funcDef << endl;
    SgBasicBlock* bblock = funcDef->get_body();
    ROSE_ASSERT(bblock);
    cout << "basic block=" << bblock << endl;
    SgStatementPtrList stmtList = bblock->get_statements();
    for(Rose_STL_Container<SgStatement*>::iterator i=stmtList.begin(); i != stmtList.end(); ++i)
    {
      // Find varRef used in each statement
      // If varName is used in the caller, we need to rename it
      SgStatement* stmt = isSgStatement(*i);
      ROSE_ASSERT(stmt);
      SgStatement* newStmt = deepCopy(stmt);
      Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(newStmt,V_SgVarRefExp);
      for(Rose_STL_Container<SgNode*>::iterator j = varList.begin(); j != varList.end(); ++j)
      {
        SgVarRefExp* varRef = isSgVarRefExp(*j);
        ROSE_ASSERT(varRef);
        SgVariableSymbol* varSymbol = varRef->get_symbol();
        SgName varName = varSymbol->get_name();
        SgSymbol* tmp = lookupSymbolInParentScopes(varName, scope);
        if(tmp != NULL)
        {
          cout << "var name found in both caller and callee: " << varName.getString() << endl;
          SgName newName = SgName(varName.getString()+"_new");
          varSymbol->get_declaration()->set_name(newName);
        }
      }
      insertStatement(funcCallStmt, newStmt, true, false);
    } 
  }

/* verify directive is given to the SgStatement*/
  SgStatement* verifyInlineCandidate(SgFunctionCallExp* funcCall)
  {
    SgStatement* stmt = isSgStatement(funcCall->get_parent());
    ROSE_ASSERT(stmt);
    AttachedPreprocessingInfoType* comments = stmt->getAttachedPreprocessingInfo();
    if (comments != NULL)
    {
//      printf ("Found attached comments (to IR node at %p of type: %s): \n",stmt,stmt->class_name().c_str());
//      int counter = 0;
      AttachedPreprocessingInfoType::iterator i;
      for (i = comments->begin(); i != comments->end(); i++)
      {
        string commentText = (*i)->getString();
        size_t pragmaPos = commentText.find("pragma",0,6);
        size_t tokenPos = commentText.find("inline", pragmaPos+1, 6);
        if(pragmaPos && tokenPos)
        {
          // Check the tailing string to verify the directive content
          const char* endString = commentText.substr(tokenPos).c_str();
          //printf("%s\n", endString);
          if (endString[6]!=' ' && endString[6]!='\0' && endString[6]!='\n' && endString[6]!='\t' && endString[6]!='!')
          {
            return NULL;
          }
          printf ("          Qualified Comment in file %s (relativePosition=%s) :\n%s\n",
               (*i)->get_file_info()->get_filenameString().c_str(),
               ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
               commentText.c_str());
          return stmt;
        }
      }
    }
    return NULL;
  }
}
