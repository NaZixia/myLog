# 指定编译器和编译选项
CXX = g++
CXXFLAGS = -std=c++11 -Iinc

# 可执行文件名称
Program_Name = myLog

# 指定源文件和目标文件的目录
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = .

# 获取所有的源文件和目标文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
OBJS += $(OBJ_DIR)/main.o  # 添加main.cpp编译生成的目标文件

# 指定最终生成的可执行文件
TARGET = $(BIN_DIR)/$(Program_Name)

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译src中每个源文件生成目标文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译main.cpp生成目标文件
$(OBJ_DIR)/main.o: main.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -rf $(OBJ_DIR)

# 伪目标
.PHONY: all clean
