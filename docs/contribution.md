# 向Metaverse内核贡献代码

通过如下方式向内核增加你的代码：

* 从[github仓库](https://github.com/metaverse-kernel/kernel-dev)或[Random World Studio内部仓库](http://git.suthby.org:2024/metaverse/kernel-dev)创建一个分支
* 在你的分支中增加代码
* 测试你增加的代码的正确性，并尽量确保对原有代码没有影响
* 无需在注释中加入代码更改日志，git可以做到
* 在文档的适当的位置增加对新特性的描述
* 完成编码和文档的工作后向主仓库发送PR
* 等待审核代码

若你的代码通过审核，将会把你的PR合并到主分支中。

## 开发引导

## vscode

vscode下的`rust-analyzer`插件会在`src/lib.rs`中误报`can't find crate for test`，实际上在`Cargo.toml`中已经增加了`test = false`选项。

解决方法是在`.vscode/settings.json`中增加两项：

```json
"rust-analyzer.cargo.allTargets": false,
"rust-analyzer.cargo.extraArgs": [
  "--target",
  "开发的目标平台的unknown-none类型目标（如x86_64-unknown-none）"
]
```

## 需要注意的还未解决的问题

* rust中所有有关字符串格式化的宏中，出现超过一个不止是`{}`的格式化占位符时内核必然崩溃。
* rust中所有有关字符串格式化的宏中，出现需要调用自定义的`Debug trait`的类型时内核必然崩溃，推荐定义`ToString trait`并调用`.to_string()`后传入宏参数。
* 鉴于以上两条原因，不建议在复杂的字符串格式化任务中使用`format!()`宏。推荐通过使用`::kernel::tty::tty::MessageBuilder`构造`kernel::tty::tty::Message`对象，或使用
  `message`宏，并调用此对象的`.to_string()`方法实现格式化字符串。
