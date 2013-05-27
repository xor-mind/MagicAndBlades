#ifndef MAB_MISC_H
#define MAB_MISC_H

#include <string>
#include <sstream>
#include <algorithm>

struct MabMisc
{
	static unsigned int CountWordsInString(std::string const& str)
	{
		std::stringstream stream(str);
		return std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
	}
};

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

#endif