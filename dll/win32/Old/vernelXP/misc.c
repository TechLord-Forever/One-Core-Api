 /*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */
 
#define NDEBUG

#include <main.h>

WINE_DEFAULT_DEBUG_CHANNEL(vernel32);

/*
 * @implemented
 */
int WINAPI WerpCurrentPeb()
{
  #ifdef _M_IX86
	return (__readfsdword(24) + 48);
  #elif defined(_M_AMD64)
	return (__readgsqword(24) + 48);
  #endif
}

HRESULT WINAPI IntToULong(INT iOperand, ULONG *pulResult)
{
  HRESULT result; // eax@2

  if ( iOperand < 0 )
  {
    *pulResult = -1;
    result = 0x80070216u;
  }
  else
  {
    *pulResult = iOperand;
    result = 0;
  }
  return result;
}

int WINAPI CreateSocketHandle()
{
  SetLastError(120u);
  return 0;
}

UINT WINAPI GetErrorMode()
{
  NTSTATUS status; // eax@1
  UINT result; // eax@3
  UINT ProcessInformation = 0; // [sp+0h] [bp-4h]@1

  status = NtQueryInformationProcess((HANDLE)0xFFFFFFFF, ProcessDefaultHardErrorMode, &ProcessInformation, 4u, 0);
  if ( status < 0 )
  {
    BaseSetLastNTError(status);
    result = 0;
  }
  else
  {
    if ( ProcessInformation & 1 )
      result = ProcessInformation & 0xFFFFFFFE;
    else
      result = ProcessInformation | 1;
  }
  return result;
}

BOOL WINAPI QueryActCtxSettingsW(DWORD dwFlags, HANDLE hActCtx, PCWSTR SourceString, PCWSTR settingName, int a5, int a6, int a7)
{
  NTSTATUS otherStatus = 0x00000000; // eax@7
  NTSTATUS status; // [sp-4h] [bp-20h]@8
  LSA_UNICODE_STRING DestinationString; // [sp+8h] [bp-14h]@16
  const UNICODE_STRING String2; // [sp+10h] [bp-Ch]@16
  BOOL resp; // [sp+18h] [bp-4h]@1

  resp = 0;
  if ( a7 )
    a7 = 0;
  if ( dwFlags )
  {
    status = 0xC00000EFu;
  }
  else
  {
    if ( settingName )
    {
      if ( a6 && !a5 )
      {
        status = 0xC000000Du;
      }
      else
      {
        if ( SourceString
          && (RtlInitUnicodeString(&DestinationString, SourceString),
              RtlInitUnicodeString((PUNICODE_STRING)&String2, L"http://schemas.microsoft.com/SMI/2005/WindowsSettings"),
              RtlCompareUnicodeString(&DestinationString, &String2, 0)) )
        {
          status = -1073741583;
        }
        else
        {
          //otherStatus = RtlQueryActivationContextApplicationSettings(0, hActCtx, 0, settingName, a5, a6, a7);
          if ( otherStatus >= 0 )
            return 1;
          status = otherStatus;
        }
      }
    }
    else
    {
      status = 0xC00000F2u;
    }
  }
  BaseSetLastNTError(status);
  return resp;
}

BOOL WINAPI SetSearchPathMode(DWORD Flags)
{
  DWORD other; // ebx@1
  BOOL verification; // esi@11

  other = Flags;
  if ( Flags & 0xFFFE7FFE )
  {
    BaseSetLastNTError(030000000015);
    return 0;
  }
  if ( Flags & 1 )
  {
    if ( other & 0x10000 )
    {
LABEL_5:
      BaseSetLastNTError(0xC000000Du);
      return 0;
    }
  }
  else
  {
    if ( !(other & 0x10000) || (Flags & 0x8000) )
      goto LABEL_5;
  }
  if ( !(BaseSearchPathMode & 0x8000) || (Flags & 0x8000) )
  {
    BaseSearchPathMode = Flags;
    verification = 1;
  }
  else
  {
    BaseSetLastNTError(0xC0000022u);
    verification = 0;
  }
  return verification;
}

