/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robFunctionRn_h
#define _robFunctionRn_h

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctDynamicVector.h>

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robFunctionRn : public robFunction {
  
 protected:

  vctDynamicVector<double>   y1;  // initial value
  vctDynamicVector<double>  y1d;  // initial 1st derivative
  vctDynamicVector<double> y1dd;  // initial 2nd derivative

  vctDynamicVector<double>   y2;  // final value
  vctDynamicVector<double>  y2d;  // final 1st derivative
  vctDynamicVector<double> y2dd;  // final 2nd derivative
  
 public:

  robFunctionRn( double t1, double y1, double y1d, double y1dd,
 		 double t2, double y2, double y2d, double y2dd );

  robFunctionRn( double t1, 
		 const vctFixedSizeVector<double,3>& y1, 
		 const vctFixedSizeVector<double,3>& y1d, 
		 const vctFixedSizeVector<double,3>& y1dd, 
		 double t2, 
		 const vctFixedSizeVector<double,3>& y2,
		 const vctFixedSizeVector<double,3>& y2d,
		 const vctFixedSizeVector<double,3>& y2dd );

  robFunctionRn( double t1, 
		 const vctDynamicVector<double>& y1, 
		 const vctDynamicVector<double>& y1d, 
		 const vctDynamicVector<double>& y1dd, 
		 double t2, 
		 const vctDynamicVector<double>& y2,
		 const vctDynamicVector<double>& y2d,
		 const vctDynamicVector<double>& y2dd );

  //robFunction* Concatenate( double t, robFunction* function );

  virtual void Evaluate( double, double&, double&, double& ){}

  virtual void Evaluate( double, 
			 vctFixedSizeVector<double,3>&, 
			 vctFixedSizeVector<double,3>&, 
			 vctFixedSizeVector<double,3>& ){}

  virtual void Evaluate( double, 
			 vctDynamicVector<double>&, 
			 vctDynamicVector<double>&, 
			 vctDynamicVector<double>& ){}
  
  virtual void Blend( double,
		      robFunction*, 
		      const vctDynamicVector<double>&, 
		      const vctDynamicVector<double>& ){}

  virtual void Blend( double, robFunction*, double, double ){}
  

};

#endif
