/* -*- mode:C; c-basic-offset:3; indent-tabs-mode:nil; -*- */

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


static char *sdui_version = "4.5";


/* This file defines the following functions:
   uims_version_string
   uims_process_command_line
   uims_open_session
   uims_create_menu
   uims_get_startup_command
   uims_get_call_command
   uims_get_resolve_command
   uims_do_comment_popup
   uims_do_outfile_popup
   uims_do_getout_popup
   uims_do_abort_popup
   uims_do_neglect_popup
   uims_do_selector_popup
   uims_do_direction_popup
   uims_do_tagger_popup
   uims_get_number_fields
   uims_do_modifier_popup
   uims_add_new_line
   uims_reduce_line_count
   uims_begin_search
   uims_begin_reconcile_history
   uims_end_reconcile_history
   uims_update_resolve_menu
   uims_terminate
   uims_database_tick_max
   uims_database_tick
   uims_database_tick_end
   uims_database_error
   uims_fatal_error
   uims_bad_argument
   uims_final_exit
*/

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdlib.h>
#include <string.h>
#include "paths.h"
#include "database.h"
#include "sdui.h"
#include "sdmatch.h"
extern void initialize_concept_sublists(void);
extern void build_database(call_list_mode_t call_list_mode);
#include "resource.h"

#pragma comment(lib, "comctl32")

static int dammit = 0;

/* Privately defined message codes. */
/* The user-definable message codes begin at 1024, according to winuser.h.
   However, our resources use up numbers in that range (and, in fact,
   start at 1003, but that's Developer Studio's business).  So we start at 2000. */
#define CM_REINIT      2000
/* The special key codes start at 128 (due to the way Sdtty
   handles them), so we subtract 128 as we embed them into the
   Windows command user segment. */
#define SPECIAL_KEY_OFFSET (CM_REINIT-128+1)

// ***** Declared specially in sdsi.c!
FILE *call_list_file;

static char szMainWindowName[] = "Sd main window class";
static char szTranscriptWindowName[] = "Sd transcript window class";

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK TranscriptWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MyEditProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MyListProc(HWND, UINT, WPARAM, LPARAM);

static WNDPROC oldEditProc;
static WNDPROC oldListProc;

static int wherearewe;


#define EDIT_INDEX 99
#define LISTBOX_INDEX 98
#define PROGRESS_INDEX 96
#define TRANSCRIPT_INDEX 95
#define ACCEPT_INDEX 94
// Concocted index for user hitting <enter>.
#define ENTER_INDEX 93
#define ESCAPE_INDEX 92
#define STATUSBAR_INDEX 91


typedef struct {
   char Line [90];
   void *Next;
} DisplayType;


#define ui_undefined -999

extern int main(int argc, char *argv[]);

void ShowListBox(int WhichOne);


static char szComment         [MAX_TEXT_LINE_LENGTH];
static char szHeader          [MAX_TEXT_LINE_LENGTH];
static char szOutFilename     [MAX_TEXT_LINE_LENGTH];
static char szCallListFilename[MAX_TEXT_LINE_LENGTH];
static char szDatabaseFilename[MAX_TEXT_LINE_LENGTH];
static char szSequenceLabel   [MAX_TEXT_LINE_LENGTH];
static char szResolveWndTitle [MAX_TEXT_LINE_LENGTH];

static HINSTANCE GLOBhInstance;
static int GLOBiCmdShow;
static LRESULT Status;
static LOGFONT LogFont;
static CHOOSEFONT cf;

static HWND hwndMain;
static HWND hwndButton;
static HWND hwndEdit;
static HWND hwndList;
static HWND hwndProgress;
static HWND hwndTranscript;
static HWND hwndStatusBar;


static BOOL NewMenu = TRUE;
static BOOL MenuEnabled = FALSE;
static BOOL FileIsOpen = FALSE;
static BOOL WaitingForCommand;
static BOOL DontPrint;
static WORD ConceptListIndex[600];
static DWORD pdFlags;
static FILE *hInpFile;
static int nLastOne;
static int nSequenceNumber;
static int nDisplayWndWidth;
static int nDisplayWndHeight;
static int nVscrollPos;
static int nVscrollMax;
static int nVscrollInc;
static int nHscrollPos;
static int nHscrollMax;
static int nHscrollInc;
static int nLineCnt;
static int nYchar;
static int nXchar;
static int SpecialConceptMenu;
static int FontSize;
static int nConceptListLength;
static int nLevelConceptListLength;
static int nConceptTwice = -1;
static int nConceptSequential = -1;
static uims_reply MenuKind;
static DisplayType *DisplayRoot = NULL;
static DisplayType *CurDisplay = NULL;

/* These control how big we think the menu windows
   are and where we center the selected item. */
#define PAGE_LEN 31
#define LISTBOX_SCROLL_POINT 15


static int XVal = 10;
static int YVal = 30;
static int screensize = 42;   // Amount that we tell the scroll that we believe the screen holds
static int pagesize = 40;     // Amount we tell it to scroll if user clicks in dead scroll area
static int position = 0;
static int BottomFudge;
static int TranscriptTextHeight;
static int AnsiTextHeight;
static int TextWidth;
static int AnsiTextWidth;
static int SystemTextWidth;
static int SystemTextHeight;





static uims_reply my_reply;
static long_boolean my_retval;

static RECT CallsClientRect;



static void YTC(HWND hwnd)
{
   MessageBox(hwndMain, "Not yet available", "Under Construction", MB_OK);
}



extern void show_match(void)
{
}


static int dialog_menu_index;
static int dialog_menu_count;
static Cstring dialog_prompt;
static Cstring *dialog_menu;



static void check_text_change(HWND hListbox, HWND hEditbox, long_boolean doing_escape)
{
   char szLocalString[MAX_TEXT_LINE_LENGTH];
   int nLen;
   int nIndex;
   char *p;
   int matches;
   BOOL changed_editbox = FALSE;

   /* Find out what the edit box contains now. */

   GetWindowText(hEditbox, szLocalString, MAX_TEXT_LINE_LENGTH);
   nLen = lstrlen(szLocalString) - 1;    /* Location of last character. */

   for (nIndex=0 ; nIndex<=nLen ; nIndex++)
      szLocalString[nIndex] = tolower(szLocalString[nIndex]);

   if (doing_escape) nLen++;

   /* Only process stuff if it changed from what we thought it was.
      Otherwise we would unnecessarily process changes that weren't typed
      by the user but were merely the stuff we put in due to completion. */

   if (doing_escape) {
      matches = match_user_input(nLastOne, FALSE, FALSE);
      user_match = static_ss.result;
      p = static_ss.extended_input;
      if (*p) {
         changed_editbox = TRUE;

         while (*p) {
            szLocalString[nLen++] = tolower(*p++);
            szLocalString[nLen] = '\0';
         }

      }
   }
   else if (nLen >= 0 && lstrcmp(szLocalString, static_ss.full_input)) {
      char cCurChar = szLocalString[nLen];

      if (cCurChar == ' ' || cCurChar == '-') {
         lstrcpy(static_ss.full_input, szLocalString);
         static_ss.full_input[nLen] = '\0';
         /* extend only to one space or hyphen, inclusive */
         matches = match_user_input(nLastOne, FALSE, FALSE);
         user_match = static_ss.result;
         p = static_ss.extended_input;

         if (*p) {
            changed_editbox = TRUE;

            while (*p) {
               if (*p != ' ' && *p != '-') {
                  szLocalString[nLen++] = *p++;
                  szLocalString[nLen] = '\0';
               }
               else {
                  szLocalString[nLen++] = cCurChar;
                  szLocalString[nLen] = '\0';
                  goto pack_us;
               }
            }
         }
         else if (!static_ss.space_ok || matches <= 1) {
            szLocalString[nLen] = '\0';    /* Do *not* pack the character. */
            changed_editbox = TRUE;
         }
      }
   }
   else
      goto scroll_listbox;

 pack_us:

   lstrcpy(static_ss.full_input, szLocalString);
   for (p=static_ss.full_input ; *p ; p++)
      *p = tolower(*p);

   /* Write it back to the window. */

   if (changed_editbox) {
      SendMessage(hEditbox, WM_SETTEXT, 0, (LPARAM) szLocalString);
      /* This moves the cursor to the end of the text, apparently. */
      SendMessage(hEditbox, EM_SETSEL, MAX_TEXT_LINE_LENGTH - 1, MAX_TEXT_LINE_LENGTH - 1);
   }

 scroll_listbox:

   /* Search list box for match on current string */

   nIndex = SendMessage(hListbox, LB_SELECTSTRING, (WPARAM) -1, (LPARAM) szLocalString);

   if (nIndex >= 0) {
      /* If possible, scroll the list box so that
         current selection remains centered. */
      SendMessage(
         hListbox, LB_SETTOPINDEX,
         (nIndex > LISTBOX_SCROLL_POINT) ? nIndex - LISTBOX_SCROLL_POINT : 0,
         0);
   }
   else if (!szLocalString[0]) {
      /* No match and no string. */
      nIndex = 0;  /* Select first entry in list box */
      SendMessage(hListbox, LB_SETCURSEL, 0, 0L);
   }

   wherearewe = nIndex;
}