BOOL WINAPI GetQueuedCompletionStatusEx(HANDLE CompletionPort, 
										LPOVERLAPPED_ENTRY lpCompletionPortEntries, 
										ULONG ulCount, 
										PULONG ulNumEntriesRemoved,
										DWORD dwMilliseconds, 
										BOOL fAlertable)
{
  BOOLEAN allert; // eax@5
  LARGE_INTEGER * receive; // esi@5
  BOOL result; // eax@2
  PRTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME_EXTENDED Frame = NULL; // [sp+10h] [bp-48h]@1
  char copy; // [sp+18h] [bp-40h]@1
  LARGE_INTEGER *large = NULL; // [sp+34h] [bp-24h]@5
  CPPEH_RECORD ms_exc; // [sp+40h] [bp-18h]@7
  FILE_IO_COMPLETION_INFORMATION information;

  information.ApcContext = NULL;


  memset(&copy, 0, 0x1Cu);
  if ( lpCompletionPortEntries && ulNumEntriesRemoved && ulCount )
  {
    receive = BaseFormatTimeOut(large, dwMilliseconds);
    if ( fAlertable )
      allert = (BOOLEAN)RtlActivateActivationContextUnsafeFast(Frame, 0);
    ms_exc.registration.TryLevel = 0;
    allert = fAlertable != 0;
    /*result = NtRemoveIoCompletionEx(
               CompletionPort,
			   information,
               ulCount,
               (PVOID)ulNumEntriesRemoved,
               receive,
               allert);*/
	result = 0x00000000;
    ms_exc.registration.TryLevel = -2;
    if ( fAlertable )
      result =  (BOOL)RtlDeactivateActivationContextUnsafeFast(Frame);
  }
  else
  {
    SetLastError(87);
    result = 0;
  }
  return result;
}

BOOL WINAPI ReplacePartitionUnit(PCWSTR TargetString, PCWSTR SourceString, ULONG unit)
{
  NTSTATUS status; // eax@1
  BOOL result; // eax@2
  LSA_UNICODE_STRING string; // [sp+4h] [bp-10h]@1
  LSA_UNICODE_STRING DestinationString; // [sp+Ch] [bp-8h]@1

  RtlInitUnicodeString(&DestinationString, SourceString);
  RtlInitUnicodeString(&string, TargetString);
  status = NtReplacePartitionUnit(&string, &DestinationString, unit);
  if ( status >= 0 )
  {
    result = 1;
  }
  else
  {
    BaseSetLastNTError(status);
    result = 0;
  }
  return result;
}

