/*
 * PROJECT:         ReactOS WMI driver
 * COPYRIGHT:       GPL - See COPYING in the top level directory
 * FILE:            drivers/bootanim/main.c
 * PURPOSE:         Windows Management Intstrumentation
 * PROGRAMMERS:     Aleksey Bragin (aleksey@reactos.org)
 *                  
 */

/* INCLUDES *****************************************************************/

#include <stdio.h>
#include <ntddk.h>
#include <wmilib.h>

#define NDEBUG
#include <debug.h>
#include <ntimage.h>
#include <inbvfuncs.h>
#include <bootvid.h>
#include <ldrfuncs.h>
#include <ntifs.h>
#include <exfuncs.h>
#include <lpctypes.h>
//#include <winternl.h>

/* GLOBALS ******************************************************************/

typedef int BOOL;

PVOID gptiCurrent;
PVOID gDwmApiPort;
PERESOURCE gpresUser;
PEPROCESS gpepDwm;
//POBJECT_TYPE LpcPortObjectType;
BOOL gfCompositing;
HANDLE ghDwmApiPort;

/* FUNCTIONS ****************************************************************/

PVOID NTAPI EnterCrit()
{
  PVOID result; // eax@1

  result = ExEnterCriticalRegionAndAcquireResourceExclusive(gpresUser);
  gptiCurrent = result;
  return result;
}

void NTAPI LeaveCrit()
{
  ExReleaseResourceAndLeaveCriticalRegion(gpresUser);
}

NTSTATUS NTAPI xxxDwmStartup()
{
  NTSTATUS status; // edi@1

  status = 0;
  if ( gfCompositing )
  {
    status = 0xC0000001u;
  }
  else
  {
    DwmHwndValidationBeginStartup();
    if ( dword_BF9CDB08 )
      StopFade();
    DwmNotifyChildrenAddRemove(1);
    bSetDevDragRect(*(_DWORD *)gpDispInfo, 0, 0);
    if ( GreDwmStartup(*(_DWORD *)gpDispInfo) )
    {
      gfCompositing = 1;
      *(_DWORD *)(gpsi + 1836) = 1;
      sub_BF87F242();
      xxxComposeDesktop(grpdeskRitInput, 1);
    }
    else
    {
      DwmNotifyChildrenAddRemove(0);
      status = 0xC0000001u;
    }
  }
  return status;
}

BOOL NTAPI NtUserDwmStartup(HANDLE Object)
{
  ULONG error; // eax@14
  BOOL resp; // esi@1
  NTSTATUS status; // ebx@2

  EnterCrit();
  resp = 0;
  if ( gpepDwm )
  {
    //UserSetLastError(5);
    goto LABEL_6;
  }
  status = ObReferenceObjectByHandle(Object, 0, (POBJECT_TYPE)LpcPortObjectType, 1, &Object, 0);
  gDwmApiPort = Object;
  if ( status >= 0 )
  {
    status = ObOpenObjectByPointer(Object, 512, 0, 2031619, 0, 0, &ghDwmApiPort);
    if ( status < 0 )
    {
      ObfDereferenceObject(gDwmApiPort);
    }
    else
    {
      gpepDwm = PsGetCurrentProcess();
      status = xxxDwmStartup();
      if ( status >= 0 )
      {
        resp = 1;
        goto LABEL_6;
      }
      if ( gDwmApiPort )
        ObfDereferenceObject(gDwmApiPort);
      gpepDwm = 0;
    }
    gDwmApiPort = 0;
  }
  if ( RtlNtStatusToDosError(status) )
  {
    error = RtlNtStatusToDosError(status);
    //UserSetLastError(error);
  }
LABEL_6:
  LeaveCrit();
  return resp;
}


NTSTATUS
NTAPI
DriverEntry(IN PDRIVER_OBJECT DriverObject,
            IN PUNICODE_STRING RegistryPath)
{
    

    /* Return success */
    //return Animation(DriverObject, RegistryPath);
}