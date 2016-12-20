/*
 * Copyright 2012 Hans Leidekker for CodeWeavers
 *
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
 */

#include "wbemprox_private.h"

#include <wingdi.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <winioctl.h>
#include <winver.h>

#include <initguid.h>
#include <d3d10.h>

static const WCHAR class_baseboardW[] =
    {'W','i','n','3','2','_','B','a','s','e','B','o','a','r','d',0};
static const WCHAR class_biosW[] =
    {'W','i','n','3','2','_','B','I','O','S',0};
static const WCHAR class_cdromdriveW[] =
    {'W','i','n','3','2','_','C','D','R','O','M','D','r','i','v','e',0};
static const WCHAR class_compsysW[] =
    {'W','i','n','3','2','_','C','o','m','p','u','t','e','r','S','y','s','t','e','m',0};
static const WCHAR class_datafileW[] =
    {'C','I','M','_','D','a','t','a','F','i','l','e',0};
static const WCHAR class_directoryW[] =
    {'W','i','n','3','2','_','D','i','r','e','c','t','o','r','y',0};
static const WCHAR class_diskdriveW[] =
    {'W','i','n','3','2','_','D','i','s','k','D','r','i','v','e',0};
static const WCHAR class_diskpartitionW[] =
    {'W','i','n','3','2','_','D','i','s','k','P','a','r','t','i','t','i','o','n',0};
static const WCHAR class_logicaldiskW[] =
    {'W','i','n','3','2','_','L','o','g','i','c','a','l','D','i','s','k',0};
static const WCHAR class_logicaldisk2W[] =
    {'C','I','M','_','L','o','g','i','c','a','l','D','i','s','k',0};
static const WCHAR class_networkadapterW[] =
    {'W','i','n','3','2','_','N','e','t','w','o','r','k','A','d','a','p','t','e','r',0};
static const WCHAR class_networkadapterconfigW[] =
    {'W','i','n','3','2','_','N','e','t','w','o','r','k','A','d','a','p','t','e','r',
     'C','o','n','f','i','g','u','r','a','t','i','o','n',0};
static const WCHAR class_osW[] =
    {'W','i','n','3','2','_','O','p','e','r','a','t','i','n','g','S','y','s','t','e','m',0};
static const WCHAR class_paramsW[] =
    {'_','_','P','A','R','A','M','E','T','E','R','S',0};
static const WCHAR class_physicalmediaW[] =
    {'W','i','n','3','2','_','P','h','y','s','i','c','a','l','M','e','d','i','a',0};
static const WCHAR class_physicalmemoryW[] =
    {'W','i','n','3','2','_','P','h','y','s','i','c','a','l','M','e','m','o','r','y',0};
static const WCHAR class_qualifiersW[] =
    {'_','_','Q','U','A','L','I','F','I','E','R','S',0};
static const WCHAR class_process_getowner_outW[] =
    {'_','_','W','I','N','3','2','_','P','R','O','C','E','S','S','_','G','E','T','O','W',
     'N','E','R','_','O','U','T',0};
static const WCHAR class_processorW[] =
    {'W','i','n','3','2','_','P','r','o','c','e','s','s','o','r',0};
static const WCHAR class_processor2W[] =
    {'C','I','M','_','P','r','o','c','e','s','s','o','r',0};
static const WCHAR class_sounddeviceW[] =
    {'W','i','n','3','2','_','S','o','u','n','d','D','e','v','i','c','e',0};
static const WCHAR class_videocontrollerW[] =
    {'W','i','n','3','2','_','V','i','d','e','o','C','o','n','t','r','o','l','l','e','r',0};

static const WCHAR prop_acceptpauseW[] =
    {'A','c','c','e','p','t','P','a','u','s','e',0};
static const WCHAR prop_acceptstopW[] =
    {'A','c','c','e','p','t','S','t','o','p',0};
static const WCHAR prop_accessmaskW[] =
    {'A','c','c','e','s','s','M','a','s','k',0};
static const WCHAR prop_adapterdactypeW[] =
    {'A','d','a','p','t','e','r','D','A','C','T','y','p','e',0};
static const WCHAR prop_adapterramW[] =
    {'A','d','a','p','t','e','r','R','A','M',0};
static const WCHAR prop_adaptertypeW[] =
    {'A','d','a','p','t','e','r','T','y','p','e',0};
static const WCHAR prop_addresswidthW[] =
    {'A','d','d','r','e','s','s','W','i','d','t','h',0};
static const WCHAR prop_availabilityW[] =
    {'A','v','a','i','l','a','b','i','l','i','t','y',0};
static const WCHAR prop_bootableW[] =
    {'B','o','o','t','a','b','l','e',0};
static const WCHAR prop_bootpartitionW[] =
    {'B','o','o','t','P','a','r','t','i','t','i','o','n',0};
static const WCHAR prop_buildnumberW[] =
    {'B','u','i','l','d','N','u','m','b','e','r',0};
static const WCHAR prop_capacityW[] =
    {'C','a','p','a','c','i','t','y',0};
static const WCHAR prop_captionW[] =
    {'C','a','p','t','i','o','n',0};
static const WCHAR prop_classW[] =
    {'C','l','a','s','s',0};
static const WCHAR prop_codesetW[] =
    {'C','o','d','e','S','e','t',0};
static const WCHAR prop_commandlineW[] =
    {'C','o','m','m','a','n','d','L','i','n','e',0};
static const WCHAR prop_countrycodeW[] =
    {'C','o','u','n','t','r','y','C','o','d','e',0};
static const WCHAR prop_cpustatusW[] =
    {'C','p','u','S','t','a','t','u','s',0};
static const WCHAR prop_csdversionW[] =
    {'C','S','D','V','e','r','s','i','o','n',0};
static const WCHAR prop_currentbitsperpixelW[] =
    {'C','u','r','r','e','n','t','B','i','t','s','P','e','r','P','i','x','e','l',0};
static const WCHAR prop_currentclockspeedW[] =
    {'C','u','r','r','e','n','t','C','l','o','c','k','S','p','e','e','d',0};
static const WCHAR prop_currenthorizontalresW[] =
    {'C','u','r','r','e','n','t','H','o','r','i','z','o','n','t','a','l','R','e','s','o','l','u','t','i','o','n',0};
static const WCHAR prop_currentverticalresW[] =
    {'C','u','r','r','e','n','t','V','e','r','t','i','c','a','l','R','e','s','o','l','u','t','i','o','n',0};
static const WCHAR prop_defaultvalueW[] =
    {'D','e','f','a','u','l','t','V','a','l','u','e',0};
static const WCHAR prop_descriptionW[] =
    {'D','e','s','c','r','i','p','t','i','o','n',0};
static const WCHAR prop_deviceidW[] =
    {'D','e','v','i','c','e','I','d',0};
static const WCHAR prop_directionW[] =
    {'D','i','r','e','c','t','i','o','n',0};
static const WCHAR prop_displaynameW[] =
    {'D','i','s','p','l','a','y','N','a','m','e',0};
static const WCHAR prop_diskindexW[] =
    {'D','i','s','k','I','n','d','e','x',0};
static const WCHAR prop_domainW[] =
    {'D','o','m','a','i','n',0};
static const WCHAR prop_domainroleW[] =
    {'D','o','m','a','i','n','R','o','l','e',0};
static const WCHAR prop_driveW[] =
    {'D','r','i','v','e',0};
static const WCHAR prop_drivetypeW[] =
    {'D','r','i','v','e','T','y','p','e',0};
static const WCHAR prop_familyW[] =
    {'F','a','m','i','l','y',0};
static const WCHAR prop_filesystemW[] =
    {'F','i','l','e','S','y','s','t','e','m',0};
static const WCHAR prop_flavorW[] =
    {'F','l','a','v','o','r',0};
static const WCHAR prop_freespaceW[] =
    {'F','r','e','e','S','p','a','c','e',0};
static const WCHAR prop_handleW[] =
    {'H','a','n','d','l','e',0};
static const WCHAR prop_idW[] =
    {'I','D',0};
static const WCHAR prop_identificationcodeW[] =
    {'I','d','e','n','t','i','f','i','c','a','t','i','o','n','C','o','d','e',0};
static const WCHAR prop_indexW[] =
    {'I','n','d','e','x',0};
static const WCHAR prop_installdateW[] =
    {'I','n','s','t','a','l','l','D','a','t','e',0};
static const WCHAR prop_interfaceindexW[] =
    {'I','n','t','e','r','f','a','c','e','I','n','d','e','x',0};
static const WCHAR prop_interfacetypeW[] =
    {'I','n','t','e','r','f','a','c','e','T','y','p','e',0};
static const WCHAR prop_intvalueW[] =
    {'I','n','t','e','g','e','r','V','a','l','u','e',0};
static const WCHAR prop_ipconnectionmetricW[] =
    {'I','P','C','o','n','n','e','c','t','i','o','n','M','e','t','r','i','c',0};
static const WCHAR prop_ipenabledW[] =
    {'I','P','E','n','a','b','l','e','d',0};
static const WCHAR prop_lastbootuptimeW[] =
    {'L','a','s','t','B','o','o','t','U','p','T','i','m','e',0};
static const WCHAR prop_localdatetimeW[] =
    {'L','o','c','a','l','D','a','t','e','T','i','m','e',0};
static const WCHAR prop_localeW[] =
    {'L','o','c','a','l','e',0};
static const WCHAR prop_macaddressW[] =
    {'M','A','C','A','d','d','r','e','s','s',0};
static const WCHAR prop_manufacturerW[] =
    {'M','a','n','u','f','a','c','t','u','r','e','r',0};
static const WCHAR prop_maxclockspeedW[] =
    {'M','a','x','C','l','o','c','k','S','p','e','e','d',0};
static const WCHAR prop_mediatypeW[] =
    {'M','e','d','i','a','T','y','p','e',0};
static const WCHAR prop_memberW[] =
    {'M','e','m','b','e','r',0};
static const WCHAR prop_methodW[] =
    {'M','e','t','h','o','d',0};
static const WCHAR prop_modelW[] =
    {'M','o','d','e','l',0};
static const WCHAR prop_netconnectionstatusW[] =
    {'N','e','t','C','o','n','n','e','c','t','i','o','n','S','t','a','t','u','s',0};
static const WCHAR prop_numcoresW[] =
    {'N','u','m','b','e','r','O','f','C','o','r','e','s',0};
static const WCHAR prop_numlogicalprocessorsW[] =
    {'N','u','m','b','e','r','O','f','L','o','g','i','c','a','l','P','r','o','c','e','s','s','o','r','s',0};
static const WCHAR prop_numprocessorsW[] =
    {'N','u','m','b','e','r','O','f','P','r','o','c','e','s','s','o','r','s',0};
static const WCHAR prop_osarchitectureW[] =
    {'O','S','A','r','c','h','i','t','e','c','t','u','r','e',0};
static const WCHAR prop_oslanguageW[] =
    {'O','S','L','a','n','g','u','a','g','e',0};
static const WCHAR prop_osproductsuiteW[] =
    {'O','S','P','r','o','d','u','c','t','S','u','i','t','e',0};
static const WCHAR prop_ostypeW[] =
    {'O','S','T','y','p','e',0};
static const WCHAR prop_parameterW[] =
    {'P','a','r','a','m','e','t','e','r',0};
static const WCHAR prop_physicaladapterW[] =
    {'P','h','y','s','i','c','a','l','A','d','a','p','t','e','r',0};
static const WCHAR prop_pnpdeviceidW[] =
    {'P','N','P','D','e','v','i','c','e','I','D',0};
static const WCHAR prop_pprocessidW[] =
    {'P','a','r','e','n','t','P','r','o','c','e','s','s','I','D',0};
static const WCHAR prop_processidW[] =
    {'P','r','o','c','e','s','s','I','D',0};
static const WCHAR prop_processoridW[] =
    {'P','r','o','c','e','s','s','o','r','I','d',0};
static const WCHAR prop_productnameW[] =
    {'P','r','o','d','u','c','t','N','a','m','e',0};
static const WCHAR prop_releasedateW[] =
    {'R','e','l','e','a','s','e','D','a','t','e',0};
static const WCHAR prop_serialnumberW[] =
    {'S','e','r','i','a','l','N','u','m','b','e','r',0};
static const WCHAR prop_servicepackmajorW[] =
    {'S','e','r','v','i','c','e','P','a','c','k','M','a','j','o','r','V','e','r','s','i','o','n',0};
static const WCHAR prop_servicepackminorW[] =
    {'S','e','r','v','i','c','e','P','a','c','k','M','i','n','o','r','V','e','r','s','i','o','n',0};
