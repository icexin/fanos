# 对内核的探索

实现了如下功能：

- 简单的分时多任务
- 系统调用
- 基于段式的内存管理
- 串口的读取
- 简单的文件读取
- 终端的模拟
- 执行可执行文件

# 实现

- 使用grub1作为引导程序
- 磁盘使用minix文件系统，放在images下面
- ramfs.img是内存的文件系统，只读的
