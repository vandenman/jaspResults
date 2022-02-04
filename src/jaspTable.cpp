#include "jaspTable.h"
#include "jaspPrintOptions.h"
#include "centertext.h"

std::string jaspColRowCombination::toString()
{
	bool ColumnsNotRows = colNames.size() + colOvertitles.size() > 0;

	std::stringstream out;
	out << "{ " << (ColumnsNotRows ? "col-" : "row-") << "combination with title(" << title << "), name("<<name<<") and " << (ColumnsNotRows ? "cols" : "rows") << ": [";

	out << "[" << (ColumnsNotRows ? colNames.toStyledString() : rowNames.toStyledString());
	out << "], does " << (overwrite? "" : "not ") << "overwrite and does " << (removeSeparator? "" : "not ") << "remove separator.";
	return out.str();
}

size_t jaspTable::lengthFromRObject(Rcpp::RObject rObj)
{
	if(rObj.isNULL())								return 0;
	else if(Rcpp::is<Rcpp::List>(rObj))				return lengthFromList((Rcpp::List)						rObj);
	else if(Rcpp::is<Rcpp::NumericVector>(rObj))	return lengthFromVector<REALSXP>((Rcpp::NumericVector)	rObj);
	else if(Rcpp::is<Rcpp::LogicalVector>(rObj))	return lengthFromVector<LGLSXP>((Rcpp::LogicalVector)	rObj);
	else if(Rcpp::is<Rcpp::IntegerVector>(rObj))	return lengthFromVector<INTSXP>((Rcpp::IntegerVector)	rObj);
	else if(Rcpp::is<Rcpp::StringVector>(rObj))		return lengthFromVector<STRSXP>((Rcpp::StringVector)	rObj);
	else if(Rcpp::is<Rcpp::CharacterVector>(rObj))	return lengthFromVector<STRSXP>((Rcpp::CharacterVector)	rObj);
	else Rf_error("Unexpected type..");

	return 0;

}


void jaspTable::setData(Rcpp::RObject newData)
{
	if(newData.isNULL())
	{
		_data.clear();
		return;
	}

	//Maybe this is overkill?
	if(Rcpp::is<Rcpp::DataFrame>(newData))				setDataFromList(convertFactorsToCharacters((Rcpp::DataFrame)	newData));
	else if(Rcpp::is<Rcpp::List>(newData))				setDataFromList((Rcpp::List)									newData);

	else if(Rcpp::is<Rcpp::NumericMatrix>(newData))		setDataFromMatrix<REALSXP>((Rcpp::NumericMatrix)	newData);
	else if(Rcpp::is<Rcpp::LogicalMatrix>(newData))		setDataFromMatrix<LGLSXP>((Rcpp::LogicalMatrix)		newData);
	else if(Rcpp::is<Rcpp::IntegerMatrix>(newData))		setDataFromMatrix<INTSXP>((Rcpp::IntegerMatrix)		newData);
	else if(Rcpp::is<Rcpp::StringMatrix>(newData))		setDataFromMatrix<STRSXP>((Rcpp::StringMatrix)		newData);
	else if(Rcpp::is<Rcpp::CharacterMatrix>(newData))	setDataFromMatrix<STRSXP>((Rcpp::CharacterMatrix)	newData);

	else if(Rcpp::is<Rcpp::NumericVector>(newData))		setDataFromVector<REALSXP>((Rcpp::NumericVector)	newData);
	else if(Rcpp::is<Rcpp::LogicalVector>(newData))		setDataFromVector<LGLSXP>((Rcpp::LogicalVector)		newData);
	else if(Rcpp::is<Rcpp::IntegerVector>(newData))		setDataFromVector<INTSXP>((Rcpp::IntegerVector)		newData);
	else if(Rcpp::is<Rcpp::StringVector>(newData))		setDataFromVector<STRSXP>((Rcpp::StringVector)		newData);
	else if(Rcpp::is<Rcpp::CharacterVector>(newData))	setDataFromVector<STRSXP>((Rcpp::CharacterVector)	newData);

	else
		Rf_error("Cannot set this kind of data to a jaspTable, it is not understood. Try a list, dataframe, vector or matrix instead.");

	notifyParentOfChanges();
}


void jaspTable::addOrSetColumnInData(std::vector<Json::Value> column, std::string colName)
{
	if(colName == "")
		_data.push_back(column);
	else
	{
		//find the right place to put it based on the name and do so
		int desiredColumnIndex = getDesiredColumnIndexFromNameForColumnAdding(colName);

		if(_data.size() <= desiredColumnIndex)
			_data.resize(desiredColumnIndex + 1); //colNames does this automagically

		_data[desiredColumnIndex]		= column;
		_colNames[desiredColumnIndex]	= colName; //Might overwrite an existing colName
	}
}

int jaspTable::getDesiredColumnIndexFromNameForColumnAdding(std::string colName)
{
	//First we check what the last filled colName is in case we need to add a new column at the end
	int lastFilledColName=-1, lastFilledColumn=-1;

	for(int i=0; i<_colNames.rowCount(); i++)
		if(_colNames[i] != "")
			lastFilledColName = i;

	for(int i=0; i<_data.size(); i++)
		if(_data[i].size() > 0)
			lastFilledColumn = i;

	//we also take max because we also want to make sure it is after the last
	int desiredIndex = std::max(lastFilledColumn, lastFilledColName) + 1;

	//And then check if actually we already had this columnName specified because in that case we can just put all the data there!
	for(int possibleColIndex=0; possibleColIndex<_colNames.rowCount(); possibleColIndex++)
		if(_colNames[possibleColIndex] == colName)
			desiredIndex = possibleColIndex;

	return desiredIndex;
}

int jaspTable::pushbackToColumnInData(std::vector<Json::Value> column, std::string colName, int equalizedColumnsLength, int previouslyAddedUnnamed)
{
	int desiredColumnIndex = getDesiredColumnIndexFromNameForRowAdding(colName, previouslyAddedUnnamed);

	if(desiredColumnIndex >= _colNames.rowCount() || _colNames[desiredColumnIndex] == "")
		previouslyAddedUnnamed++;

	if(_data.size() <= desiredColumnIndex)
		_data.resize(desiredColumnIndex + 1); //colNames does this automagically


	if(_data[desiredColumnIndex].size() < equalizedColumnsLength)
		_data[desiredColumnIndex].resize(equalizedColumnsLength);

	for(auto element : column)
		_data[desiredColumnIndex].push_back(element);

	if(colName != "")
		_colNames[desiredColumnIndex] = colName;

	return previouslyAddedUnnamed;
}

std::stringstream jaspTable::makeLineofSize(const std::string& lineChar, const std::vector<std::vector<std::string>>& vierkant, const std::string& prefix, const int extraSpaceSide, const std::string& spaceBetweenRowNamesAndTableLines) const
{
	std::stringstream line;

	line << prefix << std::string(extraSpaceSide, ' ') << spaceBetweenRowNamesAndTableLines << lineChar;
	for (size_t row=0; row<vierkant[0].size(); row++)
	{
		if (row > 0)
			line << lineChar << lineChar << lineChar;
		for (size_t i=0; i<vierkant[0][row].size(); i++)
			line << lineChar;
	}
	line << lineChar << " \n";
	return line;
}

int jaspTable::getDesiredColumnIndexFromNameForRowAdding(std::string colName, int previouslyAddedUnnamed)
{
	if(colName != "")
		for(int possibleColIndex=0; possibleColIndex<_colNames.rowCount(); possibleColIndex++)
			if(_colNames[possibleColIndex] == colName)
				return possibleColIndex;

	int foundUnnamed = 0;
	for(int col=0; col<_colNames.rowCount() || _data.size(); col++)
		if(_colNames[col] == "")
		{

			if(previouslyAddedUnnamed == foundUnnamed)
				return col;

			foundUnnamed++;
		}

	return std::max(_colNames.rowCount(), _data.size());
}

void jaspTable::setColumn(std::string columnName, Rcpp::RObject column)
{
	int colIndex = getDesiredColumnIndexFromNameForColumnAdding(columnName);

	if(Rcpp::is<Rcpp::NumericVector>(column))			setColumnFromVector<REALSXP>((Rcpp::NumericVector)	column, colIndex);
	else if(Rcpp::is<Rcpp::LogicalVector>(column))		setColumnFromVector<LGLSXP>((Rcpp::LogicalVector)	column, colIndex);
	else if(Rcpp::is<Rcpp::IntegerVector>(column))		setColumnFromVector<INTSXP>((Rcpp::IntegerVector)	column, colIndex);
	else if(Rcpp::is<Rcpp::StringVector>(column))		setColumnFromVector<STRSXP>((Rcpp::StringVector)	column, colIndex);
	else if(Rcpp::is<Rcpp::CharacterVector>(column))	setColumnFromVector<STRSXP>((Rcpp::CharacterVector)	column, colIndex);
	else if(Rcpp::is<Rcpp::List>(column))				setColumnFromList((Rcpp::List)						column,	colIndex);
	else Rf_error("Did not get a vector or list as column..");

	notifyParentOfChanges();
}