static const WCHAR prop_servicetypeW[] =
    {'S','e','r','v','i','c','e','T','y','p','e',0};
static const WCHAR prop_smbiosbiosversionW[] =
    {'S','M','B','I','O','S','B','I','O','S','V','e','r','s','i','o','n',0};
static const WCHAR prop_startmodeW[] =
    {'S','t','a','r','t','M','o','d','e',0};
static const WCHAR prop_sizeW[] =
    {'S','i','z','e',0};
static const WCHAR prop_speedW[] =
    {'S','p','e','e','d',0};
static const WCHAR prop_startingoffsetW[] =
    {'S','t','a','r','t','i','n','g','O','f','f','s','e','t',0};
static const WCHAR prop_stateW[] =
    {'S','t','a','t','e',0};
static const WCHAR prop_strvalueW[] =
    {'S','t','r','i','n','g','V','a','l','u','e',0};
static const WCHAR prop_suitemaskW[] =
    {'S','u','i','t','e','M','a','s','k',0};
static const WCHAR prop_systemdirectoryW[] =
    {'S','y','s','t','e','m','D','i','r','e','c','t','o','r','y',0};
static const WCHAR prop_systemnameW[] =
    {'S','y','s','t','e','m','N','a','m','e',0};
static const WCHAR prop_tagW[] =
    {'T','a','g',0};
static const WCHAR prop_threadcountW[] =
    {'T','h','r','e','a','d','C','o','u','n','t',0};
static const WCHAR prop_totalphysicalmemoryW[] =
    {'T','o','t','a','l','P','h','y','s','i','c','a','l','M','e','m','o','r','y',0};
static const WCHAR prop_typeW[] =
    {'T','y','p','e',0};
static const WCHAR prop_uniqueidW[] =
    {'U','n','i','q','u','e','I','d',0};
static const WCHAR prop_varianttypeW[] =
    {'V','a','r','i','a','n','t','T','y','p','e',0};
static const WCHAR prop_versionW[] =
    {'V','e','r','s','i','o','n',0};
static const WCHAR prop_volumeserialnumberW[] =
    {'V','o','l','u','m','e','S','e','r','i','a','l','N','u','m','b','e','r',0};

/* column definitions must be kept in sync with record structures below */
static const struct column col_baseboard[] =
{
    { prop_manufacturerW,  CIM_STRING },
    { prop_modelW,         CIM_STRING },
    { prop_nameW,          CIM_STRING },
    { prop_serialnumberW,  CIM_STRING },
    { prop_tagW,           CIM_STRING|COL_FLAG_KEY }
};
static const struct column col_bios[] =
{
    { prop_descriptionW,        CIM_STRING },
    { prop_identificationcodeW, CIM_STRING },
    { prop_manufacturerW,       CIM_STRING },
    { prop_releasedateW,        CIM_DATETIME },
    { prop_serialnumberW,       CIM_STRING },
    { prop_smbiosbiosversionW,  CIM_STRING },
    { prop_versionW,            CIM_STRING|COL_FLAG_KEY }
};
static const struct column col_cdromdrive[] =
{
    { prop_deviceidW,    CIM_STRING|COL_FLAG_KEY },
    { prop_driveW,       CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_mediatypeW,   CIM_STRING },
    { prop_nameW,        CIM_STRING },
    { prop_pnpdeviceidW, CIM_STRING }
};
static const struct column col_compsys[] =
{
    { prop_descriptionW,          CIM_STRING },
    { prop_domainW,               CIM_STRING },
    { prop_domainroleW,           CIM_UINT16, VT_I4 },
    { prop_manufacturerW,         CIM_STRING },
    { prop_modelW,                CIM_STRING },
    { prop_nameW,                 CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_numlogicalprocessorsW, CIM_UINT32, VT_I4 },
    { prop_numprocessorsW,        CIM_UINT32, VT_I4 },
    { prop_totalphysicalmemoryW,  CIM_UINT64 }
};
static const struct column col_datafile[] =
{
    { prop_nameW,    CIM_STRING|COL_FLAG_DYNAMIC|COL_FLAG_KEY },
    { prop_versionW, CIM_STRING|COL_FLAG_DYNAMIC }
};
static const struct column col_directory[] =
{
    { prop_accessmaskW, CIM_UINT32 },
    { prop_nameW,       CIM_STRING|COL_FLAG_DYNAMIC|COL_FLAG_KEY }
};
static const struct column col_diskdrive[] =
{
    { prop_deviceidW,      CIM_STRING|COL_FLAG_DYNAMIC|COL_FLAG_KEY },
    { prop_indexW,         CIM_UINT32, VT_I4 },
    { prop_interfacetypeW, CIM_STRING },
    { prop_manufacturerW,  CIM_STRING },
    { prop_mediatypeW,     CIM_STRING },
    { prop_modelW,         CIM_STRING },
    { prop_serialnumberW,  CIM_STRING },
    { prop_sizeW,          CIM_UINT64 }
};
static const struct column col_diskpartition[] =
{
    { prop_bootableW,       CIM_BOOLEAN },
    { prop_bootpartitionW,  CIM_BOOLEAN },
    { prop_deviceidW,       CIM_STRING|COL_FLAG_DYNAMIC|COL_FLAG_KEY },
    { prop_diskindexW,      CIM_UINT32, VT_I4 },
    { prop_indexW,          CIM_UINT32, VT_I4 },
    { prop_pnpdeviceidW,    CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_sizeW,           CIM_UINT64 },
    { prop_startingoffsetW, CIM_UINT64 },
    { prop_typeW,           CIM_STRING|COL_FLAG_DYNAMIC }
};
static const struct column col_logicaldisk[] =
{
    { prop_deviceidW,           CIM_STRING|COL_FLAG_DYNAMIC|COL_FLAG_KEY },
    { prop_drivetypeW,          CIM_UINT32, VT_I4 },
    { prop_filesystemW,         CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_freespaceW,          CIM_UINT64 },
    { prop_nameW,               CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_sizeW,               CIM_UINT64 },
    { prop_volumeserialnumberW, CIM_STRING|COL_FLAG_DYNAMIC }
};
static const struct column col_networkadapter[] =
{
    { prop_adaptertypeW,         CIM_STRING },
    { prop_deviceidW,            CIM_STRING|COL_FLAG_DYNAMIC|COL_FLAG_KEY },
    { prop_indexW,               CIM_UINT32, VT_I4 },
    { prop_interfaceindexW,      CIM_UINT32, VT_I4 },
    { prop_macaddressW,          CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_manufacturerW,        CIM_STRING },
    { prop_nameW,                CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_netconnectionstatusW, CIM_UINT16, VT_I4 },
    { prop_physicaladapterW,     CIM_BOOLEAN },
    { prop_pnpdeviceidW,         CIM_STRING },
    { prop_speedW,               CIM_UINT64 }
};
static const struct column col_networkadapterconfig[] =
{
    { prop_indexW,              CIM_UINT32|COL_FLAG_KEY, VT_I4 },
    { prop_ipconnectionmetricW, CIM_UINT32, VT_I4 },
    { prop_ipenabledW,          CIM_BOOLEAN },
    { prop_macaddressW,         CIM_STRING|COL_FLAG_DYNAMIC }
};
static const struct column col_os[] =
{
    { prop_buildnumberW,      CIM_STRING },
    { prop_captionW,          CIM_STRING },
    { prop_codesetW,          CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_countrycodeW,      CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_csdversionW,       CIM_STRING },
    { prop_installdateW,      CIM_DATETIME },
    { prop_lastbootuptimeW,   CIM_DATETIME|COL_FLAG_DYNAMIC },
    { prop_localdatetimeW,    CIM_DATETIME|COL_FLAG_DYNAMIC },
    { prop_localeW,           CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_nameW,             CIM_STRING },
    { prop_osarchitectureW,   CIM_STRING },
    { prop_oslanguageW,       CIM_UINT32, VT_I4 },
    { prop_osproductsuiteW,   CIM_UINT32, VT_I4 },
    { prop_ostypeW,           CIM_UINT16, VT_I4 },
    { prop_serialnumberW,     CIM_STRING },
    { prop_servicepackmajorW, CIM_UINT16, VT_I4 },
    { prop_servicepackminorW, CIM_UINT16, VT_I4 },
    { prop_suitemaskW,        CIM_UINT32, VT_I4 },
    { prop_systemdirectoryW,  CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_versionW,          CIM_STRING }
};
static const struct column col_param[] =
{
    { prop_classW,        CIM_STRING },
    { prop_methodW,       CIM_STRING },
    { prop_directionW,    CIM_SINT32 },
    { prop_parameterW,    CIM_STRING },
    { prop_typeW,         CIM_UINT32 },
    { prop_varianttypeW,  CIM_UINT32 },
    { prop_defaultvalueW, CIM_UINT32 }
};
static const struct column col_physicalmedia[] =
{
    { prop_serialnumberW,       CIM_STRING },
    { prop_tagW,                CIM_STRING }
};
static const struct column col_physicalmemory[] =
{
    { prop_capacityW,   CIM_UINT64 }
};
static const struct column col_process[] =
{
    { prop_captionW,     CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_commandlineW, CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_descriptionW, CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_handleW,      CIM_STRING|COL_FLAG_DYNAMIC|COL_FLAG_KEY },
    { prop_nameW,        CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_pprocessidW,  CIM_UINT32, VT_I4 },
    { prop_processidW,   CIM_UINT32, VT_I4 },
    { prop_threadcountW, CIM_UINT32, VT_I4 },
    /* methods */
    { method_getownerW,  CIM_FLAG_ARRAY|COL_FLAG_METHOD }
};
static const struct column col_processor[] =
{
    { prop_addresswidthW,         CIM_UINT16, VT_I4 },
    { prop_cpustatusW,            CIM_UINT16 },
    { prop_currentclockspeedW,    CIM_UINT32, VT_I4 },
    { prop_deviceidW,             CIM_STRING|COL_FLAG_DYNAMIC|COL_FLAG_KEY },
    { prop_familyW,               CIM_UINT16, VT_I4 },
    { prop_manufacturerW,         CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_maxclockspeedW,        CIM_UINT32, VT_I4 },
    { prop_nameW,                 CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_numcoresW,             CIM_UINT32, VT_I4 },
    { prop_numlogicalprocessorsW, CIM_UINT32, VT_I4 },
    { prop_processoridW,          CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_uniqueidW,             CIM_STRING }
};
static const struct column col_qualifier[] =
{
    { prop_classW,    CIM_STRING },
    { prop_memberW,   CIM_STRING },
    { prop_typeW,     CIM_UINT32 },
    { prop_flavorW,   CIM_SINT32 },
    { prop_nameW,     CIM_STRING },
    { prop_intvalueW, CIM_SINT32 },
    { prop_strvalueW, CIM_STRING }
};
static const struct column col_service[] =
{
    { prop_acceptpauseW,      CIM_BOOLEAN },
    { prop_acceptstopW,       CIM_BOOLEAN },
    { prop_displaynameW,      CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_nameW,             CIM_STRING|COL_FLAG_DYNAMIC|COL_FLAG_KEY },
    { prop_processidW,        CIM_UINT32 },
    { prop_servicetypeW,      CIM_STRING },
    { prop_startmodeW,        CIM_STRING },
    { prop_stateW,            CIM_STRING },
    { prop_systemnameW,       CIM_STRING|COL_FLAG_DYNAMIC },
    /* methods */
    { method_pauseserviceW,   CIM_FLAG_ARRAY|COL_FLAG_METHOD },
    { method_resumeserviceW,  CIM_FLAG_ARRAY|COL_FLAG_METHOD },
    { method_startserviceW,   CIM_FLAG_ARRAY|COL_FLAG_METHOD },
    { method_stopserviceW,    CIM_FLAG_ARRAY|COL_FLAG_METHOD }
};
static const struct column col_sounddevice[] =
{
    { prop_nameW,        CIM_STRING },
    { prop_productnameW, CIM_STRING }
};
static const struct column col_stdregprov[] =
{
    { method_enumkeyW,        CIM_FLAG_ARRAY|COL_FLAG_METHOD },
    { method_enumvaluesW,     CIM_FLAG_ARRAY|COL_FLAG_METHOD },
    { method_getstringvalueW, CIM_FLAG_ARRAY|COL_FLAG_METHOD }
};
static const struct column col_systemsecurity[] =
{
    { method_getsdW,                    CIM_FLAG_ARRAY|COL_FLAG_METHOD },
    { method_setsdW,                    CIM_FLAG_ARRAY|COL_FLAG_METHOD },
};
static const struct column col_videocontroller[] =
{
    { prop_adapterdactypeW,       CIM_STRING },
    { prop_adapterramW,           CIM_UINT32, VT_I4 },
    { prop_availabilityW,         CIM_UINT16 },
    { prop_currentbitsperpixelW,  CIM_UINT32, VT_I4 },
    { prop_currenthorizontalresW, CIM_UINT32, VT_I4 },
    { prop_currentverticalresW,   CIM_UINT32, VT_I4 },
    { prop_descriptionW,          CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_deviceidW,             CIM_STRING|COL_FLAG_KEY },
    { prop_nameW,                 CIM_STRING|COL_FLAG_DYNAMIC },
    { prop_pnpdeviceidW,          CIM_STRING|COL_FLAG_DYNAMIC }
};

