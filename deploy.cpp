/* SD -- square dance caller's helper.

    Copyright (C) 1990-1999  William B. Ackerman.

    This file is unpublished and contains trade secrets.  It is
    to be used by permission only and not to be disclosed to third
    parties without the express permission of the copyright holders.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    sdui-win.c - SD -- Microsoft Windows User Interface
  
    Copyright (C) 1995, Robert E. Cays
  
    Permission to use, copy, modify, and distribute this software for
    any purpose is hereby granted without fee, provided that the above
    copyright notice and this permission notice appear in all copies.
    The author makes no representations about the suitability of this
    software for any purpose.  It is provided "as is" WITHOUT ANY
    WARRANTY, without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.
  
    This is for version 32. */


#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "comdlg32")

#include "deploy.h"     // This takes the place of the Visual C++-generated resource.h.

enum state_type {
   CREATING_DIR,
   SAVING_OLD,
   SAVE_QUERYING,
   JUST_WRITING,
   FINISHED,
   FAILED};

state_type state;

char *file_list[] = {
   "sd.exe",
   "sdtty.exe",
   "mkcalls.exe",
   "sdlib.dll",
   "sd_calls.txt",
   "sd_calls.dat",
   "SD.lnk",
   "SD plain.lnk",
   "SDTTY.lnk",
   "SDTTY plain.lnk",
   "SD nocheckers.lnk",
   "SD couple.lnk",
   "Edit sd.ini.lnk",
   "Sd manual.lnk",
   "Release Notes.lnk",
   "sd_doc.html",
   "relnotes.html",
   "sample1.ini",
   "sample2.ini",
   "sample3.ini",
   "readme",
   (char *) 0};

char *save_list[] = {
   "sd.exe",
   "sdtty.exe",
   "mkcalls.exe",
   "sdlib.dll",
   "sd_calls.txt",
   "sd_calls.dat",
   (char *) 0};

char *shortcut_list[] = {
   "SD.lnk",
   "SD plain.lnk",
   "SDTTY.lnk",
   "SDTTY plain.lnk",
   "SD nocheckers.lnk",
   "SD couple.lnk",
   "Edit sd.ini.lnk",
   "Sd manual.lnk",
   "Release Notes.lnk",
   (char *) 0};

void do_install(HWND hwnd)
{
   char szStringBuf[1000];
   char szSysDirBuf[1000];
   char szSecondBuf[1000];
   char szShortcutBuf[1000];

   // Copy the required files.

   char **file_ptr;
   for (file_ptr = file_list ; *file_ptr ; file_ptr++) {
      lstrcpy(szStringBuf, "C:\\Sd\\");
      lstrcat(szStringBuf, *file_ptr);
                  
      if (!CopyFile(*file_ptr, szStringBuf, false)) {
         int foo = GetLastError();
         lstrcpy(szStringBuf, "ERROR!!  Can't copy file   ");
         lstrcat(szStringBuf, *file_ptr);
         lstrcat(szStringBuf, "\nThe installation has failed.");
         SetDlgItemText(hwnd, IDC_MAINCAPTION, szStringBuf );
         ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
         SetDlgItemText(hwnd, IDOK, "Exit");
         state = FAILED;
         return;
      }
   }

   UINT yyy = GetWindowsDirectory(szSysDirBuf, 999);
   if (yyy > 999 || yyy == 0) {
      SetDlgItemText(hwnd, IDC_MAINCAPTION,
                     "ERROR!!  Can't determine system directory.\n" \
                     "The installation has failed.");
      ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
      SetDlgItemText(hwnd, IDOK, "Exit");
      state = FAILED;
      return;
   }

   // Create Sd folder in start menu.

   DWORD zzz = GetEnvironmentVariable("USERPROFILE", szShortcutBuf, 999);
   if (zzz == 0) {
      // This isn't workstation NT.  Use the system directory.
      lstrcpy(szShortcutBuf, szSysDirBuf);
   }
   else if (zzz > 999) {
      SetDlgItemText(hwnd, IDC_MAINCAPTION,
                     "ERROR!!  Environment string too large.\n" \
                     "The installation has failed.");
      ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
      SetDlgItemText(hwnd, IDOK, "Exit");
      state = FAILED;
      return;
   }

   lstrcat(szShortcutBuf, "\\Start Menu\\Programs\\Sd");

   // See if the start folder exists.
   DWORD sd_att = GetFileAttributes(szShortcutBuf);
   if (sd_att == ~0UL || !(sd_att & FILE_ATTRIBUTE_DIRECTORY)) {
      // It doesn't exist -- create it.
      if (!CreateDirectory(szShortcutBuf, 0)) {
         SetDlgItemText(hwnd, IDC_MAINCAPTION,
                        "ERROR!!  Can't create Start Menu folder.\n" \
                        "The installation has failed.");
         ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
         SetDlgItemText(hwnd, IDOK, "Exit");
         state = FAILED;
         return;
      }
   }

   for (file_ptr = shortcut_list ; *file_ptr ; file_ptr++) {
      lstrcpy(szSecondBuf, szShortcutBuf);
      lstrcat(szSecondBuf, "\\");
      lstrcat(szSecondBuf, *file_ptr);
                  
      if (!CopyFile(*file_ptr, szSecondBuf, false)) {
         int foo = GetLastError();
         lstrcpy(szStringBuf, "ERROR!!  Can't copy file   ");
         lstrcat(szStringBuf, *file_ptr);
         lstrcat(szStringBuf, "\nThe installation has failed.");
         SetDlgItemText(hwnd, IDC_MAINCAPTION, szStringBuf );
         ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
         SetDlgItemText(hwnd, IDOK, "Exit");
         state = FAILED;
         return;
      }
   }

   // Display same in explorer.

   STARTUPINFO si;
   PROCESS_INFORMATION pi;

   (void) memset(&si, 0, sizeof(STARTUPINFO));
   (void) memset(&pi, 0, sizeof(PROCESS_INFORMATION));
   GetStartupInfo(&si);

   lstrcpy(szSecondBuf, szSysDirBuf);
   lstrcat(szSecondBuf, "\\explorer.exe ");
   lstrcat(szSecondBuf, szShortcutBuf);
   (void) CreateProcess(0, szSecondBuf, 0, 0, false, 0, 0, 0, &si, &pi);
   // Now pi has pi.hProcess, pi.hThread, dwProcessId, dwThreadId

   SetDlgItemText(hwnd, IDC_MAINCAPTION, "Installation complete.");
   ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
   SetDlgItemText(hwnd, IDOK, "Exit");
   state = FINISHED;
   return;
}