void jaspTable::addColumns(Rcpp::RObject newData)
{
	if(newData.isNULL())
		return;

	//Maybe this is overkill?
	if(Rcpp::is<Rcpp::DataFrame>(newData))				addColumnsFromList(convertFactorsToCharacters((Rcpp::DataFrame)	newData));
	else if(Rcpp::is<Rcpp::List>(newData))				addColumnsFromList((Rcpp::List)									newData);

	else if(Rcpp::is<Rcpp::NumericMatrix>(newData))		addColumnsFromMatrix<REALSXP>((Rcpp::NumericMatrix)	newData);
	else if(Rcpp::is<Rcpp::LogicalMatrix>(newData))		addColumnsFromMatrix<LGLSXP>((Rcpp::LogicalMatrix)	newData);
	else if(Rcpp::is<Rcpp::IntegerMatrix>(newData))		addColumnsFromMatrix<INTSXP>((Rcpp::IntegerMatrix)	newData);
	else if(Rcpp::is<Rcpp::StringMatrix>(newData))		addColumnsFromMatrix<STRSXP>((Rcpp::StringMatrix)	newData);
	else if(Rcpp::is<Rcpp::CharacterMatrix>(newData))	addColumnsFromMatrix<STRSXP>((Rcpp::CharacterMatrix)newData);

	else if(Rcpp::is<Rcpp::NumericVector>(newData))		addColumnFromVector<REALSXP>((Rcpp::NumericVector)	newData);
	else if(Rcpp::is<Rcpp::LogicalVector>(newData))		addColumnFromVector<LGLSXP>((Rcpp::LogicalVector)	newData);
	else if(Rcpp::is<Rcpp::IntegerVector>(newData))		addColumnFromVector<INTSXP>((Rcpp::IntegerVector)	newData);
	else if(Rcpp::is<Rcpp::StringVector>(newData))		addColumnFromVector<STRSXP>((Rcpp::StringVector)	newData);
	else if(Rcpp::is<Rcpp::CharacterVector>(newData))	addColumnFromVector<STRSXP>((Rcpp::CharacterVector)	newData);

	else
		Rf_error("Cannot add this kind of data as a column to a jaspTable, it is not understood. Try a list, dataframe, vector or matrix instead.");

	notifyParentOfChanges();
}

void jaspTable::addRows(Rcpp::RObject newData, Rcpp::CharacterVector rowNames)
{
	if(newData.isNULL())
		return;

	//Maybe this is overkill?
	if(Rcpp::is<Rcpp::DataFrame>(newData))				addRowsFromDataFrame((Rcpp::DataFrame)				newData);
	else if(Rcpp::is<Rcpp::List>(newData))				addRowsFromList((Rcpp::List)						newData, rowNames);

	else if(Rcpp::is<Rcpp::NumericMatrix>(newData))		addRowsFromMatrix<REALSXP>((Rcpp::NumericMatrix)	newData, rowNames);
	else if(Rcpp::is<Rcpp::LogicalMatrix>(newData))		addRowsFromMatrix<LGLSXP>((Rcpp::LogicalMatrix)		newData, rowNames);
	else if(Rcpp::is<Rcpp::IntegerMatrix>(newData))		addRowsFromMatrix<INTSXP>((Rcpp::IntegerMatrix)		newData, rowNames);
	else if(Rcpp::is<Rcpp::StringMatrix>(newData))		addRowsFromMatrix<STRSXP>((Rcpp::StringMatrix)		newData, rowNames);
	else if(Rcpp::is<Rcpp::CharacterMatrix>(newData))	addRowsFromMatrix<STRSXP>((Rcpp::CharacterMatrix)	newData, rowNames);

	else
		Rf_error("Cannot add this kind of data as rows to a jaspTable, it is not understood. Try a list, dataframe or matrix instead.");

	notifyParentOfChanges();
}

void jaspTable::addRow(Rcpp::RObject newData, Rcpp::CharacterVector rowName)
{
	if(newData.isNULL())
		return;

	if		(Rcpp::is<Rcpp::List>(newData))				addRowFromList((Rcpp::List)							newData, rowName);

	else if	(Rcpp::is<Rcpp::NumericVector>(newData))	addRowFromVector<REALSXP>((Rcpp::NumericVector)		newData, rowName);
	else if	(Rcpp::is<Rcpp::LogicalVector>(newData))	addRowFromVector<LGLSXP>((Rcpp::LogicalVector)		newData, rowName);
	else if	(Rcpp::is<Rcpp::IntegerVector>(newData))	addRowFromVector<INTSXP>((Rcpp::IntegerVector)		newData, rowName);
	else if	(Rcpp::is<Rcpp::StringVector>(newData))		addRowFromVector<STRSXP>((Rcpp::StringVector)		newData, rowName);
	else if	(Rcpp::is<Rcpp::CharacterVector>(newData))	addRowFromVector<STRSXP>((Rcpp::CharacterVector)	newData, rowName);

	else
		Rf_error("Cannot add this kind of data as a row to a jaspTable, it is not understood. Try a list or vector instead.");

	notifyParentOfChanges();
}

void jaspTable::addRowFromList(Rcpp::List newData, Rcpp::CharacterVector newRowNames)
{
	Rcpp::List newRowList;
	auto shield = new Rcpp::Shield<Rcpp::List>(newRowList);
	newRowList.push_back(newData);
	addRowsFromList(newRowList, newRowNames);
	delete shield;
}

void jaspTable::addRowsFromList(Rcpp::List newData, Rcpp::CharacterVector newRowNames)
{
	int equalizedColumnsLength		= equalizeColumnsLengths(),
		previouslyAddedUnnamedCols	= 0;

	std::vector<std::string> localRowNames = extractElementOrColumnNames(newData);

	for(size_t row=0; row<localRowNames.size(); row++)
		_rowNames[row + equalizedColumnsLength] = localRowNames[row];

	for(size_t row=0; row<newRowNames.size(); row++)
		_rowNames[row + equalizedColumnsLength] = newRowNames[row];

	for(size_t row=0; row<newData.size(); row++)
	{
		Rcpp::RObject rij = (Rcpp::RObject)newData[row];

		std::vector<std::string> localColNames;

		if(Rcpp::is<Rcpp::List>(rij))
			 localColNames = extractElementOrColumnNames<Rcpp::List>(Rcpp::as<Rcpp::List>(rij));

		auto jsonRij = jaspJson::RcppVector_to_VectorJson(rij);

		for(size_t col=0; col<jsonRij.size(); col++)
			previouslyAddedUnnamedCols	= pushbackToColumnInData(std::vector<Json::Value>({jsonRij[col]}), localColNames.size() > col ? localColNames[col] : "", equalizedColumnsLength, previouslyAddedUnnamedCols);

		equalizedColumnsLength = equalizeColumnsLengths();
	}
}

void jaspTable::addColumnsFromList(Rcpp::List newData)
{
	size_t elementLenghts = 0;
	for(int el=0; el<newData.size(); el++)
		elementLenghts = std::max(lengthFromRObject((Rcpp::RObject)newData[el]), elementLenghts);

	if(elementLenghts <= 1 && newData.size() > 1) //each entry is 1 or 0, this must be a single row with columnnames and not a set of rows with rownames..
	{
		Rcpp::List newColList;
		auto shield = new Rcpp::Shield<Rcpp::List>(newColList);
		newColList.push_back(newData);
		addColumnsFromList(newColList);
		delete shield;

		return;
	}

	std::vector<std::string> localColNames = extractElementOrColumnNames(newData);
	extractRowNames(newData, true);

	for(int col=0; col<newData.size(); col++)
		addOrSetColumnInData(jaspJson::RcppVector_to_VectorJson((Rcpp::RObject)newData[col], false), localColNames.size() > col ? localColNames[col] : "");
}

///Logically we must assume that each entry in the list is a single element vector
void jaspTable::setColumnFromList(Rcpp::List column, int colIndex)
{
	std::vector<std::string> localRowNames = extractElementOrColumnNames(column);
	setRowNamesWhereApplicable(localRowNames);

	if(_data.size() <= colIndex)
		_data.resize(colIndex+1);
	_data[colIndex].clear();

	for(int row=0; row<column.size(); row++)
	{
		std::vector<Json::Value> jsonVec = jaspJson::RcppVector_to_VectorJson((Rcpp::RObject)column[row], false);
		_data[colIndex].push_back(jsonVec.size() > 0 ? jsonVec[0u] : Json::nullValue);
	}
}

