#include "jaspPrintOptions.h"
#include <Rcpp.h>

// [[Rcpp::export]]
void jaspPrintOptionsSetPrintDevInfo	(bool value)	{			jaspPrintOptions::GetInstance()._printDevInfo = value;	}
// [[Rcpp::export]]
bool jaspPrintOptionsGetPrintDevInfo	()				{ return	jaspPrintOptions::GetInstance()._printDevInfo;			}
// [[Rcpp::export]]
void jaspPrintOptionsVisibleTableColSep	(bool value)	{			jaspPrintOptions::GetInstance()._visibleColSep = value;	}
// [[Rcpp::export]]
void jaspPrintOptionsSetIndentSize		(int value)		{			jaspPrintOptions::GetInstance().setIndentSize(value);	}

//#define JASPPRINTOPTIONS_PROPERTY_GENERATOR(PROP_TYPE_CPP, PROP_TYPE_R, PROP_NAME, PROP_CAPITALIZED_NAME) \
//	void        jaspPrintOptions ## Set ## PROP_CAPITALIZED_NAME (PROP_TYPE_R value) { jaspPrintOptions::GetInstance().PROP_NAME = value; } \
//	PROP_TYPE_R jaspPrintOptions ## Get ## PROP_CAPITALIZED_NAME ()                  { return jaspPrintOptions::GetInstance().PROP_NAME; }

//JASPPRINTOPTIONS_PROPERTY_GENERATOR(bool, bool, _printDevInfo, PrintDevInfo)

//#define JASPPRINTOPTIONS_PROPERTY_GENERATOR(PROP_TYPE_CPP, PROP_TYPE_R, PROP_NAME, PROP_CAPITALIZED_NAME) \
//	Rcpp::RcppExport SEXP jaspPrintOptions2 ## Set ## PROP_CAPITALIZED_NAME (SEXP valueSEXP) { Rcpp::traits::input_parameter< PROP_TYPE_CPP >::type value(valueSEXP); jaspPrintOptions::GetInstance().PROP_NAME = value; return R_NilValue; } \
//	Rcpp::RcppExport SEXP jaspPrintOptions2 ## Get ## PROP_CAPITALIZED_NAME ()               { return Rcpp::wrap(jaspPrintOptions::GetInstance().PROP_NAME); }

//JASPPRINTOPTIONS_PROPERTY_GENERATOR(bool, bool, _printDevInfo, PrintDevInfo)
