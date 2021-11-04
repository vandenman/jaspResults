#pragma once
#include "jaspObject.h"

class jaspPlot : public jaspObject
{
public:
	jaspPlot(Rcpp::String title = "") : jaspObject(jaspObjectType::plot, title) { initEnvName(); }

	~jaspPlot();

	float		_aspectRatio;
	int			_width,
				_height,
				_revision = 0;
	bool		_editing = false,
				_resizedByUser = false;
	std::string	_filePathPng,
				_status = "waiting",
				_envName;
	Json::Value _editOptions = Json::nullValue;

	///For safekeeping (aka state replacement?)
	void setPlotObject(Rcpp::RObject plotSerialized);
	void renderPlot();
	Rcpp::RObject getPlotObject();

	std::string dataToString(std::string prefix)			const	override;

	Json::Value	metaEntry()									const	override { return constructMetaEntry("image"); }
	Json::Value	dataEntry(std::string & errorMessage)		const	override;
	std::string toHtml()											override;

	Json::Value convertToJSON()								const	override;
	void		convertFromJSON_SetFields(Json::Value in)			override;

	bool		canShowErrorMessage()						const	override { return true; }

	void		complete()	{ if(_status == "running" || _status == "waiting") _status = "complete"; }
	void		letRun()	{ _status = "running"; }

private:
	void initEnvName();
	void setUserPlotChangesFromRStateObject();

	Rcpp::List getOldPlotInfo(Rcpp::List & plotInfo);

	//Rcpp::Vector<RAWSXP> _plotObjSerialized;
};

RCPP_EXPOSED_CLASS_NODECL(jaspPlot)