int jaspTable::equalizeColumnsLengths()
{
	if(_data.size() == 0)
		return 0;

	size_t maximumFoundColumnLength = 0;

	for(auto col : _data)
		maximumFoundColumnLength = std::max(maximumFoundColumnLength, col.size());

	for(auto col : _data)
		for(int row = col.size(); row < maximumFoundColumnLength; row++)
			col.push_back(Json::Value(Json::nullValue));

	return maximumFoundColumnLength;
}

Json::Value jaspTable::getCell(size_t col, size_t row, size_t maxCol, size_t maxRow) const
{
	bool    amIWithinBounds = col < maxCol					&& row < maxRow,
			amIExpected		= col < _expectedColumnCount	&& row < _expectedRowCount;

	if(col < _data.size() && row < _data[col].size())
		return _data[col][row];

	return !amIWithinBounds || !amIExpected ? Json::nullValue : Json::Value(".");
}


std::string	jaspTable::getCellFormatted(size_t col, size_t row, size_t maxCol, size_t maxRow) const
{
	Json::Value val(getCell(col, row, maxCol, maxRow));

	std::string format = "";
	if(_colFormats.containsField(getColName(col)))	format = _colFormats[getColName(col)];
	else if(_colFormats.rowCount() > col)			format = _colFormats[col];

	if(val.isNull())	return "";
	if(val.isString())	return val.asString();
	if(val.isBool())	return val.asBool() ? "true" : "false";

	if(format == "")
	{
		if(val.isInt())			return std::to_string(val.asInt());
		if(val.isDouble())
		{
			std::stringstream out;
			out.unsetf(std::ios_base::floatfield); //is std::defaultfloat for old compilers... RTools Im looking at you
			out << val.asDouble();
			return out.str();
		}
	}

	if(!val.isDouble() && !val.isInt())
		return "";

	//now, format != "" and the value is a number of some sort, lets apply the format!
	double value = val.isDouble() ? val.asDouble() : val.asInt();

	auto formats = stringSplit(format, ';');

	int		decPts	= -1,
			sigFig	= -1;
	double	pVal	= 0;
	bool	round	= false,
			log10	= false,
			prcnt	= false;

	std::string pValOri = "";

	for(auto & f : formats)
		if		(f == "~")		round = true;
		else if	(f == "log10")	log10 = true;
		else if(f == "pc")		prcnt = true;
		else if(f.find(':') != std::string::npos)
		{
			auto fmtval = stringSplit(f, ':');
			try
			{
				if		(fmtval[0] == "dp")	decPts	= std::stoi(fmtval[1]);
				else if	(fmtval[0] == "sf")	sigFig	= std::stoi(fmtval[1]);
				else if	(fmtval[0] == "p" )
				{
					pVal	= std::stod(fmtval[1]);
					pValOri = fmtval[1];
				}
				else
					std::cout << "unknown formatting option '" << fmtval[0] << "'" << std::endl;

			}
			catch(std::invalid_argument & e)	{}
			catch(std::out_of_range & e)		{}
		}
	else
		std::cout << "unknown formatting option '" << f << "'" << std::endl;

	if(log10)
		std::cout << "jaspTable doesnt know what to do with the formatting option 'log10', if you DO know, contact Joris Goosen or your local jaspResults-programmer..." << std::endl;

	if(prcnt)
	{
		if(sigFig > 0)
			prcnt = false;
		else
			value *= 100.0f;
	}

	if(pValOri != "" && pVal > value)
		return "p < " + pValOri;

	std::stringstream out;
	out.unsetf(std::ios_base::floatfield); //is std::defaultfloat for old compilers... RTools Im looking at you

	if(sigFig > 0)			out << std::scientific		<< std::setprecision(sigFig) << value;
	else if(decPts > 0)		out << std::fixed			<< std::setprecision(decPts) << value;
	else if(round)			out << std::round(value);
	else					out << value;

	if(prcnt)
		out << "%";

	return out.str();
}

void jaspTable::calculateMaxColRow(size_t & maxCol, size_t & maxRow) const
{
	maxRow = _expectedRowCount;
	maxCol = 0;

	for(size_t col=0; col<std::max(_colNames.rowCount(), _data.size()); col++)
	{
		if(!_showSpecifiedColumnsOnly || columnSpecified(col))
			maxCol++;

		if(col < _data.size())
			maxRow = std::max(maxRow, _data[col].size());
	}

	maxCol = std::max(maxCol, _expectedColumnCount);
}

std::vector<std::vector<std::string>> jaspTable::dataToRectangularVector(bool normalizeColLengths, bool normalizeRowLengths) const
{
	size_t	maxRow, maxCol;
	calculateMaxColRow(maxCol, maxRow);

	size_t colsSpecified = maxCol;

	if(!_showSpecifiedColumnsOnly && _expectedColumnCount > maxCol)
		maxCol = _expectedColumnCount;

	std::vector<std::vector<std::string>> uit;

	uit.resize(maxCol);
	size_t colDst = 0;
	for(size_t colSrc=0; colSrc< std::max(_data.size(), _expectedColumnCount) && colDst<maxCol; colSrc++)
	{
		if(!_showSpecifiedColumnsOnly || columnSpecified(colSrc) || colsSpecified < _expectedColumnCount)
		{
			uit[colDst].resize(maxRow);

			for(size_t row=0; row<maxRow; row++)
				uit[colDst][row] = getCellFormatted(colSrc, row, maxCol, maxRow);

			colDst++;
		}
	}

	if(normalizeColLengths)
		for(size_t col=0; col<maxCol; col++)
		{
			size_t maxLen = 0;

			for(size_t row=0; row<maxRow; row++)
				maxLen = std::max(uit[col][row].size(), maxLen);

			for(size_t row=0; row<maxRow; row++)
				stringExtend(uit[col][row], maxLen);
		}

	if(normalizeRowLengths)
		for(size_t row=0; row<maxRow; row++)
		{
			size_t maxLen = 0;

			for(size_t col=0; col<maxCol; col++)
				maxLen = std::max(uit[col][row].size(), maxLen);

			for(size_t col=0; col<maxCol; col++)
				stringExtend(uit[col][row], maxLen);
		}

	return uit;
}

std::vector<std::string> jaspTable::getDisplayableColTitles(bool normalizeLengths, bool onlySpecifiedColumns) const
{
	std::vector<std::string> names;
	size_t maxLength = 0;

	for(size_t col=0; col< std::max(_data.size(), _expectedColumnCount); col++)
		if(!onlySpecifiedColumns || columnSpecified(col))
		{
			std::string name		= col < _colNames.rowCount() ? _colNames[col] : "",
						showName	= getColName(col),
						title		= "";



			if(name != "" && _colTitles.containsField(name) && _colTitles[name] != "")	title = _colTitles[name];
			else if(col < _colTitles.rowCount() && _colTitles[col] != "")				title = _colTitles[col];

			if(title != "")
				showName = title;

			maxLength = std::max(showName.size(), maxLength);

			names.push_back(showName);
		}

	if(normalizeLengths)
		for(auto & str : names)
			stringExtend(str, maxLength);

	return names;
}

std::vector<std::string> jaspTable::getDisplayableRowTitles(bool normalizeLengths) const
{
	std::vector<std::string> names;
	size_t	maxLength	= 0,
			rowMax		= _expectedRowCount;

	for(size_t col=0; col<_data.size(); col++)
		rowMax = std::max(rowMax, _data[col].size());

	for(size_t row=0; row<rowMax; row++)
	{
		std::string name		= row < _rowNames.rowCount() ? _rowNames[row] : "",
					showName	= getRowName(row),
					title		= "";

		if(name != "" && _rowTitles.containsField(name) && _rowTitles[name] != "")	title = _rowTitles[name];
		else if(row < _rowTitles.rowCount() &&  _rowTitles[row] != "")				title = _rowTitles[row];

		if(title != "")
			showName = title;

		maxLength = std::max(showName.size(), maxLength);

		names.push_back(showName);
	}

	if(normalizeLengths)
		for(auto & str : names)
			stringExtend(str, maxLength);

	return names;
}

std::vector<std::vector<std::string>> jaspTable::transposeRectangularVector(const std::vector<std::vector<std::string>> & in)
{
	if(in.size() == 0)
		return in;

	std::vector<std::vector<std::string>> uit;

	uit.resize(in[0].size());

	for(auto & vec : uit)
		vec.resize(in.size());

	for(size_t col=0; col<in.size(); col++)
		for(size_t row=0; row<in[col].size(); row++)
			uit[row][col] = in[col][row];

	return uit;
}