/* Returns TRUE if it acted on the keystroke. */
long_boolean ProcessSpecialKeystroke(int id)
{
   /* Look for special programmed keystrokes. */

   if (id >= SPECIAL_KEY_OFFSET+128) {
      modifier_block *keyptr;
      int nc = id - SPECIAL_KEY_OFFSET;
      if (nc < FCN_KEY_TAB_LOW || nc > FCN_KEY_TAB_LAST)
         return FALSE;      /* How'd this happen?  Ignore it. */

      keyptr = fcn_key_table_normal[nc-FCN_KEY_TAB_LOW];

      /* Check for special bindings like "delete line" or "page up".
         These always come from the main binding table, even if
         we are doing something else, like a resolve. */

      if (keyptr && keyptr->index < 0) {
         long_boolean deleted_letter = FALSE;
         int nCount;
         int nLen;
         int nIndex = 1;
         char szLocalString[MAX_TEXT_LINE_LENGTH];

         switch (keyptr->index) {
         case special_index_pageup:
            nIndex -= PAGE_LEN-1;
         case special_index_lineup:
            nIndex -= PAGE_LEN+1;
         case special_index_pagedown:
            nIndex += PAGE_LEN-1;
         case special_index_linedown:
            /* nIndex now tells how far we want to move forward or back in the menu.
               Change that to the absolute new position, by adding the old position. */
            nIndex += SendMessage(hwndList, LB_GETCURSEL, 0, 0);

            /* Clamp to the menu limits. */
            nCount = SendMessage(hwndList, LB_GETCOUNT, 0, 0) - 1;
            if (nIndex > nCount) nIndex = nCount;
            if (nIndex < 0) nIndex = 0;

            /* Select the new item. */
            SendMessage(hwndList, LB_SETCURSEL, nIndex, 0);
            return TRUE;
         case special_index_deleteword:
            GetWindowText(hwndEdit, szLocalString, MAX_TEXT_LINE_LENGTH);
            nLen = lstrlen(szLocalString);

            while (nLen > 0) {
               if (szLocalString[nLen-1] == ' ') {
                  if (deleted_letter) break;
               }
               else
                  deleted_letter = TRUE;

               nLen--;
               szLocalString[nLen] = '\0';
            }

            lstrcpy(static_ss.full_input, szLocalString);
            SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM) szLocalString);
            SendMessage(hwndEdit, EM_SETSEL, MAX_TEXT_LINE_LENGTH, MAX_TEXT_LINE_LENGTH);
            return TRUE;
         case special_index_deleteline:
            SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)"");  /* Erase the edit box. */
            static_ss.full_input[0] = '\0';
            static_ss.full_input_size = 0;
            return TRUE;
         case special_index_copytext:
            SendMessage(hwndEdit, WM_COPY, 0, 0);
            return TRUE;
         case special_index_cuttext:
            SendMessage(hwndEdit, WM_CUT, 0, 0);
            return TRUE;
         case special_index_pastetext:
            SendMessage(hwndEdit, WM_PASTE, 0, 0);
            return TRUE;
         }
      }

      /* No binding as "page up" or anything like that.  Look for menu-specific
         bindings like "split phantom boxes" or "find another". */

      if (nLastOne == match_startup_commands)
         keyptr = fcn_key_table_start[nc-FCN_KEY_TAB_LOW];
      else if (nLastOne == match_resolve_commands)
         keyptr = fcn_key_table_resolve[nc-FCN_KEY_TAB_LOW];
      else if (nLastOne < 0)
         return FALSE;   /* We are scanning for direction/selector/number/etc. */

      if (keyptr) {
         /* This function key specifies a user action. */
         user_match = static_ss.result;
         user_match.match = *keyptr;
         user_match.indent = FALSE;
         user_match.valid = TRUE;

         /* We have the fully filled in match item.
            Process it and exit from the command loop. */

         WaitingForCommand = FALSE;
         return TRUE;
      }
   }

   return FALSE;
}



static uint32 ctlbits = 0;


/* This is the WndProc for the "menu", that is, the stuff on the left side of the
   window.  It is actually a modeless dialog. */

#define MENUSIZEH 340
#define MENUSIZEV 514




static void MainWindow_OnDestroy(HWND hwnd)
{
   PostQuitMessage(0);
}



/* Process "about" window messages */

LRESULT WINAPI AboutWndProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message) {
   case WM_INITDIALOG:
      return TRUE;
   case WM_COMMAND:
      EndDialog(hDlg, TRUE);
      return TRUE;
   }
   return FALSE;
}



uint32 peoplecolors[8] = {
   RGB(0, 255, 255),
   RGB(255, 0, 255),
   RGB(0, 255, 255),
   RGB(255, 0, 255),
   RGB(0, 255, 255),
   RGB(255, 0, 255),
   RGB(0, 255, 255),
   RGB(255, 0, 255)};

uint32 couplecolors[8] = {
   RGB(255, 0, 255),
   RGB(255, 0, 255),
   RGB(0, 255, 0),
   RGB(0, 255, 0),
   RGB(0, 255, 255),
   RGB(0, 255, 255),
   RGB(255, 255, 0),
   RGB(255, 255, 0)};

static uint32 *colorlist;

// Margin, in pixels, around the top, right, and bottom of the transcript.
// That is, this is the amount of gray space from the edge of the black
// space to the edge of the client area.  That is, the client area as
// defined by Windows.  So we set these values to whatever makes the gray
// border look nice.  We don't define a left edge margin, because the left
// edge is against other things, rather than against the window edge.
#define TRANSCRIPT_TOPMARGIN 2
#define TRANSCRIPT_BOTMARGIN 0
#define TRANSCRIPT_RIGHTMARGIN 0

// Distance from bottom of list box to top of "Accept" button.
#define LIST_TO_BUTTON 8
// Distance from top of "Accept" button to bottom of screen.
#define BUTTONTOP 25

// Distance from the left edge of the client rectangle (as defined
// by Windows) to the start of the listbox/editbox/etc.
#define LEFTJUNKEDGE 8
#define RIGHTJUNKEDGE 13

#define EDITTOP 2
#define EDITTOLIST 3

// Height of progress bar.
#define PROGRESSHEIGHT 15
#define PROGRESSBOT 5

// Vertical margin between the text and the edge of the transcript area.
// That is, this is the margin in the black space.  It applies at both
// top and bottom, but the bottom margin may be greater than this because
// we display only an integral number of lines.
#define TVOFFSET 5
// Horizontal margin between the left edge of the black area and the start
// of the text.  We don't define a right margin -- we display as much as
// possible when a line is very long.
#define THOFFSET 5

static void Transcript_OnPaint(HWND hwnd)
{
   PAINTSTRUCT PaintStruct;
   DisplayType *DisplayPtr;
   int Y = TVOFFSET;
   int i;
   int limit = screensize+position;

   HDC PaintDC = BeginPaint(hwnd, &PaintStruct);

   if (limit > nLineCnt) limit = nLineCnt;

   SelectFont(PaintDC, GetStockObject(OEM_FIXED_FONT));
   (void) SetBkColor(PaintDC, RGB(0, 0, 0));
   (void) SetTextColor(PaintDC, RGB(255, 255, 255));

   for (i=0,DisplayPtr=DisplayRoot;
        i<limit && DisplayPtr;    // Just being sure.
        i++,DisplayPtr=DisplayPtr->Next) {

      if (i >= position) {
         int x = THOFFSET;
         const char *cp;

         for (cp = DisplayPtr->Line ; *cp ; cp++) {
            if (*cp == '\013') {
               int c1 = *++cp;
               int c2 = *++cp;
               char cc[3];
               cc[0] = ' ';

               TextOut(PaintDC, x, Y, cc, 1);
               x += TextWidth;

               if (!no_color)
                  (void) SetTextColor(PaintDC, colorlist[c1 & 7]);

               cc[0] = pn1[c1 & 7];
               cc[1] = pn2[c1 & 7];
               cc[2] = direc[c2 & 017];

               TextOut(PaintDC, x, Y, cc, 3);
               x += TextWidth*3;

               /* Set back to plain "white". */

               if (!no_color) {
                  if (!no_intensify)
                     (void) SetTextColor(PaintDC, RGB(255, 255, 255));
                  else
                     (void) SetTextColor(PaintDC, RGB(192, 192, 192));
               }
            }
            else {
               TextOut(PaintDC, x, Y, cp, 1);
               x += TextWidth;
            }
         }

         Y += TranscriptTextHeight;
      }
   }

   EndPaint(hwnd, &PaintStruct);
}


static void Transcript_OnScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
   int oldposition = position;
   int delta;
   SCROLLINFO Scroll;
   RECT ClientRect;

   switch (code) {
   case SB_TOP:
      position = 0;
      break;
   case SB_LINEUP:
      position--;
      break;
   case SB_LINEDOWN:
      position++;
      break;
   case SB_PAGEUP:
      position -= pagesize;
      break;
   case SB_PAGEDOWN:
      position += pagesize;
      break;
   case SB_THUMBPOSITION:
   case SB_THUMBTRACK:
      position = pos;
      break;
   default:
      return;
   }

   if (position > nLineCnt-screensize) position = nLineCnt-screensize;
   if (position < 0) position = 0;

   delta = position - oldposition;

   if (delta == 0) return;

   Scroll.cbSize = sizeof(SCROLLINFO);
   Scroll.fMask = SIF_POS;
   Scroll.nPos = position;

   SetScrollInfo(hwnd, SB_VERT, &Scroll, TRUE);

   GetClientRect(hwnd, &ClientRect);
   ClientRect.left += THOFFSET;
   ClientRect.top += TVOFFSET;
   ClientRect.bottom -= BottomFudge;

   (void) ScrollWindowEx(hwnd, 0, -TranscriptTextHeight*delta,
                         &ClientRect, &ClientRect, NULL, NULL, SW_ERASE | SW_INVALIDATE);

   if (delta > 0) {
      // Invalidate bottom part only.
      ClientRect.top += (screensize-delta)*TranscriptTextHeight;
   }
   else {
      // Invalidate top part only.
      ClientRect.bottom -= (screensize+delta)*TranscriptTextHeight;
   }

   InvalidateRect(hwnd, &ClientRect, TRUE);  // Be sure we erase the background.
}



/* Process get-text dialog box messages */

static int PopupStatus;
static char szPrompt[MAX_TEXT_LINE_LENGTH];
static char szTextEntryResult[MAX_TEXT_LINE_LENGTH];
LRESULT WINAPI TEXT_ENTRY_DIALOG_WndProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   int Len;

   switch (Message) {
   case WM_INITDIALOG:
      /* If we did this, it would set the actual window title
         SetWindowText(hDlg, "FOOBAR!"); */
      SetDlgItemText(hDlg, IDC_FILE_TEXT, szPrompt);
      return TRUE;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
      case IDC_FILE_ACCEPT:
         Len = SendDlgItemMessage (hDlg, IDC_FILE_EDIT, EM_LINELENGTH, 0, 0L);
         if (Len > MAX_TEXT_LINE_LENGTH - 1)
            Len = MAX_TEXT_LINE_LENGTH - 1;

         if (Len > 0) {
            GetWindowText(GetDlgItem (hDlg, IDC_FILE_EDIT),
                          szTextEntryResult, MAX_TEXT_LINE_LENGTH);
            PopupStatus = POPUP_ACCEPT_WITH_STRING;
         }
         else
            PopupStatus = POPUP_ACCEPT;

         EndDialog(hDlg, TRUE);
         return TRUE;
      case IDC_FILE_CANCEL:
      case IDCANCEL:
         PopupStatus = POPUP_DECLINE;
         EndDialog(hDlg, TRUE);
         return TRUE;
      }

      return FALSE;
   default:
      return FALSE;      /* We do *NOT* call the system default handler. */
   }
}


