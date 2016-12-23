#include "stdafx.h"
#include "ntregmanipulator.h"

#include <windows.h>
#include <Winternl.h>
#include <cstdlib>
#include <iomanip>
#include <sstream>
ntregmanipulator::ntregmanipulator()
{
	NT_OPEN_KEY NtOpenKeyStruct=NULL;
	NT_DELETE_KEY NtDeleteKeyStruct = NULL;
	NT_DELETE_VALUE_KEY NtDeleteValueKeyStruct = NULL;
	NT_CREATE_KEY NtCreateKeyStruct = NULL;
	NT_SET_VALUE_KEY NtSetValueKeyStruct=NULL;
	NT_ENUMERATE_KEY NtEnumKeyStruct = NULL;
	NT_ENUMERATE_VALUE_KEY NtEnumValueKeyStruct = NULL;
	NT_QUERY_VALUE_KEY NtQueryValueKeyStruct = NULL;
	reg_type_to_string = { { REG_SZ,"REG_SZ" },{ REG_MULTI_SZ,"REG_MULTI_SZ" },{ REG_RESOURCE_REQUIREMENTS_LIST,"REG_RESOURCE_REQUIREMENTS_LIST" },{ REG_DWORD,"REG_DWORD" },{ REG_BINARY,"REG_BINARY" },{ REG_DWORD_LITTLE_ENDIAN,"REG_DWORD_LITTLE_ENDIAN" },{ REG_DWORD_BIG_ENDIAN,"REG_DWORD_BIG_ENDIAN" },{ REG_NONE,"REG_NONE" },{ REG_RESOURCE_LIST,"REG_RESOURCE_LIST" },{ REG_FULL_RESOURCE_DESCRIPTOR,"REG_FULL_RESOURCE_DESCRIPTOR" },{ REG_QWORD,"REG_QWORD" },{ REG_EXPAND_SZ,"REG_EXPAND_SZ" } };
	ntdllModule = LoadLibrary(_T("ntdll.dll"));

}


ntregmanipulator::~ntregmanipulator()
{
}
HANDLE ntregmanipulator::OpenKey(UNICODE_STRING KeyName, ACCESS_MASK REQUIRED_ACCESS=GENERIC_ALL)
{
	NTSTATUS result;
	OBJECT_ATTRIBUTES KeyAttributes;
	HANDLE key_handle=NULL;
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
	if (NT_SUCCESS(result)) { return 0; }
	else { return 1; }
}

int ntregmanipulator::SetKeyValue(UNICODE_STRING key_name,UNICODE_STRING value_name, ULONG value_type,ULONG data_length, VOID * data)
{
	NTSTATUS result_create;
	NTSTATUS result_set_value;
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
	if (NtSetValueKeyStruct == NULL)
		NtSetValueKeyStruct = (NT_SET_VALUE_KEY)GetProcAddress(ntdllModule, "NtSetValueKey");
	result_create = NtCreateKeyStruct(&key_handle, GENERIC_ALL, &KeyAttributes, 0, NULL, 0, NULL);
	result_set_value = NtSetValueKeyStruct(key_handle, &value_name, NULL, value_type, data, data_length);
	CloseHandle(key_handle);
	if (NT_SUCCESS(result_create) && NT_SUCCESS(result_set_value)) { return 0; }
	else { return 1; }
}

std::string ntregmanipulator::QueryKeyByName(UNICODE_STRING key_name, bool recurse)
{
	if (NtEnumValueKeyStruct == NULL)
		NtEnumValueKeyStruct = (NT_ENUMERATE_VALUE_KEY)GetProcAddress(ntdllModule, "NtEnumerateValueKey");

	if (NtEnumKeyStruct == NULL)
		NtEnumKeyStruct = (NT_ENUMERATE_KEY)GetProcAddress(ntdllModule, "NtEnumerateKey");
	HANDLE key_handle;
	key_handle = OpenKey(key_name, GENERIC_READ);
	if (key_handle == NULL) {
		std::cout << "ERROR: The system was unable to find the specified registry key or value";
		exit(1);
	}
	int value_index = 0;
	unsigned long result_length;
	UNICODE_STRING value_name_string;
	NTSTATUS commandresult;
	PKEY_VALUE_BASIC_INFORMATION value_name_return = (PKEY_VALUE_BASIC_INFORMATION)malloc(66000);
	std::cout << escapestring(key_name)<<std::endl;
	while (true) {
		commandresult=NtEnumValueKeyStruct(key_handle, value_index, KeyValueBasicInformation, value_name_return, 66000, &result_length);
		if (NT_SUCCESS(commandresult)) {
			value_name_string.Buffer =(PWSTR) &(value_name_return->Name);
			value_name_string.Length = value_name_return->NameLength;
			value_name_string.MaximumLength = value_name_return->NameLength;
			std::cout << QueryKeyValueByName(key_name, value_name_string)<<std::endl;
			
		}
		else { break; }
		value_index++;
		
	}
	unsigned long key_index = 0;
	PKEY_BASIC_INFORMATION key_buffer = (PKEY_BASIC_INFORMATION)malloc(66000);
	UNICODE_STRING subkey_name;
	while (true)	{
		commandresult = NtEnumKeyStruct(key_handle, key_index, KeyBasicInformation, key_buffer, 66000, &result_length);
		if (NT_SUCCESS(commandresult)) {
			value_name_string.Buffer = (PWSTR) &(key_buffer->Name);
			value_name_string.Length = key_buffer->NameLength;
			value_name_string.MaximumLength = key_buffer->NameLength;
			if (recurse == false) {
				std::cout << escapestring(key_name) + "\\" + escapestring(value_name_string) << std::endl;
			}
			else {
				subkey_name = unescapestring(escapestring(key_name) + "\\" + escapestring(value_name_string));
				QueryKeyByName(subkey_name,true);
				free(subkey_name.Buffer);
			}
		}
		else { break; }
		key_index++;
	}
	CloseHandle(key_handle);
	free(value_name_return);
	//return QueryKeyByHandle(key_handle, recurse);
	return "";
}

