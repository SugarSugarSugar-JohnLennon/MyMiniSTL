#ifndef EXCEPT_DEF_H_
#define EXCEPT_DEF_H_

#include <stdexcept>  
#include <cassert>    

/**
 * 封装了一些常见异常
 * do {} while(0) 的作用：
 * 解决宏的歧义和统一函数出口。
 * 参考https://blog.csdn.net/qq_44844115/article/details/102566648?fromshare=blogdetail&sharetype=blogdetail&sharerId=102566648&sharerefer=PC&sharesource=2501_91357523&sharefrom=from_link
 */

namespace sugar{
    #define DEBUG(expr) \
    assert(expr)  

    #define THROW_LENGTH_ERROR_IF(expr, what) \
    do { if ((expr)) throw std::length_error(what); } while(0)

    #define THROW_OUT_OF_RANGE_IF(expr, what) \
    do { if ((expr)) throw std::out_of_range(what); } while(0)

    #define THROW_RUNTIME_ERROR_IF(expr, what) \
    do { if ((expr)) throw std::runtime_error(what); } while(0)

    #define THROW_INVALID_ARGUMENT_IF(expr, what) \
    do { if ((expr)) throw std::invalid_argument(what); } while(0)
}

#endif // EXCEPT_DEF_H_