static int do_general_text_popup(Cstring prompt, char dest[])
{
   lstrcpy(szPrompt, prompt);
   DialogBox(GLOBhInstance, MAKEINTRESOURCE(IDD_TEXT_ENTRY_DIALOG),
             hwndMain, (DLGPROC) TEXT_ENTRY_DIALOG_WndProc);
   if (PopupStatus == POPUP_ACCEPT_WITH_STRING)
      lstrcpy(dest, szTextEntryResult);
   return PopupStatus;
}



int WINAPI WinMain(
   HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   PSTR szCmdLine,
   int iCmdShow)
{
   WNDCLASSEX wndclass;

   GLOBiCmdShow = iCmdShow;
   GLOBhInstance = hInstance;

   // Create and register the class for the main window.

   wndclass.cbSize = sizeof(wndclass);
   wndclass.style = CS_HREDRAW | CS_VREDRAW/* | CS_NOCLOSE*/;
   wndclass.lpfnWndProc = MainWndProc;
   wndclass.cbClsExtra = 0;
   wndclass.cbWndExtra = 0;
   wndclass.hInstance = hInstance;
   wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
   wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
   wndclass.hbrBackground  = (HBRUSH) (COLOR_BTNFACE+1);
   wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
   wndclass.lpszClassName = szMainWindowName;
   wndclass.hIconSm = wndclass.hIcon;

   RegisterClassEx(&wndclass);

   // Create and register the class for the transcript window.

   wndclass.cbSize = sizeof(wndclass);
   wndclass.style = 0;
   wndclass.lpfnWndProc = TranscriptWndProc;
   wndclass.cbClsExtra = 0;
   wndclass.cbWndExtra = 0;
   wndclass.hInstance = hInstance;
   wndclass.hIcon = NULL;
   wndclass.hCursor = NULL;
   wndclass.hbrBackground  = GetStockBrush(BLACK_BRUSH);
   wndclass.lpszMenuName = NULL;
   wndclass.lpszClassName = szTranscriptWindowName;
   wndclass.hIconSm = wndclass.hIcon;

   RegisterClassEx(&wndclass);

   InitCommonControls();

   hwndMain = CreateWindow(
      szMainWindowName, "Sd",
      WS_OVERLAPPEDWINDOW,
      10, 20, 780, 560,
      NULL, NULL, GLOBhInstance, NULL);

   if (!hwndMain)
      return 1;

   /* Now run the SD program.  The system-supplied variables "__argc"
      and "__argv" provide the predigested-as-in-traditional-C-programs
      command-line arguments. */

   return main(__argc, __argv);
}

BOOL MainWindow_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
   TEXTMETRIC tm;
   HDC hdc = GetDC(hwnd);
   SelectFont(hdc, GetStockObject(OEM_FIXED_FONT));
   GetTextMetrics(hdc, &tm);
   TextWidth = tm.tmAveCharWidth;
   TranscriptTextHeight = tm.tmHeight+tm.tmExternalLeading;
   SelectFont(hdc, GetStockObject(ANSI_VAR_FONT));
   GetTextMetrics(hdc, &tm);
   AnsiTextWidth = tm.tmAveCharWidth;
   AnsiTextHeight = tm.tmHeight+tm.tmExternalLeading;
   SelectFont(hdc, GetStockObject(SYSTEM_FONT));
   GetTextMetrics(hdc, &tm);
   SystemTextWidth = tm.tmAveCharWidth;
   SystemTextHeight = tm.tmHeight+tm.tmExternalLeading;
   ReleaseDC(hwnd, hdc);

   hwndEdit = CreateWindow("edit", NULL,
      /* We use "autoscroll" so that it will scroll if we type in too
         much text, but we don't put up a scroll bar with HSCROLL. */
      WS_CHILD/*|WS_VISIBLE*/|WS_BORDER|ES_LEFT|ES_AUTOHSCROLL,
      0, 0, 0, 0,
      hwnd, (HMENU) EDIT_INDEX,
      lpCreateStruct->hInstance, NULL);

   oldEditProc = (WNDPROC) SetWindowLong(hwndEdit, GWL_WNDPROC, (LONG) MyEditProc);

   hwndList = CreateWindow("listbox", NULL,
      WS_CHILD|LBS_NOTIFY|WS_VSCROLL|WS_BORDER,
      0, 0, 0, 0,
      hwnd, (HMENU) LISTBOX_INDEX,
      lpCreateStruct->hInstance, NULL);

   SendMessage(hwndList, WM_SETFONT, (WPARAM) GetStockObject(ANSI_VAR_FONT), 0);
   oldListProc = (WNDPROC) SetWindowLong(hwndList, GWL_WNDPROC, (LONG) MyListProc);

   hwndButton = CreateWindow("button", "Accept",
      WS_CHILD|BS_DEFPUSHBUTTON,
      0, 0, 0, 0,
      hwnd, (HMENU) ACCEPT_INDEX,
      lpCreateStruct->hInstance, NULL);

   SendMessage(hwndButton, WM_SETFONT, (WPARAM) GetStockObject(ANSI_VAR_FONT), 0);

   hwndProgress = CreateWindow(PROGRESS_CLASS, NULL,
      WS_CHILD|WS_CLIPSIBLINGS,
      0, 0, 0, 0,
      hwnd, (HMENU) PROGRESS_INDEX,
      lpCreateStruct->hInstance, NULL);

   hwndTranscript = CreateWindow(
      szTranscriptWindowName, NULL,
      WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS | WS_VSCROLL,
      0, 0, 0, 0,
      hwnd, (HMENU) TRANSCRIPT_INDEX,
      lpCreateStruct->hInstance, NULL);

   hwndStatusBar = CreateStatusWindow(
      WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|CCS_BOTTOM|SBARS_SIZEGRIP,
      "Ready???",
      hwnd,
      STATUSBAR_INDEX);

   if (!hwndProgress||!hwndButton||!hwndList||
       !hwndEdit||!hwndTranscript||!hwndStatusBar) {
      uims_fatal_error("Can't create windows", 0);
      uims_final_exit(0);
   }

   return TRUE;
}

void MainWindow_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
   /* We divide between the menu and the transcript at 40%. */
   int TranscriptEdge = 4*cx/10;

   SCROLLINFO Scroll;
   RECT ClientRect;
   RECT rWindow;
   int TranscriptXSize;
   int TranscriptYSize;
   int cyy;
   int Listtop;

   GetWindowRect(hwndStatusBar, &rWindow);
   cyy = rWindow.bottom - rWindow.top;
   cy -= cyy;    // Subtract the status bar height.
   MoveWindow(hwndStatusBar, 0, cy, cx, cyy, TRUE);

   TranscriptXSize = cx-TranscriptEdge-TRANSCRIPT_RIGHTMARGIN;
   TranscriptYSize = cy-TRANSCRIPT_BOTMARGIN-TRANSCRIPT_TOPMARGIN;

   // Y-coordinate of the top of the list box.
   Listtop = EDITTOP+21*SystemTextHeight/16+EDITTOLIST;

   MoveWindow(hwndEdit,
      LEFTJUNKEDGE, EDITTOP,
      TranscriptEdge-LEFTJUNKEDGE-RIGHTJUNKEDGE, 21*SystemTextHeight/16, TRUE);

   MoveWindow(hwndList,
      LEFTJUNKEDGE, Listtop,
      TranscriptEdge-LEFTJUNKEDGE-RIGHTJUNKEDGE, cy-LIST_TO_BUTTON-BUTTONTOP-Listtop, TRUE);

   MoveWindow(hwndButton,
      (TranscriptEdge/2)-5*AnsiTextWidth, cy-BUTTONTOP,
      10*AnsiTextWidth, 7*AnsiTextHeight/4, TRUE);

   MoveWindow(hwndProgress,
      LEFTJUNKEDGE, cy-PROGRESSHEIGHT-PROGRESSBOT,
      TranscriptEdge-LEFTJUNKEDGE-RIGHTJUNKEDGE, PROGRESSHEIGHT, TRUE);

   MoveWindow(hwndTranscript,
      TranscriptEdge, TRANSCRIPT_TOPMARGIN,
      TranscriptXSize, TranscriptYSize, TRUE);

   // Allow TVOFFSET amount of margin at both top and bottom.
   screensize = (TranscriptYSize-TVOFFSET-TVOFFSET)/TranscriptTextHeight;
   pagesize = screensize-2;
   if (pagesize < 2) pagesize = 2;
   BottomFudge = TranscriptYSize-TVOFFSET - screensize*TranscriptTextHeight;

   if (position+screensize > nLineCnt)
      position = nLineCnt-screensize;

   if (position < 0) position = 0;

   Scroll.cbSize = sizeof(SCROLLINFO);
   Scroll.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
   Scroll.nMin = 0;
   Scroll.nMax = nLineCnt-1;
   Scroll.nPage = screensize;
   Scroll.nPos = position;

   (void) SetScrollInfo(hwndTranscript, SB_VERT, &Scroll, TRUE);
   GetClientRect(hwnd, &ClientRect);
   // **** This is actually excessive.  Try to invalidate just the newly exposed stuff.
   InvalidateRect(hwnd, &ClientRect, TRUE);  // Be sure we erase the background.
}


static long_boolean accept_single_click = FALSE;
 
