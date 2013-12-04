邮件解析引擎库  
 
 
 
 
 
 
 
 
 
作者：陈海军 
 
时间：2004-10-13 
 
 
 
 
内容摘要 
　　本文介绍新的邮件解析和组装引擎库的实现。 
 
 
 
 
 
 
目   录 
目   录	2 
1	概述	3 
1.1	术语	3 
2	邮件解析库的实现	3 
2.1	实现过程	3 
2.2	文件列表	3 
3	邮件解析库的类	3 
3.1	类的实现	3 
3.2	类图	5 
3.3	MimeMessage类	5 
3.4	MimeMessage类结构图	6 
4	参考文献	7 
　　 
1  
概述 
　　邮件解析库API完全使用面向对象技术设计，使用C++语言开发的用于邮件解析和组装的库。它提供了一些类用来解析和组装Internet邮件，如MimeMessage和MimeBodyPart，用于Internet邮件协议实现并且遵循RFC822和RFC2045规范。这些API库用于应用程序的开发。 
1.1 术语 
　　RFC：Request For Comments, 请求注解, Internet标准(草案) 
　　MIME：Multipurpose Internet Mail Extension protocol, 多用途的网际邮件扩充协议 
2 邮件解析库的实现 
2.1 实现过程 
　　本邮件解析库（FastMail）参考JavaMail的结构设计，有与之类似的接口和类。本解析库用到基础库Fast库参考ACE库和STL、Java Framework库设计，包含一些字符串处理和容器的类。设计它们的目的是为了简化库的接口，并且尽量与Java库的接口兼容，同时使用完全标准的C++特性，使库更易维护更稳固。 
　　这两个库从2004-6月底开始开发设计至2004-10-10完成，历时3个多月，由于不是全力投入，实际耗时约2个月，由于有JavaMail、ACE和STL库的借鉴，所以设计起来还比较顺利。两个库的代码量约有32000行代码。类设计合理并且提供丰富的接口供二次开发和改造。 
    本系统测试过的编译器版本有：
    Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 13.10.3077 for 80x86
    gcc version 2.95.4 20011002 (Debian prerelease)
    gcc version 2.95.3 20010315 (release)
    gcc version 2.96 20000731 (Red Hat Linux 7.1 2.96-81)
    测试过的操作系统有：
    Windows XP Professional sp1
    Linux naven-debian 2.2.20-idepci x86
    Linux xmail2 2.4.18 (Red Hat Linux 7.1 2.96-81) 
    SunOS xmailsun 5.8 Generic sun4u sparc SUNW,Ultra-60 (Solaris 8)

　　下面是两个库的文件列表以及邮件解析库的类的介绍（Fast库另有文档专门介绍）： 
2.2 文件列表 
FAST基础库 
	comm\ 
	comm\FastArray.h		数组类 
	comm\FastAutoPtr.h		自动管理指针类 
	comm\FastBase.h			FAST库公共定义，包含一个完整的内存分配器类 
	comm\FastHashMap.h		HashMap容器类 
	comm\FastMap.h			Map容器类 
	comm\FastString.h		FastString字符串处理类 
	comm\FastVector.h		Vector容器类 
 
邮件解析引擎FastMail库 
	mime\ 
	mime\CharsetUtils.cpp		字符串编码和解码工具 
	mime\CharsetUtils.h 
	mime\MimeActivation.cpp		邮件解析初始化工具 
	mime\MimeActivation.h 
	mime\MimeBase.h			邮件解析公共定义 
	mime\MimeContainer.cpp		邮件解析Multipart容器 
	mime\MimeContainer.h 
	mime\MimeEntity.cpp		邮件解析MIME实体类 
	mime\MimeEntity.h 
	mime\MimeMessage.cpp		邮件解析MIME邮件类 
	mime\MimeMessage.h 
	mime\MimeObject.cpp		邮件解析MIME对象类 
	mime\MimeObject.h 
	mime\MimeParser.cpp		邮件解析工具类 
	mime\MimeParser.h 
	mime\MimeUtility.cpp		邮件解析工具类 
	mime\MimeUtility.h 
 
