# LRU
Least recently used，最近最少使用
## 基础LRU算法
根据OSTEP的内存虚拟化部分（尤其是第二十二章 超越物理内存：策略）
- 进程表示自己要访问某个虚拟地址VPN（虚拟页号）
- 查看缓存有没有记录VPN对应的FPN（物理页号）
所以将缓存理解成存放VPN到FPN的映射的空间，同理可以扩展到其他地方，这个缓存也还是存放了什么（key）到什么（value）的映射，我们实现的是软件层面的抽象

LRU即最近最少使用（我们还以OSTEP的VPN-->FPN为例来思考）
- 根据前面的讨论，我们要存放key（VPN）到value（FPN）的映射，故采用哈希表的数据结构
- 进程发出一个访问指令，首先去哈希表中找key
	- 找到了，说明该key的地方代表最近访问的地方，这个信息需要记录
	- 如果没找到，要看缓存有没有满，
		- 如果没有满，则将key放到最近访问的地方
		- 如果满了，则需要踢出最远访问的地方，然后将key放到最近访问的地方
- 上述操作包括：查找（哈希表）、记录最近位置、删除节点、插入节点
	- 采用双链表来插入的效率较高（单链表的效率低）
- **最终选择**：哈希表+双向链表
## LRU改进1：LRU-k
问题：热门访问数据被挤出缓存

提出解决方案：在缓存查不到的数据找到后，不立刻放入缓存，而是等待多几次再放入，避免冷门数据的侵占

利用一个LruCache作为缓存，一个作为次缓存

## LRU改进2：HashLRU

加入锁来避免并发问题

但引入了锁竞争，临界区外面的等待导致效率降低

利用LRU分片来解决LRU的锁等待的问题