std::map<std::string, std::map<size_t, size_t>> jaspTable::getOvertitleRanges(const std::vector<std::string>& names, const std::map<std::string,std::string>& overtitles) const
{
	std::map<std::string, std::map<size_t, size_t>> overtitleSpread;

	for(size_t top=0; top<names.size(); top++)
	{

		std::string trimmedName	= stringRemove(names[top]),
																	 // not [] since that modifies
					overTitle	= overtitles.count(trimmedName) > 0 ? overtitles.at(trimmedName) : "";

		if(overtitleSpread.count(overTitle) == 0)
			overtitleSpread[overTitle][top] = top;
		else
		{
			bool foundIt = false;

			for(size_t begin=0; begin < top; begin++)
				if(overtitleSpread[overTitle].count(begin) > 0 && overtitleSpread[overTitle][begin] == top - 1)
				{
					overtitleSpread[overTitle][begin] = top;
					foundIt = true;
					break;
				}

			if(!foundIt)
				overtitleSpread[overTitle][top] = top;
		}
	}

	return overtitleSpread;
}

std::array<std::string, 6> jaspTable::getRowSpaceHeaders() const
{
	// TODO: 'rowName' could use "\u2193" (down arrow) but it needs a non-unicode alternative
	if (printOpts->_printDevInfo && !_transposeTable)
		return {"overTitle: ", "colTitle: ", "colName: ", "type: ", "format: ", "rowName "};
	else
		return {"", "", "", "", "", ""};
}

std::vector<std::vector<std::string> > jaspTable::getColHeaders(const std::vector<std::string> & topNames) const
{
	std::vector<std::vector<std::string>> colHeaders;
	colHeaders.push_back(topNames);
	if (printOpts->_printDevInfo)
	{
		colHeaders.reserve(4);
		const size_t totalColumns = std::max(_colNames.rowCount(), _expectedColumnCount);

		std::vector<std::string> colNames, colTypes, colFormats;
		colNames.reserve(totalColumns); colTypes.reserve(totalColumns), colFormats.reserve(totalColumns);

		for (size_t col = 0; col < totalColumns; col++)
		{
			std::string		colName		= getColName(col),
							colType		= _colTypes		.containsField(colName)	? _colTypes		[colName]	: (_colTypes[col]	!= "" ? _colTypes[col]		: ""),
							colFormat	= _colFormats	.containsField(colName)	? _colFormats	[colName]	: (_colFormats[col] != "" ? _colFormats[col]	: "");

			colNames.push_back(colName);
			colTypes.push_back(colType);
			colFormats.push_back(colFormat);

		}

		colHeaders.push_back(colNames);
		colHeaders.push_back(colTypes);
		colHeaders.push_back(colFormats);

	}

	return colHeaders;
}

