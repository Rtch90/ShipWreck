/* Callback function class
 * Borrowed from CodeGuru.com : Modyified by Allanis
 * http://www.codeguru.com/Cpp/Cpp/cpp_mfc/callbacks/article.php/c4129/
 */

#ifndef _CCALLBACK_H_

#define _CCALLBACK_H_

class cCallback {
public:
  virtual void Execute(int ButtonID) const = 0;
};

template <class cInstance>

class TCallback : public cCallback {
public:
  TCallback() { pFunction = 0; }

  typedef void (cInstance::*tFunction)(int Param);
        
  virtual void Execute(int ButtonID) const {
    if (pFunction) (cInst->*pFunction)(ButtonID);
    else printf("ERROR : the callback function has not been defined !!!!");
  }

  void SetCallback (cInstance  *cInstancePointer, 
                    tFunction   pFunctionPointer) {
    cInst     = cInstancePointer;
    pFunction = pFunctionPointer;
  }

private:
  cInstance  *cInst;
  tFunction  pFunction;
};


#endif
 