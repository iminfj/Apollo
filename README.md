# Apollo 编程语言

Apollo 是一门全新的编程语言，旨在提供一种简洁、灵活和高效的编程体验。该项目包含了 Apollo 编程语言的规范和第一个版本的解释器。

## 特性

- **简洁易学**：Apollo 设计为一门易于学习的编程语言，具有直观的语法和清晰的结构，使得编写代码变得简单和愉快。
- **灵活性**：Apollo 提供了丰富的特性和灵活的语法，允许开发者根据需要自由地表达和组织代码，从而实现更高层次的抽象。
- **高效执行**：Apollo 解释器经过优化，能够在运行时快速执行代码，提供高性能和响应性能。(尽管目前还没有做到高效执行，但我们会朝这个方向做出持续进步。)
- **跨语言协作**：Apollo 具备跨语言协作的特性，允许在 Apollo 代码中直接实现和调用其他编程语言的代码，例如 C++，实现了内联多种编程语言的能力（前提是支持的足够多）。
- **新增数据结构**：Apollo 引入了一些新的数据结构，例如 neuron，用于实现神经网络的基本数据结构。

## 依赖和环境

为了使 Apollo 编译器正常使用，请完成以下步骤来配置系统环境变量：

1. 将 `bin` 目录添加到系统的 `PATH` 环境变量中。
2. 创建一个名为 `APOLLO_HOME` 的系统全局变量，并将其值设置为 Apollo 项目的根目录。
3. 创建一个名为 `APOLLO_MEMORY` 的系统全局变量，并将其值设置为 `acm` 目录的路径（用于指向 ACM 内存空间）。
4. 创建一个名为 `APOLLO_INCLUDE` 的系统全局变量，并将其值设置为 `include` 目录的路径（用于正常链接库实现）。

## 安装

1. 安装依赖和工具：
   - 确保您的系统为 Windows 10 及以上版本。
   - 使用 Chocolatey 工具安装 `win_flex` 和 `win_bison` 可执行文件。
   - 安装 LLVM 16 clang++ 和 Python 3.x，作为开发环境的依赖。

2. 克隆项目到本地:

    ```bash
    git clone https://github.com/scientist-loki/Apollo.git
    ```

3. 进入项目目录:

    ```bash
    cd apollo/bin
    ```

4. 编译和构建 Apollo 解释器:

    ```bash
    .\build
    ```

5. 切换到 examples 目录:

    ```bash
    cd ..
    cd examples
    ```

6. 运行示例代码:

    ```bash
    apollo hello.apo
    ```

详细的安装和使用指南，请参考项目中的 [文档](docs/README.md) 。

## 示例

下面是一个简单的 Apollo 代码示例，用于输出 "Hello World!"：

```apollo
def main()
{
    out: 'Hello World!';   // The support for the print function is coming soon.
}

main();
```

更多示例代码和详细说明，请参考 [示例目录](examples/) 。

## 贡献

我们欢迎您为 Apollo 编程语言项目做出贡献！如果您有任何建议、问题或修复，请通过 GitHub 的 issue 系统提交。

## 捐赠

感谢您对 Apollo 编程语言项目的支持！您的捐赠将用于推进项目的发展、维护社区和设立奖金等活动。如果您愿意捐赠，请访问我们的[捐赠页面](https://www.apollo-lang.org/donate)。

## 许可证

该项目基于 [Apache 2.0 许可证](LICENSE-Apollo) 进行发布和分发。

---
