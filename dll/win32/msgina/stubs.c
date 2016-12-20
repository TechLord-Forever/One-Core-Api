/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS msgina.dll
 * FILE:            lib/msgina/stubs.c
 * PURPOSE:         msgina.dll stubs
 * PROGRAMMER:      Thomas Weidenmueller (w3seek@users.sourceforge.net)
 * NOTES:           If you implement a function, remove it from this file
 * UPDATE HISTORY:
 *      24-11-2003  Created
 */

#include "msgina.h"

/*
 * @unimplemented
 */
DWORD WINAPI
ShellShutdownDialog(
    HWND   hParent,
    LPWSTR lpUsername,
    BOOL   bHideLogoff)
{
    UNREFERENCED_PARAMETER(hParent);
    UNREFERENCED_PARAMETER(lpUsername);
    UNREFERENCED_PARAMETER(bHideLogoff);

    /* Return values:
     * 0x00: Cancelled/Help
     * 0x01: Log off user
     * 0x02: Shutdown
     * 0x04: Reboot
     * 0x10: Standby
     * 0x40: Hibernate
     */
    UNIMPLEMENTED;
    return FALSE;
}


/*
 * @unimplemented
 */
BOOL WINAPI
WlxIsLockOk(
    PVOID pWlxContext)
{
    UNREFERENCED_PARAMETER(pWlxContext);

    UNIMPLEMENTED;
    return TRUE;
}


/*
 * @unimplemented
 */
VOID WINAPI
WlxLogoff(
    PVOID pWlxContext)
{
    UNREFERENCED_PARAMETER(pWlxContext);

    UNIMPLEMENTED;
}


/*
 * @unimplemented
 */
VOID WINAPI
WlxShutdown(
    PVOID pWlxContext,
    DWORD ShutdownType)
{
    UNREFERENCED_PARAMETER(pWlxContext);
    UNREFERENCED_PARAMETER(ShutdownType);

    UNIMPLEMENTED;
}


/*
 * @unimplemented
 */
BOOL WINAPI
WlxGetStatusMessage(
    PVOID pWlxContext,
    DWORD *pdwOptions,
    PWSTR pMessage,
    DWORD dwBufferSize)
{
    UNREFERENCED_PARAMETER(pWlxContext);
    UNREFERENCED_PARAMETER(pdwOptions);
    UNREFERENCED_PARAMETER(pMessage);
    UNREFERENCED_PARAMETER(dwBufferSize);

    UNIMPLEMENTED;
    return FALSE;
}


/*
 * @unimplemented
 */
BOOL WINAPI
WlxNetworkProviderLoad(
    PVOID pWlxContext,
    PWLX_MPR_NOTIFY_INFO pNprNotifyInfo)
{
    UNREFERENCED_PARAMETER(pWlxContext);
    UNREFERENCED_PARAMETER(pNprNotifyInfo);

    UNIMPLEMENTED;
    return FALSE;
}


/*
 * @unimplemented
 */
VOID WINAPI
WlxDisconnectNotify(
    PVOID pWlxContext)
{
    UNREFERENCED_PARAMETER(pWlxContext);

    UNIMPLEMENTED;
}


/*
 * @unimplemented
 */
BOOL WINAPI
WlxGetConsoleSwitchCredentials(
    PVOID pWlxContext,
    PVOID pCredInfo)
{
    UNREFERENCED_PARAMETER(pWlxContext);
    UNREFERENCED_PARAMETER(pCredInfo);

    UNIMPLEMENTED;
    return FALSE;
}
