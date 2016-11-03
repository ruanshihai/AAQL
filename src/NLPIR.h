/****************************************************************************
 *
 * NLPIR/ICTCLAS Lexical Analysis System Copyright (c) 2000-2014
 *     Dr. Kevin Zhang (Hua-Ping Zhang)
 *     All rights reserved.
 *
 * This file is the confidential and proprietary property of 
 * Kevin Zhang and the possession or use of this file requires 
 * a written license from the author.
 * Filename: 
 * Abstract:
 *          NLPIR.h: definition of the NLPIR lexical analysis system API
 * Author:   Kevin Zhang 
 *          Email: pipy_zhang@msn.com kevinzhang@bit.edu.cn
 *			Weibo: http://weibo.com/drkevinzhang
 *			Homepage: http://ictclas.nlpir.org
 * Date:     2013-12-19
 *
 * Notes:
 *                
 ****************************************************************************/
#define OS_LINUX
#if !defined(__NLPIR_ICTCLAS_2014_H_INCLUDED__)
#define __NLPIR_ICTCLAS_2014_H_INCLUDED__

#ifdef OS_LINUX
	#define NLPIR_API extern "C" 
#else
#ifdef NLPIR_EXPORTS
#define NLPIR_API extern "C" __declspec(dllexport)
#else
#define NLPIR_API extern "C"  __declspec(dllimport)
#endif
#endif

//�ڲ�����ʹ��

#ifdef NLPIR_INTERNAL_CALL
#define NLPIR_API 
#endif


//////////////////////////////////////////////////////////////////////////
//
//���¶���Ϊ����ICTCLAS��ǰ�İ汾
//
//////////////////////////////////////////////////////////////////////////
#define ICTCLAS_Init NLPIR_Init
#define ICTCLAS_Exit NLPIR_Exit
#define ICTCLAS_ImportUserDict  NLPIR_ImportUserDict
#define ICTCLAS_FileProcess NLPIR_FileProcess
#define ICTCLAS_ParagraphProcess  NLPIR_ParagraphProcess 
#define ICTCLAS_ParagraphProcessA  NLPIR_ParagraphProcessA 
#define ICTCLAS_GetParagraphProcessAWordCount   NLPIR_GetParagraphProcessAWordCount 
#define ICTCLAS_ParagraphProcessAW   NLPIR_ParagraphProcessAW 
#define ICTCLAS_AddUserWord  NLPIR_AddUserWord
#define ICTCLAS_SaveTheUsrDic  NLPIR_SaveTheUsrDic
#define ICTCLAS_DelUsrWord  NLPIR_DelUsrWord
#define ICTCLAS_GetUniProb  NLPIR_GetUniProb
#define ICTCLAS_IsWord  NLPIR_IsWord
#define ICTCLAS_SetPOSmap  NLPIR_SetPOSmap
#define CICTCLAS  CNLPIR
#define GetActiveICTCLAS GetActiveInstance 


#define POS_MAP_NUMBER 4 //add by qp 2008.11.25
#define ICT_POS_MAP_FIRST 1  //������һ����ע��
#define ICT_POS_MAP_SECOND 0 //������������ע��
#define PKU_POS_MAP_SECOND 2 //���������ע��
#define PKU_POS_MAP_FIRST 3	//����һ����ע��
#define  POS_SIZE 40

struct result_t{
  int start; //start position,��������������еĿ�ʼλ��
  int length; //length,����ĳ���
  char  sPOS[POS_SIZE];//word type������IDֵ�����Կ��ٵĻ�ȡ���Ա�
  int	iPOS;//���Ա�ע�ı��
  int word_ID; //�ôʵ��ڲ�ID�ţ������δ��¼�ʣ����0����-1
  int word_type; //�����û��ʵ�;1�����û��ʵ��еĴʣ�0�����û��ʵ��еĴ�
  int weight;//word weight,read weight
 };

#define GBK_CODE 0//Ĭ��֧��GBK����
#define UTF8_CODE GBK_CODE+1//UTF8����
#define BIG5_CODE GBK_CODE+2//BIG5����
#define GBK_FANTI_CODE GBK_CODE+3//GBK���룬�������������
#define UTF8_FANTI_CODE GBK_CODE+4//UTF8����

