HTTP Simple Queue Service for Windows - HttpsqsWin
Author： Jia Fan
E-mail: jiafan319@gmail.com
Homepage: https://github.com/jiafan319/httpsqswin

* This file is writen in CHINESE.

Httpsqs是一个基于HTTP协议的轻量级开源简单队列(FIFO Queue)服务, 原作者：张宴 http://blog.s135.com/httpsqs/7/1/
Httpsqs只能运行在Linux系统上，因此我决定将其移植到Windows上，由此产生了你们现在看到的这个产品HttpsqsWin.

HttpsqsWin采用Libevent2的Http模块提供Http服务。Kyotocabinet作为数据存储（原Httpsqs使用Tokyocabinet）。

HttpsqsWin当前版本为0.3，源代码大部分基于HttpsqsWin v1.7因此HttpsqsWin兼容Httpsqs v1.7的所有命令，原Httpsqs的各种客户端可以不加修改进行使用。

可以在wiki中的benchmark文档中找到关于HttpsqsWin的性能测试数据。

详细的使用说明和客户端可以访问原作者张宴的blog
http://blog.s135.com/httpsqs/7/1/

具体编译和安装方法可以参考wiki中的INSTALL文档。

作者：贾凡
Email：jiafan319@gmail.com
项目主页: https://github.com/jiafan319/httpsqswin