void jaspTable::rectangularDataWithNamesToString(std::stringstream & out, std::string prefix, std::vector<std::vector<std::string>> vierkant, std::vector<std::string> sideNames, std::vector<std::string> topNames, std::map<std::string,std::string> sideOvertitles, std::map<std::string,std::string> topOvertitles) const
{
	if(vierkant.size() == 0)
		return;

	/*
	 *						Layout of a non-transposed table
	 *
	 *	|	<prefix>	|	<sideOverTitleSpace>	|	<sideRowSpace>	|					<sumColSpace>						|
	 *	|				|							|					|														|
	 *	|				|							|					---------------------------------------------------------		<- topLine
	 *	|				|							|	 (overTitle)	|	<topOvertitleSpace>		95% CI						|
	 *	|				|							|					|				-----------------------------			|		<- midLine, but only above colTitles that have an overTitle
	 *	|				|							|	  (colTitle)	|	Model		|	Lower	|	upper		|	p		|
	 *	|				|							|	   (colName)	|	(m)			|	(lower)	|	(upper)		|	(pval)	|
	 *	|				|							|	   (colType)	|	(string)	|	number	|	(number)	|	(pvalue)|
	 *	|				|							|	    (format)	|				|			|	sf:4;dp:3	|			|
	 *	|				|							|					|--------------------------------------------------------		<- midLine
	 *	|				|	(sideOverTitles)		|		 (rowName)	|				|			|				|			|		<- this line is only shown if printOpts->_printDevInfo
	 *	|				|		(overTitle 1)		|		   (row 1)	|	H0			|	0.90	|	1.23		|	<0.05	|
	 *	|				|		(overTitle 2)		|		   (row 2)	|	H1			|	0.95	|	1.23		|	0.8		|
	 *	|				|		(overTitle 3)		|		   (row 3)	|	H3			|	0.80	|	1.23		|	<0.001	|
	 *	|				|							|					---------------------------------------------------------		<- botLine
	 *
	 *
	 *						TODO: Layout of a transposed table
	 *
	 *	|	<prefix>	|	<sideOverTitleSpace>	|	<sideRowSpace>	|				|				|				|					<sumColSpace>		|
	 *	|				|							|					|				|				|				|										|
	 *	|				|							|					|				|				|				-----------------------------------------		<- topLine
	 *	|				|							|	(sideOverTitle)	|				|				|				|				<topOvertitleSpace>		|
	 *	|				|							|					|				|				|				|				-------------------------		<- midLine, but only above colTitles that have an overTitle
	 *	|				|							|					|				|				|	(rowName)	|	row 1		|	row 2	|	row 3	|
	 *	|				|							|					|				|				|				|----------------------------------------		<- midLine
	 *	|				|	(sideOverTitles)		|		(colTitle)	|	(colName)	|	(colType)	|	(format)	|				|			|			|		<- this line is only shown if printOpts->_printDevInfo
	 *	|				|							|		   (row 1)	|		  (m)	|	 (string)	|				|	H0			|	H1		|		H2	|
	 *	|				|		95% CI				|		   (row 2)	|	  (lower)	|	 (number)	|				|	0.9			|	0.95	|	0.80	|
	 *	|				|		95% CI				|		   (row 3)	|	  (upper)	|	 (number)	|				|	1.23		|	1.23	|	1.23	|
	 *	|				|							|			   (p)	|	   (pval)	|	 (pvalue)	|				|	<0.05		|	0.8		|	< 0.001	|
	 *	|				|							|					|				|				|				-----------------------------------------		<- botLine
	 *
	 *
	 *	definitions:
	 *
	 *	<xxx>	: variable "xxx" indicates the amount of whitespace
	 *	(xxx)	: printing of "xxx" is only done if printOpts->_printDevInfo
	 *	---		: horizontal line
	 *	|		: invisible column separator
	 *
	 * sumColSpace is the sum of the vector colSpace, where the elements of colSpace indicate the width of the individual columns.
	 * topOvertitleSpace is a vector which indicates the width of the overTitles.
	 *
	 */

	const std::string colSep = printOpts->getColSep();
	const std::array<std::string, 6> rowSpaceHeaders = getRowSpaceHeaders();
	const std::vector<std::vector<std::string>> colHeaders = getColHeaders(topNames); // colTitle, colName, colType, format


	// determine <sideOverTitleSpace> and sideRowSpace
	size_t	sideOvertitleSpace = 0,
			sideRowSpace = 0;

	if (printOpts->_printDevInfo)
	{

		// determine sideRowSpace
		for(auto & header : rowSpaceHeaders)
			sideRowSpace = std::max(sideRowSpace, header.size());

		for (auto & sideName : sideNames)
			sideRowSpace = std::max(sideRowSpace, sideName.size());

		// add one
		sideRowSpace += 1;

		// determine sideOvertitleSpace
		for(auto & keyval : sideOvertitles)
			sideOvertitleSpace = std::max(sideOvertitleSpace, keyval.second.size());

		sideOvertitleSpace = std::max(sideOvertitleSpace, sizeof("sideOverTitles") - 1);

	}

	// TODO: determine here the maximum widht of the data and use those as start values for colSpace
	std::vector<size_t> colSpace;
	colSpace.reserve(topNames.size());

	for (size_t row = 0; row < topNames.size(); row++)
	{
		colSpace.push_back(topNames[row].size() + 2);
		for (size_t col = 0; col < vierkant.size(); col++)
			colSpace[row] = std::max(colSpace[row], vierkant[0][row].size() + 2);
	}

	std::vector<size_t> topOvertitleSpace;
	std::vector<std::string> topOvertitlesWithEmpties;
	// TODO: neither of these if clauses accounts for the width of the data!
	if (topOvertitles.empty())
	{
		// loop over the topnames and retrieve the corresponding overtitle
		// if the overtitle is unequal to the previous overtitle, we know which columns to use for determining the size.
		size_t topOverTitleSize = 0;	// total width of all colTitles of this overtitle
		for (size_t j = 0; j < topNames.size(); j++)
		{
			// find the maximum size in this column of colTitle, colName, type, format
			size_t headerMaxSize = 0;
			for (const auto & header : colHeaders)
				headerMaxSize = std::max(headerMaxSize, header[j].size());

			colSpace[j] = std::max(colSpace[j], headerMaxSize + 2);
			topOverTitleSize += headerMaxSize;
		}
		topOvertitleSpace.push_back(topOverTitleSize);
		topOvertitlesWithEmpties.push_back("");
	}
	else
	{
		size_t start = 0;
		std::string trimmedName = stringRemove(topNames[0]);
		std::string previousOvertitle = topOvertitles.count(trimmedName) > 0 ? topOvertitles.at(trimmedName) : "";
		// we intentionally loop one past the end here.
		for(size_t i=1; i<topNames.size() + 1; i++)
		{
			std::string overTitle;
			if (i < topNames.size())
			{
				trimmedName	= stringRemove(topNames[i]),
													 // not [] since that may modifies topOvertitles
				overTitle	= topOvertitles.count(trimmedName) > 0 ? topOvertitles.at(trimmedName) : "";
			}
			else
				overTitle = "";

			if (overTitle != previousOvertitle || i == topNames.size())
			{

				size_t columnSize = 0;	// total width of all colTitles of this overtitle
				for (size_t j = start; j < i; j++)
				{
					// find the maximum size in this column of colTitle, colName, type, format
					size_t headerMaxSize = 0;
					for (const auto & header : colHeaders)
						headerMaxSize = std::max(headerMaxSize, header[j].size());

					// add 2 so we always have an empty space left and right of a column
					colSpace[j] = std::max(colSpace[j], headerMaxSize + 2);
					columnSize += colSpace[j];
				}

				size_t topOverTitleSize = std::max(columnSize, previousOvertitle.size() + 2);

//				Rcpp::Rcout << "previousOvertitle " << previousOvertitle << std::endl;
//				Rcpp::Rcout << "topOverTitleSize "  << topOverTitleSize << std::endl;
//				Rcpp::Rcout << "columnSize "        << columnSize << std::endl;

				// now that we now the total width of the columns that belong to an overtitle,
				// we may need to redistribute excess width to the colTitles
				// there should be a better way to do this though
				size_t idx = start;
				while (columnSize < topOverTitleSize)
				{
					colSpace[idx]++;
					columnSize++;
					idx++;
					if (idx >= i)
						idx = start;
				}

				// add the size of colSep to the overTitle
				topOverTitleSize += colSep.size() * (i - start - 1);

				topOvertitleSpace.push_back(topOverTitleSize);
				topOvertitlesWithEmpties.push_back(previousOvertitle);

				start = i;
				previousOvertitle = overTitle;
			}
		}
	}

	size_t sumColSpace = std::accumulate(colSpace.begin(), colSpace.end(), 0);
	if (!colSep.empty()) // only true when debugging
	{
		// add the width of colsep times the number of columns
		sumColSpace += colSep.size() * (colSpace.size() - 1);

	}

//	for (size_t i = 0; i < colSpace.size(); i++)
//		Rcpp::Rcout << "colSpace[" << i << "]: " << colSpace[i] << std::endl;
//	Rcpp::Rcout << "sumColSpace: " << sumColSpace << std::endl;

//	for (size_t i = 0; i < topOvertitleSpace.size(); i++)
//		Rcpp::Rcout << "topOvertitleSpace[" << i << "]: " << topOvertitleSpace[i] << std::endl;
//	Rcpp::Rcout << "sumTopOvertitleSpace: " << std::accumulate(topOvertitleSpace.begin(), topOvertitleSpace.end(), 0) << std::endl;


	// print topLine
	out << colSep << std::setw(prefix.size()) << "" << colSep << std::setw(sideOvertitleSpace) << "" << colSep << std::setw(sideRowSpace) << "" << colSep << repeat(printOpts->topLineChar(), sumColSpace) << colSep << "\n";

	// print overTitles + midLine underneath
	if ((printOpts->_printDevInfo && !_transposeTable) || !topOvertitles.empty())
	{

		out << colSep << std::setw(prefix.size()) << "" << colSep << std::setw(sideOvertitleSpace) << "" << colSep << std::setw(sideRowSpace);

		if (printOpts->_printDevInfo) 	out << "overTitle: ";
		out << colSep;

		// print overTitles
		for (size_t i = 0; i < topOvertitlesWithEmpties.size(); i++)
		{
			const size_t overTitleSize = topOvertitleSpace[i];
			const std::string overTitle = topOvertitlesWithEmpties[i];
			out << std::setw(overTitleSize);
			if (overTitle == "")
				out << "" << colSep;
			else
				out << centered(overTitle) << colSep;
		}
		out << "\n";

		out << colSep << std::setw(prefix.size()) << "" << colSep << std::setw(sideOvertitleSpace) << "" << colSep << std::setw(sideRowSpace) << "" << colSep;

		// print midLine underneath the overTitles
		for (size_t i = 0; i < topOvertitlesWithEmpties.size(); i++)
		{
			const size_t overTitleSize = topOvertitleSpace[i];
			const std::string overTitle = topOvertitlesWithEmpties[i];
			if (overTitle == "")
				out << std::setw(overTitleSize) << "" << colSep;
			else
				out << repeat(printOpts->midLineChar(), overTitleSize) << colSep;
		}
		out << "\n";

	}

	// print colTitles, colNames, types, and formats
	for (size_t i = 0; i < colHeaders.size(); i++)
	{
		out << colSep << std::setw(prefix.size()) << "" << colSep << std::setw(sideOvertitleSpace) << "" << colSep << std::setw(sideRowSpace);

		if (printOpts->_printDevInfo)
			out << std::right << rowSpaceHeaders[i+1];

		out << colSep;
		for (size_t j = 0; j < colSpace.size(); j++)
			out << std::setw(colSpace[j]) << centered(colHeaders[i][j]) << colSep;

		out << "\n";
	}

	// line between column info and data
	out << colSep << std::setw(prefix.size()) << "" << colSep << std::setw(sideOvertitleSpace) << "" << colSep << std::setw(sideRowSpace) << "" << colSep << repeat(printOpts->midLineChar(), sumColSpace) << colSep << "\n";

	if (printOpts->_printDevInfo)
		out << colSep << std::setw(prefix.size()) << "" << colSep << std::setw(sideOvertitleSpace) << centered("rowOverTitles") << colSep << std::setw(sideRowSpace) << rowSpaceHeaders[5] << colSep << std::setw(sumColSpace) << "" << colSep << "\n";

	// now we print the actual contents of the table
	for(size_t col=0; col<vierkant.size(); col++)
	{
		out << colSep << std::setw(prefix.size()) << "" << colSep << std::setw(sideOvertitleSpace);
		if (printOpts->_printDevInfo)
		{
			out << sideOvertitles[sideNames[col]] << colSep << std::setw(sideRowSpace) << sideNames[col] << colSep;
		}
		else
		{
			out << "" << colSep << std::setw(sideRowSpace) << "" << colSep;
		}

		for(size_t row = 0; row < vierkant[col].size(); row++)
			out << std::setw(colSpace[row]) << vierkant[col][row] << colSep;

		out << "\n";
	}

	// bottom line
	out << colSep << std::setw(prefix.size()) << "" << colSep << std::setw(sideOvertitleSpace) << "" << colSep << std::setw(sideRowSpace) << "" << colSep << repeat(printOpts->botLineChar(), sumColSpace) << colSep << "\n";


}

std::string jaspTable::repeat(const std::string & str, size_t num) const
{
	std::string ret;
	ret.reserve(str.size() * num);
	while (num--)
		ret += str;
	return ret;
}

void jaspTable::padRight(std::string & str, const size_t num, const char paddingChar) const
{
	if(num > str.size())
		str.insert(str.size(), num - str.size(), paddingChar);
}


std::map<std::string, std::string> jaspTable::getOvertitlesMap() const
{
	std::map<std::string, std::string> map;

	for(size_t col=0; col<_data.size(); col++)
	{
		std::string colName	 = getColName(col);
		if(_colOvertitles.containsField(colName))
			map[stringRemove(_colTitles[colName])] = _colOvertitles[colName];
	}

	return map;
}

