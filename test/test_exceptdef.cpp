#include "../exceptdef.h"
#include <iostream>
#include <vector>
#include <string>

// 测试用的简单容器类
class TestContainer {
private:
    std::vector<int> data_;
    size_t max_size_;

public:
    TestContainer(size_t max_size = 100) : max_size_(max_size) {}

    void push_back(int value) {
        SUGAR_SIZE_CHECK(data_.size() + 1, max_size_);
        data_.push_back(value);
    }

    int at(size_t index) const {
        SUGAR_RANGE_CHECK(index, data_.size());
        return data_[index];
    }

    void set_data(const std::vector<int>* ptr) {
        SUGAR_NULL_POINTER_CHECK(ptr);
        data_ = *ptr;
    }

    void check_condition(bool condition) {
        SUGAR_CONDITION_CHECK(condition, "Condition check failed");
    }

    // 使用现代C++函数方式
    void modern_push_back(int value) {
        sugar::size_check(data_.size() + 1, max_size_);
        data_.push_back(value);
    }

    int modern_at(size_t index) const {
        sugar::range_check(index, data_.size());
        return data_[index];
    }

    void modern_set_data(const std::vector<int>* ptr) {
        sugar::null_pointer_check(ptr);
        data_ = *ptr;
    }

    void modern_check_condition(bool condition) {
        sugar::condition_check(condition, "Modern condition check failed");
    }

    size_t size() const { return data_.size(); }
    size_t max_size() const { return max_size_; }
};

void test_macro_exceptions() {
    std::cout << "\n=== 测试宏异常 ===" << std::endl;
    
    TestContainer container(2);
    
    try {
        std::cout << "测试正常操作..." << std::endl;
        container.push_back(1);
        container.push_back(2);
        std::cout << "正常操作成功" << std::endl;
        
        std::cout << "测试大小超限异常..." << std::endl;
        container.push_back(3); // 应该抛出 length_error
    } catch (const std::length_error& e) {
        std::cout << "捕获 length_error: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试范围越界异常..." << std::endl;
        int value = container.at(5); // 应该抛出 out_of_range
    } catch (const std::out_of_range& e) {
        std::cout << "捕获 out_of_range: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试空指针异常..." << std::endl;
        container.set_data(nullptr); // 应该抛出 invalid_argument
    } catch (const std::invalid_argument& e) {
        std::cout << "捕获 invalid_argument: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试条件检查异常..." << std::endl;
        container.check_condition(false); // 应该抛出 logic_error
    } catch (const std::logic_error& e) {
        std::cout << "捕获 logic_error: " << e.what() << std::endl;
    }
}

void test_function_exceptions() {
    std::cout << "\n=== 测试函数异常 ===" << std::endl;
    
    TestContainer container(2);
    
    try {
        std::cout << "测试现代C++函数方式..." << std::endl;
        container.modern_push_back(1);
        container.modern_push_back(2);
        std::cout << "正常操作成功" << std::endl;
        
        std::cout << "测试大小超限异常..." << std::endl;
        container.modern_push_back(3); // 应该抛出 length_error
    } catch (const std::length_error& e) {
        std::cout << "捕获 length_error: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试范围越界异常..." << std::endl;
        int value = container.modern_at(5); // 应该抛出 out_of_range
    } catch (const std::out_of_range& e) {
        std::cout << "捕获 out_of_range: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试空指针异常..." << std::endl;
        container.modern_set_data(nullptr); // 应该抛出 invalid_argument
    } catch (const std::invalid_argument& e) {
        std::cout << "捕获 invalid_argument: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试条件检查异常..." << std::endl;
        container.modern_check_condition(false); // 应该抛出 logic_error
    } catch (const std::logic_error& e) {
        std::cout << "捕获 logic_error: " << e.what() << std::endl;
    }
}

void test_custom_exceptions() {
    std::cout << "\n=== 测试自定义异常 ===" << std::endl;
    
    try {
        std::cout << "测试容器空异常..." << std::endl;
        throw sugar::container_empty_error("Container is empty");
    } catch (const sugar::container_empty_error& e) {
        std::cout << "捕获 container_empty_error: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试迭代器异常..." << std::endl;
        throw sugar::iterator_error("Invalid iterator");
    } catch (const sugar::iterator_error& e) {
        std::cout << "捕获 iterator_error: " << e.what() << std::endl;
    }
}

void test_debug_macro() {
    std::cout << "\n=== 测试调试宏 ===" << std::endl;
    
    int value = 42;
    
    // 这些在调试模式下会触发断言，在发布模式下会被优化掉
    SUGAR_DEBUG(value > 0);
    SUGAR_DEBUG(value == 42);
    
    std::cout << "调试宏测试完成（如果看到这个，说明没有触发断言）" << std::endl;
}

void test_direct_macro_usage() {
    std::cout << "\n=== 测试直接宏使用 ===" << std::endl;
    
    try {
        std::cout << "测试直接使用 THROW_LENGTH_ERROR_IF..." << std::endl;
        SUGAR_THROW_LENGTH_ERROR_IF(true, "Direct length error test");
    } catch (const std::length_error& e) {
        std::cout << "捕获 length_error: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试直接使用 THROW_OUT_OF_RANGE_IF..." << std::endl;
        SUGAR_THROW_OUT_OF_RANGE_IF(true, "Direct out of range test");
    } catch (const std::out_of_range& e) {
        std::cout << "捕获 out_of_range: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试直接使用 THROW_INVALID_ARGUMENT_IF..." << std::endl;
        SUGAR_THROW_INVALID_ARGUMENT_IF(true, "Direct invalid argument test");
    } catch (const std::invalid_argument& e) {
        std::cout << "捕获 invalid_argument: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试直接使用 THROW_LOGIC_ERROR_IF..." << std::endl;
        SUGAR_THROW_LOGIC_ERROR_IF(true, "Direct logic error test");
    } catch (const std::logic_error& e) {
        std::cout << "捕获 logic_error: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "测试直接使用 THROW_BAD_ALLOC_IF..." << std::endl;
        SUGAR_THROW_BAD_ALLOC_IF(true, "Direct bad alloc test");
    } catch (const std::bad_alloc& e) {
        std::cout << "捕获 bad_alloc: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "=== MyMiniSTL Exception Definition 测试 ===" << std::endl;
    
    test_macro_exceptions();
    test_function_exceptions();
    test_custom_exceptions();
    test_debug_macro();
    test_direct_macro_usage();
    
    std::cout << "\n=== 所有测试完成 ===" << std::endl;
    return 0;
} 