void MainWindow_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
   int i;
   int matches;
   int nMenuIndex;

   switch (id) {
   case ID_FILE_ABOUTSD:
      DialogBox(GLOBhInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, (DLGPROC) AboutWndProc);
      break;
   case EDIT_INDEX:
      if (codeNotify == EN_UPDATE)
         check_text_change(hwndList, hwndEdit, FALSE);
      break;
   case ESCAPE_INDEX:
      check_text_change(hwndList, hwndEdit, TRUE);
      break;
   case LISTBOX_INDEX:
      // See whether this an appropriate single-click of double-click.
      if (codeNotify != LBN_DBLCLK && (!accept_single_click || codeNotify != LBN_SELCHANGE))
         break;
      /* Fall Through! */
   case ENTER_INDEX:
   case ACCEPT_INDEX:
      /* Fell Through! */

      nMenuIndex = SendMessage(hwndList, LB_GETCURSEL, 0, 0L);

      /* If the user moves around in the call menu (listbox) while there is
         stuff in the edit box, and then types a CR, we need to clear the
         edit box, so that the listbox selection will be taken exactly.
         This is because the wandering around in the list box may have
         gone to something that has nothing to do with what was typed
         in the edit box.  We detect this condition by noticing that the
         listbox selection has changed from what we left it when we were
         last trying to make the list box track the edit box. */
      if (wherearewe != LB_ERR && wherearewe != nMenuIndex) {
         SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)"");
         static_ss.full_input[0] = '\0';
         static_ss.full_input_size = 0;
      }

      matches = match_user_input(nLastOne, FALSE, FALSE);
      user_match = static_ss.result;

      /* We forbid a match consisting of two or more "direct parse" concepts, such as
         "grand cross".  Direct parse concepts may only be stacked if they are followed
         by a call.  The "match.next" field indicates that direct parse concepts
         were stacked. */

      if ((matches == 1 || matches - static_ss.yielding_matches == 1 || user_match.exact) &&
          ((!user_match.match.packed_next_conc_or_subcall &&
            !user_match.match.packed_secondary_subcall) ||
           user_match.match.kind == ui_call_select ||
           user_match.match.kind == ui_concept_select)) {

         /* The matcher found an acceptable (and possibly quite complex)
            utterance.  Use it directly. */

         SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)"");  /* Erase the edit box. */
         WaitingForCommand = FALSE;
         return;
      }

      /* The matcher isn't happy.  If we got here because the user typed <enter>,
         that's not acceptable.  Just ignore it.  Unless, of course, the type-in
         buffer was empty and the user scrolled around, in which case the user
         clearly meant to accept the currently highlighted item. */

      if (id == ENTER_INDEX &&
          (static_ss.full_input[0] != '\0' || wherearewe == nMenuIndex)) break;

      /* Or if, for some reason, the menu isn't anywhere, we don't accept it.

      if (nMenuIndex == LB_ERR) break;

      /* But if the user clicked on "accept", or did an acceptable single- or
         double-click of a menu item, that item is clearly what she wants, so
         we use it. */
 
      i = SendMessage(hwndList, LB_GETITEMDATA, nMenuIndex, (LPARAM)0);
      user_match.match.index = LOWORD(i);
      user_match.match.kind = HIWORD(i);

   use_computed_match:

      user_match.match.packed_next_conc_or_subcall = 0;
      user_match.match.packed_secondary_subcall = 0;
      user_match.match.call_conc_options = null_options;
      user_match.real_next_subcall = (match_result *) 0;
      user_match.real_secondary_subcall = (match_result *) 0;

      SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)"");  /* Erase the edit box. */

      /* We have the needed info.  Process it and exit from the command loop.
            However, it's not a fully filled in match item from the parser.
            So we need to concoct a low-class match item. */

      if (nLastOne == match_number) {
      }
      else if (nLastOne == match_circcer) {
         user_match.match.call_conc_options.circcer =
            user_match.match.index+1;
      }
      else if (nLastOne >= match_taggers &&
               nLastOne < match_taggers+NUM_TAGGER_CLASSES) {
         user_match.match.call_conc_options.tagger =
            ((nLastOne-match_taggers) << 5)+user_match.match.index+1;
      }
      else {
         if (user_match.match.kind == ui_concept_select) {
            user_match.match.concept_ptr =
               &concept_descriptor_table[user_match.match.index];
         }
         else if (user_match.match.kind == ui_call_select) {
            user_match.match.call_ptr =
               main_call_lists[parse_state.call_list_to_use][user_match.match.index];
         }
      }

      WaitingForCommand = FALSE;
      break;

      /*
this will now do command_quit
unfortunately, it only does that if nLastOne is >= 0, that is,
we are querying for a real call.
   case ID_FILE_EXIT:
      if (MessageBox(hwnd, "Do you really want to quit?", "Confirmation", MB_YESNO) == IDYES)
         final_exit (0);
      break;
      */

   case ID_COMMAND_COPY_TEXT:
      SendMessage(hwndEdit, WM_COPY, 0, 0);
      break;
   case ID_COMMAND_CUT_TEXT:
      SendMessage(hwndEdit, WM_CUT, 0, 0);
      break;
   case ID_COMMAND_PASTE_TEXT:
      SendMessage(hwndEdit, WM_PASTE, 0, 0);
      break;
   default:
      if (nLastOne < 0) break;
      for (i=0 ; command_menu[i].command_name ; i++) {
         if (id == command_menu[i].resource_id) {
            user_match.match.index = i;
            user_match.match.kind = ui_command_select;
            goto use_computed_match;
         }
      }
   }
}


/* Return of FALSE means this is a "normal" keystroke, and we did not act on it.
   In that case, we have to be sure it goes to the edit box, not the list box. */
// TRUE means we did something.
// If we don't recognize it, return FALSE.
long_boolean CheckKeystroke(UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   int newparm = -99;

   switch (iMsg) {
   case WM_KEYDOWN:
   case WM_SYSKEYDOWN:
      if (wParam == VK_SHIFT)
         ctlbits |= 2;
      else if (wParam == VK_CONTROL)
         ctlbits |= 1;
      else { 
         if (wParam == 0x0C) {
            if (!(HIWORD(lParam) & KF_EXTENDED)) {
               if (wParam = 0x0C) {
                  newparm = 5-200;
               }
            }
         }
         else if (wParam >= VK_PRIOR && wParam <= VK_DELETE) {
            if (HIWORD(lParam) & KF_EXTENDED) {
               if (HIWORD(lParam) & KF_ALTDOWN) {
                  if (ctlbits == 0)      newparm = wParam-VK_PRIOR+1+AEKEY+SPECIAL_KEY_OFFSET;
                  else if (ctlbits == 1) newparm = wParam-VK_PRIOR+1+CAEKEY+SPECIAL_KEY_OFFSET;
               }
               else {
                  if (ctlbits == 0)      newparm = wParam-VK_PRIOR+1+EKEY+SPECIAL_KEY_OFFSET;
                  else if (ctlbits == 1) newparm = wParam-VK_PRIOR+1+CEKEY+SPECIAL_KEY_OFFSET;
                  else if (ctlbits == 2) newparm = wParam-VK_PRIOR+1+SEKEY+SPECIAL_KEY_OFFSET;
               }
            }
            else {
               switch (wParam) {
               case 0x2D: newparm = 0-200; break;
               case 0x23: newparm = 1-200; break;
               case 0x28: newparm = 2-200; break;
               case 0x22: newparm = 3-200; break;
               case 0x25: newparm = 4-200; break; // 5 is handled above
               case 0x27: newparm = 6-200; break;
               case 0x24: newparm = 7-200; break;
               case 0x26: newparm = 8-200; break;
               case 0x21: newparm = 9-200; break;
               }
            }
         }
         else if (wParam >= VK_F1 && wParam <= VK_F12) {
            if (HIWORD(lParam) & KF_ALTDOWN) {
               if (ctlbits == 0)      newparm = wParam-VK_F1+1+AFKEY+SPECIAL_KEY_OFFSET;
               else if (ctlbits == 1) newparm = wParam-VK_F1+1+CAFKEY+SPECIAL_KEY_OFFSET;
            }
            else {
               if (ctlbits == 0)      newparm = wParam-VK_F1+1+FKEY+SPECIAL_KEY_OFFSET;
               else if (ctlbits == 1) newparm = wParam-VK_F1+1+CFKEY+SPECIAL_KEY_OFFSET;
               else if (ctlbits == 2) newparm = wParam-VK_F1+1+SFKEY+SPECIAL_KEY_OFFSET;
            }
         }
         else if (wParam >= 'A' && wParam <= 'Z' && (HIWORD(lParam) & KF_ALTDOWN)) {
            // We take alt or ctl-alt letters as key presses.
            if (ctlbits & 1) newparm = wParam+CTLALTLET+SPECIAL_KEY_OFFSET;
            else             newparm = wParam+ALTLET+SPECIAL_KEY_OFFSET;
         }
         else if (wParam >= '0' && wParam <= '9') {
            if (HIWORD(lParam) & KF_ALTDOWN) {
               if (ctlbits == 0)      newparm = wParam-'0'+ALTDIG+SPECIAL_KEY_OFFSET;
               else if (ctlbits == 1) newparm = wParam-'0'+CTLALTDIG+SPECIAL_KEY_OFFSET;
            }
            else {
               if (ctlbits == 1) newparm = wParam-'0'+CTLDIG+SPECIAL_KEY_OFFSET;
            }
         }
      }
      break;
   case WM_KEYUP:
   case WM_SYSKEYUP:
      if (wParam == VK_SHIFT)
         ctlbits &= ~2;
      else if (wParam == VK_CONTROL)
         ctlbits &= ~1;
      break;
   case WM_CHAR:
   case WM_SYSCHAR:
      if (wParam >= ('A' & 0x1F) && wParam <= ('Z' & 0x1F)) {
         // We take ctl letters as characters.
         newparm = wParam+0x40+CTLLET+SPECIAL_KEY_OFFSET;
      }
      else if (wParam >= '0' && wParam <= '9') {
         // We deliberately throw away alt digits.
         // The default window proc will ding them, and we are grabbing
         // them through the KEYDOWN message, so we don't need them here.
         if (HIWORD(lParam) & KF_ALTDOWN)
            return TRUE;
      }

      break;
   }

   /* Now see whether this keystroke was a special "accelerator" key.
         If so, just do the command.
         If it was "<enter>", treat it as though we had double-clicked the selected
         menu item, or had clicked on "accept" in the menu.
         Otherwise, it goes back to the real window.  But be sure that window
         is the edit window, not the menu. */

   if (newparm != -99) {
      // Check first for special numeric keypad hit, indicated by a number
      // close to -200.  If no control or alt was pressed, treat it as a
      // plain digit.

      if (newparm < -150) {
         if (HIWORD(lParam) & KF_ALTDOWN) {
            if (ctlbits & 1) newparm += 200+CTLALTNKP+SPECIAL_KEY_OFFSET;
            else             newparm += 200+ALTNKP+SPECIAL_KEY_OFFSET;
         }
         else {
            if (ctlbits & 1) newparm += 200+CTLNKP+SPECIAL_KEY_OFFSET;
            else {
               SendMessage(hwndEdit, WM_CHAR, newparm+200+'0', lParam);
               return 1;
            }
         }
      }

      // If user hit an accelerator key, act on it.  This may fill in the
      // final parse stuff and set WaitingForCommand = FALSE.
      if (ProcessSpecialKeystroke(newparm))
         return TRUE;
      else
         return FALSE;
   }
   else if (wParam == '\r') {
      // If user hit "<enter>", act roughly as though she clicked the "ACCEPT" button.
      MainWindow_OnCommand(hwndMain, ENTER_INDEX, hwndList, 0);
      return TRUE;
   }

   return FALSE;
}

