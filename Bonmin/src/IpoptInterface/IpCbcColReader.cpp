// (C) Copyright Carnegie Mellon University 2005
// All Rights Reserved.
// This code is published under the Common Public License.
//
// Authors :
// Pierre Bonami, Carnegie Mellon University,
//
// Date : 26/05/2005
#include "IpCbcColReader.hpp"
#include <fstream>
#include <iostream>
/** Keywords of the .int file */
static std::string keywords[]= { "priorities", "PRIORITIES",
    "special_ordered_set_1",
    "SPECIAL_ORDERED_SET_1"};
IpCbcColReader::IpCbcColReader(const char * fileName)
    :
    fileName_(), varIndices_(), varNames_()
{
  assert(fileName != NULL);
  fileName_=fileName;
}
IpCbcColReader::IpCbcColReader(const std::string & fileName)
    :
    fileName_(fileName), varIndices_(), varNames_()
{}

bool IpCbcColReader::readFile()
{
  std::string colFileName = fileName_;
  int size = colFileName.size();
  bool hasNlExtension =  (colFileName.size()>4) && (colFileName[size - 1] =='l') && (colFileName[size - 2] =='n') && (colFileName[size - 3] =='.');
  if(hasNlExtension)
    colFileName.erase(size-3,3);
  colFileName+=".col";
  std::ifstream inFile(colFileName.c_str());
  if(!inFile.is_open()) {
    std::cerr<<"Error in opening Names file (probably does not exists)";
    return false;
  }
  std::string name;
  int nVar = 0;
  do {
    name="";
    inFile>>name;
    if(name.size()==0)
      continue;
    varNames_.push_back(name);
    varIndices_[varNames_[nVar].c_str()] = nVar;
    nVar++;
  }
  while(!inFile.eof());

  //  varNames_ = new std::string[nVar];
  for(int i = 0 ; i < nVar ; i++) {
    assert(i==varIndices_ [ varNames_ [i].c_str()]);
  }
  return true;
}

void
IpCbcColReader::copyNames(std::string *varNames, int n_var)
{
  assert(n_var >= 0 && (unsigned int) n_var==varNames_.size());
  for(int i = 0 ; i < n_var ; i++) {
    varNames[i] = varNames_[i];
  }
}
