/*
  Author: Pei-Hung Lin
  Contact: lin32@llnl.gov

  Date Created       : Feb 2014
*/  

#include "rose.h"
#include "fortranInliner.h"

using namespace std;

int main( int argc, char * argv[] )
{
// Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  AstTests::runAllTests(project);
  fortranInliner::fInliner(project); 
  AstTests::runAllTests(project);
  string filename = SageInterface::generateProjectName(project);
  generateWholeGraphOfAST(filename+"wholeAST");
  return backend(project);
}
