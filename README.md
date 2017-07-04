# Kernel
 ![after adding virtual mem](http://i.imgur.com/PZ7Prj3.png)

----------
- 这次是加入了虚拟内存管理，需要了解到的知识基础有分页机制总结[http://blog.csdn.net/pacosonswjtu/article/details/48293925](http://blog.csdn.net/pacosonswjtu/article/details/48293925 "分页机制总结")还有 Linux 虚拟内存和物理内存的理解[http://blog.csdn.net/dlutbrucezhang/article/details/9058583](http://blog.csdn.net/dlutbrucezhang/article/details/9058583 "Linux 虚拟内存和物理内存的理解")
- 个人理解的核心改动是修改内核进入的入口，先运行kernel_entry函数，进行了所谓的临时的虚拟内存页面的跳转，使得我们从一开始的0内存地址开始能跳转到0xC0000000的虚拟地址过去，其中注意加载临时的页目录表和页表项，但转换进入页机制后，再次更新好真正的页机制表就好。CR3是寄存页目录表的起始地址，CR0的第一位是开启页机制的标志。
- 进入kernel，打印一段欢迎语
- 找到标记好的位置确定kernel所占的位置，因为此时是直接打印标记的值，也就是虚拟地址的值，所以如图
- 接下来是我们从multiboot中获取的可用内存区域和标志内容
- 通过伙伴算法的实现管理512MB的内存，由于在连续段内是可分配页数是32250，鉴于实现一个简单的分配器，只用里面2次幂的最大公约数16384
- 验证伙伴算法，一开始分配4097的内存空间，得到两页的空间，同时再分配一个一页空间，下一步释放第一次分配的空间，再申请一页空间可以得到一开始的分配地址