LRESULT CALLBACK MainWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   switch (iMsg) {
   case WM_COMMAND:
      if (wParam == IDOK) {
         switch (state) {
         case FAILED:
         case FINISHED:
            iMsg = WM_CLOSE;
            break;
         case CREATING_DIR:
            if (!CreateDirectory("C:\\Sd", 0)) {
               SetDlgItemText(hwnd, IDC_MAINCAPTION, "ERROR!!  Can't create C:\\Sd.\n" \
                              "The installation has failed.");
               ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
               SetDlgItemText(hwnd, IDOK, "Exit");
               state = FAILED;
               return 0;
            }

            do_install(hwnd);
            return 0;
         case SAVE_QUERYING:    // In querying mode, the "OK" button means overwrite.
         case JUST_WRITING:
            do_install(hwnd);
            return 0;
         }
      }
      else if (wParam == IDCANCEL) {
         iMsg = WM_CLOSE;
      }
      else if (wParam == IDC_BUTTON1) {
         if (state == SAVE_QUERYING) {
            char szCurDir[1000];
            (void) GetCurrentDirectory(999, szCurDir);

            // Put up the dialog box to get the save directory.

            char szSaveDir[1000];
            OPENFILENAME ofn;
            szSaveDir[0] = 0;
            (void) memset(&ofn, 0, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.lpstrInitialDir = "C:";
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = 0;
            ofn.lpstrFile = szSaveDir;
            ofn.nMaxFile = _MAX_PATH;
            ofn.lpstrFileTitle = 0;
            ofn.nMaxFileTitle = 0;
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT | OFN_HIDEREADONLY;
            ofn.lpstrDefExt = "";

            if (!GetSaveFileName(&ofn)) {
               SetDlgItemText(hwnd, IDC_MAINCAPTION, "ERROR!!  Can't get save location.\n" \
                              "The installation has failed.");
               ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
               SetDlgItemText(hwnd, IDOK, "Exit");
               state = FAILED;
               return 0;
            }

            // GetSaveFileName set the working directory.  We don't want that.  Set it back.
            (void) SetCurrentDirectory(szCurDir);

            if (!CreateDirectory(szSaveDir, 0)) {
               SetDlgItemText(hwnd, IDC_MAINCAPTION,
                              "ERROR!!  Can't create save folder.\n" \
                              "The installation has failed.");
               ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
               SetDlgItemText(hwnd, IDOK, "Exit");
               state = FAILED;
               return 0;
            }

            char szFromName[1000];
            char szToName[1000];
            char **file_ptr;

            for (file_ptr = save_list ; *file_ptr ; file_ptr++) {
               lstrcpy(szFromName, "C:\\Sd\\");
               lstrcat(szFromName, *file_ptr);
               lstrcpy(szToName, szSaveDir);
               lstrcat(szToName, "\\");
               lstrcat(szToName, *file_ptr);
               (void) CopyFile(szFromName, szToName, false);
            }

            do_install(hwnd);
            return 0;
         }

         return 0;
      }
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProc(hwnd, iMsg, wParam, lParam);
}



int WINAPI WinMain(
   HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   PSTR szCmdLine,
   int iCmdShow)
{
   MSG Msg;
   static char szAppName[] = "deploy";
   WNDCLASSEX wndclass;

   // Create and register the class for the main window.

   wndclass.cbSize = sizeof(wndclass);
   wndclass.style = CS_HREDRAW | CS_VREDRAW;
   wndclass.lpfnWndProc = MainWndProc;
   wndclass.cbClsExtra = 0;
   wndclass.cbWndExtra = DLGWINDOWEXTRA;
   wndclass.hInstance = hInstance;
   wndclass.hIcon = LoadIcon(hInstance, szAppName);
   wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
   wndclass.hbrBackground  = (HBRUSH) (COLOR_BTNFACE+1);
   wndclass.lpszMenuName = 0;
   wndclass.lpszClassName = szAppName;
   wndclass.hIconSm = wndclass.hIcon;
   RegisterClassEx(&wndclass);

   HWND hwnd = CreateDialog(hInstance, szAppName, 0, 0);

   ShowWindow(hwnd, iCmdShow);
   SetFocus(GetDlgItem(hwnd, IDOK));

   DWORD sd_att = GetFileAttributes("C:\\Sd");
   if (sd_att != ~0UL && (sd_att & FILE_ATTRIBUTE_DIRECTORY)) {
      char **file_ptr;

      for (file_ptr = save_list ; *file_ptr ; file_ptr++) {
         char szFilenameBuf[1000];
         lstrcpy(szFilenameBuf, "C:\\Sd\\");
         lstrcat(szFilenameBuf, *file_ptr);
         sd_att = GetFileAttributes(szFilenameBuf);
         if (sd_att != ~0UL) {
            // We have a pre-existing program.  Try to get the database version.

            char Buffer[200];
            DWORD dwNumRead;

            lstrcpy(szFilenameBuf, "The directory C:\\Sd exists, and has an Sd program.\n\n");

            HANDLE hFile = CreateFile(
               "C:\\Sd\\sd_calls.dat",
               GENERIC_READ,
               FILE_SHARE_READ,
               0,
               OPEN_EXISTING,
               FILE_ATTRIBUTE_NORMAL,
               0);

            if (hFile != INVALID_HANDLE_VALUE &&
                ReadFile(hFile, Buffer, 100, &dwNumRead, 0)) {
               int size = (((WORD) Buffer[8]) << 8) | ((WORD) Buffer[9]);
               Buffer[10+size] = 0;
               Buffer[40] = 0;    // In case of disaster.
               lstrcat(szFilenameBuf, "The database verion appears to be ");
               lstrcat(szFilenameBuf, &Buffer[10]);
               lstrcat(szFilenameBuf, ".\n\n");
            }

            (void) CloseHandle(hFile);

            lstrcat(szFilenameBuf, "Press \"Save old version\" to save the existing software ");
            lstrcat(szFilenameBuf, "before loading the new.\n\n");
            lstrcat(szFilenameBuf, "Press \"Overwrite\" to overwrite the existing software.");

            SetDlgItemText(hwnd, IDC_MAINCAPTION, szFilenameBuf);
            ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_SHOW);
            SetFocus(GetDlgItem(hwnd, IDC_BUTTON1));
            SetDlgItemText(hwnd, IDOK, "Overwrite");
            state = SAVE_QUERYING;
            goto getout;
         }
      }

      SetDlgItemText(hwnd, IDC_MAINCAPTION,
                     "The directory C:\\Sd exists, but has no Sd program.\n\n" \
                     "Press OK to install Sd and Sdtty there.");
      state = JUST_WRITING;
   getout: ;
   }
   else {
      SetDlgItemText(hwnd, IDC_MAINCAPTION,
                     "The directory C:\\Sd does not exist.\n\n" \
                     "Press OK to create the directory and install Sd and Sdtty there.");
      state = CREATING_DIR;
   }

   while (GetMessage(&Msg, NULL, 0, 0)) {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
   }

   return Msg.wParam;
}
