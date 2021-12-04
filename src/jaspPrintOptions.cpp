#include "jaspPrintOptions.h"

// [[Rcpp::export]]
void jaspPrintOptionsSetPrintDevInfo	(bool value)	{			jaspPrintOptions::GetInstance()._printDevInfo = value;	}
// [[Rcpp::export]]
bool jaspPrintOptionsGetPrintDevInfo	()				{ return	jaspPrintOptions::GetInstance()._printDevInfo;			}



//#define JASPPRINTOPTIONS_PROPERTY_GENERATOR(PROP_TYPE_CPP, PROP_TYPE_R, PROP_NAME, PROP_CAPITALIZED_NAME) \
//	void        jaspPrintOptions ## Set ## PROP_CAPITALIZED_NAME (PROP_TYPE_R value) { jaspPrintOptions::GetInstance().PROP_NAME = value; } \
//	PROP_TYPE_R jaspPrintOptions ## Get ## PROP_CAPITALIZED_NAME ()                  { return jaspPrintOptions::GetInstance().PROP_NAME; }

//JASPPRINTOPTIONS_PROPERTY_GENERATOR(bool, bool, _printDevInfo, PrintDevInfo)

//#define JASPPRINTOPTIONS_PROPERTY_GENERATOR(PROP_TYPE_CPP, PROP_TYPE_R, PROP_NAME, PROP_CAPITALIZED_NAME) \
//	Rcpp::RcppExport SEXP jaspPrintOptions2 ## Set ## PROP_CAPITALIZED_NAME (SEXP valueSEXP) { Rcpp::traits::input_parameter< PROP_TYPE_CPP >::type value(valueSEXP); jaspPrintOptions::GetInstance().PROP_NAME = value; return R_NilValue; } \
//	Rcpp::RcppExport SEXP jaspPrintOptions2 ## Get ## PROP_CAPITALIZED_NAME ()               { return Rcpp::wrap(jaspPrintOptions::GetInstance().PROP_NAME); }

//JASPPRINTOPTIONS_PROPERTY_GENERATOR(bool, bool, _printDevInfo, PrintDevInfo)

// // [[Rcpp::export]]
// void jaspPrintOptionsSetPrintDevInfo	(bool value)	{			jaspPrintOptions::GetInstance()._printDevInfo = value;	}
// // [[Rcpp::export]]
// bool jaspPrintOptionsGetPrintDevInfo	()				{ return	jaspPrintOptions::GetInstance()._printDevInfo;			}



//template<typename charT, typename traits>
//std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& s, const center_helper<charT, traits>& c) {
//	std::streamsize w = s.width();
//	if (w > c.str_.length()) {
//		std::streamsize left = (w + c.str_.length()) / 2;
//		s.width(left);
//		s << c.str_;
//		s.width(w - left);
//		s << "";
//	} else {
//		s << c.str_;
//	}
//	return s;
//}

//// redeclare for std::string directly so we can support anything that implicitly converts to std::string
//center_helper<std::string::value_type, std::string::traits_type> centered(const std::string& str) {
//	return center_helper<std::string::value_type, std::string::traits_type>(str);
//}