std::string jaspTable::dataToString(std::string prefix) const
{
	std::stringstream out;

	std::vector<std::vector<std::string>>	vierkant = dataToRectangularVector(!_transposeTable, _transposeTable);
	std::vector<std::string>				colNames = getDisplayableColTitles(false, _showSpecifiedColumnsOnly),
											rowNames = getDisplayableRowTitles();

	if (printOpts->_printDevInfo)
		out << prefix << "status: " << _status << "\n";

	if(_error || _errorMessage != "")
	{
		out << prefix;
		if(_error		      ) out << "error: '" << _error << "'";
		if(_errorMessage != "") out << (_error     ? " msg: '" : "errormessage: '") << _errorMessage << "'";
		out << "\n";
	}
	else
	{
		if(_transposeTable) rectangularDataWithNamesToString(out, prefix, vierkant,								colNames, rowNames, getOvertitlesMap(),	{});
		else				rectangularDataWithNamesToString(out, prefix, transposeRectangularVector(vierkant),	rowNames, colNames,	{},						getOvertitlesMap());
	}

	Json::Value footnotes, footnotesMerged;
	_footnotes.convertToJSONOrdered(mapRowNamesToIndices(), mapColNamesToIndices(), footnotes, footnotesMerged);

	if(footnotes.size() > 0)
	{
		out << prefix << "footnotes:   \n";
		for(Json::Value::UInt i=0; i<footnotes.size(); i++)
		{
			std::string sym = footnotes[i]["symbol"].asString() ;
			out << prefix << printOpts->getIndent() << (sym == "" ? "" : "(" + sym  + ") " ) << "'" << footnotes[i]["text"].asString() << "'\n";
		}
	}

	return out.str();
}

std::string jaspTable::toHtml()
{
	std::stringstream out;

	std::vector<std::vector<std::string>>	vierkant = dataToRectangularVector(false, false);
	std::vector<std::string>				colNames = getDisplayableColTitles(false, _showSpecifiedColumnsOnly),
											rowNames = getDisplayableRowTitles(false);
	out		<< "<div class=\"status " << _status << " jaspTable\">\n"
			<< htmlTitle() << "\n";

	if(_error || _errorMessage != "")
	{
		out << "<p class=\"error\">\n";
		if(_error		      ) out << "error: <i>'" << _error << "'</i>";
		if(_errorMessage != "") out << (_error       ? " msg: <i>'" : "errormessage: <i>'") << _errorMessage << "'</i>";
		out << "\n</p>";
	}
	else
	{
		if(_transposeTable) rectangularDataWithNamesToHtml(out, vierkant,								colNames, rowNames, getOvertitlesMap(),	{});
		else				rectangularDataWithNamesToHtml(out, transposeRectangularVector(vierkant),	rowNames, colNames,	{},						getOvertitlesMap());
	}

	Json::Value footnotes, footnotesMerged;
	_footnotes.convertToJSONOrdered(mapRowNamesToIndices(), mapColNamesToIndices(), footnotes, footnotesMerged);

	if(footnotes.size() > 0)
	{
		out << "<h4>footnotes</h4>" "\n" "<ul>";

		for(Json::Value::UInt i=0; i<footnotes.size(); i++)
		{
			std::string sym = footnotes[i]["symbol"].asString() ;
			out << "<li>" << (sym == "" ? "" : "<i>(" + sym  + ")</i> " ) << footnotes[i]["text"].asString() << "</li>" "\n";
		}

		out << "</ul>\n";
	}

	out << "</div>\n";

	return out.str();
}

void jaspTable::rectangularDataWithNamesToHtml(std::stringstream & out, std::vector<std::vector<std::string>> vierkant, std::vector<std::string> sideNames, std::vector<std::string> topNames, std::map<std::string,std::string> sideOvertitles, std::map<std::string,std::string> topOvertitles)
{
	if(vierkant.size() == 0)
	{
		out << "\t<table>empty</table>\n";
		return;
	}

	auto	topOvertitleSpread	= getOvertitleRanges(topNames,	topOvertitles),
			sideOvertitleSpread = getOvertitleRanges(sideNames, sideOvertitles);

	bool	topOvertitlesPresent  = topOvertitleSpread.size()  > (topOvertitleSpread.count("")  > 0 ? 1 : 0),
			sideOvertitlesPresent = sideOvertitleSpread.size() > (sideOvertitleSpread.count("") > 0 ? 1 : 0);

	out << "\t<table>\n";

	if(topOvertitlesPresent)
	{
		out << "\t\t"   "<tr>\n"
			   "\t\t\t" "<th rowspan=\"2\" colspan=\"" << (sideOvertitlesPresent ? 2 : 1) << "\"></th>" "\n";

		for(size_t top=0; top<topNames.size(); top++)
		{
			std::string topName		= topNames[top],
						trimmed		= stringRemove(topName),
						overtitle	= topOvertitles.count(trimmed) > 0 ? topOvertitles[trimmed] : "";

			if(topOvertitleSpread.count(overtitle) > 0 && topOvertitleSpread[overtitle].count(top) > 0)
				out << "\t\t\t" "<th colspan=\"" <<  topOvertitleSpread[overtitle][top] - (top - 1) << "\">" << overtitle << "</th>" "\n";

		}
		out << "\t\t" "</tr>" "\n";
	}

	out << "\t\t" "<tr>\n";
	for(auto & topName : topNames)
			out << "\t\t\t" "<th>" << topName << "</th>" "\n";
	out << "\t\t" "</tr>" "\n";

	//then the actual columns X rows
	for(size_t side=0; side<vierkant.size(); side++)
	{
		out << "\t\t" "<tr>" "\n";

		std::string name		= sideNames[side],
					trimmed		= stringRemove(name),
					overtitle	= topOvertitles.count(trimmed) > 0 ? topOvertitles[trimmed] : "";

		if(sideOvertitlesPresent && sideOvertitleSpread.count(overtitle) > 0 && sideOvertitleSpread[overtitle].count(side) > 0)
			out << "\t\t\t" "<th rowspan=\"" << sideOvertitleSpread[overtitle][side] - (side - 1) << "\">" << overtitle << "</th>" "\n";

		out << "\t\t\t" "<th>" << name << "</th>" "\n";

		for(size_t top=0; top<vierkant[side].size(); top++)
			out << "\t\t\t" "<td>" << vierkant[side][top] << "</td>" "\n";

		out << "\t\t" "</tr>" "\n";
	}

	out << "\t</table>\n";
}

Json::Value footnotesNamespace::tableFields::rowsToJSON() const
{
	return _rows.size() == 0 ? Json::nullValue : jaspJson::SetJson_to_ArrayJson(_rows);
}

Json::Value footnotesNamespace::tableFields::colsToJSON() const
{
	return _cols.size() == 0 ? Json::nullValue : jaspJson::SetJson_to_ArrayJson(_cols);
}

Json::Value footnotes::convertToJSON() const
{
	Json::Value notes(Json::arrayValue);

	for (const auto & textRest : _data)
		for(const auto & symbolRest : textRest.second)
			for(const tableFields & fields : symbolRest.second)
			{
				Json::Value note(Json::objectValue);

				note["text"]	= textRest.first;
				note["symbol"]	= symbolRest.first;
				note["rows"]	= fields.rowsToJSON();
				note["cols"]	= fields.colsToJSON();

				notes.append(note);
			}

	return notes;
}