BOOL WINAPI GetVolumeInformationByHandleW(HANDLE FileHandle, LPWSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPWSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize)
{
  PVOID eigth; // ebx@1
  DWORD nine; // eax@1
  DWORD ten; // edi@1
  PVOID evelen; // esi@1
  ULONG tweleve; // edi@6
  NTSTATUS status; // eax@10
  BOOL result; // eax@15
  struct _IO_STATUS_BLOCK IoStatusBlock; // [sp+10h] [bp-30h]@10
  PVOID otherAddress; // [sp+18h] [bp-28h]@1
  PVOID Address; // [sp+1Ch] [bp-24h]@1
  ULONG VolumeInformationLength; // [sp+20h] [bp-20h]@28
  BOOL verification; // [sp+24h] [bp-1Ch]@1
  CPPEH_RECORD ms_exc; // [sp+28h] [bp-18h]@8
  DWORD nVolumeNameSizea; // [sp+50h] [bp+10h]@1
  DWORD nFileSystemNameSizea; // [sp+64h] [bp+24h]@1

  eigth = 0;
  verification = 0;
  nine = 2 * nVolumeNameSize;
  nVolumeNameSizea = 2 * nVolumeNameSize;
  ten = 2 * nFileSystemNameSize;
  nFileSystemNameSizea = 2 * nFileSystemNameSize;
  Address = 0;
  evelen = 0;
  otherAddress = 0;
  if ( lpVolumeNameBuffer )
  {
    VolumeInformationLength = nine + 24;
  }
  else
  {
    if ( !lpVolumeSerialNumber )
      goto VERIFY_FILE_SYSTEM;
    VolumeInformationLength = 284;
  }  
  evelen = RtlAllocateHeap(RtlGetProcessHeap(), BaseDllTag, VolumeInformationLength);
  otherAddress = evelen;
  if ( !evelen )
    goto GLOBAL_ERROR;
VERIFY_FILE_SYSTEM:
  if ( lpFileSystemNameBuffer )
  {
    tweleve = ten + 16;
    goto ALLOC_HEAP;
  }
  if ( lpMaximumComponentLength || lpFileSystemFlags )
  {
    tweleve = 276;
ALLOC_HEAP:
		eigth = RtlAllocateHeap(RtlGetProcessHeap(), BaseDllTag, tweleve);
    Address = eigth;
    if ( eigth )
      goto QUERY_VOLUME;
    if ( evelen )
		RtlFreeHeap(RtlGetProcessHeap(), 0, evelen);   
GLOBAL_ERROR:
    BaseSetLastNTError(0xC0000017u);
    return 0;
  }
  tweleve = 0;
QUERY_VOLUME:
  ms_exc.registration.TryLevel = 0;
  if ( !evelen
    || (status = NtQueryVolumeInformationFile(
                   FileHandle,
                   &IoStatusBlock,
                   evelen,
                   VolumeInformationLength,
                   FileFsVolumeInformation),
        status >= 0) )
  {
    if ( !eigth
      || (status = NtQueryVolumeInformationFile(FileHandle, &IoStatusBlock, eigth, tweleve, FileFsAttributeInformation),
          status >= 0) )
    {
      ms_exc.registration.TryLevel = 1;
      if ( lpVolumeNameBuffer )
      {
        if ( *((DWORD *)evelen + 3) >= nVolumeNameSizea )
          goto LAST_ERROR;
        memcpy(lpVolumeNameBuffer, (char *)evelen + 18, *((DWORD *)evelen + 3));
        lpVolumeNameBuffer[*((DWORD *)evelen + 3) >> 1] = 0;
      }
      if ( lpVolumeSerialNumber )
        *lpVolumeSerialNumber = *((DWORD *)evelen + 2);
      if ( !lpFileSystemNameBuffer )
        goto UNKNOWN;
      if ( *((DWORD *)eigth + 2) < nFileSystemNameSizea )
      {
        memcpy(lpFileSystemNameBuffer, (char *)eigth + 12, *((DWORD *)eigth + 2));
        lpFileSystemNameBuffer[*((DWORD *)eigth + 2) >> 1] = 0;
UNKNOWN:
        if ( lpMaximumComponentLength )
          *lpMaximumComponentLength = *((DWORD *)eigth + 1);
        result = (BOOL)lpFileSystemFlags;
        if ( lpFileSystemFlags )
          *lpFileSystemFlags = *(DWORD *)eigth;
        verification = 1;
        goto RELEASE_REGISTRATION;
      }
LAST_ERROR:
      SetLastError(0x18u);
      verification = 0;
RELEASE_REGISTRATION:
      ms_exc.registration.TryLevel = 0;
      goto REGISTRATION_LEVEL_TWO;
    }
  }
  BaseSetLastNTError(status);
  verification = 0;
REGISTRATION_LEVEL_TWO:
  ms_exc.registration.TryLevel = -2;
  if ( evelen )
	result = RtlFreeHeap(RtlGetProcessHeap(), 0, evelen);
  if ( eigth )
	result = RtlFreeHeap(RtlGetProcessHeap(), 0, eigth);
  return result;
}

void WINAPI CtrlRoutine(DWORD h)
{
	UNIMPLEMENTED;
}

BOOL WINAPI CeipIsOptedIn()
{
  return WinSqmIsOptedInEx(4);
}

BOOL WINAPI GetOverlappedResultEx(
  _In_   HANDLE hFile,
  _In_   LPOVERLAPPED lpOverlapped,
  _Out_  LPDWORD lpNumberOfBytesTransferred,
  _In_   DWORD dwMilliseconds,
  _In_   BOOL bAlertable
)
{
	return GetOverlappedResult(hFile, lpOverlapped, lpNumberOfBytesTransferred, bAlertable);
}

ATOM WINAPI GlobalAddAtomExA(LPCSTR SourceString, ULONG number)
{
  return GlobalAddAtomA(SourceString);
}

ATOM WINAPI GlobalAddAtomExW(LPCWSTR SourceString, ULONG number)
{
  return GlobalAddAtomW(SourceString);
}

