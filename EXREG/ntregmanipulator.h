#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <Winternl.h>
#include <iostream>
#include <ctype.h>

#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
typedef enum _KEY_INFORMATION_CLASS {
	KeyBasicInformation = 0,
	KeyNodeInformation = 1,
	KeyFullInformation = 2,
	KeyNameInformation = 3,
	KeyCachedInformation = 4,
	KeyFlagsInformation = 5,
	KeyVirtualizationInformation = 6,
	KeyHandleTagsInformation = 7,
	MaxKeyInfoClass = 8
} KEY_INFORMATION_CLASS;


typedef enum _KEY_VALUE_INFORMATION_CLASS {
	KeyValueBasicInformation = 0,
	KeyValueFullInformation,
	KeyValuePartialInformation,
	KeyValueFullInformationAlign64,
	KeyValuePartialInformationAlign64,
	MaxKeyValueInfoClass
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
	ULONG TitleIndex;
	ULONG Type;
	ULONG DataLength;
	UCHAR Data[1];
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef NTSTATUS(__stdcall *NT_OPEN_KEY)(OUT PHANDLE KeyHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes);
typedef NTSTATUS(__stdcall *NT_CREATE_KEY)(OUT PHANDLE KeyHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes, __reserved ULONG TitleIndex, __in_opt PUNICODE_STRING Class, IN ULONG CreateOptions, __out_opt PULONG Disposition);
typedef NTSTATUS(__stdcall *NT_DELETE_KEY)(IN HANDLE KeyHandle);
typedef NTSTATUS(__stdcall *NT_ENUMERATE_KEY)(IN HANDLE KeyHandle,
	IN ULONG Index,
	IN      KEY_INFORMATION_CLASS KeyInformationClass,
	_Out_opt_ PVOID                 KeyInformation,
	IN     ULONG                 Length,
	OUT     PULONG                ResultLength
	);

typedef NTSTATUS(__stdcall *NT_ENUMERATE_VALUE_KEY)(
	_In_      HANDLE                      KeyHandle,
	_In_      ULONG                       Index,
	_In_      KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
	_Out_opt_ PVOID                       KeyValueInformation,
	_In_      ULONG                       Length,
	_Out_     PULONG                      ResultLength
	);

typedef NTSTATUS(__stdcall *NT_QUERY_VALUE_KEY)(
	_In_      HANDLE                      KeyHandle,
	_In_      PUNICODE_STRING             ValueName,
	_In_      KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
	_Out_opt_ PVOID                       KeyValueInformation,
	_In_      ULONG                       Length,
	_Out_     PULONG                      ResultLength
	);
typedef NTSTATUS(__stdcall *NT_DELETE_VALUE_KEY)(
	_In_ HANDLE          KeyHandle,
	_In_ PUNICODE_STRING ValueName
);
class ntregmanipulator
{
	NT_OPEN_KEY NtOpenKeyStruct;
	NT_DELETE_KEY NtDeleteKeyStruct;
	HMODULE ntdllModule;
	NT_DELETE_VALUE_KEY NtDeleteValueKeyStruct;
public:
	ntregmanipulator();
	~ntregmanipulator();
	int DeleteKey(HANDLE KeyHandle);
	HANDLE OpenKey(UNICODE_STRING KeyName, ACCESS_MASK REQUIRED_ACCESS);
	UNICODE_STRING unescapestring(std::string escapedstr);
	int DeleteKeyByName(UNICODE_STRING key_name);
	int DeleteKeyValue(HANDLE key_handle, UNICODE_STRING value_name);
};

