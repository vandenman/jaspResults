#ifndef _JASPCOLUMN_HEADER
#define _JASPCOLUMN_HEADER

#include "jaspObject.h"

class jaspColumn : public jaspObject
{
public:
	jaspColumn(Rcpp::String columnName="");


	Json::Value		convertToJSON()								const	override;
	void			convertFromJSON_SetFields(Json::Value in)			override;
	std::string		dataToString(std::string prefix)			const	override;

	Json::Value	metaEntry()										const	override { return constructMetaEntry("column"); }
	Json::Value	dataEntry(std::string & errorMessage)			const	override;


	void setScale(		Rcpp::RObject scalarData);
	void setOrdinal(	Rcpp::RObject ordinalData);
	void setNominal(	Rcpp::RObject nominalData);
	void setNominalText(Rcpp::RObject nominalData);

private:
	std::string		_columnName		= "";
	bool			_dataChanged	= false,
					_typeChanged	= false;
	jaspColumnType	_columnType		= jaspColumnType::unknown;
};


RCPP_EXPOSED_CLASS_NODECL(jaspColumn)

#endif
