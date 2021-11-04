#pragma once
#include "jaspObject.h"

class jaspState : public jaspObject
{
public:
	jaspState(Rcpp::String title = "") : jaspObject(jaspObjectType::state, title) { initEnvName(); }

	void			setObject(Rcpp::RObject obj);
	Rcpp::RObject	getObject();

	Json::Value		convertToJSON()								const	override;
	void			convertFromJSON_SetFields(Json::Value in)			override;
	std::string		dataToString(std::string prefix)			const	override;
	std::string		_envName;

private:
	void initEnvName();
};

RCPP_EXPOSED_CLASS_NODECL(jaspState)