static const WCHAR baseboard_manufacturerW[] =
    {'I','n','t','e','l',' ','C','o','r','p','o','r','a','t','i','o','n',0};
static const WCHAR baseboard_serialnumberW[] =
    {'N','o','n','e',0};
static const WCHAR baseboard_tagW[] =
    {'B','a','s','e',' ','B','o','a','r','d',0};
static const WCHAR bios_descriptionW[] =
    {'D','e','f','a','u','l','t',' ','S','y','s','t','e','m',' ','B','I','O','S',0};
static const WCHAR bios_manufacturerW[] =
    {'T','h','e',' ','W','i','n','e',' ','P','r','o','j','e','c','t',0};
static const WCHAR bios_releasedateW[] =
    {'2','0','1','2','0','6','0','8','0','0','0','0','0','0','.','0','0','0','0','0','0','+','0','0','0',0};
static const WCHAR bios_serialnumberW[] =
    {'0',0};
static const WCHAR bios_smbiosbiosversionW[] =
    {'W','i','n','e',0};
static const WCHAR bios_versionW[] =
    {'W','I','N','E',' ',' ',' ','-',' ','1',0};
static const WCHAR cdromdrive_mediatypeW[] =
    {'C','D','-','R','O','M',0};
static const WCHAR cdromdrive_nameW[] =
    {'W','i','n','e',' ','C','D','-','R','O','M',' ','A','T','A',' ','D','e','v','i','c','e',0};
static const WCHAR cdromdrive_pnpdeviceidW[]=
    {'I','D','E','\\','C','D','R','O','M','W','I','N','E','_','C','D','-','R','O','M',
     '_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',
     '_','_','_','_','_','_','_','1','.','0','_','_','_','_','_','\\','5','&','3','A','2',
     'A','5','8','5','4','&','0','&','1','.','0','.','0',0};
static const WCHAR compsys_descriptionW[] =
    {'A','T','/','A','T',' ','C','O','M','P','A','T','I','B','L','E',0};
static const WCHAR compsys_domainW[] =
    {'W','O','R','K','G','R','O','U','P',0};
static const WCHAR compsys_manufacturerW[] =
    {'T','h','e',' ','W','i','n','e',' ','P','r','o','j','e','c','t',0};
static const WCHAR compsys_modelW[] =
    {'W','i','n','e',0};
static const WCHAR diskdrive_interfacetypeW[] =
    {'I','D','E',0};
static const WCHAR diskdrive_manufacturerW[] =
    {'(','S','t','a','n','d','a','r','d',' ','d','i','s','k',' ','d','r','i','v','e','s',')',0};
static const WCHAR diskdrive_mediatype_fixedW[] =
    {'F','i','x','e','d',' ','h','a','r','d',' ','d','i','s','k',0};
static const WCHAR diskdrive_mediatype_removableW[] =
    {'R','e','m','o','v','a','b','l','e',' ','m','e','d','i','a',0};
static const WCHAR diskdrive_modelW[] =
    {'W','i','n','e',' ','D','i','s','k',' ','D','r','i','v','e',0};
static const WCHAR diskdrive_serialW[] =
    {'W','I','N','E','H','D','I','S','K',0};
static const WCHAR networkadapter_pnpdeviceidW[]=
    {'P','C','I','\\','V','E','N','_','8','0','8','6','&','D','E','V','_','1','0','0','E','&',
     'S','U','B','S','Y','S','_','0','0','1','E','8','0','8','6','&','R','E','V','_','0','2','\\',
     '3','&','2','6','7','A','6','1','6','A','&','1','&','1','8',0};
static const WCHAR os_32bitW[] =
    {'3','2','-','b','i','t',0};
static const WCHAR os_64bitW[] =
    {'6','4','-','b','i','t',0};
static const WCHAR os_buildnumberW[] =
    {'2','6','0','0',0};
static const WCHAR os_captionW[] =
    {'M','i','c','r','o','s','o','f','t',' ','W','i','n','d','o','w','s',' ','X','P',' ',
     'V','e','r','s','i','o','n',' ','=',' ','5','.','1','.','2','6','0','0',0};
static const WCHAR os_csdversionW[] =
    {'S','e','r','v','i','c','e',' ','P','a','c','k',' ','3',0};
static const WCHAR os_installdateW[] =
    {'2','0','1','4','0','1','0','1','0','0','0','0','0','0','.','0','0','0','0','0','0','+','0','0','0',0};
static const WCHAR os_nameW[] =
    {'M','i','c','r','o','s','o','f','t',' ','W','i','n','d','o','w','s',' ','X','P',' ',
     'P','r','o','f','e','s','s','i','o','n','a','l','|','C',':','\\','W','I','N','D','O','W','S',
     '|','\\','D','e','v','i','c','e','\\','H','a','r','d','d','i','s','k','0',
     '\\','P','a','r','t','i','t','i','o','n','1',0};
static const WCHAR os_serialnumberW[] =
    {'1','2','3','4','5','-','O','E','M','-','1','2','3','4','5','6','7','-','1','2','3','4','5',0};
static const WCHAR os_versionW[] =
    {'5','.','1','.','2','6','0','0',0};
static const WCHAR physicalmedia_tagW[] =
    {'\\','\\','.','\\','P','H','Y','S','I','C','A','L','D','R','I','V','E','0',0};
static const WCHAR sounddevice_productnameW[] =
    {'W','i','n','e',' ','A','u','d','i','o',' ','D','e','v','i','c','e',0};
static const WCHAR videocontroller_dactypeW[] =
    {'I','n','t','e','g','r','a','t','e','d',' ','R','A','M','D','A','C',0};
static const WCHAR videocontroller_deviceidW[] =
    {'V','i','d','e','o','C','o','n','t','r','o','l','l','e','r','1',0};

#include "pshpack1.h"
struct record_baseboard
{
    const WCHAR *manufacturer;
    const WCHAR *model;
    const WCHAR *name;
    const WCHAR *serialnumber;
    const WCHAR *tag;
};
struct record_bios
{
    const WCHAR *description;
    const WCHAR *identificationcode;
    const WCHAR *manufacturer;
    const WCHAR *releasedate;
    const WCHAR *serialnumber;
    const WCHAR *smbiosbiosversion;
    const WCHAR *version;
};
struct record_cdromdrive
{
    const WCHAR *device_id;
    const WCHAR *drive;
    const WCHAR *mediatype;
    const WCHAR *name;
    const WCHAR *pnpdevice_id;
};
struct record_computersystem
{
    const WCHAR *description;
    const WCHAR *domain;
    UINT16       domainrole;
    const WCHAR *manufacturer;
    const WCHAR *model;
    const WCHAR *name;
    UINT32       num_logical_processors;
    UINT32       num_processors;
    UINT64       total_physical_memory;
};
struct record_datafile
{
    const WCHAR *name;
    const WCHAR *version;
};
struct record_directory
{
    UINT32       accessmask;
    const WCHAR *name;
};
struct record_diskdrive
{
    const WCHAR *device_id;
    UINT32       index;
    const WCHAR *interfacetype;
    const WCHAR *manufacturer;
    const WCHAR *mediatype;
    const WCHAR *model;
    const WCHAR *serialnumber;
    UINT64       size;
};
struct record_diskpartition
{
    int          bootable;
    int          bootpartition;
    const WCHAR *device_id;
    UINT32       diskindex;
    UINT32       index;
    const WCHAR *pnpdevice_id;
    UINT64       size;
    UINT64       startingoffset;
    const WCHAR *type;
};
struct record_logicaldisk
{
    const WCHAR *device_id;
    UINT32       drivetype;
    const WCHAR *filesystem;
    UINT64       freespace;
    const WCHAR *name;
    UINT64       size;
    const WCHAR *volumeserialnumber;
};
struct record_networkadapter
{
    const WCHAR *adaptertype;
    const WCHAR *device_id;
    UINT32       index;
    UINT32       interface_index;
    const WCHAR *mac_address;
    const WCHAR *manufacturer;
    const WCHAR *name;
    UINT16       netconnection_status;
    int          physicaladapter;
    const WCHAR *pnpdevice_id;
    UINT64       speed;
};
struct record_networkadapterconfig
{
    UINT32       index;
    UINT32       ipconnectionmetric;
    int          ipenabled;
    const WCHAR *mac_address;
};
struct record_operatingsystem
{
    const WCHAR *buildnumber;
    const WCHAR *caption;
    const WCHAR *codeset;
    const WCHAR *countrycode;
    const WCHAR *csdversion;
    const WCHAR *installdate;
    const WCHAR *lastbootuptime;
    const WCHAR *localdatetime;
    const WCHAR *locale;
    const WCHAR *name;
    const WCHAR *osarchitecture;
    UINT32       oslanguage;
    UINT32       osproductsuite;
    UINT16       ostype;
    const WCHAR *serialnumber;
    UINT16       servicepackmajor;
    UINT16       servicepackminor;
    UINT32       suitemask;
    const WCHAR *systemdirectory;
    const WCHAR *version;
};
struct record_param
{
    const WCHAR *class;
    const WCHAR *method;
    INT32        direction;
    const WCHAR *parameter;
    UINT32       type;
    UINT32       varianttype;
    UINT32       defaultvalue;
};
struct record_physicalmedia
{
    const WCHAR *serialnumber;
    const WCHAR *tag;
};
struct record_physicalmemory
{
    UINT64 capacity;
};
struct record_process
{
    const WCHAR *caption;
    const WCHAR *commandline;
    const WCHAR *description;
    const WCHAR *handle;
    const WCHAR *name;
    UINT32       pprocess_id;
    UINT32       process_id;
    UINT32       thread_count;
    /* methods */
    class_method *get_owner;
};
struct record_processor
{
    UINT16       addresswidth;
    UINT16       cpu_status;
    UINT32       currentclockspeed;
    const WCHAR *device_id;
    UINT16       family;
    const WCHAR *manufacturer;
    UINT32       maxclockspeed;
    const WCHAR *name;
    UINT32       num_cores;
    UINT32       num_logical_processors;
    const WCHAR *processor_id;
    const WCHAR *unique_id;
};
struct record_qualifier
{
    const WCHAR *class;
    const WCHAR *member;
    UINT32       type;
    INT32        flavor;
    const WCHAR *name;
    INT32        intvalue;
    const WCHAR *strvalue;
};
struct record_service
{
    int          accept_pause;
    int          accept_stop;
    const WCHAR *displayname;
    const WCHAR *name;
    UINT32       process_id;
    const WCHAR *servicetype;
    const WCHAR *startmode;
    const WCHAR *state;
    const WCHAR *systemname;
    /* methods */
    class_method *pause_service;
    class_method *resume_service;
    class_method *start_service;
    class_method *stop_service;
};
struct record_sounddevice
{
    const WCHAR *name;
    const WCHAR *productname;
};
struct record_stdregprov
{
    class_method *enumkey;
    class_method *enumvalues;
    class_method *getstringvalue;
};
struct record_systemsecurity
{
    class_method *getsd;
    class_method *setsd;
};
struct record_videocontroller
{
    const WCHAR *adapter_dactype;
    UINT32       adapter_ram;
    UINT16       availability;
    UINT32       current_bitsperpixel;
    UINT32       current_horizontalres;
    UINT32       current_verticalres;
    const WCHAR *description;
    const WCHAR *device_id;
    const WCHAR *name;
    const WCHAR *pnpdevice_id;
};
#include "poppack.h"

