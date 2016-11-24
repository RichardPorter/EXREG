#include "stdafx.h"
#include "ntregmanipulator.h"

#include <windows.h>
#include <Winternl.h>
#include <cstdlib>


ntregmanipulator::ntregmanipulator()
{
	NT_OPEN_KEY NtOpenKeyStruct=NULL;
	NT_DELETE_KEY NtDeleteKeyStruct = NULL;
	NT_DELETE_VALUE_KEY NtDeleteValueKeyStruct = NULL;
	NT_CREATE_KEY NtCreateKeyStruct = NULL;
	ntdllModule = LoadLibrary(_T("ntdll.dll"));

}


ntregmanipulator::~ntregmanipulator()
{
}
HANDLE ntregmanipulator::OpenKey(UNICODE_STRING KeyName, ACCESS_MASK REQUIRED_ACCESS=GENERIC_ALL)
{
	NTSTATUS result;
	OBJECT_ATTRIBUTES KeyAttributes;
	HANDLE key_handle;
	KeyAttributes.Length = sizeof(OBJECT_ATTRIBUTES);
	KeyAttributes.RootDirectory = NULL;
	KeyAttributes.ObjectName = &KeyName;
	KeyAttributes.Attributes = OBJ_CASE_INSENSITIVE;
	KeyAttributes.SecurityDescriptor = NULL;
	KeyAttributes.SecurityQualityOfService = NULL;
	if (NtOpenKeyStruct==NULL)
		NtOpenKeyStruct= (NT_OPEN_KEY)GetProcAddress(ntdllModule, "NtOpenKey");
	result = NtOpenKeyStruct(&key_handle, REQUIRED_ACCESS, &KeyAttributes);
	return key_handle;

}
int ntregmanipulator::DeleteKey(HANDLE key_handle)
{
	NTSTATUS result;
	if (NtDeleteKeyStruct == NULL)
		NtDeleteKeyStruct = (NT_DELETE_KEY)GetProcAddress(ntdllModule, "NtDeleteKey");
	result = NtDeleteKeyStruct(key_handle);
	CloseHandle(key_handle);
	if (NT_SUCCESS(result)) { return 0; }
	else { return 1; }
}
UNICODE_STRING ntregmanipulator::unescapestring(std::string escapedstr) {
	UNICODE_STRING unescaped;
	wchar_t * unescapedcharbuffer= NULL;

	unescapedcharbuffer=(wchar_t *)malloc(sizeof(wchar_t)*escapedstr.length());
	unescaped.Length = 0;
	unescaped.MaximumLength = (USHORT)(2*escapedstr.length());
	unsigned int hex_value;
	std::string hex_string;
	int i = 0;
	while (i < escapedstr.length()) {
		if (escapedstr[i] != '*'){
			unescapedcharbuffer[unescaped.Length++] = (wchar_t)(escapedstr[i]);
			i++;
		}
		else {
			hex_string = "0x" + escapedstr.substr(i+1,4);
			hex_value = std::stoul(hex_string, nullptr, 16);
			unescapedcharbuffer[unescaped.Length++] = (wchar_t)hex_value;
			i = i + 5;
		}
		
	}
	unescaped.Buffer = unescapedcharbuffer;
	unescaped.Length= unescaped.Length*2;
	return unescaped;
}
int ntregmanipulator::DeleteKeyByName(UNICODE_STRING key_name)
{
	HANDLE key_handle = OpenKey(key_name, GENERIC_ALL);
	return DeleteKey(key_handle);
}
int ntregmanipulator::DeleteKeyValue(HANDLE key_handle, UNICODE_STRING value_name)
{
	int result;
	if (NtDeleteValueKeyStruct == NULL)
		NtDeleteValueKeyStruct = (NT_DELETE_VALUE_KEY)GetProcAddress(ntdllModule, "NtDeleteValueKey");
	result = NtDeleteValueKeyStruct(key_handle, &value_name);
	if (NT_SUCCESS(result)) { return 0; }
	else { return 1; }
}
int ntregmanipulator::DeleteKeyValueByName(UNICODE_STRING key_name, UNICODE_STRING value_name)
{
	int result;
	HANDLE key_handle = OpenKey(key_name, GENERIC_ALL);
	result=DeleteKeyValue(key_handle,value_name);
	CloseHandle(key_handle);
	return result;
}
int ntregmanipulator::CreateKey(UNICODE_STRING key_name)
{
	int result;
	HANDLE key_handle;
	OBJECT_ATTRIBUTES KeyAttributes;
	KeyAttributes.Length = sizeof(OBJECT_ATTRIBUTES);
	KeyAttributes.RootDirectory = NULL;
	KeyAttributes.ObjectName = &key_name;
	KeyAttributes.Attributes = OBJ_CASE_INSENSITIVE;
	KeyAttributes.SecurityDescriptor = NULL;
	KeyAttributes.SecurityQualityOfService = NULL;
	if (NtCreateKeyStruct == NULL)
		NtCreateKeyStruct = (NT_CREATE_KEY)GetProcAddress(ntdllModule, "NtCreateKey");
	result = NtCreateKeyStruct(&key_handle, GENERIC_ALL, &KeyAttributes, 0, NULL, 0, NULL);
	CloseHandle(key_handle);
	return result;
}