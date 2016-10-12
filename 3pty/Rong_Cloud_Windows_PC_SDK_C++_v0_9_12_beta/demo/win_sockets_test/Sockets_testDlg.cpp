
// Sockets_testDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Sockets_test.h"
#include "Sockets_testDlg.h"
#include "afxdialogex.h"

#include "rcsdk.h"
#include "json/json.h"
#include "json/reader.h"
#include "json/writer.h"
#include "txtutil.h"

#include "char2wchar.h"
#include <functional>
#include "json/value.h"
using namespace epius;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//using namespace epius;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

HINSTANCE hdll;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CSockets_testDlg 对话框

CSockets_testDlg::CSockets_testDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSockets_testDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CSockets_testDlg::~CSockets_testDlg()
{
	FreeLibrary(hdll);
}

void CSockets_testDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSockets_testDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON5, &CSockets_testDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON13, &CSockets_testDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON3, &CSockets_testDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CSockets_testDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, &CSockets_testDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CSockets_testDlg 消息处理程序

BOOL CSockets_testDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	hdll = LoadLibrary(L"rcsdk.dll");
	if (hdll == NULL)
	{
		FreeLibrary(hdll);
		MessageBox(L"LoadLibrary rcsdk.dll failed");
		return FALSE;
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSockets_testDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSockets_testDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSockets_testDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//连接服务器
void __stdcall connect_callback(const wchar_t* json_str)
{
	std::string str = txtutil::convert_wcs_to_utf8(json_str);
	Json::Reader reader;
	Json::Value jobj;
	if (reader.parse(str, jobj))  // reader将Json字符串解析到jobj将包含Json里所有子元素  
	{ 
		std::string result = jobj["result"].asString();
		std::string userId = jobj["userId"].asString();
		int err_code = jobj["err_code"].asInt();
	}
}

void __stdcall callback(const wchar_t* json_str)
{
	
}

//异常监听
void __stdcall exception_callback(const wchar_t* json_str)
{
	std::string str = txtutil::convert_wcs_to_utf8(json_str);
	Json::Reader reader;
	Json::Value jobj;
	if (reader.parse(str, jobj))  // reader将Json字符串解析到jobj将包含Json里所有子元素  
	{
		
	}
}

//消息监听
void __stdcall message_callback(const wchar_t* json_str)
{
	Json::Reader reader;
	Json::Value jobj;
	Json::Value msg_jobj;
	std::string str = txtutil::convert_wcs_to_utf8(json_str);
	if (reader.parse(str, jobj))  // reader将Json字符串解析到jobj将包含Json里所有子元素  
	{
		string ansi_msg_json_str = jobj["m_Message"].asString();
		if (reader.parse(ansi_msg_json_str,msg_jobj))
		{
			wstring content_str = txtutil::convert_utf8_to_wcs(msg_jobj["content"].asString());
		}
	}
}
/*
线上测试账号：
xl1:
token: 7V47hux2sxdmohiybibkaUmcbyeYIrXSDa0nFvL2mH/ktTByrE9l+qMFJwoIZV7RkVZcdq+5OgTBsvVJws7aQQ==
userid:77552

xl2:
token: M4fzpKAIT4lmohiybibkaUmcbyeYIrXSDa0nFvL2mH/ktTByrE9l+vpI9c07gsj5kVZcdq+5OgQOvMx9qBHkvA==
userid:77553

xl3:
token: ctHXYZPMZ/DkoiBwst9rRM2yq+hfEluLjZ78E1qo4hGEwEdzMQZ6WajBrPU6hjQcHM0YEh0kilaflH+9jL+4eQ==
userid:77554

泛微ky：
token： bOI/lGjKES9JNKBqOueR1kmcbyeYIrXSDa0nFvL2mH/xbYhB5dHSTJ1YUVahaQoAcTZzNsYUV47WfsYXpExsWw==
*/
void CSockets_testDlg::OnBnClickedButton5()
{
	// TODO:  在此添加控件通知处理程序代码
	TCHAR FilePath[MAX_PATH + 1] = { 0 };
	CUser cuse_;
	GetModuleFileName(NULL, FilePath, sizeof(FilePath)); //获取程序当前执行文件名
	std::string s = cuse_.WcharToChar(FilePath);
	s = s.substr(0, s.rfind("\\"));
	const char* pszDir = s.c_str();
	const wchar_t* ppszdir = cuse_.CharToWchar(pszDir);
	//初始化
	typedef int(*DLLFunc)(const char *appId, const char* appName, const char* deviceId, const wchar_t* localPath, const wchar_t* databasePath);
	DLLFunc InitClient;
	InitClient = (DLLFunc)GetProcAddress(hdll, "InitClient");
	if (InitClient == NULL)
	{
		return;
	}
	else
	{//"z3v5yqkbv8v30"(sheng chan)
		//e0x9wycfx7flq(xiaoqiao)
		InitClient("z3v5yqkbv8v30", "socket_test", "deviceId", ppszdir, ppszdir);
	}

	char szToken[] = {"7V47hux2sxdmohiybibkaUmcbyeYIrXSDa0nFvL2mH/ktTByrE9l+qMFJwoIZV7RkVZcdq+5OgTBsvVJws7aQQ==" };
	//char xiao_qiao[] = {"US/17YY8jTjQWzVp8DN5bo7LpJ9aWJrhdeMrxM9/KQYdcBK9KfCJ6/6jSLW2lkVYzETBGkiaBoAGojxr2QVjuw=="};
	//设置设备信息
	typedef int(*DLLFunca)(const char* manufacturer, const char* model, const char* osVersion, const char* network, const char* networkOperator);
	DLLFunca SetDeviceInfo;

	SetDeviceInfo = (DLLFunca)GetProcAddress(hdll, "SetDeviceInfo");
	if (SetDeviceInfo == NULL)
	{
		return;
	}
	else
	{
		SetDeviceInfo("Apple", "iPhone 6 Simulator", "8.1.1", "WIFI", "");
	}

	//连接服务器
	typedef int(*DLLFuncb)(const char* token, ConnectAckListenerCallback callback,bool ipv6);
	DLLFuncb Connect;
	Connect = (DLLFuncb)GetProcAddress(hdll, "Connect");
	if (Connect == NULL)
	{
		return;
	}
	auto connectCallback = [](const wchar_t* json_str)
	{
		Json::Reader reader;
		Json::Value jobj;
		std::string str = txtutil::convert_wcs_to_utf8(json_str);
		if (reader.parse(str, jobj))  // reader将Json字符串解析到jobj将包含Json里所有子元素  
		{
			std::string result = jobj["result"].asString();
			std::string userId = jobj["userId"].asString();
			int err_code = jobj["err_code"].asInt();
		}
	};
	Connect(szToken, connectCallback,false);

	//注册消息类型
	typedef int(*DLLFuncd)(const char* clazzName, const unsigned int operateBits);
	DLLFuncd RegisterMessageType;
	RegisterMessageType = (DLLFuncd)GetProcAddress(hdll, "RegisterMessageType");
	if (RegisterMessageType == NULL)
	{
		MessageBox(L"call function GetProcAddress failed");
		return;
	}
	else
	{
		//文本消息
		RegisterMessageType("RC:TxtMsg", 3);
		//图片消息
		RegisterMessageType("RC:ImgMsg", 3);
		//图文消息
		RegisterMessageType("RC:VcMsg", 3);
		//位置消息
		RegisterMessageType("RC:LBSMsg", 3);
		//添加联系人消息
		RegisterMessageType("RC:ContactNtf", 3);
		//提示条（小灰条）通知消息
		RegisterMessageType("RC:InfoNtf", 3);
		//资料通知消息
		RegisterMessageType("RC:ProfileNtf", 3);
		//通用命令通知消息
		RegisterMessageType("RC:CmdNtf", 3);
	}

	//设置消息监听
	typedef int(*DLLFunce)(MessageListenerCallback callback);
	DLLFunce SetMessageListener;
	SetMessageListener = (DLLFunce)GetProcAddress(hdll, "SetMessageListener");
	if (SetMessageListener == NULL)
	{
		return;
	}
	else
	{
		SetMessageListener(message_callback);
	}

	//设置网络异常监听
	typedef int(*DLLFuncf)(ExceptionListenerCallback callback);
	DLLFuncf SetExceptionListener;
	SetExceptionListener = (DLLFuncf)GetProcAddress(hdll, "SetExceptionListener");
	if (SetExceptionListener == NULL)
	{
		return;
	}
	else
	{
		SetExceptionListener(exception_callback);
	}
}

//发送消息监听
void __stdcall send_image_msg_callback(const wchar_t* json_str)
{
	Json::Reader reader;
	Json::Value jobj;
	std::string str = txtutil::convert_wcs_to_utf8(json_str);
	if (reader.parse(str, jobj))  // reader将Json字符串解析到jobj将包含Json里所有子元素  
	{

	}
}
//SendImageMessage
void CSockets_testDlg::OnBnClickedButton13()
{
	//原图上传
}

//distroychannel
void CSockets_testDlg::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	typedef int(*DLLFunc)(int);
	DLLFunc Disconnect;
	Disconnect = (DLLFunc)GetProcAddress(hdll, "Disconnect");
	if (Disconnect == NULL)
	{
		MessageBox(L"call function GetProcAddress failed");
		return;
	}
	else
	{
		Disconnect(2);
	}
	
}

