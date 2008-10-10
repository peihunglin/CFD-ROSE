#ifndef COMPASS_GUI_H
#define COMPASS_GUI_H
#include "rose.h"

#include <qrose.h>
#include <QTextBrowser>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>

#include <boost/smart_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <stdint.h>
#include "BinQSupport.h"

template <typename T>
class scoped_array_with_size {
  boost::scoped_array<T> sa;
  size_t theSize;

  public:
  scoped_array_with_size(): sa(), theSize(0) {}
  scoped_array_with_size(size_t s): sa(new T[s]), theSize(s) {}

  void allocate(size_t s) {
    sa.reset(new T[s]);
    theSize = s;
  }
  size_t size() const {return theSize;}
  T* get() const {return sa.get();}

  T& operator[](size_t i) {return sa[i];}
  const T& operator[](size_t i) const {return sa[i];}

  private:
  scoped_array_with_size(const scoped_array_with_size<T>&); // Not copyable
};



struct Element {
  uint64_t size;
  uint64_t function_A ;
  uint64_t function_B ;
  uint64_t begin_index_within_function_A ;
  uint64_t end_index_within_function_A   ;
  uint64_t begin_index_within_function_B ;
  uint64_t end_index_within_function_B   ;
  std::string file_A;
  std::string function_name_A;
  std::string file_B;
  std::string function_name_B;

};


class BinQGUI
{
  public:
    BinQGUI(std::string, std::string );
    ~BinQGUI();
    void run( ) ;
    void open();
    void reset();
    void highlightFunctionRow(int);
    void unhighlightFunctionRow(int);
    void highlightInstructionRow(int);
    void unhighlightInstructionRow(int);

    void showFileA(int row);
    //SgNode* disassembleFile(std::string tsv_directory);
    //std::string normalizeInstructionsToHTML(std::vector<SgAsmx86Instruction*>::iterator beg, 
    //std::vector<SgAsmx86Instruction*>::iterator end);

    //std::pair<std::string,std::string> getAddressFromVectorsTable(uint64_t function_id, uint64_t index);

    void selectView(int selection);

  protected:
    qrs::QRWindow *window;
    qrs::QRTable *tableWidget;
    qrs::QRTable *codeTableWidget;
    //QTextEdit *codeWidget;
    QTextEdit *codeWidget2;
    QComboBox *comboBox;
    QComboBox *wholeFunction;

    QTextBrowser *codeBrowser;
    QLineEdit *smallerThanRestriction;
    QLineEdit *largerThanRestriction;
    //    QComboBox *checkBoxLockBars;
  private:

    std::vector<SgAsmFunctionDeclaration*> funcs;
    //    std::vector<SgAsmx86Instruction*> insns;
    std::vector<SgAsmStatement*> stmts;

    BinQSupport* binqsupport;
    
    std::string fileNameA,fileNameB;
    SgNode* fileA;
    SgNode* fileB;

    double similarity;
    int stride;
    int windowSize;
    int activeFunctionRow;
    int activeInstructionRow;


    scoped_array_with_size<Element > vectorOfClones;
    std::pair<std::string,std::string> normalizedView;
    std::pair<std::string,std::string> unparsedView;
    std::pair<std::string,std::string> allInsnsUnparsedView;
   
}; //class BinQGUI

#endif