void footnotes::convertToJSONOrdered(std::map<std::string, size_t> rowNames, std::map<std::string, size_t> colNames, Json::Value & fullList, Json::Value & mergedList) const
{
	fullList	= Json::nullValue;
	mergedList	= Json::nullValue;

	std::vector<Json::Value> notesToOrder;

	for (const auto & textRest : _data)
		for(const auto & symbolRest : textRest.second)
			for(const tableFields & fields : symbolRest.second)
			{
				Json::Value note(Json::objectValue);

				note["text"]	= textRest.first;
				note["symbol"]	= symbolRest.first;
				note["rows"]	= fields.rowsToJSON();
				note["cols"]	= fields.colsToJSON();

				int myOrder = 0;

				if(!(note["rows"].isNull() && note["cols"].isNull()))
				{

					const int maxColOrder = colNames.size() * 2;
					const int maxRowOrder = rowNames.size() * 2;

					auto calculateOrder = [](std::string fieldName, Json::Value & note, const int maxVal, std::map<std::string, size_t> nameToIndex)
					{
						int myOrdering = maxVal;

						if(!note[fieldName].isNull())
						{
							for(const Json::Value & val : note[fieldName])
								if(nameToIndex.count(val.asString()) > 0)
									myOrdering = std::min(static_cast<int>(nameToIndex[val.asString()]), myOrdering);
						}

						if(myOrdering == maxVal)
							myOrdering = -1;

						return myOrdering;
					};

					int		myColOrdering = calculateOrder("cols", note, maxColOrder, colNames),
							myRowOrdering = calculateOrder("rows", note, maxRowOrder, rowNames);

					if(myRowOrdering == -1)
					{
						if(myColOrdering != -1)
							myOrder = myColOrdering + maxColOrder;
					}
					else
					{
						myOrder = myRowOrdering * maxColOrder;

						if(myColOrdering != -1)
							myOrder += myColOrdering;

						myOrder += maxColOrder;
					}
				}

				note["myOrder"] = myOrder;

				notesToOrder.push_back(note);
			}

	std::sort(notesToOrder.begin(), notesToOrder.end(), [](Json::Value a, Json::Value b) { return a["myOrder"].asInt() < b["myOrder"].asInt(); });

	std::map<std::string, std::vector<size_t>>	duplicates;
	std::map<std::string, int>					assignedSymbolCounters; //We do this so that any unset symbols will be filled in by the javascriptside of things (symbolCounter stuff)

	int symbolCounter = 0;

	for(size_t i=0; i<notesToOrder.size(); i++)
	{
		std::string symbolText = notesToOrder[i]["text"].asString() + ">(*^-)<" + notesToOrder[i]["symbol"].asString();

		notesToOrder[i]["symbolText"] = symbolText;

		duplicates[symbolText].push_back(i);

		if(notesToOrder[i]["symbol"].asString() == "" && assignedSymbolCounters.count(symbolText) == 0)
			assignedSymbolCounters[symbolText] = symbolCounter++;
	}

	for(auto & symTxtSymCount : assignedSymbolCounters)
		for(size_t index : duplicates[symTxtSymCount.first])
			notesToOrder[index]["symbol"] = symTxtSymCount.second;

	std::vector<Json::Value> notesToOrderMerged(notesToOrder);

	std::set<size_t> removeThese;

	for(auto & symTxtIndices : duplicates)
	{
		bool first = true;

		for(size_t i : symTxtIndices.second)
		{
			if(!first)
				removeThese.insert(i);
			first = false;
		}
	}

	std::vector<size_t> removeTheseVec(removeThese.begin(), removeThese.end());
	std::sort(removeTheseVec.begin(), removeTheseVec.end(), [](size_t l, size_t r) { return r < l; });

	for(size_t & i : removeTheseVec)
		notesToOrderMerged.erase(notesToOrderMerged.begin() + i);

	for(size_t mergedIndex=0; mergedIndex<notesToOrderMerged.size(); mergedIndex++)
		for(size_t duplicateIndex : duplicates[notesToOrderMerged[mergedIndex]["symbolText"].asString()])
			notesToOrder[duplicateIndex]["footnoteIndex"] = int(mergedIndex);

	for(Json::Value & note : notesToOrder)
		note.removeMember("symbolText");

	for(Json::Value & note : notesToOrderMerged)
		note.removeMember("symbolText");

	fullList	= jaspJson::VectorJson_to_ArrayJson(notesToOrder);
	mergedList	= jaspJson::VectorJson_to_ArrayJson(notesToOrderMerged);
}

void footnotes::convertFromJSON_SetFields(Json::Value footnotes)
{
	if (footnotes.isArray())
		for (Json::Value & footnote : footnotes)
		{
			const std::string		text	= footnote["text"].asString(),
									symbol	= footnote["symbol"].asString();
			std::set<Json::Value>	rows	= jaspJson::ArrayJson_to_SetJson(footnote["rows"]),
									cols	= jaspJson::ArrayJson_to_SetJson(footnote["cols"]);

			_data[text][symbol].insert(tableFields(rows, cols));
		}
}


void footnotes::insert(std::string text, std::string symbol, std::vector<Json::Value> colNames, std::vector<Json::Value> rowNames)
{
	_data[text][symbol].insert(
				tableFields(
					std::set<Json::Value>(rowNames.begin(), rowNames.end()),
					std::set<Json::Value>(colNames.begin(), colNames.end())
				)
	);
}

void jaspTable::addFootnote(Rcpp::RObject message, Rcpp::RObject symbol, Rcpp::RObject col_names, Rcpp::RObject row_names)
{
	if (message.isNULL())
		Rf_error("One would expect a footnote to at least contain a message..");

	std::string strMessage	= jaspNativeToUtf8(message);
	std::string strSymbol	= symbol.isNULL() ? "" : jaspNativeToUtf8(symbol);

	std::vector<Json::Value> colNames;
	if (!col_names.isNULL())
		colNames = jaspJson::RcppVector_to_VectorJson(col_names, false);

	std::vector<Json::Value> rowNames;
	if (!row_names.isNULL())
		rowNames = jaspJson::RcppVector_to_VectorJson(row_names, false);

	_footnotes.insert(strMessage, strSymbol, colNames, rowNames);
}

Json::Value jaspTable::dataEntry(std::string & errorMessage) const
{
	Json::Value	tmpFootnotesFull,		//This should contain the full list of footnotes, aka per table/col/row/cell and in order of occurence left to right, top to bottom. This should also contain an index of the footnote in _tmpFootnotesMerged
				tmpFootnotesMerged;	//This should contain the same list, but now without duplication, just for being shown underneath the table.

	_footnotes.convertToJSONOrdered(mapRowNamesToIndices(), mapColNamesToIndices(), tmpFootnotesFull, tmpFootnotesMerged); // convert footnotes into something usable...

	Json::Value dataJson(jaspObject::dataEntry(errorMessage));

	dataJson["title"]				= _title;

	dataJson["name"]				= getUniqueNestedName();
	dataJson["schema"]				= schemaJson(tmpFootnotesFull);

	dataJson["data"]				= rowsJson(tmpFootnotesFull);
	dataJson["casesAcrossColumns"]	= _transposeTable;
	dataJson["overTitle"]			= _transposeWithOvertitle;

	dataJson["status"]				= _error ? "error" : _status;
	dataJson["footnotes"]			= tmpFootnotesMerged;

	return dataJson;
}

Json::Value	jaspTable::schemaJson(Json::Value footnotes) const
{
    Json::Value schema(Json::objectValue);
	Json::Value fields(Json::arrayValue);

	std::map<std::string, std::vector<int>> footnotesPerCol;

	for(const Json::Value & note : footnotes)
		if(!note["cols"].isNull())
			if(note["rows"].isNull())
				for(const Json::Value & colName : note["cols"])
					footnotesPerCol[colName.asString()].push_back(note["footnoteIndex"].asInt());

	for(int col=0; col< std::max(_colNames.rowCount(), _expectedColumnCount); col++)
	{
		Json::Value field(Json::objectValue);

		std::string colName		= getColName(col),
					colTitle	= _colTitles.containsField(colName)  ? _colTitles[colName]  : (_colTitles[col]  != "" ? _colTitles[col]  : colName),
					colFormat	= _colFormats.containsField(colName) ? _colFormats[colName] : (_colFormats[col] != "" ? _colFormats[col] : "");

		field["name"]	= colName;
		field["title"]	= colTitle;

		field["type"]	= getColType(col);

		if(_colOvertitles.containsField(colName))
			field["overTitle"] = _colOvertitles[colName];

		if(_colCombines.containsField(colName))
			field["combine"] = _colCombines[colName];

		if(colFormat != "")
			field["format"]	= colFormat;

		if(footnotesPerCol.count(colName) > 0)
		{
			Json::Value notes(Json::arrayValue);
			for(int noteIndex : footnotesPerCol[colName])
				notes.append(noteIndex);
			field[".footnotes"] = notes;
		}

		if(colName[0] != '.' && (!_showSpecifiedColumnsOnly || _specifiedColumns.count(_colNames[col]) > 0))
			fields.append(field);
	}

    schema["fields"] = fields;
    return schema;
}

bool jaspTable::isSpecialColumn(size_t col) const
{
	if(_colNames[col] == "") return false;

	return _colNames[col] == ".isNewGroup" || _colNames[col] == ".footnotes";
}


