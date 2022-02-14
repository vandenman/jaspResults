#pragma once
#include <string>

class jaspPrintOptions
{
public:
	static jaspPrintOptions& GetInstance()
	{
		// better than pointer version according to https://stackoverflow.com/a/13047579
		static jaspPrintOptions opts;
		return opts;
	}

	std::string		getIndent			()							const	{ return _indent;	}
	void			setIndentWithTabs	(const bool indentWithTabs)			{ if (indentWithTabs == _indentWithTabs)	return; _indentWithTabs = indentWithTabs;	updateIndent(); }
	bool			getIndentWithTabs	()							const	{ return _indentWithTabs; }
	void			setIndentSize		(const size_t indentSize)			{ if (indentSize == _indentSize)			return; _indentSize = indentSize;			updateIndent(); }
	size_t			getIndentSize		()							const	{ return _indentSize; }

	std::string		topLineChar			()							const	{ return _useUnicode ? unicodeLineChars.topLineChar : nonUnicodeLineChars.topLineChar; };
	std::string		midLineChar			()							const	{ return _useUnicode ? unicodeLineChars.midLineChar : nonUnicodeLineChars.midLineChar; };
	std::string		botLineChar			()							const	{ return _useUnicode ? unicodeLineChars.botLineChar : nonUnicodeLineChars.botLineChar; };
	std::string		getColSep			()							const	{ return _visibleColSep ? "|" : ""; };

	// TODO: should this follow the custom indent settings?
	std::string		getSummaryMiddleIndent()						const	{ return _useUnicode ? "\u251C\u2500"	: "  ";		}
	std::string		getSummaryLastIndent()							const	{ return _useUnicode ? "\u2514\u2500"	: "  ";		}
	std::string     getSummaryContinueIndent()						const	{ return _useUnicode ? "\u2502 "		: "  ";		} // for custom indent, we could add: + (_indentWithTabs ? _indent : std::string(_indentSize - 1, ' ')); }

	bool			_printDevInfo		= true,
					_useUnicode			= true,
					_visibleColSep		= false;


private:
	 jaspPrintOptions() = default;

	// Delete copy/move so extra instances can't be created/moved.
	jaspPrintOptions(const jaspPrintOptions&) = delete;
	jaspPrintOptions& operator=(const jaspPrintOptions&) = delete;
	jaspPrintOptions(jaspPrintOptions&&) = delete;
	jaspPrintOptions& operator=(jaspPrintOptions&&) = delete;

	bool					_indentWithTabs		= false;
	size_t					_indentSize			= 2;
	std::string				_indent				= "  ";

	void					updateIndent() {	_indent = std::string(_indentSize, _indentWithTabs ? '\t' : ' '); }

	struct lineChars
	{
		const std::string topLineChar;
		const std::string midLineChar;
		const std::string botLineChar;
	};

	lineChars unicodeLineChars = lineChars
	{
		.topLineChar = u8"\u2500",//u8"\u2501",
		.midLineChar = u8"\u2500",
		.botLineChar = u8"\u2500"//u8"\u2501"
	};
	lineChars nonUnicodeLineChars = lineChars
	{
		.topLineChar = "-",
		.midLineChar = "-",
		.botLineChar = "-"
	};


};
