EXREG is a reimplementation of reg.exe using the ntdll.dll apis

The syntax is very similar to reg.exe for the currently implemented ADD, QUERY, and DELETE, with two important difference:

It does not support the "friendly" names, so for example instead of HKEY_LOCAL_MACHINE, you will need to use \registry\machine

The second is that it supports escaping characters. To escape, use a * followed by the 4 hex digits that make up the unicode character you wish to write

e.g. to add a registry key with a null character on the end:

exreg add \registry\machine\software\test*0000