static const struct record_baseboard data_baseboard[] =
{
    { baseboard_manufacturerW, baseboard_tagW, baseboard_tagW, baseboard_serialnumberW, baseboard_tagW }
};
static const struct record_bios data_bios[] =
{
    { bios_descriptionW, bios_descriptionW, bios_manufacturerW, bios_releasedateW, bios_serialnumberW,
      bios_smbiosbiosversionW, bios_versionW }
};
static const struct record_param data_param[] =
{
    { class_processW, method_getownerW, -1, param_returnvalueW, CIM_UINT32, VT_I4 },
    { class_processW, method_getownerW, -1, param_userW, CIM_STRING },
    { class_processW, method_getownerW, -1, param_domainW, CIM_STRING },
    { class_serviceW, method_pauseserviceW, -1, param_returnvalueW, CIM_UINT32, VT_I4 },
    { class_serviceW, method_resumeserviceW, -1, param_returnvalueW, CIM_UINT32, VT_I4 },
    { class_serviceW, method_startserviceW, -1, param_returnvalueW, CIM_UINT32, VT_I4 },
    { class_serviceW, method_stopserviceW, -1, param_returnvalueW, CIM_UINT32, VT_I4 },
    { class_stdregprovW, method_enumkeyW, 1, param_defkeyW, CIM_SINT32, 0, 0x80000002 },
    { class_stdregprovW, method_enumkeyW, 1, param_subkeynameW, CIM_STRING },
    { class_stdregprovW, method_enumkeyW, -1, param_returnvalueW, CIM_UINT32, VT_I4 },
    { class_stdregprovW, method_enumkeyW, -1, param_namesW, CIM_STRING|CIM_FLAG_ARRAY },
    { class_stdregprovW, method_enumvaluesW, 1, param_defkeyW, CIM_SINT32, 0, 0x80000002 },
    { class_stdregprovW, method_enumvaluesW, 1, param_subkeynameW, CIM_STRING },
    { class_stdregprovW, method_enumvaluesW, -1, param_returnvalueW, CIM_UINT32, VT_I4 },
    { class_stdregprovW, method_enumvaluesW, -1, param_namesW, CIM_STRING|CIM_FLAG_ARRAY },
    { class_stdregprovW, method_enumvaluesW, -1, param_typesW, CIM_SINT32|CIM_FLAG_ARRAY },
    { class_stdregprovW, method_getstringvalueW, 1, param_defkeyW, CIM_SINT32, 0, 0x80000002 },
    { class_stdregprovW, method_getstringvalueW, 1, param_subkeynameW, CIM_STRING },
    { class_stdregprovW, method_getstringvalueW, 1, param_valuenameW, CIM_STRING },
    { class_stdregprovW, method_getstringvalueW, -1, param_returnvalueW, CIM_UINT32, VT_I4 },
    { class_stdregprovW, method_getstringvalueW, -1, param_valueW, CIM_STRING },
    { class_systemsecurityW, method_getsdW, -1, param_returnvalueW, CIM_UINT32, VT_I4 },
    { class_systemsecurityW, method_getsdW, -1, param_sdW, CIM_UINT8|CIM_FLAG_ARRAY },
    { class_systemsecurityW, method_setsdW, 1, param_sdW, CIM_UINT8|CIM_FLAG_ARRAY },
    { class_systemsecurityW, method_setsdW, -1, param_returnvalueW, CIM_UINT32, VT_I4 },
};

#define FLAVOR_ID (WBEM_FLAVOR_FLAG_PROPAGATE_TO_INSTANCE | WBEM_FLAVOR_NOT_OVERRIDABLE |\
                   WBEM_FLAVOR_ORIGIN_PROPAGATED)

static const struct record_physicalmedia data_physicalmedia[] =
{
    { diskdrive_serialW, physicalmedia_tagW }
};
static const struct record_qualifier data_qualifier[] =
{
    { class_process_getowner_outW, param_userW, CIM_SINT32, FLAVOR_ID, prop_idW, 0 },
    { class_process_getowner_outW, param_domainW, CIM_SINT32, FLAVOR_ID, prop_idW, 1 }
};
static const struct record_sounddevice data_sounddevice[] =
{
    { sounddevice_productnameW, sounddevice_productnameW }
};
static const struct record_stdregprov data_stdregprov[] =
{
    { reg_enum_key, reg_enum_values, reg_get_stringvalue }
};
static const struct record_systemsecurity data_systemsecurity[] =
{
    { security_get_sd, security_set_sd }
};

/* check if row matches condition and update status */
static BOOL match_row( const struct table *table, UINT row, const struct expr *cond, enum fill_status *status )
{
    LONGLONG val;
    UINT type;

    if (!cond)
    {
        *status = FILL_STATUS_UNFILTERED;
        return TRUE;
    }
    if (eval_cond( table, row, cond, &val, &type ) != S_OK)
    {
        *status = FILL_STATUS_FAILED;
        return FALSE;
    }
    *status = FILL_STATUS_FILTERED;
    return val != 0;
}

static BOOL resize_table( struct table *table, UINT row_count, UINT row_size )
{
    if (!table->num_rows_allocated)
    {
        if (!(table->data = heap_alloc( row_count * row_size ))) return FALSE;
        table->num_rows_allocated = row_count;
        return TRUE;
    }
    if (row_count >= table->num_rows_allocated)
    {
        BYTE *data;
        UINT count = table->num_rows_allocated * 2;
        if (!(data = heap_realloc( table->data, count * row_size ))) return FALSE;
        table->data = data;
        table->num_rows_allocated = count;
    }
    return TRUE;
}

static enum fill_status fill_cdromdrive( struct table *table, const struct expr *cond )
{
    static const WCHAR fmtW[] = {'%','c',':',0};
    WCHAR drive[3], root[] = {'A',':','\\',0};
    struct record_cdromdrive *rec;
    UINT i, row = 0, offset = 0;
    DWORD drives = GetLogicalDrives();
    enum fill_status status = FILL_STATUS_UNFILTERED;

    if (!resize_table( table, 1, sizeof(*rec) )) return FILL_STATUS_FAILED;

    for (i = 0; i < sizeof(drives); i++)
    {
        if (drives & (1 << i))
        {
            root[0] = 'A' + i;
            if (GetDriveTypeW( root ) != DRIVE_CDROM)
                continue;

            if (!resize_table( table, row + 1, sizeof(*rec) )) return FILL_STATUS_FAILED;

            rec = (struct record_cdromdrive *)(table->data + offset);
            rec->device_id    = cdromdrive_pnpdeviceidW;
            sprintfW( drive, fmtW, 'A' + i );
            rec->drive        = heap_strdupW( drive );
            rec->mediatype    = cdromdrive_mediatypeW;
            rec->name         = cdromdrive_nameW;
            rec->pnpdevice_id = cdromdrive_pnpdeviceidW;
            if (!match_row( table, row, cond, &status ))
            {
                free_row_values( table, row );
                continue;
            }
            offset += sizeof(*rec);
            row++;
        }
    }
    TRACE("created %u rows\n", row);
    table->num_rows = row;
    return status;
}

static UINT get_processor_count(void)
{
    SYSTEM_BASIC_INFORMATION info;

    if (NtQuerySystemInformation( SystemBasicInformation, &info, sizeof(info), NULL )) return 1;
    return info.NumberOfProcessors;
}

static UINT get_logical_processor_count( UINT *num_cores )
{
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION *info;
    UINT i, j, count = 0;
    NTSTATUS status;
    ULONG len;

    if (num_cores) *num_cores = get_processor_count();
    status = NtQuerySystemInformation( SystemLogicalProcessorInformation, NULL, 0, &len );
    if (status != STATUS_INFO_LENGTH_MISMATCH) return get_processor_count();

    if (!(info = heap_alloc( len ))) return get_processor_count();
    status = NtQuerySystemInformation( SystemLogicalProcessorInformation, info, len, &len );
    if (status != STATUS_SUCCESS)
    {
        heap_free( info );
        return get_processor_count();
    }
    if (num_cores) *num_cores = 0;
    for (i = 0; i < len / sizeof(*info); i++)
    {
        if (info[i].Relationship == RelationProcessorCore)
        {
            for (j = 0; j < sizeof(ULONG_PTR); j++) if (info[i].ProcessorMask & (1 << j)) count++;
        }
        else if (info[i].Relationship == RelationProcessorPackage && num_cores)
        {
            for (j = 0; j < sizeof(ULONG_PTR); j++) if (info[i].ProcessorMask & (1 << j)) (*num_cores)++;
        }
    }
    heap_free( info );
    return count;
}

static UINT64 get_total_physical_memory(void)
{
    MEMORYSTATUSEX status;

    status.dwLength = sizeof(status);
    if (!GlobalMemoryStatusEx( &status )) return 1024 * 1024 * 1024;
    return status.ullTotalPhys;
}

static WCHAR *get_computername(void)
{
    WCHAR *ret;
    DWORD size = MAX_COMPUTERNAME_LENGTH;

    if (!(ret = heap_alloc( size * sizeof(WCHAR) ))) return NULL;
    GetComputerNameW( ret, &size );
    return ret;
}

static enum fill_status fill_compsys( struct table *table, const struct expr *cond )
{
    struct record_computersystem *rec;
    enum fill_status status = FILL_STATUS_UNFILTERED;
    UINT row = 0;

    if (!resize_table( table, 1, sizeof(*rec) )) return FILL_STATUS_FAILED;

    rec = (struct record_computersystem *)table->data;
    rec->description            = compsys_descriptionW;
    rec->domain                 = compsys_domainW;
    rec->domainrole             = 0; /* standalone workstation */
    rec->manufacturer           = compsys_manufacturerW;
    rec->model                  = compsys_modelW;
    rec->name                   = get_computername();
    rec->num_logical_processors = get_logical_processor_count( NULL );
    rec->num_processors         = get_processor_count();
    rec->total_physical_memory  = get_total_physical_memory();
    if (!match_row( table, row, cond, &status )) free_row_values( table, row );
    else row++;

    TRACE("created %u rows\n", row);
    table->num_rows = row;
    return status;
}

struct dirstack
{
    WCHAR **dirs;
    UINT   *len_dirs;
    UINT    num_dirs;
    UINT    num_allocated;
};

static struct dirstack *alloc_dirstack( UINT size )
{
    struct dirstack *dirstack;

    if (!(dirstack = heap_alloc( sizeof(*dirstack) ))) return NULL;
    if (!(dirstack->dirs = heap_alloc( sizeof(WCHAR *) * size )))
    {
        heap_free( dirstack );
        return NULL;
    }
    if (!(dirstack->len_dirs = heap_alloc( sizeof(UINT) * size )))
    {
        heap_free( dirstack->dirs );
        heap_free( dirstack );
        return NULL;
    }
    dirstack->num_dirs = 0;
    dirstack->num_allocated = size;
    return dirstack;
}

static void clear_dirstack( struct dirstack *dirstack )
{
    UINT i;
    for (i = 0; i < dirstack->num_dirs; i++) heap_free( dirstack->dirs[i] );
    dirstack->num_dirs = 0;
}

static void free_dirstack( struct dirstack *dirstack )
{
    clear_dirstack( dirstack );
    heap_free( dirstack->dirs );
    heap_free( dirstack->len_dirs );
    heap_free( dirstack );
}

static BOOL push_dir( struct dirstack *dirstack, WCHAR *dir, UINT len )
{
    UINT size, i = dirstack->num_dirs;

    if (!dir) return FALSE;

    if (i == dirstack->num_allocated)
    {
        WCHAR **tmp;
        UINT *len_tmp;

        size = dirstack->num_allocated * 2;
        if (!(tmp = heap_realloc( dirstack->dirs, size * sizeof(WCHAR *) ))) return FALSE;
        dirstack->dirs = tmp;
        if (!(len_tmp = heap_realloc( dirstack->len_dirs, size * sizeof(UINT) ))) return FALSE;
        dirstack->len_dirs = len_tmp;
        dirstack->num_allocated = size;
    }
    dirstack->dirs[i] = dir;
    dirstack->len_dirs[i] = len;
    dirstack->num_dirs++;
    return TRUE;
}

static WCHAR *pop_dir( struct dirstack *dirstack, UINT *len )
{
    if (!dirstack->num_dirs)
    {
        *len = 0;
        return NULL;
    }
    dirstack->num_dirs--;
    *len = dirstack->len_dirs[dirstack->num_dirs];
    return dirstack->dirs[dirstack->num_dirs];
}

static const WCHAR *peek_dir( struct dirstack *dirstack )
{
    if (!dirstack->num_dirs) return NULL;
    return dirstack->dirs[dirstack->num_dirs - 1];
}

