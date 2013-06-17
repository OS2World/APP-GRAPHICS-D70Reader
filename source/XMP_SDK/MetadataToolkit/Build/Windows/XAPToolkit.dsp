# Microsoft Developer Studio Project File - Name="XAPToolkit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XAPToolkit - Win32 Debug Multithreaded
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XAPToolkit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XAPToolkit.mak" CFG="XAPToolkit - Win32 Debug Multithreaded"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XAPToolkit - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XAPToolkit - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XAPToolkit - Win32 Release Single Threaded" (based on "Win32 (x86) Static Library")
!MESSAGE "XAPToolkit - Win32 Debug Single Threaded" (based on "Win32 (x86) Static Library")
!MESSAGE "XAPToolkit - Win32 Release Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "XAPToolkit - Win32 Debug Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "XAPToolkit - Win32 Release Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "XAPToolkit - Win32 Debug Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "XAPToolkit"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XAPToolkit - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "NDEBUG" /D "_WINDOWS" /D "XAP_LIB" /D XAP_DEBUG_BUILD=0 /D "WIN32" /D "WIN_ENV" /D "XAP_BUILT_IN" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\XMPToolkitMT.lib"

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /GR /GX /ZI /Od /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "XAP_LIB" /D XAP_DEBUG_BUILD=1 /D "WIN32" /D "WIN_ENV" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "XAP_BUILT_IN" /D "XAP_DEBUG_CERR" /D "XAP_DEBUG_VALIDATE" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\XMPToolkitMT.lib"

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Release Single Threaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "XAPToolkit___Win32_Release_Single_Threaded"
# PROP BASE Intermediate_Dir "XAPToolkit___Win32_Release_Single_Threaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "XAPToolkit___Win32_Release_Single_Threaded"
# PROP Intermediate_Dir "XAPToolkit___Win32_Release_Single_Threaded"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "NDEBUG" /D "_WINDOWS" /D "XAP_LIB" /D XAP_DEBUG_BUILD=0 /D "WIN32" /D "WIN_ENV" /D "XAP_BUILT_IN" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /GR /GX /O2 /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "NDEBUG" /D "_WINDOWS" /D "XAP_LIB" /D XAP_DEBUG_BUILD=0 /D "WIN32" /D "WIN_ENV" /D "XAP_BUILT_IN" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Release\XAPToolkitRelease.lib"
# ADD LIB32 /nologo /out:"Release\XMPToolkitST.lib"

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Debug Single Threaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "XAPToolkit___Win32_Debug_Single_Threaded"
# PROP BASE Intermediate_Dir "XAPToolkit___Win32_Debug_Single_Threaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "XAPToolkit___Win32_Debug_Single_Threaded"
# PROP Intermediate_Dir "XAPToolkit___Win32_Debug_Single_Threaded"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "XAP_LIB" /D XAP_DEBUG_BUILD=1 /D "WIN32" /D "WIN_ENV" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "XAP_BUILT_IN" /D "XAP_DEBUG_CERR" /D "XAP_DEBUG_VALIDATE" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /GZ /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /W3 /GR /GX /ZI /Od /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "XAP_LIB" /D XAP_DEBUG_BUILD=1 /D "WIN32" /D "WIN_ENV" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "XAP_BUILT_IN" /D "XAP_DEBUG_CERR" /D "XAP_DEBUG_VALIDATE" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\XAPToolkitDebug.lib"
# ADD LIB32 /nologo /out:"Debug\XMPToolkitST.lib"

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Release Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "XAPToolkit___Win32_Release_Multithreaded_DLL"
# PROP BASE Intermediate_Dir "XAPToolkit___Win32_Release_Multithreaded_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "XAPToolkit___Win32_Release_Multithreaded_DLL"
# PROP Intermediate_Dir "XAPToolkit___Win32_Release_Multithreaded_DLL"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "NDEBUG" /D "_WINDOWS" /D "XAP_LIB" /D XAP_DEBUG_BUILD=0 /D "WIN32" /D "WIN_ENV" /D "XAP_BUILT_IN" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "NDEBUG" /D "_WINDOWS" /D "XAP_LIB" /D XAP_DEBUG_BUILD=0 /D "WIN32" /D "WIN_ENV" /D "XAP_BUILT_IN" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Release\XAPToolkitRelease.lib"
# ADD LIB32 /nologo /out:"Release\XMPToolkitMTDLL.lib"

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Debug Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "XAPToolkit___Win32_Debug_Multithreaded_DLL"
# PROP BASE Intermediate_Dir "XAPToolkit___Win32_Debug_Multithreaded_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "XAPToolkit___Win32_Debug_Multithreaded_DLL"
# PROP Intermediate_Dir "XAPToolkit___Win32_Debug_Multithreaded_DLL"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /MTd /W3 /GR /GX /ZI /Od /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "XAP_LIB" /D XAP_DEBUG_BUILD=1 /D "WIN32" /D "WIN_ENV" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "XAP_BUILT_IN" /D "XAP_DEBUG_CERR" /D "XAP_DEBUG_VALIDATE" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /GZ /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "XAP_LIB" /D XAP_DEBUG_BUILD=1 /D "WIN32" /D "WIN_ENV" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "XAP_BUILT_IN" /D "XAP_DEBUG_CERR" /D "XAP_DEBUG_VALIDATE" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\XAPToolkitDebug.lib"
# ADD LIB32 /nologo /out:"Debug\XMPToolkitMTDLL.lib"

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Release Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "XAPToolkit___Win32_Release_Multithreaded"
# PROP BASE Intermediate_Dir "XAPToolkit___Win32_Release_Multithreaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "XAPToolkit___Win32_Release_Multithreaded"
# PROP Intermediate_Dir "XAPToolkit___Win32_Release_Multithreaded"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "NDEBUG" /D "_WINDOWS" /D "XAP_LIB" /D XAP_DEBUG_BUILD=0 /D "WIN32" /D "WIN_ENV" /D "XAP_BUILT_IN" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "NDEBUG" /D "_WINDOWS" /D "XAP_LIB" /D XAP_DEBUG_BUILD=0 /D "WIN32" /D "WIN_ENV" /D "XAP_BUILT_IN" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Release\XAPToolkitRelease.lib"
# ADD LIB32 /nologo /out:"Release\XMPToolkitMT.lib"

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Debug Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "XAPToolkit___Win32_Debug_Multithreaded"
# PROP BASE Intermediate_Dir "XAPToolkit___Win32_Debug_Multithreaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "XAPToolkit___Win32_Debug_Multithreaded"
# PROP Intermediate_Dir "XAPToolkit___Win32_Debug_Multithreaded"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /MTd /W3 /GR /GX /ZI /Od /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "XAP_LIB" /D XAP_DEBUG_BUILD=1 /D "WIN32" /D "WIN_ENV" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "XAP_BUILT_IN" /D "XAP_DEBUG_CERR" /D "XAP_DEBUG_VALIDATE" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /GZ /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MTd /W3 /GR /GX /ZI /Od /I "..\..\Headers" /I "..\..\Source\DOM" /I "..\..\Source\Parser" /I "..\..\Source\Toolkit" /D "XAP_LIB" /D XAP_DEBUG_BUILD=1 /D "WIN32" /D "WIN_ENV" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "XAP_BUILT_IN" /D "XAP_DEBUG_CERR" /D "XAP_DEBUG_VALIDATE" /D XAP_FORCE_NORMAL_ALLOC=1 /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\XAPToolkitDebug.lib"
# ADD LIB32 /nologo /out:"Debug\XMPToolkitMT.lib"

