/*
 * @file exceptdef.h
 * @author sugar
 * @date 2024-06-09
 * @brief MyMiniSTL 异常定义与断言工具，封装常用异常类型和调试宏
 */

#ifndef EXCEPT_DEF_H_
#define EXCEPT_DEF_H_

#include <stdexcept>  
#include <cassert>    
#include <cstddef>

/**
 * @brief MyMiniSTL 异常定义
 * 封装了STL中常用的异常类型和调试宏
 * 宏命名规范：
 * - SUGAR_ 前缀避免命名冲突
 * - DEBUG 用于调试断言
 * - THROW_XXX_IF 用于条件异常抛出
 */

// 调试宏
#ifdef NDEBUG
    #define SUGAR_DEBUG(expr) ((void)0)
#else
    #define SUGAR_DEBUG(expr) assert(expr)
#endif

// 异常抛出宏 - 使用do {} while(0)避免宏展开问题
#define SUGAR_THROW_LENGTH_ERROR_IF(expr, what) \
    do { if ((expr)) throw std::length_error(what); } while(0)

#define SUGAR_THROW_OUT_OF_RANGE_IF(expr, what) \
    do { if ((expr)) throw std::out_of_range(what); } while(0)

#define SUGAR_THROW_RUNTIME_ERROR_IF(expr, what) \
    do { if ((expr)) throw std::runtime_error(what); } while(0)

#define SUGAR_THROW_INVALID_ARGUMENT_IF(expr, what) \
    do { if ((expr)) throw std::invalid_argument(what); } while(0)

#define SUGAR_THROW_BAD_ALLOC_IF(expr, what) \
    do { if ((expr)) throw std::bad_alloc(); } while(0)

#define SUGAR_THROW_LOGIC_ERROR_IF(expr, what) \
    do { if ((expr)) throw std::logic_error(what); } while(0)

// 范围检查宏
#define SUGAR_RANGE_CHECK(index, size) \
    SUGAR_THROW_OUT_OF_RANGE_IF((index) >= (size), "Index out of range")

// 大小检查宏
#define SUGAR_SIZE_CHECK(size, max_size) \
    SUGAR_THROW_LENGTH_ERROR_IF((size) > (max_size), "Size exceeds maximum")

// 空指针检查宏
#define SUGAR_NULL_POINTER_CHECK(ptr) \
    SUGAR_THROW_INVALID_ARGUMENT_IF((ptr) == nullptr, "Null pointer")

// 条件检查宏
#define SUGAR_CONDITION_CHECK(condition, message) \
    SUGAR_THROW_LOGIC_ERROR_IF(!(condition), message)

namespace sugar {
    // ============================ 异常类型别名 ============================
    /** @brief 异常类型别名，便于使用 */
    using length_error = std::length_error;
    using out_of_range = std::out_of_range;
    using runtime_error = std::runtime_error;
    using invalid_argument = std::invalid_argument;
    using bad_alloc = std::bad_alloc;
    using logic_error = std::logic_error;

    // ============================ 自定义异常类 ============================
    /** @brief 容器为空异常 */
    class container_empty_error : public std::logic_error {
    public:
        explicit container_empty_error(const std::string& what) 
            : std::logic_error(what) {}
    };

    /** @brief 迭代器异常 */
    class iterator_error : public std::logic_error {
    public:
        explicit iterator_error(const std::string& what) 
            : std::logic_error(what) {}
    };

    // ============================ 异常抛出函数 ============================
    /**
     * @brief 条件抛出异常（通用模板）
     */
    template<typename ExceptionType>
    inline void throw_if(bool condition, const std::string& message) {
        if (condition) {
            throw ExceptionType(message);
        }
    }

    /** @brief 抛出length_error */
    inline void throw_length_error_if(bool condition, const std::string& message) {
        throw_if<length_error>(condition, message);
    }

    /** @brief 抛出out_of_range */
    inline void throw_out_of_range_if(bool condition, const std::string& message) {
        throw_if<out_of_range>(condition, message);
    }

    /** @brief 抛出invalid_argument */
    inline void throw_invalid_argument_if(bool condition, const std::string& message) {
        throw_if<invalid_argument>(condition, message);
    }

    /** @brief 抛出logic_error */
    inline void throw_logic_error_if(bool condition, const std::string& message) {
        throw_if<logic_error>(condition, message);
    }

    // ============================ 检查函数 ============================
    /** @brief 范围检查 */
    inline void range_check(size_t index, size_t size) {
        throw_out_of_range_if(index >= size, "Index " + std::to_string(index) + 
                              " out of range [0, " + std::to_string(size) + ")");
    }

    /** @brief 大小检查 */
    inline void size_check(size_t size, size_t max_size) {
        throw_length_error_if(size > max_size, "Size " + std::to_string(size) + 
                             " exceeds maximum " + std::to_string(max_size));
    }

    /** @brief 空指针检查 */
    template<typename T>
    inline void null_pointer_check(const T* ptr) {
        throw_invalid_argument_if(ptr == nullptr, "Null pointer");
    }

    /** @brief 条件检查 */
    inline void condition_check(bool condition, const std::string& message) {
        throw_logic_error_if(!condition, message);
    }
}

#endif // EXCEPT_DEF_H_