LRESULT CALLBACK MyEditProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   switch (iMsg) {
   case WM_CHAR:
      if (wParam == '\r') {
         PostMessage(hwndMain, WM_COMMAND, ENTER_INDEX, (LPARAM) hwndList);
         return 1;
      }
      else if (wParam == '\033') {
         PostMessage(hwndMain, WM_COMMAND, ESCAPE_INDEX, (LPARAM) hwndList);
         return 1;
      } 
      break;
   case WM_KEYDOWN:
      // These were already handled as "WM_CHAR" messages.
      // Don't let either kind of message get to the default windproc
      // when "enter" or "escape" is typed.
      if (wParam == '\r' || wParam == '\033') return 1;
      break;
   }

   if (CheckKeystroke(iMsg, wParam, lParam)) {
      return 1;
   }
   else if (iMsg == WM_KEYDOWN && (HIWORD(lParam) & KF_EXTENDED) &&
            (wParam == VK_PRIOR || wParam == VK_NEXT ||
             wParam == VK_UP || wParam == VK_DOWN)) {
      /* If user presses up/down/etc key while in the edit box,
            and there is no accelerator definition for it,
            she probably really wants to scroll, so we send the keystroke
            to the list box.  But we only do it for up/down stuff, not
            left/right/home/end.  The latter are meaningful in the edit box. */
      PostMessage(hwndList, iMsg, wParam, lParam);
      return 1;
   }
   else
      return CallWindowProc(oldEditProc, hwnd, iMsg, wParam, lParam);
}


LRESULT CALLBACK MyListProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   switch (iMsg) {
   case WM_KEYDOWN:
      /* If user presses up/down/etc key while in the list box,
         she probably really wants to scroll, so we leave the keystroke here. */
      if (wParam >= VK_PRIOR && wParam <= VK_DELETE && (HIWORD(lParam) & KF_EXTENDED))
         break;
      /* Fall Through! */
   case WM_CHAR:
   case WM_SYSCHAR:
   case WM_SYSKEYDOWN:
      /* Fell Through! */
      /* If a key is typed while the list box has the focus, it is
         obviously intended as input to the edit box.  Change the focus
         to the edit box and send the character to same. */
      SetFocus(hwndEdit);
      PostMessage(hwndEdit, iMsg, wParam, lParam);
      return 1;
   }

   return CallWindowProc(oldListProc, hwnd, iMsg, wParam, lParam);
}




LRESULT CALLBACK TranscriptWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   switch (iMsg) {
      HANDLE_MSG(hwnd, WM_PAINT, Transcript_OnPaint);
      HANDLE_MSG(hwnd, WM_VSCROLL, Transcript_OnScroll);
   case WM_LBUTTONDOWN:
      /* User clicked mouse in the transcript area.  Presumably this
         is because she wishes to use the up/down arrow keys for scrolling.
         Take the focus, so that we can do that. */
      SetFocus(hwndTranscript);
      break;
   case WM_KEYDOWN:
      /* User typed a key while the focus was in the transcript area.
         If it's an arrow key, scroll the window.  Otherwise, give the focus
         to the edit window and send the character there. */
      switch (wParam) {
      case VK_PRIOR:
         Transcript_OnScroll(hwnd, hwnd, SB_PAGEUP, 0);
         return 0;
      case VK_NEXT:
         Transcript_OnScroll(hwnd, hwnd, SB_PAGEDOWN, 0);
         return 0;
      case VK_UP:
         Transcript_OnScroll(hwnd, hwnd, SB_LINEUP, 0);
         return 0;
      case VK_DOWN:
         Transcript_OnScroll(hwnd, hwnd, SB_LINEDOWN, 0);
         return 0;
      default:
         /* Some other character.  It must be a mistake that we have the focus.
            The edit window is a much more plausible recipient.  Set the focus
            there and post the message. */
         SetFocus(hwndEdit);
         PostMessage(hwndEdit, iMsg, wParam, lParam);
         return 0;
      }

      break;
   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProc(hwnd, iMsg, wParam, lParam);
}



LRESULT CALLBACK MainWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   switch (iMsg) {
      HANDLE_MSG(hwnd, WM_DESTROY, MainWindow_OnDestroy);
      HANDLE_MSG(hwnd, WM_CREATE, MainWindow_OnCreate);
      HANDLE_MSG(hwnd, WM_SIZE, MainWindow_OnSize);
      HANDLE_MSG(hwnd, WM_COMMAND, MainWindow_OnCommand);
   case WM_SETFOCUS:
      SetFocus(hwndList);    // Is this right?
      return 0;
   }

   return DefWindowProc(hwnd, iMsg, wParam, lParam);
}




static void setup_level_menu(HWND hDlg)
{
   dance_level lev;

   SetDlgItemText(hDlg, IDC_START_CAPTION, "Choose a level");

   SendDlgItemMessage(hDlg, IDC_START_LIST, LB_RESETCONTENT, 0, 0L);

   for (lev=l_mainstream ; ; lev++) {
      Cstring this_string = getout_strings[lev];
      if (!this_string[0]) break;
      SendDlgItemMessage(hDlg, IDC_START_LIST, LB_ADDSTRING, 0, (LPARAM) this_string);
   }

   SendDlgItemMessage (hDlg, IDC_START_LIST, LB_SETCURSEL, 0, 0L);
}


/* This is the last title sent by the main program.  We add stuff to it. */
static char szMainTitle[MAX_TEXT_LINE_LENGTH];


void SetTitle(void)
{
   char szTitle[MAX_TEXT_LINE_LENGTH];

   lstrcpy(szTitle, szMainTitle);

   if (using_active_phantoms || allowing_all_concepts) {
      lstrcat(szTitle, "    [");

      if (using_active_phantoms)
         lstrcat(szTitle, "Active Phantoms Enabled");

      if (allowing_all_concepts) {
         if (using_active_phantoms) lstrcat(szTitle, ", ");
         lstrcat(szTitle, "All Concepts Enabled");
      }

      lstrcat(szTitle, "]");
   }

   SetWindowText(hwndMain, (LPSTR) szTitle);
}


extern void uims_set_window_title(char s[])
{
   lstrcpy(szMainTitle, "Sd ");
   lstrcat(szMainTitle, s);
   SetTitle();
}



extern char *new_outfile_string;
extern char *call_list_string;
extern long_boolean get_first_session_line(void);
extern long_boolean get_next_session_line(char *dest);
extern int process_session_info(Cstring *error_msg);
static long_boolean doing_level_dialog;



/* 0 for normal, 1 to delete session line, 2 to put up fatal error and exit. */
static int startup_retval;
/* These have the fatal error. */
static char session_error_msg1[200], session_error_msg2[200];

static HWND hDialogWindow;

/* Process Startup dialog box messages. */

static void Startup_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
   int i;
   int session_outcome = 0;
   Cstring session_error_msg;

   switch (id) {
   case IDC_WRITE_LIST:
   case IDC_WRITE_FULL_LIST:
   case IDC_ABRIDGED:
      /* User clicked on some call list option.  Enable and seed the file name. */
      EnableWindow(GetDlgItem(hwnd, IDC_CALL_LIST_NAME), TRUE);
      GetWindowText(GetDlgItem(hwnd, IDC_CALL_LIST_NAME),
                    szDatabaseFilename, MAX_TEXT_LINE_LENGTH);
      if (!szDatabaseFilename[0])
         SetDlgItemText(hwnd, IDC_CALL_LIST_NAME, "call_list");
      return;
   case IDC_NORMAL:
      EnableWindow(GetDlgItem(hwnd, IDC_CALL_LIST_NAME), FALSE);
      return;
   case IDC_USERDEFINED:
      /* User clicked on a special database file.  Enable and seed the file name. */
      EnableWindow(GetDlgItem(hwnd, IDC_DATABASE_NAME), TRUE);
      GetWindowText(GetDlgItem(hwnd, IDC_DATABASE_NAME),
                    szDatabaseFilename, MAX_TEXT_LINE_LENGTH);
      if (!szDatabaseFilename[0])
         SetDlgItemText(hwnd, IDC_DATABASE_NAME, "database");
      return;
   case IDC_DEFAULT:
      EnableWindow(GetDlgItem(hwnd, IDC_DATABASE_NAME), FALSE);
      return;
   case IDCANCEL:         /* User hit the "close window" thing in the upper right corner. */
   case IDC_START_CANCEL: /* User hit the "cancel" button. */
      EndDialog(hwnd, TRUE);
      uims_final_exit(0);
      return;
   case IDC_START_ACCEPT:

      /* User hit the "accept" button.  Read out all the information.
         But it's more complicated than that.  We sometimes do a two-stage
         presentation of this dialog, getting the session number and then
         the level.  So we may have to go back to the second stage.
         The variable "doing_level_dialog" tells what we were getting. */

      startup_retval = 0;
      i = SendDlgItemMessage(hwnd, IDC_START_LIST, LB_GETCURSEL, 0, 0L);

      if (!doing_level_dialog) {
         /* The user has just responded to the session selection.
            Figure out what to do.  We may need to go back and get the
            level. */

         session_index = i;

         /* If the user wants that session deleted, do that, and get out
            immediately.  Setting the number to zero will cause it to
            be deleted when the session file is updated during program
            termination. */

         if (IsDlgButtonChecked(hwnd, IDC_START_DELETE_SESSION_CHECKED)) {
            session_index = -session_index;
            startup_retval = 1;
            goto getoutahere;
         }

         /* Analyze the indicated session number. */

         session_outcome = process_session_info(&session_error_msg);

         if (session_outcome & 1) {
            /* We are not using a session, either because the user selected
               "no session", or because of some error in processing the
               selected session. */
            session_index = 0;
            sequence_number = -1;
         }

         /* If the level never got specified, either from a command line
            argument or from the session file, put up the level selection
            screen and go back for another round. */

         if (calling_level == l_nonexistent_concept) {
            setup_level_menu(hwnd);
            doing_level_dialog = TRUE;
            return;
         }
      }
      else {
         /* Either there was no session file, or there was a session
            file but the user selected no session or a new session.
            In the latter case, we went back and asked for the level.
            So now we have both, and we can proceed. */
         calling_level = (dance_level) i;
         (void) strncat(outfile_string, filename_strings[calling_level], MAX_FILENAME_LENGTH);
      }

      if (IsDlgButtonChecked(hwnd, IDC_ABRIDGED))
         glob_call_list_mode = call_list_mode_abridging;
      else if (IsDlgButtonChecked(hwnd, IDC_WRITE_LIST))
         glob_call_list_mode = call_list_mode_writing;
      else if (IsDlgButtonChecked(hwnd, IDC_WRITE_FULL_LIST))
         glob_call_list_mode = call_list_mode_writing_full;

      /* ****** Need to do this later! */

      if (session_outcome & 2)
         MessageBox(hwnd, session_error_msg, "Error", MB_OK);


      /* If the user specified a call list file, get the name. */

      if (glob_call_list_mode != call_list_mode_none) {

         /* This may have come from the command-line switches,
            in which case we already have the file name. */

         if (!call_list_string) {
            GetWindowText(GetDlgItem(hwnd, IDC_CALL_LIST_NAME),
                          szCallListFilename, MAX_TEXT_LINE_LENGTH);

            call_list_string = szCallListFilename;
         }

         if (open_call_list_file(call_list_string)) exit_program(1);
      }

      /* If user specified the output file during startup dialog, install that.
         It overrides anything from the command line. */

      GetWindowText(GetDlgItem(hwnd, IDC_OUTPUT_NAME),
                    szOutFilename, MAX_TEXT_LINE_LENGTH);

      if (szOutFilename[0])
         new_outfile_string = szOutFilename;

      /* Handle user-specified database file. */

      if (IsDlgButtonChecked(hwnd, IDC_USERDEFINED)) {
         GetWindowText(GetDlgItem(hwnd, IDC_DATABASE_NAME),
                       szDatabaseFilename, MAX_TEXT_LINE_LENGTH);
         database_filename = szDatabaseFilename;
      }

   getoutahere:

      EndDialog(hwnd, TRUE);
      return;
   }
}

