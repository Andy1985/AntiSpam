�ʼ����������  
 
 
 
 
 
 
 
 
 
���ߣ��º��� 
 
ʱ�䣺2004-10-13 
 
 
 
 
����ժҪ 
�������Ľ����µ��ʼ���������װ������ʵ�֡� 
 
 
 
 
 
 
Ŀ   ¼ 
Ŀ   ¼	2 
1	����	3 
1.1	����	3 
2	�ʼ��������ʵ��	3 
2.1	ʵ�ֹ���	3 
2.2	�ļ��б�	3 
3	�ʼ����������	3 
3.1	���ʵ��	3 
3.2	��ͼ	5 
3.3	MimeMessage��	5 
3.4	MimeMessage��ṹͼ	6 
4	�ο�����	7 
���� 
1  
���� 
�����ʼ�������API��ȫʹ�������������ƣ�ʹ��C++���Կ����������ʼ���������װ�Ŀ⡣���ṩ��һЩ��������������װInternet�ʼ�����MimeMessage��MimeBodyPart������Internet�ʼ�Э��ʵ�ֲ�����ѭRFC822��RFC2045�淶����ЩAPI������Ӧ�ó���Ŀ����� 
1.1 ���� 
����RFC��Request For Comments, ����ע��, Internet��׼(�ݰ�) 
����MIME��Multipurpose Internet Mail Extension protocol, ����;�������ʼ�����Э�� 
2 �ʼ��������ʵ�� 
2.1 ʵ�ֹ��� 
�������ʼ������⣨FastMail���ο�JavaMail�Ľṹ��ƣ�����֮���ƵĽӿں��ࡣ���������õ�������Fast��ο�ACE���STL��Java Framework����ƣ�����һЩ�ַ���������������ࡣ������ǵ�Ŀ����Ϊ�˼򻯿�Ľӿڣ����Ҿ�����Java��Ľӿڼ��ݣ�ͬʱʹ����ȫ��׼��C++���ԣ�ʹ�����ά�����ȹ̡� 
�������������2004-6�µ׿�ʼ���������2004-10-10��ɣ���ʱ3�����£����ڲ���ȫ��Ͷ�룬ʵ�ʺ�ʱԼ2���£�������JavaMail��ACE��STL��Ľ������������������Ƚ�˳����������Ĵ�����Լ��32000�д��롣����ƺ������ṩ�ḻ�Ľӿڹ����ο����͸��졣 
    ��ϵͳ���Թ��ı������汾�У�
    Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 13.10.3077 for 80x86
    gcc version 2.95.4 20011002 (Debian prerelease)
    gcc version 2.95.3 20010315 (release)
    gcc version 2.96 20000731 (Red Hat Linux 7.1 2.96-81)
    ���Թ��Ĳ���ϵͳ�У�
    Windows XP Professional sp1
    Linux naven-debian 2.2.20-idepci x86
    Linux xmail2 2.4.18 (Red Hat Linux 7.1 2.96-81) 
    SunOS xmailsun 5.8 Generic sun4u sparc SUNW,Ultra-60 (Solaris 8)

������������������ļ��б��Լ��ʼ����������Ľ��ܣ�Fast�������ĵ�ר�Ž��ܣ��� 
2.2 �ļ��б� 
FAST������ 
	comm\ 
	comm\FastArray.h		������ 
	comm\FastAutoPtr.h		�Զ�����ָ���� 
	comm\FastBase.h			FAST�⹫�����壬����һ���������ڴ�������� 
	comm\FastHashMap.h		HashMap������ 
	comm\FastMap.h			Map������ 
	comm\FastString.h		FastString�ַ��������� 
	comm\FastVector.h		Vector������ 
 
�ʼ���������FastMail�� 
	mime\ 
	mime\CharsetUtils.cpp		�ַ�������ͽ��빤�� 
	mime\CharsetUtils.h 
	mime\MimeActivation.cpp		�ʼ�������ʼ������ 
	mime\MimeActivation.h 
	mime\MimeBase.h			�ʼ������������� 
	mime\MimeContainer.cpp		�ʼ�����Multipart���� 
	mime\MimeContainer.h 
	mime\MimeEntity.cpp		�ʼ�����MIMEʵ���� 
	mime\MimeEntity.h 
	mime\MimeMessage.cpp		�ʼ�����MIME�ʼ��� 
	mime\MimeMessage.h 
	mime\MimeObject.cpp		�ʼ�����MIME������ 
	mime\MimeObject.h 
	mime\MimeParser.cpp		�ʼ����������� 
	mime\MimeParser.h 
	mime\MimeUtility.cpp		�ʼ����������� 
	mime\MimeUtility.h 
 