Json::Value	jaspTable::rowsJson(Json::Value footnotes) const
{
	Json::Value rows(Json::arrayValue);

	std::map<std::string, std::map<std::string, std::vector<int>>> footnotesPerRowCol;

	for(const Json::Value & note : footnotes)
		if (!note["rows"].isNull())
			for (const Json::Value & rowName : note["rows"])
			{
				if (!note["cols"].isNull())
					for (const Json::Value & colName : note["cols"])
						footnotesPerRowCol[rowName.asString()][colName.asString()].push_back(note["footnoteIndex"].asInt());
				else
					for (size_t col=0; col<_data.size(); col++)
						footnotesPerRowCol[rowName.asString()][getColName(col)].push_back(note["footnoteIndex"].asInt());
			}

	size_t	maxRow, maxCol;
	calculateMaxColRow(maxCol, maxRow);

	bool keepGoing = true;
	for(size_t row=0; keepGoing; row++)
	{
		Json::Value aRow(Json::objectValue);
		bool aColumnKeepsGoing = false;

		for(size_t col=0; col<std::max(_data.size(), maxCol); col++)
		{
			bool hasDataHere = col < _data.size() && row < _data[col].size();

			if(hasDataHere)
				aColumnKeepsGoing = true;

			if(
					(hasDataHere || !isSpecialColumn(col)) &&										//Either it is a normal entry, which can lack data but should still be included. Or it is a specialColumn without data and it shouldn't be included
					(!_showSpecifiedColumnsOnly || columnSpecified(col) || isSpecialColumn(col))	//if not _showSpecifiedColumnsOnly then were done. Otherwise we need to check whether it is either specified or a specialColumn (with data)
			)
				aRow[getColName(col)] = getCell(col, row, maxCol, maxRow); //The add the data to the row!
		}

		std::string rowName = getRowName(row);
		if(footnotesPerRowCol.count(rowName) > 0)
		{
			Json::Value notes(Json::objectValue);

			for(auto & keyval : footnotesPerRowCol[rowName])
			{
				auto colName = keyval.first;
				notes[colName] = Json::arrayValue;

				for(int noteIndex : keyval.second)
					notes[colName].append(noteIndex);
			}

			aRow[".footnotes"] = notes;
		}

		if(aColumnKeepsGoing || row < _expectedRowCount)
			rows.append(aRow);
		else
			keepGoing = false;
	}

	return rows;
}

std::string jaspTable::deriveColumnType(int col) const
{
	if(col >= _data.size())
		return "null";

	Json::ValueType workingType = Json::nullValue;
	const std::string variousType = "various";

	for(auto & cell : _data[col])
		switch(workingType)
		{
		case Json::nullValue:
			workingType = cell.type();
			break;

		case Json::stringValue:
		case Json::booleanValue:
			if(cell.type() != workingType)
				return variousType;
			break;

		case Json::intValue:
		case Json::uintValue:
			if(cell.type() == Json::realValue)
				workingType = Json::realValue;
			else if(cell.type() != workingType)
				return variousType;
			break;

		case Json::realValue:
			if(!(cell.type() == workingType || cell.type() == Json::intValue || cell.type() == Json::uintValue))
				return variousType;
			break;

		default:
			return "composite"; //arrays and objects are not really supported as cells at the moment but maybe we could add that in the future?
		}

	switch(workingType)
	{
	case Json::nullValue:		return "null";
	case Json::stringValue:		return "string";
	case Json::booleanValue:	return "logical";
	case Json::intValue:
	case Json::uintValue:		return "integer";
	case Json::realValue:		return "number";
	default:					return "unknown";
	}
}

std::string jaspTable::getColType(size_t col) const
{
	std::string colName = getColName(col);

	if(_colTypes[colName] != "")	return _colTypes[colName];
	if(_colTypes[col] != "")		return _colTypes[col];

	return deriveColumnType(col);
}

///Going to assume it is called like addColumInfo(name=NULL, title=NULL, type=NULL, format=NULL, combine=NULL)
void jaspTable::addColumnInfo(Rcpp::RObject name, Rcpp::RObject title, Rcpp::RObject type, Rcpp::RObject format, Rcpp::RObject combine, Rcpp::RObject overtitle)
{
	std::string colName = name.isNULL() ? defaultColName(_colNames.rowCount()) : Rcpp::as<std::string>(name);
	_specifiedColumns.insert(colName);

	_colNames.add(colName);

	std::string lastAddedColName = getColName(_colNames.rowCount() - 1);

	if(!title.isNULL())		_colTitles[		lastAddedColName ] = jaspNativeToUtf8(title);
	if(!type.isNULL())		_colTypes[		lastAddedColName ] = jaspNativeToUtf8(type);
	if(!format.isNULL())	_colFormats[	lastAddedColName ] = jaspNativeToUtf8(format);
	if(!combine.isNULL())	_colCombines[	lastAddedColName ] = Rcpp::as<bool>(combine);
	if(!overtitle.isNULL())	_colOvertitles[	lastAddedColName ] = jaspNativeToUtf8(overtitle);
}


Json::Value jaspTable::convertToJSON() const
{
	Json::Value obj		= jaspObject::convertToJSON();

	obj["status"]					= _status;
	obj["transposeTable"]			= _transposeTable;
	obj["transposeWithOvertitle"]	= _transposeWithOvertitle;
	obj["showSpecifiedColumnsOnly"]	= _showSpecifiedColumnsOnly;

	obj["footnotes"]				= _footnotes.convertToJSON();
	obj["colNames"]					= _colNames.convertToJSON();
	obj["colTypes"]					= _colTypes.convertToJSON();
	obj["rowNames"]					= _rowNames.convertToJSON();
	obj["rowTitles"]				= _rowTitles.convertToJSON();
	obj["colTitles"]				= _colTitles.convertToJSON();
	obj["colOvertitles"]			= _colOvertitles.convertToJSON();
	obj["colFormats"]				= _colFormats.convertToJSON();
	obj["colCombines"]				= _colCombines.convertToJSON();
	obj["expectedRowCount"]			= int(_expectedRowCount);
	obj["expectedColumnCount"]		= int(_expectedColumnCount);

	Json::Value dataColumns(Json::arrayValue);

	for(auto & col : _data)
	{
		Json::Value dataRows(Json::arrayValue);

		for(auto & row : col)
			dataRows.append(row);

		dataColumns.append(dataRows);
	}

	obj["data"]	= dataColumns;

	Json::Value colRowCombos(Json::arrayValue);

	for(auto & colRow : _colRowCombinations)
		colRowCombos.append(colRow.convertToJSON());

	obj["colRowCombinations"] = colRowCombos;

	obj["specifiedColumns"] = Json::arrayValue;
	for(const std::string & specifiedColumnName : _specifiedColumns)
		obj["specifiedColumns"].append(specifiedColumnName);

	return obj;
}

void jaspTable::convertFromJSON_SetFields(Json::Value in)
{
	jaspObject::convertFromJSON_SetFields(in);

	_status						= in.get("status",						"null").asString();
	_transposeTable				= in.get("transposeTable",				false).asBool();
	_transposeWithOvertitle		= in.get("transposeWithOvertitle",		false).asBool();
	_showSpecifiedColumnsOnly	= in.get("showSpecifiedColumnsOnly",	false).asBool();
	_expectedRowCount			= in.get("expectedRowCount",			0).asUInt();
	_expectedColumnCount		= in.get("expectedColumnCount",			0).asUInt();

	_footnotes.convertFromJSON_SetFields(		in.get("footnotes", 	Json::nullValue));
	_colNames.convertFromJSON_SetFields(		in.get("colNames",		Json::objectValue));
	_colTypes.convertFromJSON_SetFields(		in.get("colTypes",		Json::objectValue));
	_rowNames.convertFromJSON_SetFields(		in.get("rowNames",		Json::objectValue));
	_rowTitles.convertFromJSON_SetFields(		in.get("rowTitles",		Json::objectValue));
	_colTitles.convertFromJSON_SetFields(		in.get("colTitles",		Json::objectValue));
	_colFormats.convertFromJSON_SetFields(		in.get("colFormats",	Json::objectValue));
	_colCombines.convertFromJSON_SetFields(		in.get("colCombines",	Json::objectValue));
	_colOvertitles.convertFromJSON_SetFields(	in.get("colOvertitles",	Json::objectValue));

	_data.clear();
	Json::Value dataColumns(in.get("data",	Json::arrayValue));
	for(auto & col : dataColumns)
	{
		std::vector<Json::Value> newCol;

		for(auto & rowElem : col)
			newCol.push_back(rowElem);

		_data.push_back(newCol);
	}

	_colRowCombinations.clear();
	Json::Value colRowCombos(in.get("colRowCombinations",	Json::arrayValue));

	for(auto & colRowCombo : colRowCombos)
		_colRowCombinations.push_back(jaspColRowCombination(colRowCombo));

	 _specifiedColumns.clear();
	for(Json::Value & specifiedColumnName : in.get("specifiedColumns", Json::arrayValue))
		_specifiedColumns.insert(specifiedColumnName.asString());
}

std::map<std::string, size_t> jaspTable::mapColNamesToIndices() const
{
	std::map<std::string, size_t> out;

	for(size_t i=0; i< _data.size(); i++)
		out[getColName(i)] = i;

	return out;
}

std::map<std::string, size_t> jaspTable::mapRowNamesToIndices() const
{
	std::map<std::string, size_t> out;

	size_t maxRowCount = 0;

	for(size_t i=0; i< _data.size(); i++)
		maxRowCount = std::max(maxRowCount, _data[i].size());

	for(size_t i=0; i<maxRowCount; i++)
		out[getRowName(i)] = i;

	return out;
}

