#include "jaspJson.h"


std::string jaspJson::jsonToPrefixedStrings(Json::Value val, std::string prefix) const
{
	if(prefix == "")
		return val.toStyledString();

	std::string styled = val.toStyledString();

	std::stringstream out;

	for(char letter : styled)
		if(letter != '\n')
			out << letter;
		else
			out << letter << prefix;

	return out.str();
}

Json::Value jaspJson::dataEntry(std::string & errorMessage) const
{
	Json::Value data(jaspObject::dataEntry(errorMessage));

	data["title"]		= _title;
	data["json"]		= _json;
	data["name"]		= getUniqueNestedName();

	return data;
}

Json::Value jaspJson::convertToJSON() const
{
	Json::Value obj = jaspObject::convertToJSON();
	obj["json"]		= _json;

	return obj;
}

void jaspJson::convertFromJSON_SetFields(Json::Value in)
{
	jaspObject::convertFromJSON_SetFields(in);

	_json = in.get("json", Json::nullValue);
}