static BOOL Startup_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
   hDialogWindow = hwnd;

   /* Select the default radio buttons. */

   CheckRadioButton(hwnd, IDC_NORMAL, IDC_ABRIDGED, IDC_NORMAL);
   CheckRadioButton(hwnd, IDC_DEFAULT, IDC_USERDEFINED, IDC_DEFAULT);

   /* Seed the various file names with the null string. */

   SetDlgItemText(hwnd, IDC_OUTPUT_NAME, "");
   SetDlgItemText(hwnd, IDC_CALL_LIST_NAME, "");
   SetDlgItemText(hwnd, IDC_DATABASE_NAME, "");

   /* Put up the session list or the level list,
      depending on whether a session file is in use. */

   SendDlgItemMessage(hwnd, IDC_START_LIST, LB_RESETCONTENT, 0, 0L);

   if (!get_first_session_line()) {
      char line[MAX_FILENAME_LENGTH];

      SetDlgItemText(hwnd, IDC_START_CAPTION, "Choose a session");

      while (get_next_session_line(line))
         SendDlgItemMessage(hwnd, IDC_START_LIST, LB_ADDSTRING, 0, (LPARAM) line);
      doing_level_dialog = FALSE;
      SendDlgItemMessage(hwnd, IDC_START_LIST, LB_SETCURSEL, 0, 0L);
   }
   else {
      setup_level_menu(hwnd);
      doing_level_dialog = TRUE;
   }

   return TRUE;
}


LRESULT WINAPI Startup_Dialog_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message) {
      HANDLE_MSG(hwnd, WM_INITDIALOG, Startup_OnInitDialog);
      HANDLE_MSG(hwnd, WM_COMMAND, Startup_OnCommand);
   default:
      return FALSE;      /* We do *NOT* call the system default handler. */
   }
}





extern long_boolean uims_open_session(int argc, char **argv)
{
   /* At this point the following may have been filled in from the
      command-line switches or the "[Options]" stuff in the initialization file:

         glob_call_list_mode [default = call_list_mode_none]
         calling_level       [default = l_nonexistent_concept]
         new_outfile_string  [default = (char *) 0, this is just a pointer]
         call_list_string    [default = (char *) 0], this is just a pointer]
         database_filename   [default = "sd_calls.dat", this is just a pointer]

         The startup screen may get more info.
   */

   /* Put up (and then take down) the dialog box for the startup screen. */

   DialogBox(GLOBhInstance, MAKEINTRESOURCE(IDD_START_DIALOG),
             hwndMain, (DLGPROC) Startup_Dialog_WndProc);

   if (calling_level == l_nonexistent_concept)
      calling_level = l_mainstream;   /* User really doesn't want to tell us the level. */

   if (new_outfile_string)
      (void) install_outfile_string(new_outfile_string);

   /* We now have the following things filled in:
      session_index
      glob_call_list_mode
   */

   starting_sequence_number = sequence_number;

   /* Put up the main window. */

   ShowWindow(hwndMain, GLOBiCmdShow);
   UpdateWindow(hwndMain);

   SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) "Reading database");

/*
   {    int a = 3; int b = 5;
      int foo = SendMessage(hwndStatusBar, SB_GETPARTS, 0, 0);
      int bar = SendMessage(hwndStatusBar, SB_SETPARTS, (WPARAM) 0, (LPARAM) &b);
      int bletch = SendMessage(hwndStatusBar, SB_GETPARTS, 0, 0);
   }
*/

   UpdateWindow(hwndStatusBar);

   prepare_to_read_menus();

   /* Opening the database sets up the values of
         abs_max_calls and max_base_calls.
         Must do before telling the uims so any open failure messages
         come out first. */

   if (open_database(session_error_msg1, session_error_msg2)) {
      uims_fatal_error(session_error_msg1, session_error_msg2);
      exit_program(1);
   }

   build_database(glob_call_list_mode);   /* This calls uims_database_tick_max,
                                             which calibrates the progress bar. */

   ShowWindow(hwndProgress, SW_SHOWNORMAL);

   SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) "Creating Menus");
   UpdateWindow(hwndStatusBar);

   /* This is the thing that takes all the time! */
   initialize_menus(glob_call_list_mode);

   /* If we wrote a call list file, that's all we do. */
   if (glob_call_list_mode == call_list_mode_writing ||
       glob_call_list_mode == call_list_mode_writing_full) {
      startup_retval = 1;
   }

   if (startup_retval == 1) {
      close_init_file();
      return TRUE;
   }
   else if (startup_retval == 2) {
      uims_fatal_error(session_error_msg1, session_error_msg2);
      exit_program(1);
   }

   initialize_concept_sublists();
   initialize_misc_lists();
   matcher_initialize();

   ShowWindow(hwndProgress, SW_HIDE);
   SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) "Processing Accelerator Keys");
   UpdateWindow(hwndStatusBar);

   {
      long_boolean save_allow = allowing_all_concepts;
      allowing_all_concepts = TRUE;

      /* Process the keybindings for user-definable calls, concepts, and commands. */

      if (open_accelerator_region()) {
         char q[INPUT_TEXTLINE_SIZE];
         while (get_accelerator_line(q))
            do_accelerator_spec(q);
      }
      else {
         Cstring *q;
         for (q = concept_key_table ; *q ; q++)
            do_accelerator_spec(*q);
      }

      allowing_all_concepts = save_allow;
   }

   close_init_file();

   use_escapes_for_drawing_people = TRUE;

   /* Install the pointy triangles. */

   if (!no_graphics)
      direc = "?\020?\021????\036?\037?????";

   if (color_by_couple)
      colorlist = couplecolors;
   else
      colorlist = peoplecolors;

   SetTitle();

   ShowWindow(hwndList, SW_SHOWNORMAL);
   ShowWindow(hwndEdit, SW_SHOWNORMAL);
   ShowWindow(hwndButton, SW_SHOWNORMAL);

   UpdateWindow(hwndMain);

   /* Initialize the display window linked list */

   DisplayRoot = get_mem(sizeof(DisplayType));
   DisplayRoot->Line[0] = -1;
   DisplayRoot->Next = NULL;
   CurDisplay = DisplayRoot;
   nLineCnt = 0;
   return FALSE;
}



/* Handle WM_DESTROY message */

void DoDestroy(HWND hWnd)
{
   PostQuitMessage(0);
}


/* Process Windows Messages */

void EnterMessageLoop(void)
{
   MSG Msg;

   user_match.valid = FALSE;
   static_ss.full_input[0] = '\0';
   static_ss.full_input_size = 0;
   WaitingForCommand = TRUE;

   while (GetMessage(&Msg, NULL, 0, 0) && WaitingForCommand) {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
   }

   if (WaitingForCommand)
      final_exit(Msg.wParam);   /* User closed the window. */
}


extern void uims_display_ui_intro_text(void)
{
}


extern void uims_display_help(void)
{
}



Private char version_mem[12];

extern char *
uims_version_string (void)
{
   sprintf (version_mem, "%swin", sdui_version);
   return version_mem;
}


extern void uims_process_command_line(int *argcp, char ***argvp)
{
}


