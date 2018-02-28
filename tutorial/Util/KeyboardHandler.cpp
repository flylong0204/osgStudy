#include "KeyboardHandler.h"
#include <iostream>
using namespace std;

bool keyboardEventHandler::addFunction(int whatKey, functionType newFunction)
{
   if ( keyFuncMap.end() != keyFuncMap.find( whatKey ))
   {
      std::cout << "duplicate key '" << whatKey << "' ignored." << std::endl;
      return false;
   }
   else
   {
      keyFuncMap[whatKey].keyFunction = newFunction;
      return true;
   }
}

bool keyboardEventHandler::addFunction (int whatKey, keyStatusType keyPressStatus, functionType newFunction)
{
   if (keyPressStatus == KEY_DOWN)
   {
	//   std::string str="addFuntion:  "+whatKey+" KEY_DOWN ";
	//   std::cout<<str.c_str()<<std::endl;
       return addFunction(whatKey,newFunction);
   }
   else
   {
      if ( keyUPFuncMap.end() != keyUPFuncMap.find( whatKey )) 
      {
          std::cout << "duplicate key '" << whatKey << "' ignored." << std::endl;
          return false;
      }
      else
      {
//		  string str="addFuction: "+whatKey+ " KEY_UP ";
//		  cout<<str.c_str()<<endl;
          keyUPFuncMap[whatKey].keyFunction = newFunction;
          return true;
      }
   } // KEY_UP
}

bool keyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
   bool newKeyDownEvent = false;
   bool newKeyUpEvent   = false;
   
   //cout<<"\n************************************************************\n";
   //cout<<ea.getKey()<<endl;

   switch(ea.getEventType())
   {
   case(osgGA::GUIEventAdapter::KEYDOWN):
      {
         keyFunctionMap::iterator itr = keyFuncMap.find(ea.getKey());
         if (itr != keyFuncMap.end())
         {
            if ( (*itr).second.keyState == KEY_UP )
            {
                (*itr).second.keyState = KEY_DOWN;
                newKeyDownEvent = true;
				(*itr).second.keyFunction();
				newKeyDownEvent = false;
			
            }           
            return true;
         }
 

		 /*if (itr != keyFuncMap.end()) 
		 {
			(*itr).second.keyFunction();
			return true;
		 }*/

         return false;
      }
   case(osgGA::GUIEventAdapter::KEYUP):
      {
         keyFunctionMap::iterator itr = keyFuncMap.find(ea.getKey());
         if (itr != keyFuncMap.end() )
         {
		 //	 cout<<"KEYUP´¦Àí:"
             (*itr).second.keyState = KEY_UP;
         }

         itr = keyUPFuncMap.find(ea.getKey());
         if (itr != keyUPFuncMap.end())
         {
             (*itr).second.keyFunction();
             return true;
         }
         return false; 
      }
   default:
      return false;
   }
}
