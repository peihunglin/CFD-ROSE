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
    SgExpressionPtrList callerArgExpList = funcCallExp->get_args()->get_expressions();
    // FunctionDeclaration to be inlined
    SgFunctionDeclaration* funcDecl = funcCallExp->getAssociatedFunctionDeclaration();
    ROSE_ASSERT(funcDecl);
    SgFunctionDefinition* funcDef = funcDecl->get_definition();
    ROSE_ASSERT(funcDef);
    SgInitializedNamePtrList calleeArgList = funcDecl->get_args();

    // Matching argument list
    // Building the map to make an easier replacement later
    ROSE_ASSERT(callerArgExpList.size() == calleeArgList.size());
    std::map<SgInitializedName*, SgExpression*> argsMap;
    Rose_STL_Container<SgInitializedName*>::iterator calleeIt=calleeArgList.begin();
    Rose_STL_Container<SgExpression*>::iterator callerIt=callerArgExpList.begin();
    while(calleeIt != calleeArgList.end() && callerIt != callerArgExpList.end()) 
    {
      argsMap.insert(std::make_pair(*calleeIt, *callerIt));
      ++calleeIt;
      ++callerIt;
    }

//  The symbol table at callee's funcDef
    SgSymbolTable* funcDefSymTable = funcDef->get_symbol_table();
    SgBasicBlock* bblock = funcDef->get_body();
    ROSE_ASSERT(bblock);
//  The symbol table at callee's basic block 
    SgSymbolTable* calleeSymTable = bblock->get_symbol_table();
