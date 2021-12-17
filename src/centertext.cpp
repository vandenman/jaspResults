#include "centertext.h"

center_helper<std::string::value_type, std::string::traits_type> centered(const std::string& str)
{
	return center_helper<std::string::value_type, std::string::traits_type>(str);
}