/*********************************************************************
 *
 *  Func Name  : Init
 *
 *  Description: Init NLPIR
 *               The function must be invoked before any operation listed as following
 *
 *  Parameters : const char * sInitDirPath=NULL
 *				 sDataPath:  Path where Data directory stored.
 *				 the default value is NULL, it indicates the initial directory is current working directory path
 *				 encode: encoding code;
 *				 sLicenseCode: license code for unlimited usage. common user ignore it
 *  Returns    : success or fail
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2013-6-8
 *********************************************************************/
NLPIR_API int NLPIR_Init(const char * sDataPath=0,int encode=GBK_CODE,const char*sLicenceCode=0);
/*********************************************************************
 *
 *  Func Name  : NLPIR_Exit
 *
 *  Description: Exist NLPIR and free related buffer
 *               Exit the program and free memory
 *				 The function must be invoked while you needn't any lexical anlysis
 *
 *  Parameters : None
 *
 *  Returns    : success or fail
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-8-6
 *********************************************************************/
NLPIR_API bool NLPIR_Exit();
/*********************************************************************
 *
 *  Func Name  : ParagraphProcessing
 *
 *  Description: Process a paragraph
 *    
 *
 *  Parameters : sParagraph: The source paragraph 
 *               
 *				 bPOStagged:Judge whether need POS tagging, 0 for no tag;default:1
 *				i.e.  �Ż�ƽ��1978��3��9�ճ����ڽ���ʡ�����ء�
 *					Result: �Ż�ƽ/nr  ��/p  1978��/t  3��/t  9��/t  ������/v  ����ʡ/ns  ������/ns  ��/w   
 *  Returns    : the result buffer pointer 
 *
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2003-12-22
 *********************************************************************/
NLPIR_API const char * NLPIR_ParagraphProcess(const char *sParagraph,int bPOStagged=1);
/*********************************************************************
 *
 *  Func Name  : NLPIR_GetLastErrorMsg
 *
 *  Description: GetLastErrorMessage
 *    
 *
 *  Parameters : void
 *               
 *				  
 *  Returns    : the result buffer pointer 
 *
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2014-2-27
 *********************************************************************/
NLPIR_API const char * NLPIR_GetLastErrorMsg();
/*********************************************************************
 *
 *  Func Name  : ParagraphProcessingA
 *
 *  Description: Process a paragraph
 *    
 *
 *  Parameters : sParagraph: The source paragraph 
 *               pResultCount: pointer to result vector size
 *  Returns    : the pointer of result vector, it is managed by system,user cannot alloc and free it 
 *  Author     :  Kevin Zhang  
 *  History    : 
 *              1.create 2006-10-26
 *********************************************************************/
NLPIR_API const result_t * NLPIR_ParagraphProcessA(const char *sParagraph,int *pResultCount,bool bUserDict=true);

/*********************************************************************
 *
 *  Func Name  : NLPIR_GetParagraphProcessAWordCount
 *
 *  Description: Get ProcessAWordCount, API for C#
 *				 Get word count and it helps us prepare the proper size buffer for result_t vector
 *
 *  Parameters : sParagraph: The source paragraph 
 *               
 *  Returns    : result vector size 
 *  Author     :  Kevin Zhang  
 *  History    : 
 *              1.create 2007-3-15
 *********************************************************************/
NLPIR_API int NLPIR_GetParagraphProcessAWordCount(const char *sParagraph);
/*********************************************************************
 *
 *  Func Name  : NLPIR_ParagraphProcessAW
 *
 *  Description: Process a paragraph, API for C#
 *    
 *
 *  Parameters : sParagraph: The source paragraph 
 *               result_t * result: pointer to result vector size, it is allocated by the invoker
 *  Returns    : None 
 *  Author     :  
 *  History    : 
 *              1.create 2007-3-15
 *********************************************************************/
NLPIR_API void NLPIR_ParagraphProcessAW(int nCount,result_t * result);

/*********************************************************************
 *
 *  Func Name  : NLPIR_FileProcess
 *
 *  Description: Process a text file
 *    
 *
 *  Parameters : sSourceFilename: The source file name  
 *               sResultFilename: The result file name 
 *				 bPOStagged:Judge whether need POS tagging, 0 for no tag;default:1
 *  i.e. FileProcess("E:\\Sample\\Corpus_NewPOS\\199802_Org.txt","E:\\Sample\\Corpus_NewPOS\\199802_Org_cla.txt");
 *  Returns    : success: 
 *               fail: 
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2005-11-22
 *********************************************************************/
