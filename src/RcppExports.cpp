// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// jaspPrintOptionsSetPrintDevInfo
void jaspPrintOptionsSetPrintDevInfo(bool value);
RcppExport SEXP _jaspResults_jaspPrintOptionsSetPrintDevInfo(SEXP valueSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< bool >::type value(valueSEXP);
    jaspPrintOptionsSetPrintDevInfo(value);
    return R_NilValue;
END_RCPP
}
// jaspPrintOptionsGetPrintDevInfo
bool jaspPrintOptionsGetPrintDevInfo();
RcppExport SEXP _jaspResults_jaspPrintOptionsGetPrintDevInfo() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(jaspPrintOptionsGetPrintDevInfo());
    return rcpp_result_gen;
END_RCPP
}
// jaspPrintOptionsVisibleTableColSep
void jaspPrintOptionsVisibleTableColSep(bool value);
RcppExport SEXP _jaspResults_jaspPrintOptionsVisibleTableColSep(SEXP valueSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< bool >::type value(valueSEXP);
    jaspPrintOptionsVisibleTableColSep(value);
    return R_NilValue;
END_RCPP
}

RcppExport SEXP _rcpp_module_boot_jaspResults();

static const R_CallMethodDef CallEntries[] = {
    {"_jaspResults_jaspPrintOptionsSetPrintDevInfo", (DL_FUNC) &_jaspResults_jaspPrintOptionsSetPrintDevInfo, 1},
    {"_jaspResults_jaspPrintOptionsGetPrintDevInfo", (DL_FUNC) &_jaspResults_jaspPrintOptionsGetPrintDevInfo, 0},
    {"_jaspResults_jaspPrintOptionsVisibleTableColSep", (DL_FUNC) &_jaspResults_jaspPrintOptionsVisibleTableColSep, 1},
    {"_rcpp_module_boot_jaspResults", (DL_FUNC) &_rcpp_module_boot_jaspResults, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_jaspResults(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
