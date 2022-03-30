# LUA 虚拟机的CPP实现

## 简介
    使用c++对lua虚拟机进行实现，实现了基本算数逻辑操作，函数调用，闭包，迭代器的功能，并能够读取luac编译器编译得到的lua二进制文件并执行。

    要求lua版本为5.3

    TODO：lua前端，lua标准库支持

## 安装和使用方法
```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./LuaCpp /path/to/your/.out/file
```

## 目录结构
```
--include
 |- binchunk // read and parse lua binary chunk
 |- common // global macro and lua data object
 |- number // function for lua number and integer compute
 |- state // lua state object definition and data structure table and closure
 |- vm // lua vm definition and opcode related methods
--lua // test scripts
--src // implementation of functions in /include
```

## 参考
- https://github.com/Manistein/dummylua-tutorial
- [构建lua解释器](https://zhuanlan.zhihu.com/p/49972755)
- 《自己动手实现Lua：虚拟机、编译器和标准库》 -- 张秀宏
