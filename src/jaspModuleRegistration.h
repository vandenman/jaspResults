#include <Rcpp.h>
#include "jaspResults.h"

JASP_OBJECT_CREATOR2(jaspHtml)
JASP_OBJECT_CREATOR2(jaspPlot)
JASP_OBJECT_CREATOR2(jaspTable)
JASP_OBJECT_CREATOR2(jaspState)
JASP_OBJECT_CREATOR2(jaspColumn)
JASP_OBJECT_CREATOR2(jaspContainer)
JASP_OBJECT_CREATOR2(jaspQmlSource)
JASP_OBJECT_CREATOR_ARG2(jaspResults, oldState)

// we should do something better with these.
template <typename T>
void finalizerForR( T* obj)
{
	jaspPrint("R called finalizerForR for jasp object with type: " + obj->type());
};

void finalizerForR( jaspResults* obj)
{
	jaspPrint("R called finalizerForR for jaspResults, now running destroy allocated objects");
//	obj->destroyAllAllocatedObjects();
}



RCPP_MODULE(jaspResults)
{
	JASP_OBJECT_CREATOR_FUNCTIONREGISTRATION(jaspPlot);
	JASP_OBJECT_CREATOR_FUNCTIONREGISTRATION(jaspHtml);
	JASP_OBJECT_CREATOR_FUNCTIONREGISTRATION(jaspTable);
	JASP_OBJECT_CREATOR_FUNCTIONREGISTRATION(jaspState);
	JASP_OBJECT_CREATOR_FUNCTIONREGISTRATION(jaspColumn);
	JASP_OBJECT_CREATOR_FUNCTIONREGISTRATION(jaspResults);
	JASP_OBJECT_CREATOR_FUNCTIONREGISTRATION(jaspContainer);
	JASP_OBJECT_CREATOR_FUNCTIONREGISTRATION(jaspQmlSource);


	Rcpp::function("cpp_startProgressbar",	jaspResults::staticStartProgressbar);
	Rcpp::function("cpp_progressbarTick",	jaspResults::staticProgressbarTick);

	Rcpp::function("destroyAllAllocatedObjects", jaspObject::destroyAllAllocatedObjects);


	const std::string addRowsGeneralDoc =
			"Before the data is added all existing columns will be made the same length by appending null-values. "
			"If the new data contains more columns than currently present empty columns will be added. "
			"Columnnames will be extracted and used to place the data in the correct column, they can be specified through the elementnames of a list, names of a data.frame and colnames of a matrix. "
			"To also set the rownames you can fill pass a characterVector with the desired names in the second argument.";

	const std::string addRowsDoc = "Add rows to the table, where 'rows' is a list (of rows), dataframe or matrix. " + addRowsGeneralDoc;
	const std::string addRowDoc  = "Add a row to the table, where 'rows' is a list (of values) or vector. " + addRowsGeneralDoc;

	Rcpp::class_<jaspObject>("jaspObject")

		.finalizer(&finalizerForR)

		.method("print",		&jaspObject::print,								"Prints the contents of the jaspObject")
		.method("toHtml",		&jaspObject::toHtml,							"gives a string with the contents of the jaspObject nicely formatted as html")
		.method("printHtml",	&jaspObject::printHtml,							"Prints the contents of the jaspObject nicely formatted as html")

		.method("addMessage",	&jaspObject::addMessage,						"Add a message to this object")
		.method("addCitation",	&jaspObject::addCitation,						"Add a citation to this object")

		.field("title",			&jaspObject::_title,							"Set the title of this object")
		.field("info",			&jaspObject::_info,								"Set info aka help MD for this object")
		.field("position",		&jaspObject::_position,							"Set the position of this object in it's container. By default this is at the end in the order of adding. You can specify any other value, they do not need to be next to each other or unique. The rule is: lower values (including negative) are higher in the container and when multiple objects in a container have the same position-value order is derived from adding-order.")

		.property("type",		&jaspObject::type,								"The type of this jaspObject as a string, something like: container, table, plot, json, list, results, html, state")

		.method("setOptionMustBeDependency",		&jaspObject::setOptionMustBeDependency,						"Specifies an option and it's required value, if the analysis is restarted and this option is no longer defined (like that) it will automatically destroy the object. Otherwise it will keep it.")
		.method("setOptionMustContainDependency",	&jaspObject::setOptionMustContainDependency,				"Specifies an option that should define an array and a required value that should be in it, if the analysis is restarted and this option is no longer defined or no longer contains the specified value it will automatically destroy the object. Otherwise it will keep it.")
		.method("dependOnOptions",					&jaspObject::dependOnOptions,								"Will make the object depend on the current values of the options specified in the charactervector.")
		//TODO: this doesn't work!
//		.method("copyDependenciesFromJaspObject",	&jaspObject::copyDependenciesFromJaspObject,				"Will make the object depend on whatever the other jaspObject depends.")
		.method("getError",							&jaspObject::getError, 										"Get the error status of this object.")
		.method("setError",							&jaspObject::setErrorForR, 									"Set an error message on this object that which be shown in JASP. Errors set on jaspContainers or jaspResults are propagated to children, such that the first child shows the error and the others are greyed out.")
	;

	Rcpp::class_<jaspPlot>("jaspPlot")
		.derives<jaspObject>("jaspObject")

		.finalizer(&finalizerForR)

		.field("aspectRatio",		&jaspPlot::_aspectRatio,											"Stores the aspect ratio used to make the plot, will not redraw the plot on change.")
		.field("width",				&jaspPlot::_width,													"Stores the width used to make the plot, will not redraw the plot on change.")
		.field("height",			&jaspPlot::_height,													"Stores the height used to make the plot, will not redraw the plot on change.")
		.field("status",			&jaspPlot::_status,													"Stores the status of the plot, default is complete, set to 'running' if it takes a long time to calculate it.")
		.field("filePathPng",		&jaspPlot::_filePathPng,											"Stores the filepath to the image-file generated by the plot.")

		.property("plotObject",		&jaspPlot::getPlotObject,		&jaspPlot::setPlotObject,			"Stores the plotObj used to generate the graphic, will (should) be stored in a way that is later accesible to saveImage an editImage.")

		.field("editing",			&jaspPlot::_editing,												"If set to true will overwrite current png file when rendering a plot.")
		.field("resizedByUser",		&jaspPlot::_resizedByUser,											"If set to true, a user resized the plot and its width and height may be recycled in future runs of this analysis.")
		.field("revision",			&jaspPlot::_revision,												"return the current revision of the plot.")

	;

	Rcpp::class_<jaspContainer>("jaspContainer")
		.derives<jaspObject>("jaspObject")

		.finalizer(&finalizerForR)

		.property("length",							&jaspContainer::length,									"Returns how many objects are stored in this container.")
		.field("initCollapsed",						&jaspContainer::_initiallyCollapsed,					"If this is set true the container will be collapsed initially.")
		.method( "[[",								&jaspContainer::at,										"Retrieve an object from this container as specified under the fieldname.")
		.method( "[[<-",							&jaspContainer::insert,									"Insert an object into this container under a fieldname, if this object is a jaspObject and without a title it will get the fieldname as title.")
		.method( "findObjectWithUniqueNestedName",	&jaspContainer::findObjectWithUniqueNestedName,			"Find a jasp object from its unique name")
	;

	Rcpp::class_<jaspTable>("jaspTable")
		.derives<jaspObject>("jaspObject")

		.finalizer(&finalizerForR)

		// TODO: Ask Joris why this was implemented as "property with getter + method" instead of "property with getter and setter"
		.property("colNames",					&jaspTable::getColNames,					"List of columnnames, single elements can be get and set here directly through [['']] notation but setting all columnnames at once should be done through setColNames")
		.method("setColNames",					&jaspTable::setColNames,					"Accepts a list of strings to be used as columnnames, if the elements are named they will be accessible later through fieldname.")

		.property("colTypes",					&jaspTable::getColNames,					"List of columntypes, single elements can be get and set here directly through [['']] notation but setting all columntypes at once should be done through setColTypes")
		.method("setColTypes",					&jaspTable::setColTypes,					"Accepts a list of strings to be used as columntypes, if the elements are named they will be accessible later through fieldname.")

		.property("colTitles",					&jaspTable::getColTitles,					"List of columntitles, single elements can be get and set here directly through [['']] notation but setting all columntitles at once should be done through setColTitles.\nIf fieldnames are used to set the title (aka aTable$colTitles[['some text']]) then this will override any columntitle set by index for that specific columnname. What this means is that if the first column is named 'a' and you set both \"colTitles[[0]] <- 'one'\" and \"colTitles[['a']] <- 'two'\" than the first column will have 'two' as its title if it's name is 'a'.")
		.method("setColTitles",					&jaspTable::setColTitles,					"Accepts a list of strings to be used as columntitles, if the elements are named they will be accessible later through fieldname.")

		.property("colOvertitles",				&jaspTable::getColOverTitles,				"List of columnovertitles, single elements can be get and set here directly through [['']] notation but setting all columntitles at once should be done through setColTitles.\nIf fieldnames are used to set the title (aka aTable$colOvertitles[['some text']]) then this will override any columntitle set by index for that specific columnname. What this means is that if the first column is named 'a' and you set both \"colTitles[[0]] <- 'one'\" and \"colTitles[['a']] <- 'two'\" than the first column will have 'two' as its title if it's name is 'a'.")
		.method("setColOvertitles",				&jaspTable::setColOvertitles,				"Accepts a list of strings to be used as columnovertitles, if the elements are named they will be accessible later through fieldname.")

		.property("colFormats",					&jaspTable::getColFormats,					"List of columnformats, single elements can be get and set here directly through [['']] notation but setting all columnformats at once should be done through setColFormats")
		.method("setColFormats",				&jaspTable::setColFormats,					"Accepts a list of strings to be used as columnformats, if the elements are named they will be accessible later through fieldname.")

		.property("colCombines",				&jaspTable::getColCombines,					"List of column combines, single elements can be get and set here directly through [['']] notation but setting all columncombines at once should be done through setColFormats")
		.method("setColCombiness",				&jaspTable::setColCombines,					"Accepts a list of logicals to be used as columncombines, if the elements are named they will be accessible later through fieldname.")

		.property("rowNames",					&jaspTable::getRowNames,					"List of rownames, single elements can be get and set here directly through [['']] notation but setting all rownames at once should be done through setRowNames")
		.method("setRowNames",					&jaspTable::setRowNames,					"Accepts a list of strings to be used as rownames, if the elements are named they will be accessible later through fieldname.")

		.property("rowTitles",					&jaspTable::getRowTitles,					"List of rowtitles, single elements can be get and set here directly through [['']] notation but setting all rowtitles at once should be done through setRowTitles. This will respond in a similar manner to conflicts between an indexed title and a fieldnamed title. See documentation of colTitles.")
		.method("setRowTitles",					&jaspTable::setRowTitles,					"Accepts a list of strings to be used as rowtitles, if the elements are named they will be accessible later through fieldname.")

		.method("addColumnInfoHelper",			&jaspTable::addColumnInfo,					"addColumnInfoHelper(name=NULL, title=NULL, type=NULL, format=NULL, combine=NULL) -> Adds column info, an entry to columName wether you specify it or not and if the others are not NULL then they are set for the column")

		.method("addFootnoteHelper",			&jaspTable::addFootnote,					"addFootnoteHelper(message="", symbol=NULL, column=NULL, row=NULL) === Add a footnote to the table, if column or row is not -1 it will be added to the specified column or row, if both are changed then it will be a footnote on a cell and otherwise it will just be a footnote of the entire table. A symbol may also be specified.")

		.method("setData",						&jaspTable::setData,						"Set the data of the table, this accepts lists, dataframes, matrices and vectors. If any column- or rownames are specified they are set on the object, but only if they aren't set yet. Any one-dimensional data given will be assumed to be a row.")


		.method("addRows",						&jaspTable::addRows,						addRowsDoc.c_str())
		.method("addRows",						&jaspTable::addRowsWithoutNames,			addRowsDoc.c_str())

		.method("addRow",						&jaspTable::addRow,							addRowDoc.c_str())
		.method("addRow",						&jaspTable::addRowWithoutNames,				addRowDoc.c_str())

		//.method("addColumn",					&jaspTable::addColumns,						"Add one or more columns to the object, this class accepts the same datatypes as setdata.  Column- and rownames will be extracted as well but used only if the corresponding names aren't set yet.")
		.method("addColumns",					&jaspTable::addColumns,						"Add one or more columns to the object, this class accepts the same datatypes as setdata.  Column- and rownames will be extracted as well but used only if the corresponding names aren't set yet.")
		// is [[<- === .method("setColumn",					&jaspTable::setColumn,					"setColumn(columnName, columnData): set a vector or a list as a column for columnName.")

		.field("showSpecifiedColumnsOnly",		&jaspTable::_showSpecifiedColumnsOnly,		"If set to true will make only the specified columns (through addColumnInfo etc) show in the results.")

		.field("transposeWithOvertitle",		&jaspTable::_transposeWithOvertitle,		"If set to true in combination with transpose == true it will use the first column of the data as overtitle.")

		.field("transpose",						&jaspTable::_transposeTable,				"If set to true will swap rows and columns in the results.")

		.field("status",						&jaspTable::_status,						"The status of the table, usually (and by default) 'complete'")

		.method( "[[<-",						&jaspTable::setColumn,						"Insert a single column into the table, if a string is used then it will look for an existing column name and set that column with the new data and otherwise will just add it at the end. If it is indexed by integer it will simply set it there.")

		.method("setExpectedSize",				&jaspTable::setExpectedSize,				"Set the expected size of this table to the specified columnCount and rowCount. It will make your table show up, filled with dots, at this size and as you add data the dots will be replaced with it.")
		.method("setExpectedColumns",			&jaspTable::setExpectedColumns,				"Set the expected size of this table to the specified columnCount. It will make your table show up, filled with dots, at this size and as you add data the dots will be replaced with it.")
		.method("setExpectedRows",				&jaspTable::setExpectedRows,				"Set the expected size of this table to the specified rowCount. It will make your table show up, filled with dots, at this size and as you add data the dots will be replaced with it.")
	;

	Rcpp::class_<jaspHtml>("jaspHtml")
		.derives<jaspObject>("jaspObject")
		.finalizer(&finalizerForR)
		.property("text",			&jaspHtml::getText,			&jaspHtml::setText,			"The text of this element")
		.property("html",			&jaspHtml::getHtml,										"The text of this element")
		.field("elementType",		&jaspHtml::_elementType,								"The type of this html element, default is 'p' but other useful values include 'H1', 'h2' etc. If you want to write your own html element completely set this to \"\"")
		.field("class",				&jaspHtml::_class,										"The Css-class of this element, for monospace one could use jasp-code or simply leave it empty.")
		.field("maxWidth",			&jaspHtml::_maxWidth,									"The Css-max-width property. It will be set on a span around your html.")
	;


	Rcpp::class_<jaspState>("jaspState")
		.derives<jaspObject>("jaspObject")
		.finalizer(&finalizerForR)
		.property("object", &jaspState::getObject, &jaspState::setObject, "The object that you might want to keep for the next revision of your analysis.")
	;

	Rcpp::class_<jaspColumn>("jaspColumn")
		.derives<jaspObject>("jaspObject")
		.finalizer(&finalizerForR)
		.method("setScale",				&jaspColumn::setScale,			"Overwrite the contents of the specified column with scalar data.")
		.method("setOrdinal",			&jaspColumn::setOrdinal,		"Overwrite the contents of the specified column with ordinal data.")
		.method("setNominal",			&jaspColumn::setNominal,		"Overwrite the contents of the specified column with nominal data.")
		.method("setNominalText",		&jaspColumn::setNominalText,	"Overwrite the contents of the specified column with nominal text data.")
	;

	Rcpp::class_<jaspResults>("jaspResultsClass")
		.derives<jaspContainer>("jaspContainer")
		.finalizer(&finalizerForR)
		.method("send",						&jaspResults::send,									"Constructs the results/response-json and sends it to Desktop, but only if jaspResults::setSendFunc was called with an appropriate sendFuncDef first.")
		.method("complete",					&jaspResults::complete,								"Constructs the results/response-json and sends it to Desktop but sets status to complete first.")
		.method("setErrorMessage",			&jaspResults::setErrorMessage,						"Sets an errormessage on the results.")
		.method("getPlotObjectsForState",	&jaspResults::getPlotObjectsForState,				"Retrieves all plot objects and stores them in a list with the filePath of the plot as name of the element.")
		.method("getOtherObjectsForState",	&jaspResults::getOtherObjectsForState,				"Retrieves all non-plot objects to store them in state (currently only jaspState objects) . Makes a list with the envName of the object as name of the element.")


		.method("getKeepList",				&jaspResults::getKeepList,							"Builds a list of filenames to keep.")

		.property("status",					&jaspResults::getStatus,
											&jaspResults::setStatus,							"The status of the jaspResults object")

		.field("relativePathKeep",			&jaspResults::_relativePathKeep,					"The relative path to where state is kept")

		.method("getResults",				&jaspResults::getResults,							"Returns the latest version of the results json as a string")

		.method("setOptions",				&jaspResults::setOptions,							"Tells jaspResults which options are currently set, should not be used in an analysis!")
		.method("changeOptions",			&jaspResults::changeOptions,						"Changes the currently set options and removes all objects that depend on the changed options. Mostly useful for unit tests because this we we can simulate re-running the analysis. Should not be used in an analysis!")

		.method("prepareForWriting",		&jaspResults::prepareForWriting,					"Remove seal for writing")
		.method("finishWriting",			&jaspResults::finishWriting,						"Set seal for writing")
		.method("saveResults",				&jaspResults::saveResults,							"save results")

		.method("setCurrentColumnNames",	&jaspResults::setCurrentColumnNamesForR,			"setCurrentColumnNames")
		.method("encodeColumnName",			&jaspResults::encodeColumnName,						"encodeColumnName")
		.method("decodeColumnName",			&jaspResults::decodeColumnName,						"decodeColumnName")
		.method("encodeAllColumnNames",		&jaspResults::encodeAllColumnNames,					"encodeAllColumnNames")
		.method("decodeAllColumnNames",		&jaspResults::decodeAllColumnNames,					"decodeAllColumnNames")
	;

	Rcpp::class_<jaspJson>("jaspJson")
		.derives<jaspJson>("jaspObject")
		.finalizer(&finalizerForR)
		.method("getValue",					&jaspJson::getValue,								"Get json string encoded value")
		.method("setValue",					&jaspJson::setValue,								"Set R object to value")
	;

	Rcpp::class_<jaspQmlSource>("jaspQmlSource")
		.derives<jaspJson>("jaspJson")
		.finalizer(&finalizerForR)
		.property("sourceID",				&jaspQmlSource::sourceID,				&jaspQmlSource::setSourceIDForR)
	;


}


Rcpp::RObject givejaspResultsModule()
{
	/* Should be done through LOAD_RCPP_MODULE actually but there seems to be something wrong with the macro.. So we just copy the underlying code like  https://stackoverflow.com/questions/45344260/load-rcpp-module-seems-not-right */
	Rcpp::Shield<SEXP> __load_module_call__( Rf_lang2( GET_MODULE_SYM, _rcpp_module_boot_jaspResults() ) );
	return Rcpp_eval( __load_module_call__, R_GlobalEnv );
}
