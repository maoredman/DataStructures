/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{
   cout << setw(15) << left << "MTReset: "
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
  vector<string> tokens;
  if (!CmdExec::lexOptions(option, tokens))
    return CMD_EXEC_ERROR;
  if (tokens.size() == 0) {
     return CmdExec::errorOption(CMD_OPT_MISSING, "");
  }
  if(tokens.size() > 3)
      return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[3]);


  if(!myStrNCmp("-Array", tokens[0], 2)){
    if(tokens.size() == 1)
      return CmdExec::errorOption(CMD_OPT_MISSING, tokens[0]);

    int array_size;
    if(!myStr2Int(tokens[1], array_size) || array_size <= 0){
      cerr << "Illegal array size (" << tokens[2] << ")!!" << endl;
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
    }

    if(tokens.size() == 2)
      return CmdExec::errorOption(CMD_OPT_MISSING, tokens[1]);

    int num_objects;
    if (!myStr2Int(tokens[2], num_objects) || num_objects < 0){
       cerr << "Illegal number of objects (" << tokens[2] << ")!!" << endl;
       return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
    }

    mtest.newArrs(num_objects, array_size);
  }

  else{
    int num_objects;
    if (!myStr2Int(tokens[0], num_objects) || num_objects < 0){
       cerr << "Illegal number of objects (" << tokens[0] << ")!!" << endl;
       return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
    }

    if(tokens.size()==1){
      //new Object for num_objects times
      mtest.newObjs(num_objects);
    }

    else if(tokens.size() > 1){

      //tokens.size() is now 2 or 3
      if(!myStrNCmp("-Array", tokens[1], 2)){ //2nd token is -a

        if(tokens.size() == 2)
          return CmdExec::errorOption(CMD_OPT_MISSING,tokens[1]);

        int array_size;
        if(!myStr2Int(tokens[2], array_size) || array_size <= 0){
          cerr << "Illegal array size (" << tokens[2] << ")!!" << endl;
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
        }
        //new ObjectArray for num_objects times
        mtest.newArrs(num_objects, array_size);
        //Allocate "n" number of MemTestObj arrays with size "s"
      }

      else{
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
      }

    }
  }

  return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{
   cout << setw(15) << left << "MTNew: "
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO

  vector<string> tokens;
  if (!CmdExec::lexOptions(option, tokens))
    return CMD_EXEC_ERROR;

  if (tokens.size() == 0) {
     return CmdExec::errorOption(CMD_OPT_MISSING, "");
  }
  if(tokens.size() > 3)
    return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[3]);
  //3 >= tokens.size() >= 1

  if(!myStrNCmp("-Array", tokens[0], 2)){

    if(tokens.size() == 1)
      return CmdExec::errorOption(CMD_OPT_MISSING,tokens[0]);

    if(!myStrNCmp("-Index", tokens[1], 2)){
      if(tokens.size() == 2)
        return CmdExec::errorOption(CMD_OPT_MISSING,tokens[1]);

      if(mtest.getArrListSize() == 0){
        cerr << "size of array list is 0!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
      }

      int objId;
      if(!myStr2Int(tokens[2], objId) || objId < 0 || objId > mtest.getArrListSize() - 1)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);

      mtest.deleteArr(objId);
    }
    else if(!myStrNCmp("-Random", tokens[1], 2)){
      if(tokens.size() == 2)
        return CmdExec::errorOption(CMD_OPT_MISSING,tokens[1]);

      if(mtest.getArrListSize() == 0){
        cerr << "size of array list is 0!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
      }

      int numRandId;
      if(!myStr2Int(tokens[2], numRandId) || numRandId < 0)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
      //cout<<"about to rnGen!"<<endl;

      int gen_int;
      int range = mtest.getArrListSize() - 1;
      for(size_t i = 0; i < numRandId; ++i){
        //cout<<range<<endl;
        gen_int = rnGen(range);
        //cout<<gen_int<<endl;
        mtest.deleteArr(gen_int); //deleteObj() auto doesn't do anything if already deleted
      }
    }
    else
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
  }
  else if(!myStrNCmp("-Index", tokens[0], 2)){
    //cout << "index detected!" << endl;
    if(tokens.size() == 1)
      return CmdExec::errorOption(CMD_OPT_MISSING,tokens[0]);

    int objId; //tokens.size() >= 2
    if(!myStr2Int(tokens[1], objId) || objId < 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);

    if(tokens.size() == 2){
      //vector.size() is unsigned, and unsigned(-1) is a big integer (2's complement)!
      if(objId > int(mtest.getObjListSize()) - 1){
        cerr << '"' << tokens[1] << '"' << " is too big to be an _objList index!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
      }
      mtest.deleteObj(objId);
    }
    else{ //tokens.size() == 3
      if(!myStrNCmp("-Array", tokens[2], 2)){
        if(objId > mtest.getArrListSize() - 1){
          cerr << '"' << tokens[1] << '"' << " is too big to be an _arrList index!" << endl;
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
        }
        mtest.deleteArr(objId);
      }
      else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
    }
  }
  else if(!myStrNCmp("-Random", tokens[0], 2)){
    //is random
    if(tokens.size() == 1)
      return CmdExec::errorOption(CMD_OPT_MISSING,tokens[0]);

    int numRandId;
    if(!myStr2Int(tokens[1], numRandId) || numRandId < 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);

    if(tokens.size() == 2){ //delete int(numRandId) objects
      if(mtest.getObjListSize() == 0){
        cerr << "size of object list is 0!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
      }

      int gen_int;
      int range = mtest.getObjListSize() - 1;
      for(size_t i = 0; i < numRandId; ++i){
        gen_int = rnGen(range);
        mtest.deleteObj(gen_int); //deleteObj() auto doesn't do anything if already deleted
      }
    }
    else{ //tokens.size() == 3
      if(!myStrNCmp("-Array", tokens[2], 2)){
        if(mtest.getArrListSize() == 0){
          cerr << "size of array list is 0!" << endl;
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
        }

        int gen_int;
        int range = mtest.getArrListSize() - 1;
        for(size_t i = 0; i < numRandId; ++i){
          gen_int = rnGen(range);
          mtest.deleteArr(gen_int); //deleteObj() auto doesn't do anything if already deleted
        }
      }
      else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
    }
  }
  else
    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);

  return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{
   cout << setw(15) << left << "MTDelete: "
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{
   cout << setw(15) << left << "MTPrint: "
        << "(memory test) print memory manager info" << endl;
}


