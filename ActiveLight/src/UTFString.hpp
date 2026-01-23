//
//  UTFString.hpp
//  ActiveLight
//
//  Created by DiCode77.
//

/*
 
 UString is a wrapper over the basic std::basic_string, needed for better interaction with larger characters and additional functionality.
 
 */

#ifndef UTFString_hpp
#define UTFString_hpp

#include <string>

class UString : public std::basic_string<char32_t>{
public:
    UString() = default;
    UString(const char32_t *str) : std::basic_string<char32_t>(str){}
};

#endif /* UTFString_hpp */
