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
#include "parser.h"

namespace RPN
{
	const unsigned char Parser::cInitial[256] = {
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		BLANK,
		BLANK,
		0,
		0,
		BLANK,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		BLANK,
		OPERATOR,
		IDENTIFIER,
		OPERATOR,
		OPERATOR,
		OPERATOR,
		OPERATOR,
		IDENTIFIER,
		BRACKET,
		BRACKET,
		OPERATOR,
		OPERATOR,
		OPERATOR,
		OPERATOR,
		NUMBER,
		OPERATOR,
		NUMBER,
		NUMBER,
		NUMBER,
		NUMBER,
		NUMBER,
		NUMBER,
		NUMBER,
		NUMBER,
		NUMBER,
		NUMBER,
		OPERATOR,
		OPERATOR,
		0,
		0,
		0,
		OPERATOR,
		OPERATOR,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		BRACKET,
		OPERATOR,
		BRACKET,
		OPERATOR,
		IDENTIFIER,
		0,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		BRACKET,
		OPERATOR,
		BRACKET,
		OPERATOR,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};
	
	const unsigned char Parser::cSubsequent[256] = {
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		IDENTIFIER,
		0,
		0,
		0,
		0,
		IDENTIFIER,
		0,
		0,
		0,
		0,
		0,
		0,
		NUMBER,
		0,
		NUMBER | IDENTIFIER,
		NUMBER | IDENTIFIER,
		NUMBER | IDENTIFIER,
		NUMBER | IDENTIFIER,
		NUMBER | IDENTIFIER,
		NUMBER | IDENTIFIER,
		NUMBER | IDENTIFIER,
		NUMBER | IDENTIFIER,
		NUMBER | IDENTIFIER,
		NUMBER | IDENTIFIER,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		0,
		0,
		0,
		0,
		IDENTIFIER,
		0,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		IDENTIFIER,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};
}
