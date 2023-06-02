# Apollo 编程语言

Apollo 是一门全新的编程语言，旨在提供一种简洁、灵活和高效的编程体验。该项目包含了 Apollo 编程语言的规范和第一个版本的解释器。

## 特性

- **简洁易学**：Apollo 设计为一门易于学习的编程语言，具有直观的语法和清晰的结构，使得编写代码变得简单和愉快。
- **灵活性**：Apollo 提供了丰富的特性和灵活的语法，允许开发者根据需要自由地表达和组织代码，从而实现更高层次的抽象。
- **高效执行**：Apollo 解释器经过优化，能够在运行时快速执行代码，提供高性能和响应性能。(尽管目前还没有做到高效执行，但我们会朝这个方向做出持续进步。)
- **共同开发和改进**：我们欢迎有兴趣的开发者加入我们的项目，一起合作改进 Apollo 解释器的执行效率。我们相信通过集体智慧和协作，我们可以不断优化代码执行，提升项目的性能和响应性能。如果您对提升执行效率和性能方面有任何想法、建议或贡献，请随时加入我们的团队，共同努力推动项目的发展。

## 安装

1. 克隆项目到本地:

    ```
    git clone https://github.com/scientist-loki/Apollo.git
    ```

2. 进入项目目录:

    ```
    cd apollo/bin
    ```

3. 编译和构建 Apollo 解释器:

    ```
    build
    ```

4. 切换到 examples 目录

    ```
    cd ..
    cd examples
    ```

5. 运行示例代码:

    ```
    apollo hello.apo
    ```

详细的安装和使用指南，请参考项目中的 [文档](docs/README.md) 。

## 示例

下面是一个简单的 Apollo 代码示例，用于输出 "Hello World!"：

```apollo
def main ()
{
    out : 'Hello World!';   // The support for the print function is coming soon.
}

main();
```

更多示例代码和详细说明，请参考 [示例目录](examples/) 。

## 贡献

我们欢迎您为 Apollo 编程语言项目做出贡献！如果您有任何建议、问题或修复，请通过 GitHub 的 issue 系统提交。

## 许可证

该项目基于 [Apache 2.0 许可证](LICENSE-Apollo) 进行发布和分发。

---