NLPIR_API double NLPIR_FileProcess(const char *sSourceFilename,const char *sResultFilename,int bPOStagged=1);

/*********************************************************************
 *
 *  Func Name  : ImportUserDict
 *
 *  Description: Import User-defined dictionary
 *  Parameters : 
 *				sFilename:Text filename for user dictionary 
 *				bOverwrite: true,  overwrite the existing dictionary 
 *						   false, add to  the existing dictionary 		
 *  Returns    : The  number of  lexical entry imported successfully
 *  Author     : Kevin Zhang
 *  History    : 
 *              1.create 2014-8-3
 *********************************************************************/
NLPIR_API unsigned int NLPIR_ImportUserDict(const char *sFilename,bool bOverwrite=true);

/*********************************************************************
 *
 *  Func Name  : NLPIR_ImportKeyBlackList
 *
 *  Description: Import keyword black list 
 *  Parameters : Text filename for user dictionary, each line for a stop keyword
 *  Returns    : The  number of  lexical entry imported successfully
 *  Author     : Kevin Zhang
 *  History    : 
 *              1.create 2014-6-26
 *********************************************************************/
NLPIR_API unsigned int NLPIR_ImportKeyBlackList(const char *sFilename);
/*********************************************************************
*
*  Func Name  : NLPIR_AddUserWord
*
*  Description: add a word to the user dictionary ,example:���	
*													 i3s	n
*
*  Parameters : sFilename: file name
*               
*  Returns    : 1,true ; 0,false
*
*  Author     :   
*  History    : 
*              1.create 11:10:2008
*********************************************************************/
NLPIR_API int NLPIR_AddUserWord(const char *sWord);//add by qp 2008.11.10

/*********************************************************************
*
*  Func Name  : Save
*
*  Description: Save dictionary to file
*
*  Parameters :
*               
*  Returns    : 1,true; 2,false
*
*  Author     :   
*  History    : 
*              1.create 11:10:2008
*********************************************************************/
NLPIR_API int NLPIR_SaveTheUsrDic();

/*********************************************************************
*
*  Func Name  : NLPIR_DelUsrWord
*
*  Description: delete a word from the  user dictionary
*
*  Parameters : 
*  Returns    : -1, the word not exist in the user dictionary; else, the handle of the word deleted
*
*  Author     :   
*  History    : 
*              1.create 11:10:2008
*********************************************************************/
NLPIR_API int NLPIR_DelUsrWord(const char *sWord);

/*********************************************************************
*
*  Func Name  : NLPIR_GetUniProb
*
*  Description: Get Unigram Probability
*    
*
*  Parameters : sWord: input word 
*  Returns    : success: 
*               fail: 
*  Author     : Kevin Zhang  
*  History    : 
*              1.create 2005-11-22
*********************************************************************/
NLPIR_API double NLPIR_GetUniProb(const char *sWord);
/*********************************************************************
*
*  Func Name  : NLPIR_IsWord
*
*  Description: Judge whether the word is included in the core dictionary
*    
*
*  Parameters : sWord: input word 
*  Returns    : success: 
*               fail: 
*  Author     : Kevin Zhang  
*  History    : 
*              1.create 2005-11-22
*********************************************************************/
NLPIR_API int NLPIR_IsWord(const char *sWord);

/*********************************************************************
*
*  Func Name  : NLPIR_GetWordPOS
*
*  Description: Get the word Part-Of-Speech��information
*    
*
*  Parameters : sWord: input word 
*
*  Returns    : success: 
*               fail: 
*  Author     : Kevin Zhang  
*  History    : 
*              1.create 2014-10-10
*********************************************************************/
NLPIR_API const char * NLPIR_GetWordPOS(const char *sWord);

/*********************************************************************
*
*  Func Name  : NLPIR_GetKeyWords
*
*  Description: Extract keyword from sLine
*
*  Parameters : sLine, the input paragraph 
				bArguOut,whether  the keyword weight output
				nMaxKeyLimt:maximum of key words, up to 50
*  Returns    : keywords list like:
*               "��ѧ��չ�� ��۾��� " or
				"��ѧ��չ��/23.80/12#��۾���/12.20/1" with weight(��Ϣ�ؼ��ϴ�Ƶ��Ϣ)
*
*  Author     :   
*  History    : 
*              1.create 2012/11/12
*********************************************************************/
NLPIR_API const char * NLPIR_GetKeyWords(const char *sLine,int nMaxKeyLimit=50,bool bWeightOut=false);

