#pragma once
#include "jaspObject.h"
#include <limits>
#include "stringutils.h"

class jaspJson : public jaspObject
{
public:
	jaspJson(Json::Value json = Json::nullValue)	: jaspObject(jaspObjectType::json, ""), _json(json)	{}
	jaspJson(Rcpp::RObject Robj)					: jaspObject(jaspObjectType::json, ""), _json(RObject_to_JsonValue(Robj)) {}

	void		setValue(Rcpp::RObject Robj)	{ _json = RObject_to_JsonValue(Robj); _changed = true;	}
	std::string	getValue()						{ return _json.toStyledString();		}


	std::string dataToString(std::string prefix) const override { return jsonToPrefixedStrings(prefix + "\t"); }

	std::string jsonToPrefixedStrings(std::string prefix = "") const { return jsonToPrefixedStrings(_json, prefix); }
	std::string jsonToPrefixedStrings(Json::Value val, std::string prefix) const;

	 Json::Value RcppVector_to_ArrayJson(Rcpp::RObject obj, bool throwError=true) { return VectorJson_to_ArrayJson(RcppVector_to_VectorJson(obj, throwError)); }


	Json::Value	metaEntry()									const	override { return constructMetaEntry("json"); }
	Json::Value	dataEntry(std::string & errorMessage)		const	override;

	Json::Value convertToJSON()								const	override;
	void		convertFromJSON_SetFields(Json::Value in)			override;

	bool		changed()									const			 { return _changed; }

protected:
	Json::Value _json;
	bool		_changed	= false;
};

class jaspJson_Interface : public jaspObject_Interface
{
public:
	jaspJson_Interface(jaspObject * dataObj) : jaspObject_Interface(dataObj) {}

	void 		setValue(Rcpp::RObject Robj)	{ 			static_cast<jaspJson *>(myJaspObject)->setValue(Robj); }
	std::string geValue()						{ return 	static_cast<jaspJson *>(myJaspObject)->getValue(); }
};

RCPP_EXPOSED_CLASS_NODECL(jaspJson)
