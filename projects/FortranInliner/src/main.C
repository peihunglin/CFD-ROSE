/*
  Author: Pei-Hung Lin
  Contact: lin32@llnl.gov

  Date Created       : Feb 2014
*/  

#include "rose.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

int main( int argc, char * argv[] )
{
// Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  AstTests::runAllTests(project);   
  return backend(project);
}