//  The symbol table at caller side
    SgSymbolTable* callerSymTable = scope->get_symbol_table();

    // using map to store all variable names to avoid duplication
    std::map<SgName,SgVariableDeclaration*> symbolMap;

    // Handling symbolTable under funcDefinition
    std::set<SgNode*> funArgSymbolList = funcDefSymTable->get_symbols();
    for(std::set<SgNode*>::iterator it = funArgSymbolList.begin(); it !=funArgSymbolList.end(); ++it)
    {
      SgVariableSymbol* sym = isSgVariableSymbol(*it);
      ROSE_ASSERT(sym);
      SgInitializedName* argInitName = sym->get_declaration();
      // get the mapped SgExpression from caller's argument list
      SgExpression* callerExp = argsMap.find(argInitName)->second; 

      // I think we only need to handle arrRefExp for now. 
      if(isSgPntrArrRefExp(callerExp) != NULL)
      {
        SgVarRefExp* lhsRef = isSgVarRefExp(isSgBinaryOp(callerExp)->get_lhs_operand());
        SgInitializedName* callerVarInitName = isSgVarRefExp(lhsRef)->get_symbol()->get_declaration();
        SgType* varType = sym->get_type();
        SgName varName = sym->get_name();
        // Name has conflict at caller side
        if(varName == callerVarInitName->get_name())
        {
          string uniqName = generateUniqueVariableName(scope,varName.getString());
          varName = SgName(uniqName);
        }
        SgVariableDeclaration* varDecl = buildVariableDeclaration(varName,varType,NULL,scope);
        varDecl->set_parent(scope);
        symbolMap.insert(std::make_pair(sym->get_name(), varDecl));
        // A equivalence stmt has to be inserted
//        SgEquivalenceStatement* equivStmt = buildEquivalenceStatement(buildVarRefExp(callerVarInitName),buildVarRefExp(varDecl));
//        equivStmt->set_parent(scope);
//        setSourcePositionForTransformation(equivStmt);
//        insertStatementAfterLastDeclaration(equivStmt,scope);
      } 

    }

    // Handling symbolTable in basicblock
    std::set<SgNode*> calleeSymbolList = calleeSymTable->get_symbols();
    for(std::set<SgNode*>::iterator it = calleeSymbolList.begin(); it !=calleeSymbolList.end(); ++it)
    {
      SgVariableSymbol* sym = isSgVariableSymbol(*it);
      // Skip invalid symbol or symbol for function name
      if((sym == NULL) || (isSgFunctionDeclaration(sym->get_declaration()->get_parent()))) continue;
      SgName varName = sym->get_name();
      SgType* varType = sym->get_type();
      // variable name is used in caller side
      if(callerSymTable->exists(varName) == true)
      {
//        cout << varName << " exist!" << endl;
        string uniqName = generateUniqueVariableName(scope,varName.getString());
        varName = SgName(uniqName);
      }

      SgVariableDeclaration* varDecl = buildVariableDeclaration(varName,varType,NULL,scope);
      varDecl->set_parent(scope);
      symbolMap.insert(std::make_pair(sym->get_name(), varDecl));
    }

    SgStatementPtrList stmtList = bblock->get_statements();
    for(Rose_STL_Container<SgStatement*>::iterator i=stmtList.begin(); i != stmtList.end(); ++i)
    {
      SgStatement* stmt = isSgStatement(*i);
      ROSE_ASSERT(stmt);
      // serach for all VarRefExp in SgAttributeSpecificationStatement
      // Apprently deepCopy doesn't really copy SgAttributeSpecificationStatement
      // We need to build this step by step 
      SgAttributeSpecificationStatement* attributeStmt = isSgAttributeSpecificationStatement(stmt);
      if((attributeStmt !=NULL) && (attributeStmt->get_attribute_kind() == SgAttributeSpecificationStatement::e_dimensionStatement))
      {
        SgExpressionPtrList parameterList =  attributeStmt->get_parameter_list()->get_expressions();
        SgExprListExp* newparameterList = buildExprListExp();
        SgAttributeSpecificationStatement* newattributeStmt = buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_dimensionStatement); 

        for(SgExpressionPtrList::iterator k=parameterList.begin(); k != parameterList.end(); ++k)
        {
          SgPntrArrRefExp* pntrArrRefExp = deepCopy(isSgPntrArrRefExp(*k));
          ROSE_ASSERT(pntrArrRefExp);
          SgVarRefExp* varRef = isSgVarRefExp(pntrArrRefExp->get_lhs_operand());
          ROSE_ASSERT(varRef);
          SgName tmpName = varRef->get_symbol()->get_name();
          SgVariableDeclaration* newDecl = symbolMap.find(tmpName)->second;
//          newDecl->set_parent(scope);
          SgInitializedNamePtrList varList = newDecl->get_variables();
          ROSE_ASSERT(varList.size() == 1);
          SgInitializedName* initname = varList.at(0);
          //cout << "found " << tmpName << " new name: " << initname->get_name() << " " << newDecl->get_parent()<< endl;
          SgVariableSymbol* symbol = isSgVariableSymbol(initname->get_symbol_from_symbol_table());
          varRef->set_symbol(symbol);
          newparameterList->append_expression(pntrArrRefExp);
        }
        newattributeStmt->set_parameter_list(newparameterList);
        insertStatement(funcCallStmt, newattributeStmt, true, false);
      }
      else
      {
        SgStatement* newStmt = deepCopy(stmt);
        Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(newStmt,V_SgVarRefExp);
        for(Rose_STL_Container<SgNode*>::iterator j = varList.begin(); j != varList.end(); ++j)
        {
          SgVarRefExp* varRef = isSgVarRefExp(*j);
          ROSE_ASSERT(varRef);
          SgName tmpName = varRef->get_symbol()->get_name();
          
          // replacing all varRef for arguments
          SgInitializedName* varInitializedName = varRef->get_symbol()->get_declaration();
          SgType* varType = varInitializedName->get_type();
          Rose_STL_Container<SgInitializedName*>::iterator it = find(calleeArgList.begin(),calleeArgList.end(),varInitializedName);        
          if(it != calleeArgList.end())
          {
            cout << tmpName << " used Arg" << endl;
            // Simply replacing the varRef with the SgExpression at caller's argument list
            if(isSgArrayType(varType) == NULL)
              replaceExpression(varRef, argsMap.find(varInitializedName)->second);
            continue;
          }
  
          SgVariableDeclaration* newDecl = symbolMap.find(tmpName)->second;
          newDecl->set_parent(scope);
          SgInitializedNamePtrList varList = newDecl->get_variables();
          ROSE_ASSERT(varList.size() == 1);
          SgInitializedName* initname = varList.at(0);
          SgVariableSymbol* symbol = isSgVariableSymbol(initname->get_symbol_from_symbol_table());
          varRef->set_symbol(symbol);
        }
        insertStatement(funcCallStmt, newStmt, true, false);
      }
    }
    fixVariableReferences(scope);
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
