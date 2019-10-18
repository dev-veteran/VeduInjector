#include <windows.h>
#include <TlHelp32.h>
#include <iostream>

/* sources:
https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex
https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress
https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory
https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess
https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread
https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowthreadprocessid
https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-findwindowa
https://arvanaghi.com/blog/dll-injection-using-loadlibrary-in-C/
*/

DWORD processId; //variable for capsulation of process id.
LPCSTR targetWindowTitle = "TARGET:WINDOW:TITLE"; //window title variable. For example: 'Untitled - Notepad'

HMODULE kernel32 = GetModuleHandle("kernel32.dll"); //getting kernel32 module.
LPVOID method = GetProcAddress(kernel32, "LoadLibraryW"); //getting loadlibrary adress from kernel and equalize it to method.

void initialize(HANDLE process, const wchar_t* path) //'initialize' function to inject your dll into process.
{
	int length = wcslen(path) + 1; //gets the length of the string so we can know how much memory we need.

	LPVOID vAllocEx = VirtualAllocEx(process, NULL, length * 2, MEM_COMMIT, PAGE_EXECUTE); //allocating new memory for our dll from path.
	WriteProcessMemory(process, VirtualAllocEx, path, length * 2, NULL); //writeprocessmemory to write our dllpath into application.

	//CreateRemoteThread -> creates a thread that runs in the virtual address space of another process.
	HANDLE thread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)method, vAllocEx, NULL, NULL);
	//creating thread by using CreateRemoteThread function.

	WaitForSingleObject(thread, INFINITE); //waits until the specified object is in the signaled state or timeout elapses.
	CloseHandle(thread); //closes an open object handle.
}

int main()
{
	HWND applicationWindow = FindWindowA(NULL, targetWindowTitle);

	GetWindowThreadProcessId(applicationWindow, &processId); //getting processid from application window and send it to 'processId' variable.
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId); //getting full access from process.

	initialize(handle, L"C:\\FILE-NAME\\DLL-NAME.dll"); //calling 'initialize' to inject your dll. You need to change example directory with your own dll directory.
	return 0;
}
