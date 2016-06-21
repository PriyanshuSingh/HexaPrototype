//
// Created by ashish on 6/22/16.
//

#ifndef MYGAME_ANDTOSTRING_HPP
#define MYGAME_ANDTOSTRING_HPP

#include <string>
#include <sstream>
#include "cocos2d.h"
#if defined(CC_PLATFORM_ANDROID)
#define TO_STRING to_stringAndroid

template <typename T>
inline std::string to_stringAndroid(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}


#else

#define TO_STRING std::to_string

#endif



#endif //MYGAME_ANDTOSTRING_HPP