static WCHAR *build_glob( WCHAR drive, const WCHAR *path, UINT len )
{
    UINT i = 0;
    WCHAR *ret;

    if (!(ret = heap_alloc( (len + 6) * sizeof(WCHAR) ))) return NULL;
    ret[i++] = drive;
    ret[i++] = ':';
    ret[i++] = '\\';
    if (path && len)
    {
        memcpy( ret + i, path, len * sizeof(WCHAR) );
        i += len;
        ret[i++] = '\\';
    }
    ret[i++] = '*';
    ret[i] = 0;
    return ret;
}

static WCHAR *build_name( WCHAR drive, const WCHAR *path )
{
    UINT i = 0, len = 0;
    const WCHAR *p;
    WCHAR *ret;

    for (p = path; *p; p++)
    {
        if (*p == '\\') len += 2;
        else len++;
    };
    if (!(ret = heap_alloc( (len + 5) * sizeof(WCHAR) ))) return NULL;
    ret[i++] = drive;
    ret[i++] = ':';
    ret[i++] = '\\';
    ret[i++] = '\\';
    for (p = path; *p; p++)
    {
        if (*p != '\\') ret[i++] = *p;
        else
        {
            ret[i++] = '\\';
            ret[i++] = '\\';
        }
    }
    ret[i] = 0;
    return ret;
}

static WCHAR *build_dirname( const WCHAR *path, UINT *ret_len )
{
    const WCHAR *p = path, *start;
    UINT len, i;
    WCHAR *ret;

    if (!isalphaW( p[0] ) || p[1] != ':' || p[2] != '\\' || p[3] != '\\' || !p[4]) return NULL;
    start = path + 4;
    len = strlenW( start );
    p = start + len - 1;
    if (*p == '\\') return NULL;

    while (p >= start && *p != '\\') { len--; p--; };
    while (p >= start && *p == '\\') { len--; p--; };

    if (!(ret = heap_alloc( (len + 1) * sizeof(WCHAR) ))) return NULL;
    for (i = 0, p = start; p < start + len; p++)
    {
        if (p[0] == '\\' && p[1] == '\\')
        {
            ret[i++] = '\\';
            p++;
        }
        else ret[i++] = *p;
    }
    ret[i] = 0;
    *ret_len = i;
    return ret;
}

static BOOL seen_dir( struct dirstack *dirstack, const WCHAR *path )
{
    UINT i;
    for (i = 0; i < dirstack->num_dirs; i++) if (!strcmpW( dirstack->dirs[i], path )) return TRUE;
    return FALSE;
}

/* optimize queries of the form WHERE Name='...' [OR Name='...']* */
static UINT seed_dirs( struct dirstack *dirstack, const struct expr *cond, WCHAR root, UINT *count )
{
    const struct expr *left, *right;

    if (!cond || cond->type != EXPR_COMPLEX) return *count = 0;

    left = cond->u.expr.left;
    right = cond->u.expr.right;
    if (cond->u.expr.op == OP_EQ)
    {
        UINT len;
        WCHAR *path;
        const WCHAR *str = NULL;

        if (left->type == EXPR_PROPVAL && right->type == EXPR_SVAL &&
            !strcmpW( left->u.propval->name, prop_nameW ) &&
            toupperW( right->u.sval[0] ) == toupperW( root ))
        {
            str = right->u.sval;
        }
        else if (left->type == EXPR_SVAL && right->type == EXPR_PROPVAL &&
                 !strcmpW( right->u.propval->name, prop_nameW ) &&
                 toupperW( left->u.sval[0] ) == toupperW( root ))
        {
            str = left->u.sval;
        }
        if (str && (path = build_dirname( str, &len )))
        {
            if (seen_dir( dirstack, path ))
            {
                heap_free( path );
                return ++*count;
            }
            else if (push_dir( dirstack, path, len )) return ++*count;
            heap_free( path );
            return *count = 0;
        }
    }
    else if (cond->u.expr.op == OP_OR)
    {
        UINT left_count = 0, right_count = 0;

        if (!(seed_dirs( dirstack, left, root, &left_count ))) return *count = 0;
        if (!(seed_dirs( dirstack, right, root, &right_count ))) return *count = 0;
        return *count += left_count + right_count;
    }
    return *count = 0;
}

static WCHAR *append_path( const WCHAR *path, const WCHAR *segment, UINT *len )
{
    UINT len_path = 0, len_segment = strlenW( segment );
    WCHAR *ret;

    *len = 0;
    if (path) len_path = strlenW( path );
    if (!(ret = heap_alloc( (len_path + len_segment + 2) * sizeof(WCHAR) ))) return NULL;
    if (path && len_path)
    {
        memcpy( ret, path, len_path * sizeof(WCHAR) );
        ret[len_path] = '\\';
        *len += len_path + 1;
    }
    memcpy( ret + *len, segment, len_segment * sizeof(WCHAR) );
    *len += len_segment;
    ret[*len] = 0;
    return ret;
}

static WCHAR *get_file_version( const WCHAR *filename )
{
    static const WCHAR slashW[] = {'\\',0}, fmtW[] = {'%','u','.','%','u','.','%','u','.','%','u',0};
    VS_FIXEDFILEINFO *info;
    DWORD size;
    void *block;
    WCHAR *ret;

    if (!(ret = heap_alloc( (4 * 5 + sizeof(fmtW) / sizeof(fmtW[0])) * sizeof(WCHAR) ))) return NULL;
    if (!(size = GetFileVersionInfoSizeW( filename, NULL )) || !(block = heap_alloc( size )))
    {
        heap_free( ret );
        return NULL;
    }
    if (!GetFileVersionInfoW( filename, 0, size, block ) ||
        !VerQueryValueW( block, slashW, (void **)&info, &size ))
    {
        heap_free( block );
        heap_free( ret );
        return NULL;
    }
    sprintfW( ret, fmtW, info->dwFileVersionMS >> 16, info->dwFileVersionMS & 0xffff,
                         info->dwFileVersionLS >> 16, info->dwFileVersionLS & 0xffff );
    heap_free( block );
    return ret;
}

static enum fill_status fill_datafile( struct table *table, const struct expr *cond )
{
    static const WCHAR dotW[] = {'.',0}, dotdotW[] = {'.','.',0};
    struct record_datafile *rec;
    UINT i, len, row = 0, offset = 0, num_expected_rows;
    WCHAR *glob = NULL, *path = NULL, *new_path, root[] = {'A',':','\\',0};
    DWORD drives = GetLogicalDrives();
    WIN32_FIND_DATAW data;
    HANDLE handle;
    struct dirstack *dirstack;
    enum fill_status status = FILL_STATUS_UNFILTERED;

    if (!resize_table( table, 8, sizeof(*rec) )) return FILL_STATUS_FAILED;

    dirstack = alloc_dirstack(2);

    for (i = 0; i < sizeof(drives); i++)
    {
        if (!(drives & (1 << i))) continue;

        root[0] = 'A' + i;
        if (GetDriveTypeW( root ) != DRIVE_FIXED) continue;

        num_expected_rows = 0;
        if (!seed_dirs( dirstack, cond, root[0], &num_expected_rows )) clear_dirstack( dirstack );

        for (;;)
        {
            path = pop_dir( dirstack, &len );
            if (!(glob = build_glob( root[0], path, len )))
            {
                status = FILL_STATUS_FAILED;
                goto done;
            }
            if ((handle = FindFirstFileW( glob, &data )) != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if (!resize_table( table, row + 1, sizeof(*rec) ))
                    {
                        status = FILL_STATUS_FAILED;
                        FindClose( handle );
                        goto done;
                    }
                    if (!strcmpW( data.cFileName, dotW ) || !strcmpW( data.cFileName, dotdotW )) continue;
                    new_path = append_path( path, data.cFileName, &len );

                    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        if (push_dir( dirstack, new_path, len )) continue;
                        heap_free( new_path );
                        FindClose( handle );
                        status = FILL_STATUS_FAILED;
                        goto done;
                    }
                    rec = (struct record_datafile *)(table->data + offset);
                    rec->name    = build_name( root[0], new_path );
                    rec->version = get_file_version( rec->name );
                    if (!match_row( table, row, cond, &status ))
                    {
                        free_row_values( table, row );
                        continue;
                    }
                    else if (num_expected_rows && row == num_expected_rows - 1)
                    {
                        row++;
                        FindClose( handle );
                        status = FILL_STATUS_FILTERED;
                        goto done;
                    }
                    offset += sizeof(*rec);
                    row++;
                }
                while (FindNextFileW( handle, &data ));
                FindClose( handle );
            }
            if (!peek_dir( dirstack )) break;
            heap_free( glob );
            heap_free( path );
        }
    }

done:
    free_dirstack( dirstack );
    heap_free( glob );
    heap_free( path );

    TRACE("created %u rows\n", row);
    table->num_rows = row;
    return status;
}

static enum fill_status fill_directory( struct table *table, const struct expr *cond )
{
    static const WCHAR dotW[] = {'.',0}, dotdotW[] = {'.','.',0};
    struct record_directory *rec;
    UINT i, len, row = 0, offset = 0, num_expected_rows;
    WCHAR *glob = NULL, *path = NULL, *new_path, root[] = {'A',':','\\',0};
    DWORD drives = GetLogicalDrives();
    WIN32_FIND_DATAW data;
    HANDLE handle;
    struct dirstack *dirstack;
    enum fill_status status = FILL_STATUS_UNFILTERED;

    if (!resize_table( table, 4, sizeof(*rec) )) return FILL_STATUS_FAILED;

    dirstack = alloc_dirstack(2);

    for (i = 0; i < sizeof(drives); i++)
    {
        if (!(drives & (1 << i))) continue;

        root[0] = 'A' + i;
        if (GetDriveTypeW( root ) != DRIVE_FIXED) continue;

        num_expected_rows = 0;
        if (!seed_dirs( dirstack, cond, root[0], &num_expected_rows )) clear_dirstack( dirstack );

        for (;;)
        {
            path = pop_dir( dirstack, &len );
            if (!(glob = build_glob( root[0], path, len )))
            {
                status = FILL_STATUS_FAILED;
                goto done;
            }
            if ((handle = FindFirstFileW( glob, &data )) != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if (!resize_table( table, row + 1, sizeof(*rec) ))
                    {
                        FindClose( handle );
                        status = FILL_STATUS_FAILED;
                        goto done;
                    }
                    if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                        !strcmpW( data.cFileName, dotW ) || !strcmpW( data.cFileName, dotdotW ))
                        continue;

                    new_path = append_path( path, data.cFileName, &len );
                    if (!(push_dir( dirstack, new_path, len )))
                    {
                        heap_free( new_path );
                        FindClose( handle );
                        status = FILL_STATUS_FAILED;
                        goto done;
                    }
                    rec = (struct record_directory *)(table->data + offset);
                    rec->accessmask = FILE_ALL_ACCESS;
                    rec->name       = build_name( root[0], new_path );
                    if (!match_row( table, row, cond, &status ))
                    {
                        free_row_values( table, row );
                        continue;
                    }
                    else if (num_expected_rows && row == num_expected_rows - 1)
                    {
                        row++;
                        FindClose( handle );
                        status = FILL_STATUS_FILTERED;
                        goto done;
                    }
                    offset += sizeof(*rec);
                    row++;
                }
                while (FindNextFileW( handle, &data ));
                FindClose( handle );
            }
            if (!peek_dir( dirstack )) break;
            heap_free( glob );
            heap_free( path );
        }
    }

done:
    free_dirstack( dirstack );
    heap_free( glob );
    heap_free( path );

    TRACE("created %u rows\n", row);
    table->num_rows = row;
    return status;
}

static UINT64 get_freespace( const WCHAR *dir, UINT64 *disksize )
{
    WCHAR root[] = {'\\','\\','.','\\','A',':',0};
    ULARGE_INTEGER free;
    DISK_GEOMETRY_EX info;
    HANDLE handle;
    DWORD bytes_returned;

    free.QuadPart = 512 * 1024 * 1024;
    GetDiskFreeSpaceExW( dir, NULL, NULL, &free );

    root[4] = dir[0];
    handle = CreateFileW( root, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0 );
    if (handle != INVALID_HANDLE_VALUE)
    {
        if (DeviceIoControl( handle, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, &info, sizeof(info), &bytes_returned, NULL ))
            *disksize = info.DiskSize.QuadPart;
        CloseHandle( handle );
    }
    return free.QuadPart;
}

static enum fill_status fill_diskdrive( struct table *table, const struct expr *cond )
{
    static const WCHAR fmtW[] =
        {'\\','\\','\\','\\','.','\\','\\','P','H','Y','S','I','C','A','L','D','R','I','V','E','%','u',0};
    WCHAR device_id[sizeof(fmtW)/sizeof(fmtW[0]) + 10], root[] = {'A',':','\\',0};
    struct record_diskdrive *rec;
    UINT i, row = 0, offset = 0, index = 0, type;
    UINT64 size = 1024 * 1024 * 1024;
    DWORD drives = GetLogicalDrives();
    enum fill_status status = FILL_STATUS_UNFILTERED;