void ShowListBox(int nWhichOne)
{
   if (nWhichOne != nLastOne) {
      HDC hDC;
      SIZE Size;
      int nLongest;
      int i;
      int menu_length;
      int *item;
      int nIndex = 1;

      nLastOne = nWhichOne;
      wherearewe = 0;

      SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);
      GetClientRect(hwndList, &CallsClientRect);
      hDC = GetDC(hwndList);
      SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
      nLongest = 0;

      if (nLastOne == match_number) {
         int iu;
         uims_reply kind;

         SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) "<number>");
         UpdateWindow(hwndStatusBar);

         kind = 0;

         for (iu=0 ; iu<NUM_CARDINALS; iu++) {
            Cstring name = cardinals[iu];
            int nMenuIndex;

            GetTextExtentPoint(hDC, name, lstrlen(name), &Size);
            if ((Size.cx > nLongest) && (Size.cx > CallsClientRect.right)) {
               SendMessage(hwndList, LB_SETHORIZONTALEXTENT, Size.cx, 0L);
               nLongest = Size.cx;
            }

            nMenuIndex = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) name);
            SendMessage(hwndList, LB_SETITEMDATA, nMenuIndex,
                        (LPARAM) MAKELONG(iu, kind));
         }
      }
      else if (nLastOne == match_circcer) {
         unsigned int iu;
         uims_reply kind;

         SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) "<circulate replacement>");
         UpdateWindow(hwndStatusBar);

         kind = 0;

         for (iu=0 ; iu<number_of_circcers ; iu++) {
            Cstring name = circcer_calls[iu]->menu_name;
            int nMenuIndex;

            GetTextExtentPoint(hDC, name, lstrlen(name), &Size);
            if ((Size.cx > nLongest) && (Size.cx > CallsClientRect.right)) {
               SendMessage(hwndList, LB_SETHORIZONTALEXTENT, Size.cx, 0L);
               nLongest = Size.cx;
            }

            nMenuIndex = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) name);
            SendMessage(hwndList, LB_SETITEMDATA, nMenuIndex,
                        (LPARAM) MAKELONG(iu, kind));
         }
      }
      else if (nLastOne >= match_taggers &&
               nLastOne < match_taggers+NUM_TAGGER_CLASSES) {
         int tagclass = nLastOne - match_taggers;

         unsigned int iu;
         uims_reply kind;

         SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) "<tagging call>");
         UpdateWindow(hwndStatusBar);

         kind = 0;

         for (iu=0 ; iu<number_of_taggers[tagclass] ; iu++) {
            Cstring name = tagger_calls[tagclass][iu]->menu_name;
            int nMenuIndex;

            GetTextExtentPoint(hDC, name, lstrlen(name), &Size);
            if ((Size.cx > nLongest) && (Size.cx > CallsClientRect.right)) {
               SendMessage(hwndList, LB_SETHORIZONTALEXTENT, Size.cx, 0L);
               nLongest = Size.cx;
            }

            nMenuIndex = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) name);
            SendMessage(hwndList, LB_SETITEMDATA, nMenuIndex,
                        (LPARAM) MAKELONG(iu, kind));
         }
      }
      else if (nLastOne == match_startup_commands) {
         Cstring *menu;
         uims_reply kind;

         SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) "<startup>");
         UpdateWindow(hwndStatusBar);

         kind = ui_start_select;
         menu = startup_commands;
         menu_length = NUM_START_SELECT_KINDS;

         for (i=0 ; i<menu_length ; i++) {
            Cstring name = menu[i];
            int nMenuIndex;

            GetTextExtentPoint(hDC, name, lstrlen(name), &Size);
            if ((Size.cx > nLongest) && (Size.cx > CallsClientRect.right)) {
               SendMessage(hwndList, LB_SETHORIZONTALEXTENT, Size.cx, 0L);
               nLongest = Size.cx;
            }

            nMenuIndex = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) name);
            SendMessage(hwndList, LB_SETITEMDATA, nMenuIndex,
                        (LPARAM) MAKELONG(i, kind));
         }
      }
      else if (nLastOne == match_resolve_commands) {
         Cstring *menu;
         uims_reply kind;

         kind = ui_resolve_select;
         menu = resolve_command_strings;
         menu_length = number_of_resolve_commands;

         for (i=0 ; i<menu_length ; i++) {
            Cstring name = menu[i];
            int nMenuIndex;

            GetTextExtentPoint(hDC, name, lstrlen(name), &Size);
            if ((Size.cx > nLongest) && (Size.cx > CallsClientRect.right)) {
               SendMessage(hwndList, LB_SETHORIZONTALEXTENT, Size.cx, 0L);
               nLongest = Size.cx;
            }

            nMenuIndex = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) name);
            SendMessage(hwndList, LB_SETITEMDATA, nMenuIndex,
                        (LPARAM) MAKELONG(i, kind));
         }
      }
      else if (nLastOne == match_directions) {
         Cstring *menu;
         uims_reply kind;

         SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) "<direction>");
         UpdateWindow(hwndStatusBar);

         kind = ui_special_concept;
         menu = &direction_names[1];
         menu_length = last_direction_kind;

         for (i=0 ; i<menu_length ; i++) {
            Cstring name = menu[i];
            int nMenuIndex;

            GetTextExtentPoint(hDC, name, lstrlen(name), &Size);
            if ((Size.cx > nLongest) && (Size.cx > CallsClientRect.right)) {
               SendMessage(hwndList, LB_SETHORIZONTALEXTENT, Size.cx, 0L);
               nLongest = Size.cx;
            }

            nMenuIndex = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) name);
            SendMessage(hwndList, LB_SETITEMDATA, nMenuIndex,
                        (LPARAM) MAKELONG(i, kind));
         }
      }
      else if (nLastOne == match_selectors) {
         Cstring *menu;
         uims_reply kind;

         SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) "<selector>");
         UpdateWindow(hwndStatusBar);

         kind = ui_special_concept;
         menu = selector_menu_list;
         menu_length = last_selector_kind;

         for (i=0 ; i<menu_length ; i++) {
            Cstring name = menu[i];
            int nMenuIndex;

            GetTextExtentPoint(hDC, name, lstrlen(name), &Size);
            if ((Size.cx > nLongest) && (Size.cx > CallsClientRect.right)) {
               SendMessage(hwndList, LB_SETHORIZONTALEXTENT, Size.cx, 0L);
               nLongest = Size.cx;
            }

            nMenuIndex = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) name);
            SendMessage(hwndList, LB_SETITEMDATA, nMenuIndex,
                        (LPARAM) MAKELONG(i, kind));
         }
      }
      else {
         uims_reply kind;

         SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) menu_names[nLastOne]);
         UpdateWindow(hwndStatusBar);

         kind = ui_call_select;

         for (i = 0; i < number_of_calls[nLastOne]; i++) {
            Cstring name = main_call_lists[nLastOne][i]->menu_name;
            int nMenuIndex;

            GetTextExtentPoint (hDC, name, lstrlen(name), &Size);
            if ((Size.cx > nLongest) && (Size.cx > CallsClientRect.right)) {
               SendMessage (hwndList, LB_SETHORIZONTALEXTENT, Size.cx, 0L);
               nLongest = Size.cx;
            }

            nMenuIndex = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) name);
            SendMessage(hwndList, LB_SETITEMDATA, nMenuIndex,
                        (LPARAM) MAKELONG(i, kind));
         }

         if (allowing_all_concepts) {
            item = concept_list;
            menu_length = concept_list_length;
         }
         else {
            item = level_concept_list;
            menu_length = level_concept_list_length;
         }

         for (i=0 ; i<menu_length ; i++) {
            Cstring name = concept_descriptor_table[item[i]].menu_name;
            int nMenuIndex;

            GetTextExtentPoint(hDC, name, lstrlen(name), &Size);
            if ((Size.cx > nLongest) && (Size.cx > CallsClientRect.right)) {
               SendMessage(hwndList, LB_SETHORIZONTALEXTENT, Size.cx, 0L);
               nLongest = Size.cx;
            }

            nMenuIndex = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) name);
            SendMessage(hwndList, LB_SETITEMDATA, nMenuIndex,
                        (LPARAM) MAKELONG(item[i], ui_concept_select));
         }

         for (i=0 ;  ; i++) {
            Cstring name = command_menu[i].command_name;
            int nMenuIndex;

            if (!name) break;
            GetTextExtentPoint(hDC, name, lstrlen(name), &Size);
            if ((Size.cx > nLongest) && (Size.cx > CallsClientRect.right)) {
               SendMessage(hwndList, LB_SETHORIZONTALEXTENT, Size.cx, 0L);
               nLongest = Size.cx;
            }

            nMenuIndex = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) name);
            SendMessage(hwndList, LB_SETITEMDATA, nMenuIndex,
                        (LPARAM) MAKELONG(i, ui_command_select));
         }
      }

      SendMessage (hwndList, WM_SETREDRAW, TRUE, 0L);
      InvalidateRect (hwndList, NULL, TRUE);

      ReleaseDC (hwndList, hDC);
      SendMessage (hwndList, LB_SETCURSEL, 0, (LPARAM) 0);
   }

   SetFocus(hwndEdit);
}




extern void uims_create_menu(call_list_kind cl)
{
}




extern uims_reply uims_get_startup_command(void)
{
   nLastOne = ui_undefined;
   MenuKind = ui_start_select;
   ShowListBox(match_startup_commands);

   EnterMessageLoop();

   uims_menu_index = user_match.match.index;

   if (uims_menu_index < 0)
      /* Special encoding from a function key. */
      uims_menu_index = -1-uims_menu_index;
   else if (user_match.match.kind == ui_command_select) {
      /* Translate the command. */
      uims_menu_index = (int) command_command_values[uims_menu_index];
   }

   return user_match.match.kind;
}


extern long_boolean uims_get_call_command(uims_reply *reply_p)
{
   if (allowing_modifications)
      parse_state.call_list_to_use = call_list_any;

   SetTitle();
   nLastOne = ui_undefined;    /* Make sure we get a new menu,
                                  in case concept levels were toggled. */
   MenuKind = ui_call_select;
   ShowListBox(parse_state.call_list_to_use);
   my_retval = FALSE;
   EnterMessageLoop();

   my_reply = user_match.match.kind;
   uims_menu_index = user_match.match.index;

   if (uims_menu_index < 0)
      /* Special encoding from a function key. */
      uims_menu_index = -1-uims_menu_index;
   else if (my_reply == ui_command_select) {
      /* Translate the command. */
      uims_menu_index = (int) command_command_values[uims_menu_index];
   }
   else if (my_reply == ui_special_concept) {
   }
   else {
      call_conc_option_state save_stuff = user_match.match.call_conc_options;
      there_is_a_call = FALSE;
      my_retval = deposit_call_tree(&user_match.match, (parse_block *) 0, 2);
      user_match.match.call_conc_options = save_stuff;
      if (there_is_a_call) {
         parse_state.topcallflags1 = the_topcallflags;
         my_reply = ui_call_select;
      }
   }

   *reply_p = my_reply;
   return my_retval;
}


extern uims_reply uims_get_resolve_command (void)
{
   SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) szResolveWndTitle);
   UpdateWindow(hwndStatusBar);
   nLastOne = ui_undefined;
   MenuKind = ui_resolve_select;
   ShowListBox(match_resolve_commands);
   my_retval = FALSE;
   EnterMessageLoop();

   if (user_match.match.index < 0)
      uims_menu_index = -1-user_match.match.index;   /* Special encoding from a function key. */
   else
      uims_menu_index = (int) resolve_command_values[user_match.match.index];

   return user_match.match.kind;
}



extern int uims_do_comment_popup(char dest[])
{
   if (do_general_text_popup("Enter comment:", dest) == POPUP_ACCEPT_WITH_STRING)
      return POPUP_ACCEPT_WITH_STRING;
   else
      return POPUP_DECLINE;
}


extern int uims_do_outfile_popup(char dest[])
{
   char myPrompt[MAX_TEXT_LINE_LENGTH];

   sprintf(myPrompt, "Sequence output file is \"%s\". Enter new name:", outfile_string);
   return do_general_text_popup(myPrompt, dest);
}