!ENDIF 

# Begin Target

# Name "XAPToolkit - Win32 Release"
# Name "XAPToolkit - Win32 Debug"
# Name "XAPToolkit - Win32 Release Single Threaded"
# Name "XAPToolkit - Win32 Debug Single Threaded"
# Name "XAPToolkit - Win32 Release Multithreaded DLL"
# Name "XAPToolkit - Win32 Debug Multithreaded DLL"
# Name "XAPToolkit - Win32 Release Multithreaded"
# Name "XAPToolkit - Win32 Debug Multithreaded"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Source\Toolkit\AllWalkers.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\ConvertUTF.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\ConvertUTF.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\DefaultAllocator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\DefaultAllocator.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\DOMGlue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\DOMGlue.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\DOMWalker.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\gen_convert_text.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\gen_convert_text.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\MetaXAP.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\NormTree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\NormTree.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\NormTreeSerialize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\Paths.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\Paths.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\RDFToNormTrees.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\RDFToNormTrees.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\UtilityXAP.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\XAPObjWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\XAPStatics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\XAPTkAlloc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\XAPTkAlloc.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\XAPTkData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\XAPTkData.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\XAPTkDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\XAPTkFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Toolkit\XAPTkFuncs.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Headers\ConvCodes.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\CWDebugPrefix.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\CWReleasePrefix.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\MetaXAP.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\UtilityXAP.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\XAPAllocator.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\XAPConfigure.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\XAPDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\XAPExcep.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\XAPPaths.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\XAPPathTree.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\XAPStart.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\XAPToolkit.h
# End Source File
# End Group
# Begin Group "DOM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Source\DOM\DOM.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\DOMDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\DOMDoc.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\ElementFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\MacXMLPrefixDebug.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\MacXMLPrefixRelease.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XAP_XPDOMConf.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPDOM.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPDOM.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPElementFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPElementFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPHashtable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPHashtable.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPNode.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPPtrList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPPtrList.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPString.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPStringDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPTypedPtrList.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPXMLParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\DOM\XPXMLParser.h
# End Source File
# End Group
# Begin Group "Parser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Source\Parser\asciitab.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\dllmain.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\hashtable.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\hashtable.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\iasciitab.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\latin1tab.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\nametab.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\utf8tab.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\xmldef.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\xmlparse.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\xmlparse.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\xmlrole.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\xmlrole.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\xmltok.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\xmltok.h
# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\xmltok_impl.c

!IF  "$(CFG)" == "XAPToolkit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Release Single Threaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Debug Single Threaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Release Multithreaded DLL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Debug Multithreaded DLL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Release Multithreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XAPToolkit - Win32 Debug Multithreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Source\Parser\xmltok_impl.h
# End Source File
# End Group
# End Target
# End Project
