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


# LFU
Least frequently used，最少使用
## 基础LFU算法
LFU的基本思想：最近使用频率高的数据很大概率将会再次被使用，而最近使用频率低的数据，将来大概率不会再使用

需要：
- 频率链表，即每个频率对应一个链表，链表里面有出现频率为该频率的节点
	- 删除指定节点
	- 末尾加节点
	- 判断链表是否为空
	- 获得头节点
- LruCache类
	- 属性
		- 缓存的大小
		- 记录多个频率链表的频率最小值
		- 哈希表key到节点映射
		- 锁
	- 方法
		- 在缓存中取某个key对应的值
			- 取到，对应值的频率需要加一
			- 取不到
				- 则要删除频率最低的节点（频率链表、哈希表）
				- 从其他地方找到key的value，构建节点，存入缓存（频率链表、哈希表）

存在的问题？
- 计数不断增长导致其难以被替换，甚至数值溢出
- 刚加入的数据容易很快被替换掉
- 锁的粒度大，多线程高并发访问下锁的同步等待时间过长。

## 改进方案1：最大平均访问次数限制

如何防止计数不断增大？需要定期清理
- 引入访问次数平均值概念
- 当平均值大于最大平均值限制时，对计数进行全局的衰减

## 改进方案2：利用hash表来
利用LFU分片来解决LFU的锁等待的问题


# ARC
Adaptive Replacement Cache，自适应替换缓存
## 基础ARC算法
- LRU和LFU的缺点
	- LRU：热门数据被挤出缓存
	- LFU：计数不断增长导致其难以被替换，甚至数值溢出
- ARC算法：结合LRU和LFU的优点
	- 将缓存分成两个部分，一个部分是LRU部分，一个部分是LFU部分