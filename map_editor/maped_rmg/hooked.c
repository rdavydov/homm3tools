﻿#include "hooked.h"
#include "globals.h"
#include "gui.h"
#include "nt_undoc.h"
#include "rmgdialog_fix.h"
#include "rmg_postfix.h"

#include "traverse_funcs.h"

#include <h3mlib.h>

#include <hook_utils.h>

#include <windows.h>
#include <CommCtrl.h>
#include <string.h>
#include <stdio.h>

#pragma warning(disable:4996)

typedef BOOL (WINAPI *GetSaveFileNameA_t)(
  _Inout_  LPOPENFILENAMEA lpofn
);

typedef int (WINAPI *MessageBoxA_t)(
    _In_opt_  HWND hWnd,
    _In_opt_  LPCSTR lpText,
    _In_opt_  LPCSTR lpCaption,
    _In_      UINT uType
    );

typedef HWND (WINAPI *CreateDialogIndirectParamA_t)(
  _In_opt_  HINSTANCE hInstance,
  _In_      LPCDLGTEMPLATE lpTemplate,
  _In_opt_  HWND hWndParent,
  _In_opt_  DLGPROC lpDialogFunc,
  _In_      LPARAM lParamInit
);

typedef void (__stdcall *orig_get_terrain_type_for_new_zone_t)(void);
typedef void (__stdcall *orig_early_gen_func_t)(void);

NtClose_t				     orig_NtClose				= (NtClose_t)NULL;
GetSaveFileNameA_t           orig_GetSaveFileNameA = (GetSaveFileNameA_t)NULL;
MessageBoxA_t                orig_MessageBoxA    = (MessageBoxA_t)NULL;
CreateDialogIndirectParamA_t orig_CreateDialogIndirectParamA = (CreateDialogIndirectParamA_t)NULL;

orig_get_terrain_type_for_new_zone_t orig_get_terrain_type_for_new_zone = (orig_get_terrain_type_for_new_zone_t)0x0049B3C1;
orig_early_gen_func_t orig_early_gen_func = (orig_early_gen_func_t)0x004A218C;

static int f_player_count;
static int f_player_current;

//
// Hooked functions
//

static int WINAPI hooked_MessageBoxA(
    _In_opt_  HWND hWnd,
    _In_opt_  LPCSTR lpText,
    _In_opt_  LPCSTR lpCaption,
    _In_      UINT uType
    )
{
    int ret;

    if (NULL != strstr(lpText, "Unable"))
    {
        // Czech: Nelze vygenerovat
        // Maped RMG selhalo s tímto nastavením + šablony, budete muset zvolit jiné nastavení (nebo to zkuste znovu např. hodnota pro hráče bude náhodná.
        lpText = "Maped RMG failed with these settings + template, you will need to select other settings (or try again if e.g, player amount is random)";
    }

    ret = orig_MessageBoxA(hWnd, lpText, lpCaption, uType);
    OutputDebugStringA(lpText);
    if (NULL == strstr(lpText, "Failed to find GUI element") && NULL == strstr(lpText, "No templates"))
    {
        SendMessage(g_hwnd_main, 0x1337, 0, 0);
    }
    return ret;
}

HWND WINAPI hooked_CreateDialogIndirectParamA(
    _In_opt_  HINSTANCE hInstance,
    _In_      LPCDLGTEMPLATE lpTemplate,
    _In_opt_  HWND hWndParent,
    _In_opt_  DLGPROC lpDialogFunc,
    _In_      LPARAM lParamInit
    )
{
    HWND hwnd;
    BOOL fix_rmg = FALSE;

    if (NULL != lpTemplate && 35 == lpTemplate->cdit)
    {
        #define TITLE "Heroes of Might & Magic III - HD Edition"
        hWndParent = NULL;//FindWindowA("SDL_app", TITLE);

        if (NULL != hWndParent)
            OutputDebugStringA("Parent found");
        else
            OutputDebugStringA("2 Parent not found");
        fix_rmg = TRUE;
    }

    hwnd = orig_CreateDialogIndirectParamA(
        hInstance,
        lpTemplate,
        hWndParent,
        lpDialogFunc,
        lParamInit);
    
    __asm pushad
    if (FALSE != fix_rmg)
    {
        FixRMGDialog(hwnd);
    }
    __asm popad

    return hwnd;
}


