#ifndef JASPPRINTOPTIONS_H
#define JASPPRINTOPTIONS_H

#include <Rcpp.h>
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
	void			setIndentSize		(const size_t indentSize)			{ if (indentSize == _indentSize)			return; _indentSize = indentSize;			updateIndent(); }

	std::string		topLineChar			()							const	{ return _useUnicode ? unicodeLineChars.topLineChar : nonUnicodeLineChars.topLineChar; };
	std::string		midLineChar			()							const	{ return _useUnicode ? unicodeLineChars.midLineChar : nonUnicodeLineChars.midLineChar; };
	std::string		botLineChar			()							const	{ return _useUnicode ? unicodeLineChars.botLineChar : nonUnicodeLineChars.botLineChar; };

	bool			_printDevInfo		= true,
					_useUnicode			= true;


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

//// to nicely center strings, see https://stackoverflow.com/a/14861289
//template<typename charT, typename traits = std::char_traits<charT> >
//class center_helper {
//	std::basic_string<charT, traits> str_;
//public:
//	explicit center_helper(const std::basic_string<charT, traits> & str) : str_(str) {}
//	template<typename a, typename b>
//	friend std::basic_ostream<a, b>& operator<<(std::basic_ostream<a, b>& s, const center_helper<a, b>& c);
//};

//template<typename charT, typename traits = std::char_traits<charT> >
//center_helper<charT, traits> centered(std::basic_string<charT, traits> str) {
//	return center_helper<charT, traits>(str);
//}


#endif // JASPPRINTOPTIONS_H
