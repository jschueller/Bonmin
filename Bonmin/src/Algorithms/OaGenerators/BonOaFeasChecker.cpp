// (C) Copyright International Business Machines 2006
// All Rights Reserved.
// This code is published under the Common Public License.
//
// Authors :
// P. Bonami, Carnegie Mellon University
//
// Date : 12/26/2006
//#define OA_DEBUG

#include "BonOaFeasChecker.hpp"
#include "BonminConfig.h"

#include "OsiAuxInfo.hpp"
#include "BonSolverHelp.hpp"

#define SAVE_MEM
namespace Bonmin
{

  /// New usefull constructor
  OaFeasibilityChecker::OaFeasibilityChecker(BabSetupBase &b):
      OaDecompositionBase(b, false, true)
  {}
  OaFeasibilityChecker ::~OaFeasibilityChecker ()
  {}

  /// OaDecomposition method
  double
  OaFeasibilityChecker::performOa(OsiCuts & cs, solverManip &lpManip,
      SubMipSolver * &subMip, BabInfo * babInfo, double &cutoff) const
  {
    bool isInteger = true;
    bool feasible = 1;

    OsiSolverInterface * lp = lpManip.si();
    OsiBranchingInformation info(lp,false);
    //int numcols = lp->getNumCols();
    double milpBound = -COIN_DBL_MAX;
    int numberPasses = 0;
    double * nlpSol =  NULL;
    int numberCutsBefore = cs.sizeRowCuts();
   
    while (isInteger && feasible ) {
      numberPasses++;

      //setup the nlp

      //Fix the variable which have to be fixed, after having saved the bounds
      double * colsol = const_cast<double *>(lp->getColSolution());
      info.solution_ = colsol;
      fixIntegers(*nlp_,info, parameters_.cbcIntegerTolerance_,objects_, nObjects_);


      //Now solve the NLP get the cuts, and intall them in the local LP

      nlp_->resolve();
      if (post_nlp_solve(babInfo, cutoff)) {
        //nlp solved and feasible
        // Update the cutoff
        cutoff = nlp_->getObjValue() *(1 - parameters_.cbcCutoffIncrement_);
        // Update the lp solver cutoff
        lp->setDblParam(OsiDualObjectiveLimit, cutoff);
      }
      // Get the cuts outer approximation at the current point

      nlpSol = const_cast<double *>(nlp_->getColSolution());

      const double * toCut = (parameter().addOnlyViolated_)?
          colsol:NULL;
#ifndef SAVE_MEM
      nlp_->getOuterApproximation(cs, nlpSol, 1, toCut,
                                  parameter().global_);
#else
      nlp_->getBendersCut(cs, parameter().global_);
#endif
      int numberCuts = cs.sizeRowCuts() - numberCutsBefore;
      if (numberCuts > 0)
        installCuts(*lp, cs, numberCuts);

      lp->resolve();
      double objvalue = lp->getObjValue();
      //milpBound = max(milpBound, lp->getObjValue());
      feasible = (lp->isProvenOptimal() &&
          !lp->isDualObjectiveLimitReached() && (objvalue<cutoff)) ;
      //if value of integers are unchanged then we have to get out
      bool changed = true;//if lp is infeasible we don't have to check anything
      isInteger = 0;
      //	  if(!fixed)//fathom on bounds
      //           milpBound = 1e200;
      if (feasible) {
        changed = isDifferentOnIntegers(*nlp_, objects_, nObjects_,
                                        0.1,
                                        nlp_->getColSolution(), lp->getColSolution());
      }
      if (changed) {
        info.solution_ = lp->getColSolution();
        isInteger = integerFeasible(*lp, info, parameters_.cbcIntegerTolerance_,
                                     objects_, nObjects_);
      }
      else {
        isInteger = 0;
        //	  if(!fixed)//fathom on bounds
         milpBound = 1e200;
      }
#ifdef OA_DEBUG
      printf("Obj value after cuts %g %d rows\n",lp->getObjValue(),
          numberCuts) ;
#endif
    }
    int num_cuts_now = cs.sizeRowCuts();
    for(int i = numberCutsBefore ; i < num_cuts_now ; i++){
      cs.rowCut(i).setEffectiveness(99.9e99);
    }
#ifdef OA_DEBUG
    debug_.printEndOfProcedureDebugMessage(cs, true, cutoff, milpBound, isInteger, feasible, std::cout);
    std::cout<<"milpBound found: "<<milpBound<<std::endl;
#endif
    return milpBound;
  }

}/* End namespace Bonmin. */