//发送消息监听
void __stdcall send_msg_callback(const wchar_t* json_str)
{
	Json::Reader reader;
	Json::Value jobj;
	std::string str = txtutil::convert_wcs_to_utf8(json_str);
	if (reader.parse(str, jobj))  // reader将Json字符串解析到jobj将包含Json里所有子元素  
	{

	}
}

//sendmessage
void CSockets_testDlg::OnBnClickedButton4()
{
	// TODO:  在此添加控件通知处理程序代码
	Json::Value jobj;
	Json::FastWriter writer;
	//发送消息
	jobj["content"] = "hello 融云!";
	//发消息前存储消息
	int messageId = 0;
	typedef int(*DLLFuncb)(const char* targetId, int categoryId, const char* clazzName, const char* senderId, const wchar_t* message, const char* push, const char* appData);
	DLLFuncb SaveMessage;
	SaveMessage = (DLLFuncb)GetProcAddress(hdll, "SaveMessage");
	if (SaveMessage == NULL)
	{
		MessageBox(L"call function GetProcAddress failed");
		return;
	}
	else
	{
		messageId = SaveMessage("77553", 1, "RC:TxtMsg", "77553", txtutil::convert_ansi_to_wcs(writer.write(jobj)).c_str(), "", "");
	}

	typedef int(*DLLFunc)(const char* targetId, int categoryId, int transferType, const char* clazzname, const wchar_t* message, const char* push, const char* appData, int messageId, PublishAckListenerCallback callback);
	DLLFunc sendMessage;
	sendMessage = (DLLFunc)GetProcAddress(hdll, "sendMessage");
	if (sendMessage == NULL)
	{
		MessageBox(L"call function GetProcAddress failed");
		return;
	}
	else
	{
		auto sendMessageCallback = [](const wchar_t* json_str) 
		{
			Json::Reader reader;
			Json::Value jobj;
			std::string str = txtutil::convert_wcs_to_utf8(json_str);
			if (reader.parse(str, jobj))  // reader将Json字符串解析到jobj将包含Json里所有子元素  
			{

			}
		};
		sendMessage("77553", 1, 2, "RC:TxtMsg", txtutil::convert_ansi_to_wcs(writer.write(jobj)).c_str(), "", " ", messageId, sendMessageCallback);
	}
}

//registermessage
void CSockets_testDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	typedef int(*DLLFunc)(const char* clazzName, const unsigned int operateBits);
	DLLFunc RegisterMessageType;
	RegisterMessageType = (DLLFunc)GetProcAddress(hdll, "RegisterMessageType");
	if (RegisterMessageType == NULL)
	{
		MessageBox(L"call function GetProcAddress failed");
		return;
	}
	else
	{
		RegisterMessageType("RC:TxtMsg", 3);
	}
}