BOOL WINAPI hooked_GetSaveFileNameA(
    _Inout_  LPOPENFILENAMEA lpofn
    )
{
    char dest[MAX_PATH] = { 0 };
    STARTUPINFOA si = { 0 };
    GetStartupInfoA(&si);
    if (NULL == si.lpReserved || 0 == strlen(si.lpReserved))
    {
        _snprintf(lpofn->lpstrFile, lpofn->nMaxFile - 1, "rm.h3m");
    }
    else
    {
        //BOOL ret = orig_GetSaveFileNameA(lpofn);
        OutputDebugStringA(lpofn->lpstrFile);
        OutputDebugStringA("q rmg dll");
        _snprintf(lpofn->lpstrFile, lpofn->nMaxFile - 1, "%s", si.lpReserved);
        char *delim = strchr(lpofn->lpstrFile, '|');
        if(NULL == delim)
        {
            //MessageBoxA(NULL, "No templates dir was sent to HD RMG integration, please make sure all h3mtool files are latest versions", "Error", MB_ICONERROR);
        }
        else
        {
            delim[0] = 0;
        }
        strcat(lpofn->lpstrFile, "\\data\\maps\\rmgtmp");
        OutputDebugStringA(lpofn->lpstrFile);
        //DeleteFileA(lpofn->lpstrFile);
#if 0
        OutputDebugStringA("rmg dll");
        _snprintf(lpofn->lpstrFile, lpofn->nMaxFile - 1, "%s\\data\\maps\\EN\\rm.h3m", si.lpReserved);
        OutputDebugStringA(lpofn->lpstrFile);
#endif
    }
    return TRUE;
    //ExitProcess(1337);
}


NTSTATUS NTAPI hooked_NtClose(
    _In_  HANDLE Handle
    )
{
    NTSTATUS ret;
    static BOOL disable_hook;
    char buf[MAX_PATH] = { 0 };
    char *c = NULL;
    char *mapsroot = NULL;

    if (FALSE == disable_hook 
        && 0 != GetFinalPathNameByHandleA(Handle, buf, sizeof(buf), 0))
    {
        if (NULL != (c = strstr(buf, "maps\\rmgtmp")) 
            && NULL == strstr(buf, "."))
        {
            OutputDebugStringA("qz @close for .h3m");
            ret = orig_NtClose(Handle);
            disable_hook = TRUE;
            rmg_postfix(buf);
            char *src = strdup(buf);
            // Remove rmgtmp at end ofpath
            sprintf(c, "maps\\");
            traverse_copy_map(buf, "*", src);
            free(src);
            disable_hook = FALSE;
            SendMessage(g_hwnd_main, 0x1338, 0, 0);
        }
        else
        {
            ret = orig_NtClose(Handle);
        }
    }
    else
    {
        ret = orig_NtClose(Handle);
    }
    
    return ret;
}

void __declspec(naked) hooked_get_terrain_type_for_new_zone(void)
{
    static int town;

    __asm PUSHAD
    if (-1 == g_selected_towns[f_player_current] || f_player_current > f_player_count)
    {
        ++f_player_current;
        __asm POPAD
        __asm JMP orig_get_terrain_type_for_new_zone
    }
    else
    {
        town = g_selected_towns[f_player_current];
        ++f_player_current;
        __asm POPAD
        __asm MOV EAX, town
    }

    // Function epilogue
    __asm RETN
}

// A function called early on in map generation, used to get amount of players
// and to reset current player to 0
void __declspec(naked) hooked_early_gen_func(void)
{
    static uint32_t _esp;
    
    // Call original function, preserving args and stack
    __asm MOV EAX, [ESP]
    __asm MOV _esp, EAX
    // eip -> eax
    __asm CALL get_eip
get_eip:
    __asm POP EAX
    __asm ADD EAX, 0x0D
    __asm MOV [ESP], EAX
    __asm JMP orig_early_gen_func

    // Save player count, set current player t o0
    __asm MOV EAX, [ESP - 0x40];
    __asm MOV f_player_count, EAX
    __asm MOV f_player_current, 0

    // Return to original caller
    __asm MOV EAX, _esp
    __asm PUSH EAX
    __asm RETN
}

static BOOL _inline_hook_function(const char *dll_name,
    const char *proc_name,
    void *new_proc,
    void **old_proc)
{
    if (NULL == (*old_proc = GetProcAddress(GetModuleHandleA(dll_name),
        proc_name)))
    {
        MessageBoxA(NULL, "GetProcAddress failed", dll_name,
            MB_OK | MB_ICONERROR);
        return FALSE;
    }

    if (FALSE == hook_trampoline_dis_x86(old_proc, new_proc))
    {
        MessageBoxA(NULL, "failed to hook", "gg", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}

void hooked_init(void)
{
    _inline_hook_function("user32.dll", "MessageBoxA", hooked_MessageBoxA, (void**)&orig_MessageBoxA);
    _inline_hook_function("user32.dll", "CreateDialogIndirectParamA", hooked_CreateDialogIndirectParamA, (void**)&orig_CreateDialogIndirectParamA);
    _inline_hook_function("comdlg32.dll", "GetSaveFileNameA", hooked_GetSaveFileNameA, (void**)&orig_GetSaveFileNameA);
    _inline_hook_function("ntdll.dll", "NtClose", hooked_NtClose, (void**)&orig_NtClose);

    hook_trampoline_dis_x86((void **)&orig_get_terrain_type_for_new_zone, hooked_get_terrain_type_for_new_zone);
    hook_trampoline_dis_x86((void **)&orig_early_gen_func, hooked_early_gen_func);

    // todo hook  - get_terrain_type_for_zone
}
