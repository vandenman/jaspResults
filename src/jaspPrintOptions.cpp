#include "jaspPrintOptions.h"
#include <cstddef>
#include <Rcpp.h>


/*
 *		Two alternatives:
 *
 *		1. Share the environment/ list  .jaspPrintOptions directly between R and C++
 *			pros: relatively easy, no duplicate options in R/ C++
 *			cons: lose type safety (e.g., we'd have to do stuff like static_cast<bool>(lst[["Key"]]);
 *
 *		2. Make another Rcpp Modules class
 *
 */

// [[Rcpp::export]]
void jaspPrintOptionsSetPrintDevInfo	(bool value)		{			jaspPrintOptions::GetInstance()._printDevInfo = value;		}
// [[Rcpp::export]]
bool jaspPrintOptionsGetPrintDevInfo	()					{ return	jaspPrintOptions::GetInstance()._printDevInfo;				}

// [[Rcpp::export]]
void jaspPrintOptionsSetVisibleTableColSep	(bool value)	{	jaspPrintOptions::GetInstance()._visibleColSep = value;				}
// [[Rcpp::export]]
bool jaspPrintOptionsGetVisibleTableColSep	()				{	return jaspPrintOptions::GetInstance()._visibleColSep;				}

// [[Rcpp::export]]
void jaspPrintOptionsSetIndentSize		(int value)			{	jaspPrintOptions::GetInstance().setIndentSize(value);				}
// [[Rcpp::export]]
std::size_t jaspPrintOptionsGetIndentSize	()				{	return jaspPrintOptions::GetInstance().getIndentSize();				}

// [[Rcpp::export]]
void jaspPrintOptionsSetIndentWithTabs	(bool value)		{	return jaspPrintOptions::GetInstance().setIndentWithTabs(value);	}
// [[Rcpp::export]]
bool jaspPrintOptionsGetIndentWithTabs	()					{	return jaspPrintOptions::GetInstance().getIndentWithTabs();			}

// [[Rcpp::export]]
void jaspPrintOptionsSetUseUnicode	(bool value)			{	jaspPrintOptions::GetInstance()._useUnicode = value;				}
// [[Rcpp::export]]
bool jaspPrintOptionsGetUseUnicode	()						{	return jaspPrintOptions::GetInstance()._useUnicode;					}



//#define JASPPRINTOPTIONS_PROPERTY_GENERATOR(PROP_TYPE_CPP, PROP_TYPE_R, PROP_NAME, PROP_CAPITALIZED_NAME) \
//	void        jaspPrintOptions ## Set ## PROP_CAPITALIZED_NAME (PROP_TYPE_R value) { jaspPrintOptions::GetInstance().PROP_NAME = value; } \
//	PROP_TYPE_R jaspPrintOptions ## Get ## PROP_CAPITALIZED_NAME ()                  { return jaspPrintOptions::GetInstance().PROP_NAME; }

//JASPPRINTOPTIONS_PROPERTY_GENERATOR(bool, bool, _printDevInfo, PrintDevInfo)

//#define JASPPRINTOPTIONS_PROPERTY_GENERATOR(PROP_TYPE_CPP, PROP_TYPE_R, PROP_NAME, PROP_CAPITALIZED_NAME) \
//	Rcpp::RcppExport SEXP jaspPrintOptions2 ## Set ## PROP_CAPITALIZED_NAME (SEXP valueSEXP) { Rcpp::traits::input_parameter< PROP_TYPE_CPP >::type value(valueSEXP); jaspPrintOptions::GetInstance().PROP_NAME = value; return R_NilValue; } \
//	Rcpp::RcppExport SEXP jaspPrintOptions2 ## Get ## PROP_CAPITALIZED_NAME ()               { return Rcpp::wrap(jaspPrintOptions::GetInstance().PROP_NAME); }

//JASPPRINTOPTIONS_PROPERTY_GENERATOR(bool, bool, _printDevInfo, PrintDevInfo)