3 邮件解析库的类 
3.1 类的实现 
　　邮件解析库包含一系列的类，主要有MimeMessage（邮件实现类）、MimeBodyPart（邮件正文段体类）、MimeMultipart（邮件多部段体类）、InternetHeaders（邮件头类）、InternetAddress（邮件地址类）和ContentType（段体类型类）等。解析和组装邮件主要使用这些类进行组装和分解。 
　　下面是所有的类的说明： 
类名                名称            说明                                    基类 
ContentDisposition  MIME邮件头      实现MIME邮件头的ContentDisposition  
ContentID           MIME邮件头      实现MIME邮件头的ContentID  
ContentType         MIME邮件头      实现MIME邮件头的ContentType  
MimeType            MIME类型类      实现MIME的类型，记录在ContentType里，如text/plain  
ConverterFactory    编码转换工厂类   用于创建编码转换对象  
ICodeConverter      编码转换接口类   
Base64Converter     BASE64编码类    BASE64的编码和解码                      ICodeConverter 
QPConverter         QP编码类        Quote-Printable的编码和解码             ICodeConverter 
hdr                 MIME邮件头类    实现MIME邮件头HEADER行  
HeaderTokenizer     邮件头分解类    实现分解MIME邮件头的各元素，如Content-Type的mimetype和各个参数。  
IMimePart           MIME邮件段体接口类 邮件段体类的基类  
MimeBodyPart        MIME邮件段体类  实现MIME邮件各个段体的类                IMimePart 
MimeMessage         MIME邮件类      MIME邮件主类                            IMimePart 
IMultipart          多部分接口类    多部分类的基类  
MimeMultipart       多部分实现类    保存段体类对象的多部分容器类              IMultipart 
InternetAddress     MIME邮件地址类  实现MIME邮件地址的类  
InternetHeaders     MIME邮件头部分类 实现保存MIME各邮件头的容器类  
MailDateFormat      邮件时间转换类  实现MIME邮件的格式的时间转换  
MailDateParser      时间分解处理类  实现MIME邮件格式的时间的分解  
MimeInitialization  邮件解析全局初始化类 实现初始化MIME邮件用到的各个全局变量  
MimetypesFileTypeMap 邮件Mime类型映射类 实现邮件MimeType和FileType类型的映射表，用于查询  
MimeUtility         MIME邮件分析工具类 实现邮件解析用到的各个解析工具函数  
ParameterList       参数列表类      实现ContentType和ContentDisposition用到的参数列表  
SystemProperty      系统环境类      实现读取系统环境参数的方法  
UniqueValue         邮件唯一值生成类 实现生成MIME邮件用到的唯一值的类，如boundary等  

3.2 类图 

3.3 MimeMessage类 
　　现在介绍一下最主要的也是提供主要的调用接口API的类MimeMessage。 
　　MimeMessage提供了一系列的方法供调用者使用，如定义了获取地址信息和获取邮件正文内容的结构（可以为具体的数据也可以为一个MimeMultipart对象），用来实现RFC822和MIME规范。 
　　一个MimeMessage对象里保存了一个邮件内容数据（Content），以及一些记录特定的邮件地址信息（如发件人（Sender）和收件人（recipients））的属性（InternetHeaders）。还有关于这封邮件的结构信息（structural information），以及它的邮件主体（body）的段体类型（Content-Type）。 
　　下面用图来描述一个MimeMessage对象内部可能的结构： 
3.4 MimeMessage类结构图 
　　 
　　 
　　 
 
4 参考文献 
[1]《JavaMail 1.2》 
[2]《JavaMailTM API Design Specification Version 1.2》 
[3]《C++网络编程 卷1：运用ACE和模式消除复杂性》 
[4]《MIME邮件面面观》 
[5] http://www.faqs.org/rfcs/ 
   
 
   
 
   
 
   
 
 
10/17/2004                                             8/8                                         
 

