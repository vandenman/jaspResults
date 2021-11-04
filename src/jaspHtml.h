#pragma once
#include "jaspObject.h"

class jaspHtml : public jaspObject
{
public:
  jaspHtml(Rcpp::String text = "", std::string elementType = "p", std::string maxWidth="15cm", std::string Class = "") : jaspObject(jaspObjectType::html, ""), _rawText(jaspNativeToUtf8(text)), _elementType(elementType), _class(Class), _maxWidth(maxWidth) {}

	~jaspHtml() {}

	std::string dataToString(std::string prefix="")			const	override;
	std::string toHtml()											override;

	Json::Value	metaEntry()									const	override { return constructMetaEntry("htmlNode"); }
	Json::Value	dataEntry(std::string & errorMessage)		const	override;

	std::string _rawText, _elementType, _class, _maxWidth;

	Json::Value convertToJSON()								const	override;
	void		convertFromJSON_SetFields(Json::Value in)			override;

	std::string convertTextToHtml(const std::string text)	const;

    void setText(std::string newRawText);
	void setTextFromR(std::string newRawText)	{ setText(jaspNativeToUtf8(newRawText)); };
    std::string getText();
    std::string getHtml();
};

RCPP_EXPOSED_CLASS_NODECL(jaspHtml)
