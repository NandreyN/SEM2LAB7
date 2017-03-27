/******************************************************************************
   Copyright 2012 Kevin Burk
   
   This file is part of RPN.
  
   RPN is free software: you can redistribute it and/or modify it under the
   terms of the GNU General Public License as published by the Free Software
   Foundation, either version 3 of the License, or (at your option) any later
   version.
  
   RPN is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
   details.
  
   You should have received a copy of the GNU General Public License along
   with RPN.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#ifndef RPN_LIBHEADER
	#include <cmath>
#endif

#include "arccosecant.h"

namespace RPN
{
/**
 * Constructor.
 */
	ArcCosecantNode::ArcCosecantNode(): FunctionNode(1)
	{
		//Nothing else to do...
	}
	
/**
 * Pops a value off the stack and returns its arc cosecant.
 * @param evaluator The current evaluation.
 * @return The arc cosecant of the popped value.
 */
	double ArcCosecantNode::evaluate(Evaluator& evaluator) const
	{
		double arg = evaluator.pop();
		return asin(1.0 / arg);
	}
}

