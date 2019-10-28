# PROC_THREAD_ATTRIBUTE_BNO_ISOLATION

The repository includes example of usage undocumented process attribute PROC_THREAD_ATTRIBUTE_BNO_ISOLATION.

#Overview
Windows 10 RS3 (1709 Fall Creators Update- October 2017) introduced a new undocumented feature, BNO Isolation. When I say it's undocumented I can't find any public reference to it, and the expected definitions don't appear in the Windows SDK headers. More information: https://tyranidslair.blogspot.com/2019/02/a-brief-history-of-basenamedobjects-on.html.
The feature helps to isolate processes and breaks dependency of most of singleton applications.

#Example
- Build project and run BNO_ISOLATION.exe
```c++
"Enter program full path (for example: C:\\Windows\\system32\\notepad.exe): "
```
- fill program's full path  (for ex.: notepad++) and press enter:
```
C:\Program Files (x86)\Notepad++\notepad++.exe
```
After that you get two instances of singleton application.