    if (!resize_table( table, 2, sizeof(*rec) )) return FILL_STATUS_FAILED;

    for (i = 0; i < sizeof(drives); i++)
    {
        if (drives & (1 << i))
        {
            root[0] = 'A' + i;
            type = GetDriveTypeW( root );
            if (type != DRIVE_FIXED && type != DRIVE_REMOVABLE)
                continue;

            if (!resize_table( table, row + 1, sizeof(*rec) )) return FILL_STATUS_FAILED;

            rec = (struct record_diskdrive *)(table->data + offset);
            sprintfW( device_id, fmtW, index );
            rec->device_id     = heap_strdupW( device_id );
            rec->index         = index;
            rec->interfacetype = diskdrive_interfacetypeW;
            rec->manufacturer  = diskdrive_manufacturerW;
            if (type == DRIVE_FIXED)
                rec->mediatype = diskdrive_mediatype_fixedW;
            else
                rec->mediatype = diskdrive_mediatype_removableW;
            rec->model         = diskdrive_modelW;
            rec->serialnumber  = diskdrive_serialW;
            get_freespace( root, &size );
            rec->size          = size;
            if (!match_row( table, row, cond, &status ))
            {
                free_row_values( table, row );
                continue;
            }
            offset += sizeof(*rec);
            index++;
            row++;
        }
    }
    TRACE("created %u rows\n", row);
    table->num_rows = row;
    return status;
}

static WCHAR *get_filesystem( const WCHAR *root )
{
    static const WCHAR ntfsW[] = {'N','T','F','S',0};
    WCHAR buffer[MAX_PATH + 1];

    if (GetVolumeInformationW( root, NULL, 0, NULL, NULL, NULL, buffer, MAX_PATH + 1 ))
        return heap_strdupW( buffer );
    return heap_strdupW( ntfsW );
}

static enum fill_status fill_diskpartition( struct table *table, const struct expr *cond )
{
    static const WCHAR fmtW[] =
        {'D','i','s','k',' ','#','%','u',',',' ','P','a','r','t','i','t','i','o','n',' ','#','0',0};
    WCHAR device_id[32], root[] = {'A',':','\\',0};
    struct record_diskpartition *rec;
    UINT i, row = 0, offset = 0, type, index = 0;
    UINT64 size = 1024 * 1024 * 1024;
    DWORD drives = GetLogicalDrives();
    enum fill_status status = FILL_STATUS_UNFILTERED;

    if (!resize_table( table, 4, sizeof(*rec) )) return FILL_STATUS_FAILED;

    for (i = 0; i < sizeof(drives); i++)
    {
        if (drives & (1 << i))
        {
            root[0] = 'A' + i;
            type = GetDriveTypeW( root );
            if (type != DRIVE_FIXED && type != DRIVE_REMOVABLE)
                continue;

            if (!resize_table( table, row + 1, sizeof(*rec) )) return FILL_STATUS_FAILED;

            rec = (struct record_diskpartition *)(table->data + offset);
            rec->bootable       = (i == 2) ? -1 : 0;
            rec->bootpartition  = (i == 2) ? -1 : 0;
            sprintfW( device_id, fmtW, index );
            rec->device_id      = heap_strdupW( device_id );
            rec->diskindex      = index;
            rec->index          = 0;
            rec->pnpdevice_id   = heap_strdupW( device_id );
            get_freespace( root, &size );
            rec->size           = size;
            rec->startingoffset = 0;
            rec->type           = get_filesystem( root );
            if (!match_row( table, row, cond, &status ))
            {
                free_row_values( table, row );
                continue;
            }
            offset += sizeof(*rec);
            row++;
            index++;
        }
    }
    TRACE("created %u rows\n", row);
    table->num_rows = row;
    return status;
}

static WCHAR *get_volumeserialnumber( const WCHAR *root )
{
    static const WCHAR fmtW[] = {'%','0','8','X',0};
    DWORD serial = 0;
    WCHAR buffer[9];

    GetVolumeInformationW( root, NULL, 0, &serial, NULL, NULL, NULL, 0 );
    sprintfW( buffer, fmtW, serial );
    return heap_strdupW( buffer );
}

static enum fill_status fill_logicaldisk( struct table *table, const struct expr *cond )
{
    static const WCHAR fmtW[] = {'%','c',':',0};
    WCHAR device_id[3], root[] = {'A',':','\\',0};
    struct record_logicaldisk *rec;
    UINT i, row = 0, offset = 0, type;
    UINT64 size = 1024 * 1024 * 1024;
    DWORD drives = GetLogicalDrives();
    enum fill_status status = FILL_STATUS_UNFILTERED;

    if (!resize_table( table, 4, sizeof(*rec) )) return FILL_STATUS_FAILED;

    for (i = 0; i < sizeof(drives); i++)
    {
        if (drives & (1 << i))
        {
            root[0] = 'A' + i;
            type = GetDriveTypeW( root );
            if (type != DRIVE_FIXED && type != DRIVE_CDROM && type != DRIVE_REMOVABLE)
                continue;

            if (!resize_table( table, row + 1, sizeof(*rec) )) return FILL_STATUS_FAILED;

            rec = (struct record_logicaldisk *)(table->data + offset);
            sprintfW( device_id, fmtW, 'A' + i );
            rec->device_id          = heap_strdupW( device_id );
            rec->drivetype          = type;
            rec->filesystem         = get_filesystem( root );
            rec->freespace          = get_freespace( root, &size );
            rec->name               = heap_strdupW( device_id );
            rec->size               = size;
            rec->volumeserialnumber = get_volumeserialnumber( root );
            if (!match_row( table, row, cond, &status ))
            {
                free_row_values( table, row );
                continue;
            }
            offset += sizeof(*rec);
            row++;
        }
    }
    TRACE("created %u rows\n", row);
    table->num_rows = row;
    return status;
}

static UINT16 get_connection_status( IF_OPER_STATUS status )
{
    switch (status)
    {
    case IfOperStatusDown:
        return 0; /* Disconnected */
    case IfOperStatusUp:
        return 2; /* Connected */
    default:
        ERR("unhandled status %u\n", status);
        break;
    }
    return 0;
}
static WCHAR *get_mac_address( const BYTE *addr, DWORD len )
{
    static const WCHAR fmtW[] =
        {'%','0','2','x',':','%','0','2','x',':','%','0','2','x',':',
         '%','0','2','x',':','%','0','2','x',':','%','0','2','x',0};
    WCHAR *ret;

    if (len != 6 || !(ret = heap_alloc( 18 * sizeof(WCHAR) ))) return NULL;
    sprintfW( ret, fmtW, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5] );
    return ret;
}
static const WCHAR *get_adaptertype( DWORD type, int *physical )
{
    static const WCHAR ethernetW[] = {'E','t','h','e','r','n','e','t',' ','8','0','2','.','3',0};
    static const WCHAR wirelessW[] = {'W','i','r','e','l','e','s','s',0};
    static const WCHAR firewireW[] = {'1','3','9','4',0};
    static const WCHAR tunnelW[]   = {'T','u','n','n','e','l',0};

    switch (type)
    {
    case IF_TYPE_ETHERNET_CSMACD: *physical = -1; return ethernetW;
    case IF_TYPE_IEEE80211:       *physical = -1; return wirelessW;
    case IF_TYPE_IEEE1394:        *physical = -1; return firewireW;
    case IF_TYPE_TUNNEL:          *physical = 0; return tunnelW;
    default:                      *physical = 0; return NULL;
    }
}

static enum fill_status fill_networkadapter( struct table *table, const struct expr *cond )
{
    static const WCHAR fmtW[] = {'%','u',0};
    WCHAR device_id[11];
    struct record_networkadapter *rec;
    IP_ADAPTER_ADDRESSES *aa, *buffer;
    UINT row = 0, offset = 0, count = 0;
    DWORD size = 0, ret;
    int physical;
    enum fill_status status = FILL_STATUS_UNFILTERED;

    ret = GetAdaptersAddresses( AF_UNSPEC, 0, NULL, NULL, &size );
    if (ret != ERROR_BUFFER_OVERFLOW) return FILL_STATUS_FAILED;

    if (!(buffer = heap_alloc( size ))) return FILL_STATUS_FAILED;
    if (GetAdaptersAddresses( AF_UNSPEC, 0, NULL, buffer, &size ))
    {
        heap_free( buffer );
        return FILL_STATUS_FAILED;
    }
    for (aa = buffer; aa; aa = aa->Next)
    {
        if (aa->IfType != IF_TYPE_SOFTWARE_LOOPBACK) count++;
    }
    if (!resize_table( table, count, sizeof(*rec) ))
    {
        heap_free( buffer );
        return FILL_STATUS_FAILED;
    }
    for (aa = buffer; aa; aa = aa->Next)
    {
        if (aa->IfType == IF_TYPE_SOFTWARE_LOOPBACK) continue;

        rec = (struct record_networkadapter *)(table->data + offset);
        sprintfW( device_id, fmtW, aa->u.s.IfIndex );
        rec->adaptertype          = get_adaptertype( aa->IfType, &physical );
        rec->device_id            = heap_strdupW( device_id );
        rec->index                = aa->u.s.IfIndex;
        rec->interface_index      = aa->u.s.IfIndex;
        rec->mac_address          = get_mac_address( aa->PhysicalAddress, aa->PhysicalAddressLength );
        rec->manufacturer         = compsys_manufacturerW;
        rec->name                 = heap_strdupW( aa->FriendlyName );
        rec->netconnection_status = get_connection_status( aa->OperStatus );
        rec->physicaladapter      = physical;
        rec->pnpdevice_id         = networkadapter_pnpdeviceidW;
        rec->speed                = 1000000;
        if (!match_row( table, row, cond, &status ))
        {
            free_row_values( table, row );
            continue;
        }
        offset += sizeof(*rec);
        row++;
    }
    TRACE("created %u rows\n", row);
    table->num_rows = row;

    heap_free( buffer );
    return status;
}

static enum fill_status fill_networkadapterconfig( struct table *table, const struct expr *cond )
{
    struct record_networkadapterconfig *rec;
    IP_ADAPTER_ADDRESSES *aa, *buffer;
    UINT row = 0, offset = 0, count = 0;
    DWORD size = 0, ret;
    enum fill_status status = FILL_STATUS_UNFILTERED;

    ret = GetAdaptersAddresses( AF_UNSPEC, 0, NULL, NULL, &size );
    if (ret != ERROR_BUFFER_OVERFLOW) return FILL_STATUS_FAILED;

    if (!(buffer = heap_alloc( size ))) return FILL_STATUS_FAILED;
    if (GetAdaptersAddresses( AF_UNSPEC, 0, NULL, buffer, &size ))
    {
        heap_free( buffer );
        return FILL_STATUS_FAILED;
    }
    for (aa = buffer; aa; aa = aa->Next)
    {
        if (aa->IfType != IF_TYPE_SOFTWARE_LOOPBACK) count++;
    }
    if (!resize_table( table, count, sizeof(*rec) ))
    {
        heap_free( buffer );
        return FILL_STATUS_FAILED;
    }
    for (aa = buffer; aa; aa = aa->Next)
    {
        if (aa->IfType == IF_TYPE_SOFTWARE_LOOPBACK) continue;

        rec = (struct record_networkadapterconfig *)(table->data + offset);
        rec->index              = aa->u.s.IfIndex;
        rec->ipconnectionmetric = 20;
        rec->ipenabled          = -1;
        rec->mac_address        = get_mac_address( aa->PhysicalAddress, aa->PhysicalAddressLength );
        if (!match_row( table, row, cond, &status ))
        {
            free_row_values( table, row );
            continue;
        }
        offset += sizeof(*rec);
        row++;
    }
    TRACE("created %u rows\n", row);
    table->num_rows = row;

    heap_free( buffer );
    return status;
}

static enum fill_status fill_physicalmemory( struct table *table, const struct expr *cond )
{
    struct record_physicalmemory *rec;
    enum fill_status status = FILL_STATUS_UNFILTERED;
    UINT row = 0;

    if (!resize_table( table, 1, sizeof(*rec) )) return FILL_STATUS_FAILED;

    rec = (struct record_physicalmemory *)table->data;
    rec->capacity = get_total_physical_memory();
    if (!match_row( table, row, cond, &status )) free_row_values( table, row );
    else row++;

