#!/bin/bash

# 设置颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 获取脚本所在目录的上级目录（项目根目录）
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
TEST_BIN_DIR="$PROJECT_ROOT/test/bin"

print_info "项目根目录: $PROJECT_ROOT"
print_info "构建目录: $BUILD_DIR"
print_info "测试二进制目录: $TEST_BIN_DIR"

# 1. 清理编译产物
print_info "正在清理编译产物..."
if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"
    print_success "已删除构建目录: $BUILD_DIR"
else
    print_warning "构建目录不存在，跳过清理"
fi

# 清理test/bin目录下的可执行文件
if [ -d "$TEST_BIN_DIR" ]; then
    find "$TEST_BIN_DIR" -type f -executable -name "*" -delete
    print_success "已清理test/bin目录下的可执行文件"
fi

# 2. 创建构建目录
print_info "创建构建目录..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 3. 配置和编译
print_info "正在配置CMake..."
if cmake ..; then
    print_success "CMake配置成功"
else
    print_error "CMake配置失败"
    exit 1
fi

print_info "正在编译项目..."
if make -j$(nproc); then
    print_success "编译成功"
else
    print_error "编译失败"
    exit 1
fi

# 4. 运行测试
print_info "正在运行测试..."

# 检查test/bin目录是否存在
if [ ! -d "$TEST_BIN_DIR" ]; then
    print_error "测试二进制目录不存在: $TEST_BIN_DIR"
    exit 1
fi

# 计数器
total_tests=0
passed_tests=0
failed_tests=0

# 遍历test/bin目录下的所有子目录，查找可执行文件
for test_dir in "$TEST_BIN_DIR"/*/; do
    if [ -d "$test_dir" ]; then
        # 查找目录下的可执行文件
        for executable in "$test_dir"/*; do
            if [ -f "$executable" ] && [ -x "$executable" ]; then
                total_tests=$((total_tests + 1))
                test_name=$(basename "$executable")
                test_dir_name=$(basename "$test_dir")
                
                print_info "运行测试: $test_dir_name/$test_name"
                echo "----------------------------------------"
                
                # 运行测试
                if "$executable"; then
                    print_success "测试通过: $test_dir_name/$test_name"
                    passed_tests=$((passed_tests + 1))
                else
                    print_error "测试失败: $test_dir_name/$test_name"
                    failed_tests=$((failed_tests + 1))
                fi
                echo "----------------------------------------"
                echo
            fi
        done
    fi
done

# 5. 输出测试结果统计
echo "========================================"
print_info "测试结果统计:"
echo "总测试数: $total_tests"
print_success "通过: $passed_tests"
if [ $failed_tests -gt 0 ]; then
    print_error "失败: $failed_tests"
else
    print_success "失败: $failed_tests"
fi
echo "========================================"

# 如果有失败的测试，返回非零退出码
if [ $failed_tests -gt 0 ]; then
    print_error "有测试失败，退出码: 1"
    exit 1
else
    print_success "所有测试通过！"
    exit 0
fi
