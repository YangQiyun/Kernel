# Kernel
 ![after adding virtual mem](http://i.imgur.com/PZ7Prj3.png)

----------


- 做这个玩具内核的想法开学的时候就想过了，一开始很茫然，不知道从哪里开始入手，上知乎看了很多遍相关的内容大佬的过来路，他们推荐了很多的书籍和资料，也刚刚好是操作系统概念这个课程的开课。一开始寒假几天是跟着王爽的《汇编语言》读完了x86的汇编指令，然后开学看着《操作系统概念》还有于渊的《orange‘s 一个操作系统的实现》，前后将近花了一个月吧，期间都是晚上一大块时间在研究后者。其实对于后者那本书，说实话作者对于实现一个操作系统用了过多的汇编操作，使得实现起来及其的繁琐和复杂很容易打击自信心的，前前后后花了很长的时间去理解里面的汇编操作写法，因为有时候并不知道为什么01与或操作的处理，比如描述符的占用，那么段选择子其实就运用掩码就可以得到描述符的对应个数的选择，这些小细节作者直接就代码实现了，看汇编理解起来比较痛苦的。但是不得不说作家将各种基础的寄存器等等概念分析得很透彻，这或许又和汇编离不开了，又爱又恨，后来在看到文件系统那一块我就停了，重新摸索新的解决方案。其实看这本书的动力还有就是前人学长学姐借过这本书有些标记，看到了最后几页中间等等都有读书标记，这是令我十分惊讶的。现在这个kernel是参考github上面的一个文档实现的[hurley](https://github.com/hurley25/hurlex-doc)以及它所参考的[JamesM's kernel文档](http://www.jamesmolloy.co.uk/tutorial_html/)。在实现的时候阅读了很多linux的源码，感觉收货还是蛮大的，见识了很多数据处理的操作，还有有些看似神秘强大的函数print等的底层实现，这是一个蛮有意思的事情同时而又繁琐艰难的。大致跟随着文档再同时加上一些参考源码实现的一些算法处理的第一个版本是在暑假打工过程中完成的，最后推荐另一个看到别人实现的内核版本觉得很强[ToyOS](https://github.com/UKeeySDis/ToyOS)
- 一开始的实现都是较为容易理解的，因为看过于渊的操作系统实现，对于相应的原理还是实现都是比较清楚，看着文档再加上自己的理解进行简单的修改操作就好，较好的是包括一些8259等IO处理模块在这个学期中也都学到了不难理解
- 加入了虚拟内存管理，需要了解到的知识基础有[分页机制总结](http://blog.csdn.net/pacosonswjtu/article/details/48293925)还有 [Linux 虚拟内存和物理内存的理解](http://blog.csdn.net/dlutbrucezhang/article/details/9058583)
- 个人理解的核心改动是修改内核进入的入口，先运行kernel_entry函数，进行了所谓的临时的虚拟内存页面的跳转，使得我们从一开始的0内存地址开始能跳转到0xC0000000的虚拟地址过去，其中注意加载临时的页目录表和页表项，但转换进入页机制后，再次更新好真正的页机制表就好。CR3是寄存页目录表的起始地址，CR0的第一位是开启页机制的标志。
- 进入kernel，打印一段欢迎语
- 找到标记好的位置确定kernel所占的位置，因为此时是直接打印标记的值，也就是虚拟地址的值，所以如图
- 接下来是我们从multiboot中获取的可用内存区域和标志内容
- 通过伙伴算法的实现管理512MB的内存，由于在连续段内是可分配页数是32250，鉴于实现一个简单的分配器，只用里面2次幂的最大公约数16384
- 验证伙伴算法，一开始分配4097的内存空间，得到两页的空间，同时再分配一个一页空间，下一步释放第一次分配的空间，再申请一页空间可以得到一开始的分配地址
- 最后加入的线程管理是内核初始化的实现申请的线程栈空间，实现的思路是通过中断不断的交叉运行打印出AB,整体的方式的，调用线程栈的一个线程空间，初始化为我们所需要的内容，加入到时间中断中。遇到页错误问题，是前面实现malloc函数中没有把所有申请到的物理内存都映射到对应的虚拟内存地址中去，所以导致了发声页错误。