extern int uims_do_header_popup(char dest[])
{
   char myPrompt[MAX_TEXT_LINE_LENGTH];

   if (header_comment[0])
      sprintf(myPrompt, "Current title is \"%s\". Enter new title:", header_comment);
   else
      sprintf(myPrompt, "Enter new title:");

   return do_general_text_popup(myPrompt, dest);
}


extern int uims_do_getout_popup (char dest[])
{
   return do_general_text_popup("Sequence title:", dest);
}


#ifdef NEGLECT
extern int
uims_do_neglect_popup (char dest[])
{
   MessageBox(hwndMain, "uims_do_neglect_popup", "Message", MB_OK);
   return 0;
}
#endif


extern int uims_do_write_anyway_popup(void)
{
   if (MessageBox(hwndMain, "This sequence is not resolved.\n"
                  "Do you want to write it anyway?",
                  "Confirmation", MB_YESNO) == IDYES)
      return 1;
   else
      return 0;
}

extern int uims_do_delete_clipboard_popup(void)
{
   if (MessageBox(hwndMain, "There are calls in the clipboard.\n"
                  "Do you want to delete all of them?",
                  "Confirmation", MB_YESNO) == IDYES)
      return 1;
   else
      return 0;
}

extern int uims_do_session_init_popup(void)
{
   if (MessageBox(hwndMain, "You already have a session file.\n"
                  "Do you really want to delete it and start over?",
                  "Confirmation", MB_YESNO) == IDYES)
      return 1;
   else
      return 0;
}


extern int uims_do_abort_popup(void)
{
   if (MessageBox(hwndMain, "Do you really want to abort this sequence?",
                  "Confirmation", MB_YESNO) == IDYES)
      return 1;
   else
      return 0;
}


extern int uims_do_modifier_popup(Cstring callname, modify_popup_kind kind)
{
   char modifier_question[150];

   switch (kind) {
   case modify_popup_any:
      sprintf (modifier_question,
               "The \"%s\" can be replaced.", callname);
      break;
   case modify_popup_only_tag:
      sprintf (modifier_question,
               "The \"%s\" can be replaced with a tagging call.", callname);
      break;
   case modify_popup_only_circ:
      sprintf (modifier_question,
               "The \"%s\" can be replaced with a modified circulate-like call.", callname);
      break;
   default:
      lstrcpy (modifier_question, "Internal error: unknown modifier kind.");
   }

   if (MessageBox (hwndMain, modifier_question, "Do you want to replace it?", MB_YESNO) == IDYES)
      return 1;
   else
      return 0;
}

extern int uims_do_selector_popup(void)
{
   int retval;

   if (!user_match.valid || (user_match.match.call_conc_options.who == selector_uninitialized)) {
      match_result saved_match = user_match;
      uims_reply SavedMenuKind = MenuKind;
      uims_reply SavedMy_reply = my_reply;

      nLastOne = ui_undefined;
      MenuKind = ui_call_select;
      ShowListBox((int) match_selectors);
      EnterMessageLoop();

      MenuKind = SavedMenuKind;
      my_reply = SavedMy_reply;

      /* We skip the zeroth selector, which is selector_uninitialized. */
      retval = user_match.match.index+1;
      user_match = saved_match;
   }
   else {
      retval = (int) user_match.match.call_conc_options.who;
      user_match.match.call_conc_options.who = selector_uninitialized;
   }
   return retval;
}


extern int uims_do_direction_popup(void)
{
   int retval;

   if (!user_match.valid ||
       (user_match.match.call_conc_options.where == direction_uninitialized)) {
      match_result saved_match = user_match;
      uims_reply SavedMenuKind = MenuKind;
      uims_reply SavedMy_reply = my_reply;

      nLastOne = ui_undefined;
      MenuKind = ui_call_select;
      ShowListBox((int) match_directions);
      EnterMessageLoop();

      MenuKind = SavedMenuKind;
      my_reply = SavedMy_reply;

      /* We skip the zeroth direction, which is direction_uninitialized. */
      retval = user_match.match.index+1;
      user_match = saved_match;
   }
   else {
      retval = (int) user_match.match.call_conc_options.where;
      user_match.match.call_conc_options.where = direction_uninitialized;
   }
   return retval;
}



extern int uims_do_circcer_popup(void)
{
   uint32 retval;

   if (interactivity == interactivity_verify) {
      retval = verify_options.circcer;
      if (retval == 0) retval = 1;
   }
   else if (!user_match.valid || (user_match.match.call_conc_options.circcer == 0)) {
      match_result saved_match = user_match;
      uims_reply SavedMenuKind = MenuKind;
      uims_reply SavedMy_reply = my_reply;

      nLastOne = ui_undefined;
      MenuKind = ui_call_select;
      ShowListBox((int) match_circcer);
      EnterMessageLoop();

      MenuKind = SavedMenuKind;
      my_reply = SavedMy_reply;
      retval = user_match.match.call_conc_options.circcer;
      user_match = saved_match;
   }
   else {
      retval = user_match.match.call_conc_options.circcer;
      user_match.match.call_conc_options.circcer = 0;
   }

   return retval;
}



extern int uims_do_tagger_popup(int tagger_class)
{
   int retval;

   if (!user_match.valid ||
       (user_match.match.call_conc_options.tagger == 0)) {
      match_result saved_match = user_match;
      uims_reply SavedMenuKind = MenuKind;
      uims_reply SavedMy_reply = my_reply;

      nLastOne = ui_undefined;
      MenuKind = ui_call_select;
      ShowListBox(((int) match_taggers) + tagger_class);
      EnterMessageLoop();

      MenuKind = SavedMenuKind;
      my_reply = SavedMy_reply;
      saved_match.match.call_conc_options.tagger = user_match.match.call_conc_options.tagger;
      user_match = saved_match;
   }

   retval = user_match.match.call_conc_options.tagger;
   user_match.match.call_conc_options.tagger = 0;
   return retval;
}


extern uint32 uims_get_number_fields(int nnumbers, long_boolean forbid_zero)
{
   int i;
   uint32 number_fields = user_match.match.call_conc_options.number_fields;
   int howmanynumbers = user_match.match.call_conc_options.howmanynumbers;
   uint32 number_list = 0;

   for (i=0 ; i<nnumbers ; i++) {
      uint32 this_num;

      if (!user_match.valid || (howmanynumbers <= 0)) {
         match_result saved_match = user_match;
         uims_reply SavedMenuKind = MenuKind;
         uims_reply SavedMy_reply = my_reply;
         nLastOne = ui_undefined;
         MenuKind = ui_call_select;
         ShowListBox((int) match_number);
         EnterMessageLoop();

         MenuKind = SavedMenuKind;
         my_reply = SavedMy_reply;
         this_num = user_match.match.index;
         user_match = saved_match;
      }
      else {
         this_num = number_fields & 0xF;
         number_fields >>= 4;
         howmanynumbers--;
      }

      if (forbid_zero && this_num == 0) return ~0;
      if (this_num > 15) return ~0;    /* User gave bad answer. */
      number_list |= (this_num << (i*4));
   }

   return number_list;
}


static void Update_text_display(void)
{
   SCROLLINFO Scroll;
   RECT ClientRect;

   position = nLineCnt-screensize;
   if (position < 0) position = 0;

   Scroll.cbSize = sizeof(SCROLLINFO);
   Scroll.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
   Scroll.nMin = 0;
   Scroll.nMax = nLineCnt-1;
   Scroll.nPage = screensize;
   Scroll.nPos = position;

   SetScrollInfo(hwndTranscript, SB_VERT, &Scroll, TRUE);
   GetClientRect(hwndTranscript, &ClientRect);
   InvalidateRect(hwndTranscript, &ClientRect, TRUE);  // Be sure we erase the background.
}


extern void uims_add_new_line(char the_line[])
{
   lstrcpy(CurDisplay->Line, the_line);

   if (!CurDisplay->Next) {
      CurDisplay->Next = get_mem(sizeof (DisplayType));
      CurDisplay = CurDisplay->Next;
      CurDisplay->Next = NULL;
   }
   else
      CurDisplay = CurDisplay->Next;

   CurDisplay->Line[0] = -1;
   nLineCnt++;

   Update_text_display();
}


extern void uims_reduce_line_count(int n)
{
   CurDisplay = DisplayRoot;
   nLineCnt = n;
   while (CurDisplay->Line[0] != -1 && n--) {
      CurDisplay = CurDisplay->Next;
   }

   CurDisplay->Line[0] = -1;

   Update_text_display();
}


extern void uims_terminate (void)
{
}


extern void uims_begin_search(command_kind goal)
{
}

extern int uims_begin_reconcile_history (int currentpoint, int maxpoint)
{
   return FALSE;
}


extern int uims_end_reconcile_history (void)
{
   return FALSE;
}



extern void uims_update_resolve_menu(command_kind goal, int cur, int max,
                                     resolver_display_state state)
{
   create_resolve_menu_title(goal, cur, max, state, szResolveWndTitle);
   SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM) szResolveWndTitle);
   UpdateWindow(hwndStatusBar);
}


extern void uims_database_tick_max(int n)
{
   SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELONG(0, n));
   SendMessage(hwndProgress, PBM_SETSTEP, 1, 0);
}


extern void uims_database_tick(int n)
{
   SendMessage(hwndProgress, PBM_SETSTEP, n, 0);
   SendMessage(hwndProgress, PBM_STEPIT, 0, 0);
}


extern void uims_database_tick_end(void)
{
}


extern void uims_database_error(Cstring message, Cstring call_name)
{
   MessageBox(hwndMain, call_name, message, MB_OK);
   uims_final_exit (0);
}


extern void uims_bad_argument(Cstring s1, Cstring s2, Cstring s3)
{
   /* We don't seem to look at the args.  What a shame. */
   uims_fatal_error("Bad Command Line Argument", 0);
   uims_final_exit (0);
}


extern void uims_fatal_error(Cstring pszLine1, Cstring pszLine2)
{
   session_index = 0;    /* We don't write back the session file in this case. */

   if (pszLine2 && pszLine2[0]) {
      char msg[200];
      sprintf(msg, "%s: %s", pszLine1, pszLine2);
      pszLine1 = msg;   /* Yeah, we can do that.  Yeah, it's sleazy. */
   }

   MessageBox(hwndMain, pszLine1, "Internal Error", MB_OK);
}


extern void uims_final_exit(int code)
{
   if (hwndMain)
      SendMessage(hwndMain, WM_CLOSE, 0, 0L);

   ExitProcess(code);
}
