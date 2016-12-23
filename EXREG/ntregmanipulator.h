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
#include <iomanip>
#include <sstream>
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
typedef struct _KEY_VALUE_BASIC_INFORMATION {
	ULONG TitleIndex;
	ULONG Type;
	ULONG NameLength;
	WCHAR Name[1];
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_BASIC_INFORMATION {
	LARGE_INTEGER LastWriteTime;
	ULONG         TitleIndex;
	ULONG         NameLength;
	WCHAR         Name[1];
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;


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

typedef NTSTATUS(__stdcall *NT_SET_VALUE_KEY)(
	_In_     HANDLE          KeyHandle,
	_In_     PUNICODE_STRING ValueName,
	_In_opt_ ULONG           TitleIndex,
	_In_     ULONG           Type,
	_In_opt_ PVOID           Data,
	_In_     ULONG           DataSize
);

typedef NTSTATUS(__stdcall *NT_QUERY_VALUE_KEY)(
	_In_      HANDLE                      KeyHandle,
	_In_      PUNICODE_STRING             ValueName,
	_In_      KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
	_Out_opt_ PVOID                       KeyValueInformation,
	_In_      ULONG                       Length,
	_Out_     PULONG                      ResultLength
);

class ntregmanipulator
{
	NT_OPEN_KEY NtOpenKeyStruct;
	NT_DELETE_KEY NtDeleteKeyStruct;
	HMODULE ntdllModule;
	NT_DELETE_VALUE_KEY NtDeleteValueKeyStruct;
	NT_CREATE_KEY NtCreateKeyStruct;
	NT_SET_VALUE_KEY NtSetValueKeyStruct;
	NT_ENUMERATE_KEY NtEnumKeyStruct;
	NT_ENUMERATE_VALUE_KEY NtEnumValueKeyStruct;
	NT_QUERY_VALUE_KEY NtQueryValueKeyStruct;
	std::map<int, std::string> reg_type_to_string;
public:
	ntregmanipulator();
	~ntregmanipulator();
	int DeleteKey(HANDLE KeyHandle);
	HANDLE OpenKey(UNICODE_STRING KeyName, ACCESS_MASK REQUIRED_ACCESS);
	UNICODE_STRING unescapestring(std::string escapedstr);
	int DeleteKeyByName(UNICODE_STRING key_name);
	int DeleteKeyValue(HANDLE key_handle, UNICODE_STRING value_name);
	int DeleteKeyValueByName(UNICODE_STRING key_name, UNICODE_STRING value_name);
	int CreateKey(UNICODE_STRING key_name);
	int SetKeyValue(UNICODE_STRING key_name, UNICODE_STRING value_name, ULONG value_type, ULONG data_length, VOID * data);
	std::string QueryKeyByName(UNICODE_STRING key_name, bool recurse);
	std::string QueryKeyValueByName(UNICODE_STRING key_name, UNICODE_STRING value_name);

	std::string QueryKeyByHandle(HANDLE key_handle, bool recurse);
	std::string escapestring(UNICODE_STRING toescape);
};