    TRACE("created %u rows\n", row);
    table->num_rows = row;
    return status;
}

static WCHAR *get_cmdline( DWORD process_id )
{
    if (process_id == GetCurrentProcessId()) return heap_strdupW( GetCommandLineW() );
    return NULL; /* FIXME handle different process case */
}

static enum fill_status fill_process( struct table *table, const struct expr *cond )
{
    static const WCHAR fmtW[] = {'%','u',0};
    WCHAR handle[11];
    struct record_process *rec;
    PROCESSENTRY32W entry;
    HANDLE snap;
    enum fill_status status = FILL_STATUS_FAILED;
    UINT row = 0, offset = 0;

    snap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if (snap == INVALID_HANDLE_VALUE) return FILL_STATUS_FAILED;

    entry.dwSize = sizeof(entry);
    if (!Process32FirstW( snap, &entry )) goto done;
    if (!resize_table( table, 8, sizeof(*rec) )) goto done;

    do
    {
        if (!resize_table( table, row + 1, sizeof(*rec) )) goto done;

        rec = (struct record_process *)(table->data + offset);
        rec->caption      = heap_strdupW( entry.szExeFile );
        rec->commandline  = get_cmdline( entry.th32ProcessID );
        rec->description  = heap_strdupW( entry.szExeFile );
        sprintfW( handle, fmtW, entry.th32ProcessID );
        rec->handle       = heap_strdupW( handle );
        rec->name         = heap_strdupW( entry.szExeFile );
        rec->process_id   = entry.th32ProcessID;
        rec->pprocess_id  = entry.th32ParentProcessID;
        rec->thread_count = entry.cntThreads;
        rec->get_owner    = process_get_owner;
        if (!match_row( table, row, cond, &status ))
        {
            free_row_values( table, row );
            continue;
        }
        offset += sizeof(*rec);
        row++;
    } while (Process32NextW( snap, &entry ));

    TRACE("created %u rows\n", row);
    table->num_rows = row;
    status = FILL_STATUS_UNFILTERED;

done:
    CloseHandle( snap );
    return status;
}

static inline void do_cpuid( unsigned int ax, unsigned int *p )
{
#ifdef __i386__
#ifdef _MSC_VER
    __cpuid(p, ax);
#else
    __asm__("pushl %%ebx\n\t"
                "cpuid\n\t"
                "movl %%ebx, %%esi\n\t"
                "popl %%ebx"
                : "=a" (p[0]), "=S" (p[1]), "=c" (p[2]), "=d" (p[3])
                :  "0" (ax));
#endif
#endif
}

static void get_processor_id( WCHAR *processor_id )
{
    static const WCHAR fmtW[] = {'%','0','8','X','%','0','8','X',0};
    unsigned int regs[4] = {0, 0, 0, 0};

    do_cpuid( 1, regs );
    sprintfW( processor_id, fmtW, regs[3], regs[0] );
}
static void regs_to_str( unsigned int *regs, unsigned int len, WCHAR *buffer )
{
    unsigned int i;
    unsigned char *p = (unsigned char *)regs;

    for (i = 0; i < len; i++) { buffer[i] = *p++; }
    buffer[i] = 0;
}
static void get_processor_manufacturer( WCHAR *manufacturer )
{
    unsigned int tmp, regs[4] = {0, 0, 0, 0};

    do_cpuid( 0, regs );
    tmp = regs[2];      /* swap edx and ecx */
    regs[2] = regs[3];
    regs[3] = tmp;

    regs_to_str( regs + 1, 12, manufacturer );
}
static void get_processor_name( WCHAR *name )
{
    unsigned int regs[4] = {0, 0, 0, 0};

    do_cpuid( 0x80000000, regs );
    if (regs[0] >= 0x80000004)
    {
        do_cpuid( 0x80000002, regs );
        regs_to_str( regs, 16, name );
        do_cpuid( 0x80000003, regs );
        regs_to_str( regs, 16, name + 16 );
        do_cpuid( 0x80000004, regs );
        regs_to_str( regs, 16, name + 32 );
    }
}
static UINT get_processor_currentclockspeed( UINT index )
{
    PROCESSOR_POWER_INFORMATION *info;
    UINT ret = 1000, size = get_processor_count() * sizeof(PROCESSOR_POWER_INFORMATION);
    NTSTATUS status;

    if ((info = heap_alloc( size )))
    {
        status = NtPowerInformation( ProcessorInformation, NULL, 0, info, size );
        if (!status) ret = info[index].CurrentMhz;
        heap_free( info );
    }
    return ret;
}
static UINT get_processor_maxclockspeed( UINT index )
{
    PROCESSOR_POWER_INFORMATION *info;
    UINT ret = 1000, size = get_processor_count() * sizeof(PROCESSOR_POWER_INFORMATION);
    NTSTATUS status;

    if ((info = heap_alloc( size )))
    {
        status = NtPowerInformation( ProcessorInformation, NULL, 0, info, size );
        if (!status) ret = info[index].MaxMhz;
        heap_free( info );
    }
    return ret;
}
static const WCHAR *get_osarchitecture(void)
{
    SYSTEM_INFO info;
    GetNativeSystemInfo( &info );
    if (info.u.s.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) return os_64bitW;
    return os_32bitW;
}

static enum fill_status fill_processor( struct table *table, const struct expr *cond )
{
    static const WCHAR fmtW[] = {'C','P','U','%','u',0};
    WCHAR device_id[14], processor_id[17], manufacturer[13], name[49] = {0};
    struct record_processor *rec;
    UINT i, offset = 0, num_cores, num_logical_processors, count = get_processor_count();
    enum fill_status status = FILL_STATUS_UNFILTERED;

    if (!resize_table( table, count, sizeof(*rec) )) return FILL_STATUS_FAILED;

    get_processor_id( processor_id );
    get_processor_manufacturer( manufacturer );
    get_processor_name( name );

    num_logical_processors = get_logical_processor_count( &num_cores ) / count;
    num_cores /= count;

    for (i = 0; i < count; i++)
    {
        rec = (struct record_processor *)(table->data + offset);
        rec->addresswidth           = get_osarchitecture() == os_32bitW ? 32 : 64;
        rec->cpu_status             = 1; /* CPU Enabled */
        rec->currentclockspeed      = get_processor_currentclockspeed( i );
        sprintfW( device_id, fmtW, i );
        rec->device_id              = heap_strdupW( device_id );
        rec->family                 = 2; /* Unknown */
        rec->manufacturer           = heap_strdupW( manufacturer );
        rec->maxclockspeed          = get_processor_maxclockspeed( i );
        rec->name                   = heap_strdupW( name );
        rec->num_cores              = num_cores;
        rec->num_logical_processors = num_logical_processors;
        rec->processor_id           = heap_strdupW( processor_id );
        rec->unique_id              = NULL;
        if (!match_row( table, i, cond, &status ))
        {
            free_row_values( table, i );
            continue;
        }
        offset += sizeof(*rec);
    }

    TRACE("created %u rows\n", count);
    table->num_rows = count;
    return status;
}

static WCHAR *get_lastbootuptime(void)
{
    static const WCHAR fmtW[] =
        {'%','0','4','u','%','0','2','u','%','0','2','u','%','0','2','u','%','0','2','u','%','0','2','u',
         '.','%','0','6','u','+','0','0','0',0};
    SYSTEM_TIMEOFDAY_INFORMATION ti;
    TIME_FIELDS tf;
    WCHAR *ret;

    if (!(ret = heap_alloc( 26 * sizeof(WCHAR) ))) return NULL;

    NtQuerySystemInformation( SystemTimeOfDayInformation, &ti, sizeof(ti), NULL );
    RtlTimeToTimeFields( &ti.liKeBootTime, &tf );
    sprintfW( ret, fmtW, tf.Year, tf.Month, tf.Day, tf.Hour, tf.Minute, tf.Second, tf.Milliseconds * 1000 );
    return ret;
}
static WCHAR *get_localdatetime(void)
{
    static const WCHAR fmtW[] =
        {'%','0','4','u','%','0','2','u','%','0','2','u','%','0','2','u','%','0','2','u','%','0','2','u',
         '.','%','0','6','u','%','+','0','3','d',0};
    TIME_ZONE_INFORMATION tzi;
    SYSTEMTIME st;
    WCHAR *ret;
    DWORD Status;
    LONG Bias;

    Status = GetTimeZoneInformation(&tzi);

    if(Status == TIME_ZONE_ID_INVALID) return NULL;
    Bias = tzi.Bias;
    if(Status == TIME_ZONE_ID_DAYLIGHT)
        Bias+= tzi.DaylightBias;
    else
        Bias+= tzi.StandardBias;
    if (!(ret = heap_alloc( 26 * sizeof(WCHAR) ))) return NULL;

    GetLocalTime(&st);
    sprintfW( ret, fmtW, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds * 1000, -Bias);
    return ret;
}
static WCHAR *get_systemdirectory(void)
{
    void *redir;
    WCHAR *ret;

    if (!(ret = heap_alloc( MAX_PATH * sizeof(WCHAR) ))) return NULL;
    Wow64DisableWow64FsRedirection( &redir );
    GetSystemDirectoryW( ret, MAX_PATH );
    Wow64RevertWow64FsRedirection( redir );
    return ret;
}
static WCHAR *get_codeset(void)
{
    static const WCHAR fmtW[] = {'%','u',0};
    WCHAR *ret = heap_alloc( 11 * sizeof(WCHAR) );
    if (ret) sprintfW( ret, fmtW, GetACP() );
    return ret;
}
static WCHAR *get_countrycode(void)
{
    WCHAR *ret = heap_alloc( 6 * sizeof(WCHAR) );
    if (ret) GetLocaleInfoW( LOCALE_SYSTEM_DEFAULT, LOCALE_ICOUNTRY, ret, 6 );
    return ret;
}
static WCHAR *get_locale(void)
{
    WCHAR *ret = heap_alloc( 5 * sizeof(WCHAR) );
    if (ret) GetLocaleInfoW( LOCALE_SYSTEM_DEFAULT, LOCALE_ILANGUAGE, ret, 5 );
    return ret;
}

static enum fill_status fill_os( struct table *table, const struct expr *cond )
{
    struct record_operatingsystem *rec;
    enum fill_status status = FILL_STATUS_UNFILTERED;
    UINT row = 0;

    if (!resize_table( table, 1, sizeof(*rec) )) return FILL_STATUS_FAILED;

    rec = (struct record_operatingsystem *)table->data;
    rec->buildnumber      = os_buildnumberW;
    rec->caption          = os_captionW;
    rec->codeset          = get_codeset();
    rec->countrycode      = get_countrycode();
    rec->csdversion       = os_csdversionW;
    rec->installdate      = os_installdateW;
    rec->lastbootuptime   = get_lastbootuptime();
    rec->localdatetime    = get_localdatetime();
    rec->locale           = get_locale();
    rec->name             = os_nameW;
    rec->osarchitecture   = get_osarchitecture();
    rec->oslanguage       = GetSystemDefaultLangID();
    rec->osproductsuite   = 2461140; /* Windows XP Professional  */
    rec->ostype           = 18;      /* WINNT */
    rec->serialnumber     = os_serialnumberW;
    rec->servicepackmajor = 3;
    rec->servicepackminor = 0;
    rec->suitemask        = 272;     /* Single User + Terminal */
    rec->systemdirectory  = get_systemdirectory();
    rec->version          = os_versionW;
    if (!match_row( table, row, cond, &status )) free_row_values( table, row );
    else row++;

    TRACE("created %u rows\n", row);
    table->num_rows = row;
    return status;
}