/*********************************************************************
*
*  Func Name  : NLPIR_GetFileKeyWords
*
*  Description: Extract keyword from a text file
*
*  Parameters : sFilename, the input text file name 
				bArguOut,whether  the keyword weight output
				nMaxKeyLimt:maximum of key words, up to 50
*  Returns    : keywords list like:
*               "��ѧ��չ�� ��۾��� " or
				"��ѧ��չ�� 23.80 ��۾��� 12.20" with weight

*
*  Author     :   
*  History    : 
*              1.create 2012/11/12
*********************************************************************/
NLPIR_API const char * NLPIR_GetFileKeyWords(const char *sFilename,int nMaxKeyLimit=50,bool bWeightOut=false);
/*********************************************************************
*
*  Func Name  : NLPIR_GetNewWords
*
*  Description: Extract New words from sLine
*
*  Parameters : sLine, the input paragraph 
				bArguOut,whether  the keyword weight output
				nMaxKeyLimt:maximum of key words, up to 50
*  Returns    : new words list like:
*               "��ѧ��չ�� ��˿ "or
				"��ѧ��չ�� 23.80 ��˿ 12.20" with weight
*
*  Author     :   
*  History    : 
*              1.create  2012/11/12
*********************************************************************/
NLPIR_API const char * NLPIR_GetNewWords(const char *sLine,int nMaxKeyLimit=50,bool bWeightOut=false);

/*********************************************************************
*
*  Func Name  : NLPIR_GetFileNewWords
*
*  Description: Extract new words from a text file
*
*  Parameters : sFilename, the input text file name 
				bArguOut,whether  the keyword weight output
				nMaxKeyLimt:maximum of key words, up to 50
*  Returns    : keywords list like:
*               "��ѧ��չ�� ��۾��� " or
				"��ѧ��չ�� 23.80 ��۾��� 12.20" with weight

*
*  Author     :   
*  History    : 
*              1.create 2012/11/12
*********************************************************************/
NLPIR_API const char * NLPIR_GetFileNewWords(const char *sFilename,int nMaxKeyLimit=50,bool bWeightOut=false);
/*********************************************************************
*
*  Func Name  : NLPIR_FingerPrint
*
*  Description: Extract a finger print from the paragraph
*
*  Parameters :
*  Returns    : 0, failed; else, the finger print of the content
*
*  Author     :   
*  History    : 
*              1.create 11:10:2008
*********************************************************************/
NLPIR_API unsigned long NLPIR_FingerPrint(const char *sLine);

/*********************************************************************
*
*  Func Name  : NLPIR_SetPOSmap
*
*  Description: select which pos map will use
*
*  Parameters :nPOSmap, ICT_POS_MAP_FIRST  ������һ����ע��
						ICT_POS_MAP_SECOND  ������������ע��
						PKU_POS_MAP_SECOND   ���������ע��
						PKU_POS_MAP_FIRST 	  ����һ����ע��
*  Returns    : 0, failed; else, success
*
*  Author     :   
*  History    : 
*              1.create 11:10:2008
*********************************************************************/
NLPIR_API int NLPIR_SetPOSmap(int nPOSmap);


/*********************************************************************
*
*  class CNLPIR
*   ������
*		   NLPIR �࣬ʹ��֮ǰ�������NLPIR_Init(),�˳��������NLPIR_Exit
*		   ��ʹ�ù����п���ʹ�ö��CNLPIR��֧�ֶ��̷ִ߳ʴ���
*			ÿ���߳��ȵ���GetActiveInstance����ȡ�����࣬Ȼ������SetAvailable(false)��ʾ�߳���Ȩ��
*			������ɺ�SetAvailable(true)�ͷ��߳���Ȩ
*  History    : 
*              1.create 2005-11-10
*********************************************************************/
#ifdef OS_LINUX
class  CNLPIR {
#else
class  __declspec(dllexport) CNLPIR {
#endif
	public:
		CNLPIR();
		~CNLPIR();
		double FileProcess(const char *sSourceFilename,const char *sResultFilename,int bPOStagged=1);
		//Process a file��������C�µ�NLPIR_FileProcess
		const char * ParagraphProcess(const char *sLine,int bPOStagged=1); 
		//Process a file��������C�µ�NLPIR_ParagraphProcess
		const result_t * ParagraphProcessA(const char *sParagraph,int *pResultCount,bool bUserDict=true);
		//Process a file��������C�µ�NLPIR_ParagraphProcessA