BOOL WINAPI IsNativeVhdBoot(PBOOL *NativeVhdBoot)
{
  PBOOL *localNativeBoot; // edi@1
  NTSTATUS status; // eax@3
  PVOID alloca; // esi@5
  NTSTATUS queryInfo; // ebx@7
  NTSTATUS error; // [sp-Ch] [bp-10h]@4

  localNativeBoot = NativeVhdBoot;
  if ( !NativeVhdBoot )
  {
    RtlSetLastWin32Error(87);
    return 0;
  }
  status = NtQuerySystemInformation(SystemDeleteSession|0x40, 0, 0, (PULONG)&NativeVhdBoot);
  if ( status != 030000000043 )
  {
    error = status;
LABEL_9:
    BaseSetLastNTError(error);
    return 0;
  }
  alloca = RtlAllocateHeap(RtlGetProcessHeap(), 0, (ULONG)NativeVhdBoot);
  if ( !alloca )
  {
    RtlSetLastWin32Error(8);
    return 0;
  }
  queryInfo = NtQuerySystemInformation(SystemDeleteSession|0x40, alloca, (ULONG)NativeVhdBoot, (PULONG)&NativeVhdBoot);
  if ( queryInfo < 0 )
  {
    RtlFreeHeap(RtlGetProcessHeap(), 0, alloca);
    error = queryInfo;
    goto LABEL_9;
  }
  *localNativeBoot = (PBOOL)(*(BYTE *)alloca != 0);
  RtlFreeHeap(RtlGetProcessHeap(), 0, alloca);
  return 1;
}

HMODULE WINAPI LoadPackagedLibrary(LPCWSTR lpLibFileName, DWORD Reserved)
{
  WCHAR libName; // cx@5
  LPCWSTR otherLibName; // eax@5
  WCHAR compare; // dx@6
  WCHAR hum; // dx@9

  if ( Reserved )
  {
    BaseSetLastNTError(0xC000000Du);
    return 0;
  }
  if ( RtlDetermineDosPathNameType_U((PWSTR)lpLibFileName) != 5 )
  {
    BaseSetLastNTError(0xC000000Du);
    return 0;
  }
  libName = *lpLibFileName;
  otherLibName = lpLibFileName;
  if ( !*lpLibFileName )
    return LoadLibraryExW(lpLibFileName, 0, 4u);
  compare = 92;
  while ( 1 )
  {
    if ( libName != 46 )
      goto LOAD_LIBRARY_EX;
    if ( otherLibName[1] == 46 )
      break;
    do
    {
CALCULE:
      if ( libName == compare )
        break;
      if ( libName == 47 )
        break;
      ++otherLibName;
      libName = *otherLibName;
    }
    while ( *otherLibName );
    if ( *otherLibName )
    {
      ++otherLibName;
      libName = *otherLibName;
      if ( *otherLibName )
        continue;
    }
    return LoadLibraryExW(lpLibFileName, 0, 4u);
  }
  hum = otherLibName[2];
  if ( hum != 92 && hum != 47 )
  {
    compare = 92;
LOAD_LIBRARY_EX:
    if ( !libName )
      return LoadLibraryExW(lpLibFileName, 0, 4u);
    goto CALCULE;
  }
  BaseSetLastNTError(0xC000000Du);
  return 0;
}

BOOL  WINAPI SetDefaultDllDirectories(
  _In_  DWORD DirectoryFlags
)
{
	UNIMPLEMENTED;
	return TRUE;
}

BOOL WINAPI SetCachedSigningLevel(PHANDLE SourceFiles, ULONG SourceFileCount, ULONG Flags, HANDLE TargetFile)
{
	UNIMPLEMENTED;
	return TRUE;
}

BOOL WINAPI GetCachedSigningLevel(HANDLE File, 
								  PULONG Flags, 
								  PULONG SigningLevel, 
								  PUCHAR Thumbprint, 
								  PULONG ThumbprintSize, 
								  PULONG ThumbprintAlgorithm)
{
	UNIMPLEMENTED;
	return TRUE;
}

BOOL WINAPI AddScopedPolicyIDAce(
  _Inout_  PACL pAcl,
  _In_     DWORD dwAceRevision,
  _In_     DWORD AceFlags,
  _In_     DWORD AccessMask,
  _In_     PSID pSid
)
{
	UNIMPLEMENTED;
	return TRUE;
}

ULONG WINAPI KernelBaseGetGlobalData()
{
  return KernelBaseGlobalData;
}