/*std::string ntregmanipulator::QueryKeyByHandle(HANDLE key_handle, bool recurse)
{
	std::string query_result;
	int value_index = 0;
	while (1) {
	}
}*/

std::string ntregmanipulator::escapestring(UNICODE_STRING toescape) {
	std::string escapedstring = "";
	std::string hexpad;
	char hexconvert[5];
	hexconvert[4] = 0;
	int charindex = 0;
	for (charindex = 0;charindex < toescape.Length/2; charindex++)
	{
		if (toescape.Buffer[charindex]>31&&toescape.Buffer[charindex]<128&& toescape.Buffer[charindex]!='*'){
			escapedstring.push_back((char)toescape.Buffer[charindex]);
		}else{

			_itoa_s((int)toescape.Buffer[charindex], hexconvert, 5,16);
			escapedstring = escapedstring + "*";
			hexpad = std::string(hexconvert);
				hexpad.insert(hexpad.begin(), 4 - hexpad.size(), '0');
				escapedstring += hexpad;
		}
	}
	return escapedstring;
	
}
std::string ntregmanipulator::QueryKeyValueByName(UNICODE_STRING key_name, UNICODE_STRING value_name) {
	if (NtQueryValueKeyStruct == NULL)
		NtQueryValueKeyStruct = (NT_QUERY_VALUE_KEY)GetProcAddress(ntdllModule, "NtQueryValueKey");
	HANDLE key_handle;
	NTSTATUS commandresult;
	UNICODE_STRING value_data_string;
	std::string value_name_string;
	key_handle = OpenKey(key_name, GENERIC_READ);
	if (key_handle == NULL) {
		std::cout << "ERROR: The system was unable to find the specified registry key or valuek";
		exit(1);
	}
	PKEY_VALUE_PARTIAL_INFORMATION value_buffer =(PKEY_VALUE_PARTIAL_INFORMATION) malloc(65536);
	ULONG result_length=0;
	commandresult=NtQueryValueKeyStruct(key_handle, &value_name, KeyValuePartialInformation, value_buffer, 65536, &result_length);
	CloseHandle(key_handle);
	value_name_string = escapestring(value_name);
	std::stringstream hexstream;
	std::string value_data;
	DWORD * dword_p;
	UINT64 * qword_p;
	if (!NT_SUCCESS(commandresult))
	{
		std::cout << "ERROR: The system was unable to find the specified registry key or valuev";
		exit(1);
	}
	if (value_name.Length <= 2)
		value_name_string = "(Default)";
	if (value_buffer->Type == REG_DWORD || value_buffer->Type == REG_DWORD_BIG_ENDIAN || value_buffer->Type == REG_DWORD_LITTLE_ENDIAN) {
		dword_p = (DWORD *)&value_buffer->Data;
		hexstream << std::hex << *dword_p;
		value_data = "    " + value_name_string + "    " + reg_type_to_string[value_buffer->Type] + "    " + "0x" + hexstream.str();
		free(value_buffer);
		return value_data;
	}
	else if (value_buffer->Type == REG_QWORD || value_buffer->Type == REG_QWORD_LITTLE_ENDIAN) {
		qword_p = (UINT64 *)&value_buffer->Data;
		hexstream << std::hex << *qword_p;
		value_data = "    " + value_name_string + "    " + reg_type_to_string[value_buffer->Type] + "    " + "0x" + hexstream.str();
		free(value_buffer);
		return value_data;
	}
	else
	{
		value_data_string.Length = value_buffer->DataLength-2;
		value_data_string.MaximumLength = value_buffer->DataLength;
		value_data_string.Buffer = (PWSTR)&value_buffer->Data;
		value_data= "    " + value_name_string + "    " + reg_type_to_string[value_buffer->Type] + "    " + escapestring(value_data_string);
		free(value_buffer);
		return value_data;
	}
	return "";
}