		void ParagraphProcessAW(int nCount,result_t * result);
		int GetParagraphProcessAWordCount(const char *sParagraph);

		const char * GetKeyWords(const char *sLine,int nMaxKeyLimit,bool bWeightOut);
		//��ȡ�ؼ���
		const char * GetFileKeyWords(const char *sFilename,int nMaxKeyLimit,bool bWeightOut);
		//���ı��ļ��л�ȡ�ؼ���
		const char * GetNewWords(const char *sFilename,int nMaxKeyLimit,bool bWeightOut);
		//��ȡ�´�
		const char * GetFileNewWords(const char *sFilename,int nMaxKeyLimit,bool bWeightOut);
		//���ı��ļ��л�ȡ�´�

		bool SetAvailable(bool bAvailable=true);//��ǰ�߳��ͷŸ��࣬��Ϊ��һ���߳�ʹ��
		bool IsAvailable();//�жϵ�ǰ�ִ����Ƿ��߳�ռ��
		unsigned int GetHandle()
		{
			return m_nHandle;
		}
private:
		unsigned int m_nHandle;//�ó�Ա��Ϊ�����Handleֵ����ϵͳ�Զ����䣬�û������޸�
		bool m_bAvailable;//�ó�Ա��Ϊ���̹߳�����ƵĲ�������ϵͳ�Զ����䣬�û������޸�
		int m_nThreadCount;//Thread Count
		bool m_bWriting;//writing  protection
};

/*********************************************************************
*
*  Func Name  : GetActiveInstance
*
*  Description: ��ȡ���õ�CNLPIR�࣬�����ڶ��߳̿������Ȼ�ȡ���õ�CNLP���ٵ������еĹ���

*
*  Parameters : None
*  Returns    : CNLPIR*
*
*  Author     : Kevin Zhang
*  History    : 
*              1.create 1:10:2012
*********************************************************************/
NLPIR_API CNLPIR* GetActiveInstance();

/*********************************************************************
*
*  ���º���Ϊ2013�汾ר������´ʷ��ֵĹ��̣�һ�㽨���ѻ�ʵ�֣��������ߴ���
*  �´�ʶ����ɺ����Զ����뵽�ִ�ϵͳ�У��������
*  ������NLPIR_NWI(New Word Identification)��ͷ
*********************************************************************/
/*********************************************************************
*
*  Func Name  : NLPIR_NWI_Start
*
*  Description: �����´�ʶ��

*
*  Parameters : None
*  Returns    : bool, true:success, false:fail
*
*  Author     : Kevin Zhang
*  History    : 
*              1.create 2013/11/23
*********************************************************************/
NLPIR_API int NLPIR_NWI_Start();//New Word Indentification Start
/*********************************************************************
*
*  Func Name  : NLPIR_NWI_AddFile
*
*  Description: ���´�ʶ��ϵͳ����Ӵ�ʶ���´ʵ��ı��ļ�
*				��Ҫ������NLPIR_NWI_Start()֮�󣬲���Ч
*
*  Parameters : const char *sFilename���ļ���
*  Returns    : bool, true:success, false:fail
*
*  Author     : Kevin Zhang
*  History    : 
*              1.create 20132/11/23
*********************************************************************/
NLPIR_API int  NLPIR_NWI_AddFile(const char *sFilename);
/*********************************************************************
*
*  Func Name  : NLPIR_NWI_AddMem
*
*  Description: ���´�ʶ��ϵͳ�����һ�δ�ʶ���´ʵ��ڴ�
*				��Ҫ������NLPIR_NWI_Start()֮�󣬲���Ч
*
*  Parameters : const char *sFilename���ļ���
*  Returns    : bool, true:success, false:fail
*
*  Author     : Kevin Zhang
*  History    : 
*              1.create 2013/11/23
*********************************************************************/
NLPIR_API int NLPIR_NWI_AddMem(const char *sText);
/*********************************************************************
*
*  Func Name  : NLPIR_NWI_Complete
*
*  Description: �´�ʶ��������ݽ���
*				��Ҫ������NLPIR_NWI_Start()֮�󣬲���Ч
*
*  Parameters : None
*  Returns    : bool, true:success, false:fail
*
*  Author     : Kevin Zhang
*  History    : 
*              1.create 2013/11/23
*********************************************************************/
NLPIR_API int NLPIR_NWI_Complete();//�´�
/*********************************************************************
*
*  Func Name  : NLPIR_NWI_GetResult
*
*  Description: ��ȡ�´�ʶ��Ľ��
*				��Ҫ������NLPIR_NWI_Complete()֮�󣬲���Ч
*
*  Parameters : bWeightOut���Ƿ���Ҫ���ÿ���´ʵ�Ȩ�ز���
*
*  Returns    : �����ʽΪ
*				���´�1�� ��Ȩ��1�� ���´�2�� ��Ȩ��2�� ... 
*
*  Author     : Kevin Zhang
*  History    : 
*              1.create 2013/11/23
*********************************************************************/
NLPIR_API const char * NLPIR_NWI_GetResult(bool bWeightOut=false);//����´�ʶ����
/*********************************************************************
*
*  Func Name  : NLPIR_NWI_Result2UserDict
*
*  Description: ���´�ʶ�������뵽�û��ʵ���
*				��Ҫ������NLPIR_NWI_Complete()֮�󣬲���Ч
*				�����Ҫ���´ʽ�����ñ��棬������ִ��NLPIR_SaveTheUsrDic
*  Parameters : None
*  Returns    : bool, true:success, false:fail
*
*  Author     : Kevin Zhang
*  History    : 
*              1.create 2013/11/23
*********************************************************************/
NLPIR_API unsigned int  NLPIR_NWI_Result2UserDict();//�´�ʶ����תΪ�û��ʵ�,�����´ʽ����Ŀ
/*********************************************************************
*
*  Func Name  : NLPIR_FinerSegment(const char *sLine)
*
*  Description: ��ǰ���зֽ������ʱ���硰�л����񹲺͹���
*				��Ҫִ�иú��������зֽ��ϸ��Ϊ���л� ���� ���͹���
*				ϸ���������Ϊ��������
*  Parameters : sLine:������ַ���
*  Returns    : ���ص�ϸ�ִ����������ϸ�֣��򷵻�Ϊ���ַ���""
*
*  Author     : Kevin Zhang
*  History    : 
*              1.create 2014/10/10
*********************************************************************/
NLPIR_API const char*  NLPIR_FinerSegment(const char *sLine);
/*********************************************************************
*
*  Func Name  : NLPIR_GetEngWordOrign(const char *sWord)
*
*  Description: ��ȡ����Ӣ�ĵ��ʵ�ԭ�ͣ������˹�ȥ�ִʡ������������
*  Parameters : sWord:����ĵ���
*  Returns    : ���صĴ�ԭ����ʽ
*               driven->drive   drives->drive   drove-->drive
*  Author     : Kevin Zhang
*  History    : 
*              1.create 2014/12/11
*********************************************************************/
NLPIR_API const char*  NLPIR_GetEngWordOrign(const char *sWord);

/*********************************************************************
*
*  Func Name  : NLPIR_WordFreqStat(const char *sText)
*
*  Description: ��ȡ�����ı��Ĵʣ����ԣ�Ƶͳ�ƽ�������մ�Ƶ��С����
*  Parameters : sWord:������ı�����
*  Returns    : ���ص��Ǵ�Ƶͳ�ƽ����ʽ���£�
*				�Ż�ƽ/nr/10#��ʿ/n/9#�ִ�/n/8
*  Author     : Kevin Zhang
*  History    : 
*              1.create 2014/12/11
*********************************************************************/
NLPIR_API const char*  NLPIR_WordFreqStat(const char *sText);
/*********************************************************************
*
*  Func Name  : NLPIR_FileWordFreqStat(const char *sFilename)
*
*  Description: ��ȡ�����ı��Ĵʣ����ԣ�Ƶͳ�ƽ�������մ�Ƶ��С����
*  Parameters : sWord:������ı�����
*  Returns    : ���ص��Ǵ�Ƶͳ�ƽ����ʽ���£�
*				�Ż�ƽ/nr/10#��ʿ/n/9#�ִ�/n/8
*  Author     : Kevin Zhang
*  History    : 
*              1.create 2014/12/11
*********************************************************************/
NLPIR_API const char*  NLPIR_FileWordFreqStat(const char *sFilename);

#endif//#define __NLPIR_ICTCLAS_2014_H_INCLUDED__
