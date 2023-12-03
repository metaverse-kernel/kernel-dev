# Metaverse内核

基于multiboot2引导的64位内核。

## 试用

* 编译

```bash
make
```

* 编译发布版

```bash
make release=1
```

* 运行

```bash
make run
```

* gdb调试

```bash
make debug
```

## 依赖项

* nbd
* qemu

## 开发计划

* 重新设置内核分页前的准备：
  * multiboot2启动信息
  * 重设显示模式前的内核日志
  * 显示模式设置
  * 键盘的连接

## 文档

### kernel

### libk

## 版权声明

版权所有 (c) 2023 - 现在 Random World Studio 保留所有权利。

本软件使用MIT开源许可证进行许可。详细信息请参阅许可证文件。