static const WCHAR *get_service_type( DWORD type )
{
    static const WCHAR filesystem_driverW[] =
        {'F','i','l','e',' ','S','y','s','t','e','m',' ','D','r','i','v','e','r',0};
    static const WCHAR kernel_driverW[] =
        {'K','e','r','n','e','l',' ','D','r','i','v','e','r',0};
    static const WCHAR own_processW[] =
        {'O','w','n',' ','P','r','o','c','e','s','s',0};
    static const WCHAR share_processW[] =
        {'S','h','a','r','e',' ','P','r','o','c','e','s','s',0};

    if (type & SERVICE_KERNEL_DRIVER)            return kernel_driverW;
    else if (type & SERVICE_FILE_SYSTEM_DRIVER)  return filesystem_driverW;
    else if (type & SERVICE_WIN32_OWN_PROCESS)   return own_processW;
    else if (type & SERVICE_WIN32_SHARE_PROCESS) return share_processW;
    else ERR("unhandled type 0x%08x\n", type);
    return NULL;
}
static const WCHAR *get_service_state( DWORD state )
{
    static const WCHAR runningW[] =
        {'R','u','n','n','i','n','g',0};
    static const WCHAR start_pendingW[] =
        {'S','t','a','r','t',' ','P','e','n','d','i','n','g',0};
    static const WCHAR stop_pendingW[] =
        {'S','t','o','p',' ','P','e','n','d','i','n','g',0};
    static const WCHAR stoppedW[] =
        {'S','t','o','p','p','e','d',0};
    static const WCHAR unknownW[] =
        {'U','n','k','n','o','w','n',0};

    switch (state)
    {
    case SERVICE_STOPPED:       return stoppedW;
    case SERVICE_START_PENDING: return start_pendingW;
    case SERVICE_STOP_PENDING:  return stop_pendingW;
    case SERVICE_RUNNING:       return runningW;
    default:
        ERR("unknown state %u\n", state);
        return unknownW;
    }
}
static const WCHAR *get_service_startmode( DWORD mode )
{
    static const WCHAR bootW[] = {'B','o','o','t',0};
    static const WCHAR systemW[] = {'S','y','s','t','e','m',0};
    static const WCHAR autoW[] = {'A','u','t','o',0};
    static const WCHAR manualW[] = {'M','a','n','u','a','l',0};
    static const WCHAR disabledW[] = {'D','i','s','a','b','l','e','d',0};
    static const WCHAR unknownW[] = {'U','n','k','n','o','w','n',0};

    switch (mode)
    {
    case SERVICE_BOOT_START:   return bootW;
    case SERVICE_SYSTEM_START: return systemW;
    case SERVICE_AUTO_START:   return autoW;
    case SERVICE_DEMAND_START: return manualW;
    case SERVICE_DISABLED:     return disabledW;
    default:
        ERR("unknown mode 0x%x\n", mode);
        return unknownW;
    }
}
static QUERY_SERVICE_CONFIGW *query_service_config( SC_HANDLE manager, const WCHAR *name )
{
    QUERY_SERVICE_CONFIGW *config = NULL;
    SC_HANDLE service;
    DWORD size;

    if (!(service = OpenServiceW( manager, name, SERVICE_QUERY_CONFIG ))) return NULL;
    QueryServiceConfigW( service, NULL, 0, &size );
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) goto done;
    if (!(config = heap_alloc( size ))) goto done;
    if (QueryServiceConfigW( service, config, size, &size )) goto done;
    heap_free( config );
    config = NULL;

done:
    CloseServiceHandle( service );
    return config;
}

static enum fill_status fill_service( struct table *table, const struct expr *cond )
{
    struct record_service *rec;
    SC_HANDLE manager;
    ENUM_SERVICE_STATUS_PROCESSW *tmp, *services = NULL;
    SERVICE_STATUS_PROCESS *status;
    WCHAR sysnameW[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD len = sizeof(sysnameW) / sizeof(sysnameW[0]);
    UINT i, row = 0, offset = 0, size = 256, needed, count;
    enum fill_status fill_status = FILL_STATUS_FAILED;
    BOOL ret;

    if (!(manager = OpenSCManagerW( NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE ))) return FILL_STATUS_FAILED;
    if (!(services = heap_alloc( size ))) goto done;

    ret = EnumServicesStatusExW( manager, SC_ENUM_PROCESS_INFO, SERVICE_TYPE_ALL,
                                 SERVICE_STATE_ALL, (BYTE *)services, size, &needed,
                                 &count, NULL, NULL );
    if (!ret)
    {
        if (GetLastError() != ERROR_MORE_DATA) goto done;
        size = needed;
        if (!(tmp = heap_realloc( services, size ))) goto done;
        services = tmp;
        ret = EnumServicesStatusExW( manager, SC_ENUM_PROCESS_INFO, SERVICE_TYPE_ALL,
                                     SERVICE_STATE_ALL, (BYTE *)services, size, &needed,
                                     &count, NULL, NULL );
        if (!ret) goto done;
    }
    if (!resize_table( table, count, sizeof(*rec) )) goto done;

    GetComputerNameW( sysnameW, &len );
    fill_status = FILL_STATUS_UNFILTERED;

    for (i = 0; i < count; i++)
    {
        QUERY_SERVICE_CONFIGW *config;

        if (!(config = query_service_config( manager, services[i].lpServiceName ))) continue;

        status = &services[i].ServiceStatusProcess;
        rec = (struct record_service *)(table->data + offset);
        rec->accept_pause   = (status->dwControlsAccepted & SERVICE_ACCEPT_PAUSE_CONTINUE) ? -1 : 0;
        rec->accept_stop    = (status->dwControlsAccepted & SERVICE_ACCEPT_STOP) ? -1 : 0;
        rec->displayname    = heap_strdupW( services[i].lpDisplayName );
        rec->name           = heap_strdupW( services[i].lpServiceName );
        rec->process_id     = status->dwProcessId;
        rec->servicetype    = get_service_type( status->dwServiceType );
        rec->startmode      = get_service_startmode( config->dwStartType );
        rec->state          = get_service_state( status->dwCurrentState );
        rec->systemname     = heap_strdupW( sysnameW );
        rec->pause_service  = service_pause_service;
        rec->resume_service = service_resume_service;
        rec->start_service  = service_start_service;
        rec->stop_service   = service_stop_service;
        heap_free( config );
        if (!match_row( table, row, cond, &fill_status ))
        {
            free_row_values( table, row );
            continue;
        }
        offset += sizeof(*rec);
        row++;
    }

    TRACE("created %u rows\n", row);
    table->num_rows = row;

done:
    CloseServiceHandle( manager );
    heap_free( services );
    return fill_status;
}

static UINT32 get_bits_per_pixel( UINT *hres, UINT *vres )
{
    HDC hdc = GetDC( NULL );
    UINT32 ret;

    if (!hdc) return 32;
    ret = GetDeviceCaps( hdc, BITSPIXEL );
    *hres = GetDeviceCaps( hdc, HORZRES );
    *vres = GetDeviceCaps( hdc, VERTRES );
    ReleaseDC( NULL, hdc );
    return ret;
}
static WCHAR *get_pnpdeviceid( DXGI_ADAPTER_DESC *desc )
{
    static const WCHAR fmtW[] =
        {'P','C','I','\\','V','E','N','_','%','0','4','X','&','D','E','V','_','%','0','4','X',
         '&','S','U','B','S','Y','S','_','%','0','8','X','&','R','E','V','_','%','0','2','X','\\',
         '0','&','D','E','A','D','B','E','E','F','&','0','&','D','E','A','D',0};
    WCHAR *ret;

    if (!(ret = heap_alloc( sizeof(fmtW) + 2 * sizeof(WCHAR) ))) return NULL;
    sprintfW( ret, fmtW, desc->VendorId, desc->DeviceId, desc->SubSysId, desc->Revision );
    return ret;
}

static enum fill_status fill_videocontroller( struct table *table, const struct expr *cond )
{

    struct record_videocontroller *rec;
    HRESULT hr;
    IDXGIFactory *factory = NULL;
    IDXGIAdapter *adapter = NULL;
    DXGI_ADAPTER_DESC desc;
    UINT hres = 1024, vres = 768, vidmem = 512 * 1024 * 1024;
    const WCHAR *name = videocontroller_deviceidW;
    enum fill_status status = FILL_STATUS_UNFILTERED;
    UINT row = 0;

    if (!resize_table( table, 1, sizeof(*rec) )) return FILL_STATUS_FAILED;

    memset (&desc, 0, sizeof(desc));
    hr = CreateDXGIFactory( &IID_IDXGIFactory, (void **)&factory );
    if (FAILED(hr)) goto done;

    hr = IDXGIFactory_EnumAdapters( factory, 0, &adapter );
    if (FAILED(hr)) goto done;

    hr = IDXGIAdapter_GetDesc( adapter, &desc );
    if (SUCCEEDED(hr))
    {
        vidmem = desc.DedicatedVideoMemory;
        name   = desc.Description;
    }

done:
    rec = (struct record_videocontroller *)table->data;
    rec->adapter_dactype       = videocontroller_dactypeW;
    rec->adapter_ram           = vidmem;
    rec->availability          = 3; /* Running or Full Power */
    rec->current_bitsperpixel  = get_bits_per_pixel( &hres, &vres );
    rec->current_horizontalres = hres;
    rec->current_verticalres   = vres;
    rec->description           = heap_strdupW( name );
    rec->device_id             = videocontroller_deviceidW;
    rec->name                  = heap_strdupW( name );
    rec->pnpdevice_id          = get_pnpdeviceid( &desc );
    if (!match_row( table, row, cond, &status )) free_row_values( table, row );
    else row++;

    TRACE("created %u rows\n", row);
    table->num_rows = row;

    if (adapter) IDXGIAdapter_Release( adapter );
    if (factory) IDXGIFactory_Release( factory );
    return status;
}

static struct table builtin_classes[] =
{
    { class_baseboardW, SIZEOF(col_baseboard), col_baseboard, SIZEOF(data_baseboard), 0, (BYTE *)data_baseboard },
    { class_biosW, SIZEOF(col_bios), col_bios, SIZEOF(data_bios), 0, (BYTE *)data_bios },
    { class_cdromdriveW, SIZEOF(col_cdromdrive), col_cdromdrive, 0, 0, NULL, fill_cdromdrive },
    { class_compsysW, SIZEOF(col_compsys), col_compsys, 0, 0, NULL, fill_compsys },
    { class_datafileW, SIZEOF(col_datafile), col_datafile, 0, 0, NULL, fill_datafile },
    { class_directoryW, SIZEOF(col_directory), col_directory, 0, 0, NULL, fill_directory },
    { class_diskdriveW, SIZEOF(col_diskdrive), col_diskdrive, 0, 0, NULL, fill_diskdrive },
    { class_diskpartitionW, SIZEOF(col_diskpartition), col_diskpartition, 0, 0, NULL, fill_diskpartition },
    { class_logicaldiskW, SIZEOF(col_logicaldisk), col_logicaldisk, 0, 0, NULL, fill_logicaldisk },
    { class_logicaldisk2W, SIZEOF(col_logicaldisk), col_logicaldisk, 0, 0, NULL, fill_logicaldisk },
    { class_networkadapterW, SIZEOF(col_networkadapter), col_networkadapter, 0, 0, NULL, fill_networkadapter },
    { class_networkadapterconfigW, SIZEOF(col_networkadapterconfig), col_networkadapterconfig, 0, 0, NULL,
      fill_networkadapterconfig },
    { class_osW, SIZEOF(col_os), col_os, 0, 0, NULL, fill_os },
    { class_paramsW, SIZEOF(col_param), col_param, SIZEOF(data_param), 0, (BYTE *)data_param },
    { class_physicalmediaW, SIZEOF(col_physicalmedia), col_physicalmedia, SIZEOF(data_physicalmedia), 0, (BYTE *)data_physicalmedia },
    { class_physicalmemoryW, SIZEOF(col_physicalmemory), col_physicalmemory, 0, 0, NULL, fill_physicalmemory },
    { class_processW, SIZEOF(col_process), col_process, 0, 0, NULL, fill_process },
    { class_processorW, SIZEOF(col_processor), col_processor, 0, 0, NULL, fill_processor },
    { class_processor2W, SIZEOF(col_processor), col_processor, 0, 0, NULL, fill_processor },
    { class_qualifiersW, SIZEOF(col_qualifier), col_qualifier, SIZEOF(data_qualifier), 0, (BYTE *)data_qualifier },
    { class_serviceW, SIZEOF(col_service), col_service, 0, 0, NULL, fill_service },
    { class_sounddeviceW, SIZEOF(col_sounddevice), col_sounddevice, SIZEOF(data_sounddevice), 0, (BYTE *)data_sounddevice },
    { class_stdregprovW, SIZEOF(col_stdregprov), col_stdregprov, SIZEOF(data_stdregprov), 0, (BYTE *)data_stdregprov },
    { class_systemsecurityW, SIZEOF(col_systemsecurity), col_systemsecurity, SIZEOF(data_systemsecurity), 0, (BYTE *)data_systemsecurity },
    { class_videocontrollerW, SIZEOF(col_videocontroller), col_videocontroller, 0, 0, NULL, fill_videocontroller }
};

void init_table_list( void )
{
    static struct list tables = LIST_INIT( tables );
    UINT i;

    for (i = 0; i < SIZEOF(builtin_classes); i++) list_add_tail( &tables, &builtin_classes[i].entry );
    table_list = &tables;
}