3 �ʼ���������� 
3.1 ���ʵ�� 
�����ʼ����������һϵ�е��࣬��Ҫ��MimeMessage���ʼ�ʵ���ࣩ��MimeBodyPart���ʼ����Ķ����ࣩ��MimeMultipart���ʼ��ಿ�����ࣩ��InternetHeaders���ʼ�ͷ�ࣩ��InternetAddress���ʼ���ַ�ࣩ��ContentType�����������ࣩ�ȡ���������װ�ʼ���Ҫʹ����Щ�������װ�ͷֽ⡣ 
�������������е����˵���� 
����                ����            ˵��                                    ���� 
ContentDisposition  MIME�ʼ�ͷ      ʵ��MIME�ʼ�ͷ��ContentDisposition  
ContentID           MIME�ʼ�ͷ      ʵ��MIME�ʼ�ͷ��ContentID  
ContentType         MIME�ʼ�ͷ      ʵ��MIME�ʼ�ͷ��ContentType  
MimeType            MIME������      ʵ��MIME�����ͣ���¼��ContentType���text/plain  
ConverterFactory    ����ת��������   ���ڴ�������ת������  
ICodeConverter      ����ת���ӿ���   
Base64Converter     BASE64������    BASE64�ı���ͽ���                      ICodeConverter 
QPConverter         QP������        Quote-Printable�ı���ͽ���             ICodeConverter 
hdr                 MIME�ʼ�ͷ��    ʵ��MIME�ʼ�ͷHEADER��  
HeaderTokenizer     �ʼ�ͷ�ֽ���    ʵ�ַֽ�MIME�ʼ�ͷ�ĸ�Ԫ�أ���Content-Type��mimetype�͸���������  
IMimePart           MIME�ʼ�����ӿ��� �ʼ�������Ļ���  
MimeBodyPart        MIME�ʼ�������  ʵ��MIME�ʼ������������                IMimePart 
MimeMessage         MIME�ʼ���      MIME�ʼ�����                            IMimePart 
IMultipart          �ಿ�ֽӿ���    �ಿ����Ļ���  
MimeMultipart       �ಿ��ʵ����    ������������Ķಿ��������              IMultipart 
InternetAddress     MIME�ʼ���ַ��  ʵ��MIME�ʼ���ַ����  
InternetHeaders     MIME�ʼ�ͷ������ ʵ�ֱ���MIME���ʼ�ͷ��������  
MailDateFormat      �ʼ�ʱ��ת����  ʵ��MIME�ʼ��ĸ�ʽ��ʱ��ת��  
MailDateParser      ʱ��ֽ⴦����  ʵ��MIME�ʼ���ʽ��ʱ��ķֽ�  
MimeInitialization  �ʼ�����ȫ�ֳ�ʼ���� ʵ�ֳ�ʼ��MIME�ʼ��õ��ĸ���ȫ�ֱ���  
MimetypesFileTypeMap �ʼ�Mime����ӳ���� ʵ���ʼ�MimeType��FileType���͵�ӳ������ڲ�ѯ  
MimeUtility         MIME�ʼ����������� ʵ���ʼ������õ��ĸ����������ߺ���  
ParameterList       �����б���      ʵ��ContentType��ContentDisposition�õ��Ĳ����б�  
SystemProperty      ϵͳ������      ʵ�ֶ�ȡϵͳ���������ķ���  
UniqueValue         �ʼ�Ψһֵ������ ʵ������MIME�ʼ��õ���Ψһֵ���࣬��boundary��  

3.2 ��ͼ 

3.3 MimeMessage�� 
�������ڽ���һ������Ҫ��Ҳ���ṩ��Ҫ�ĵ��ýӿ�API����MimeMessage�� 
����MimeMessage�ṩ��һϵ�еķ�����������ʹ�ã��綨���˻�ȡ��ַ��Ϣ�ͻ�ȡ�ʼ��������ݵĽṹ������Ϊ���������Ҳ����Ϊһ��MimeMultipart���󣩣�����ʵ��RFC822��MIME�淶�� 
����һ��MimeMessage�����ﱣ����һ���ʼ��������ݣ�Content�����Լ�һЩ��¼�ض����ʼ���ַ��Ϣ���緢���ˣ�Sender�����ռ��ˣ�recipients���������ԣ�InternetHeaders�������й�������ʼ��Ľṹ��Ϣ��structural information�����Լ������ʼ����壨body���Ķ������ͣ�Content-Type���� 
����������ͼ������һ��MimeMessage�����ڲ����ܵĽṹ�� 
3.4 MimeMessage��ṹͼ 
���� 
���� 
���� 
 
4 �ο����� 
[1]��JavaMail 1.2�� 
[2]��JavaMailTM API Design Specification Version 1.2�� 
[3]��C++������ ��1������ACE��ģʽ���������ԡ� 
[4]��MIME�ʼ�����ۡ� 
[5] http://www.faqs.org/rfcs/ 
   
 
   
 
   
 
   
 
 
10/17/2004                                             8/8                                         
 