BOOLEAN WINAPI NotifyMountMgr(PWSTR BytesReturned, PCWSTR SourceString, BOOL verfication)
{
  BOOLEAN result; // al@1
  int otherLength; // ebx@2
  ULONG length; // ebx@2
  ULONG *kernelGlobal; // eax@2
  PUNICODE_STRING otherString; // eax@2
  PUNICODE_STRING localSource; // esi@2
  HANDLE localHandle; // edi@4
  LSA_UNICODE_STRING DestinationString; // [sp+4h] [bp-10h]@2
  LSA_UNICODE_STRING ntname; // [sp+Ch] [bp-8h]@1

  result = RtlDosPathNameToNtPathName_U(BytesReturned, &ntname, 0, 0);
  if ( result )
  {
    RtlInitUnicodeString(&DestinationString, SourceString);
    otherLength = (unsigned __int16)(ntname.Length - 2) + 8;
    ntname.Length -= 2;
    DestinationString.Length -= 2;
    length = DestinationString.Length + otherLength;
    kernelGlobal = (ULONG *)KernelBaseGetGlobalData();
    otherString = (PUNICODE_STRING)RtlAllocateHeap(*(HANDLE *)(__readfsdword(48) + 24), *kernelGlobal, length);
    localSource = otherString;
    if ( otherString )
    {
      otherString->Length = 8;
      otherString->MaximumLength = ntname.Length;
      otherString->Buffer = (PWSTR)ntname.Length;
      otherString->Buffer = (PWSTR)DestinationString.Length;
      memcpy(&otherString[1], ntname.Buffer, otherString->MaximumLength);
      memcpy(localSource, DestinationString.Buffer, (size_t)localSource->Buffer);
      localSource->MaximumLength  = 63; 
	  localSource->Buffer = (PWSTR)63;
      localHandle = CreateFileW(L"\\\\.\\MountPointManager", 0xC0000000u, 3u, 0, 3u, 0x80u, (HANDLE)0xFFFFFFFF);
      if ( localHandle != (HANDLE)-1 )
      {
        DeviceIoControl(
          localHandle,
          4 * (verfication == 0) + 7192600,
          localSource,
          length,
          0,
          0,
          (LPDWORD)&BytesReturned,
          0);
        CloseHandle(localHandle);
      }
      RtlFreeHeap(RtlGetProcessHeap(), 0, localSource);
      result = RtlFreeHeap(RtlGetProcessHeap(), 0, ntname.Buffer);
    }
    else
    {
      result = RtlFreeHeap(RtlGetProcessHeap(), 0, ntname.Buffer);
    }
  }
  return result;
}

BOOL WINAPI AddResourceAttributeAce(PACL pAcl, 
								    DWORD dwAceRevision, 
									DWORD AceFlags, 
									DWORD AccessMask, 
									PSID pSid, 
									PCLAIM_SECURITY_ATTRIBUTES_INFORMATION  pAttributeInfo, 
									PDWORD pReturnLength)
{
  NTSTATUS status; // eax@1
  BOOL result; // eax@2

  status = RtlAddResourceAttributeAce(pAcl, dwAceRevision, AceFlags, AccessMask, pSid, pAttributeInfo, pReturnLength);
  if ( status >= 0 )
  {
    result = 1;
  }
  else
  {
    BaseSetLastNTError(status);
    result = 0;
  }
  return result;
}

BOOL WINAPI GetVolumeBandwidthContractProperties(HANDLE FileHandle, int a2, int a3, int a4)
{
  NTSTATUS status; // eax@1
  BOOL result; // eax@2
  int FileInformation; // [sp+0h] [bp-14h]@1
  int one = 0; // [sp+4h] [bp-10h]@4
  int two = 0; // [sp+8h] [bp-Ch]@4
  struct _IO_STATUS_BLOCK IoStatusBlock; // [sp+Ch] [bp-8h]@1

  status = NtQueryInformationFile(
             FileHandle,
             &IoStatusBlock,
             &FileInformation,
             0xCu,
             FileInformationReserved33|FileNamesInformation);
  if ( status >= 0 )
  {
    a2 = FileInformation;
    a4 = two;
    a3 = one;
    result = 1;
  }
  else
  {
    BaseSetLastNTError(status);
    result = 0;
  }
  return result;
}

BOOL WINAPI Wow64DisableWow64FsRedirection(IN PVOID * OldValue) 
{
	return FALSE;
}	

BOOL WINAPI Wow64RevertWow64FsRedirection(PVOID value) 	
{
	return FALSE;
}

BOOLEAN WINAPI Wow64EnableWow64FsRedirection(_In_ BOOLEAN value) 
{
	return FALSE;
}
	