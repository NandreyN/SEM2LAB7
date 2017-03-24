#pragma once 

#ifdef REVNOTATION_EXPORTS  
#define REVNOTATION_API __declspec(dllexport)   
#else  
#define REVNOTATION_API __declspec(dllimport)   
#endif  

#include <string>
#include <stack>
using namespace std;

namespace MathLibrary
{
	// This class is exported from the MathLibrary.dll  
	class Functions
	{
	public:
		// Returns a + b  
		static REVNOTATION_API int Add(double a, double b);		
	};
}

