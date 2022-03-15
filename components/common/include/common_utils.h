/*
 * common_utils.h
 *
 *  Created on: 13.06.2017
 *      Author: michaelboeckling
 *  Modified on 04.05.2021
 *          by: andriy@malyshenko.com
 */

#ifndef _INCLUDE_COMMON_UTILS_H_
#define _INCLUDE_COMMON_UTILS_H_

int starts_with(const char *str, const char *pre)
{
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

// std::string int_to_string(int n)
// {
//     std::ostringstream stm;
//     stm << n;
//     return stm.str();
// }  

#endif /* _INCLUDE_COMMON_UTILS_H_ */
