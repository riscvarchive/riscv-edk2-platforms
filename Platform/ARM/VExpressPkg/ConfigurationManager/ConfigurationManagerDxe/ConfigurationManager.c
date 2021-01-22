/** @file
  Configuration Manager Dxe

  Copyright (c) 2017 - 2021, Arm Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
    - Cm or CM   - Configuration Manager
    - Obj or OBJ - Object
**/

#include <IndustryStandard/DebugPort2Table.h>
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>
#include <Library/ArmLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/ConfigurationManagerProtocol.h>

#include "ArmPlatform.h"
#include "ConfigurationManager.h"
#include "Platform.h"

/** The platform configuration repository information.
*/
STATIC
EDKII_PLATFORM_REPOSITORY_INFO VExpressPlatRepositoryInfo = {
  /// Configuration Manager information
  { CONFIGURATION_MANAGER_REVISION, CFG_MGR_OEM_ID },

  // ACPI Table List
  {
    // FADT Table
    {
      EFI_ACPI_6_2_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
      EFI_ACPI_6_2_FIXED_ACPI_DESCRIPTION_TABLE_REVISION,
      CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdFadt),
      NULL
    },
    // GTDT Table
    {
      EFI_ACPI_6_2_GENERIC_TIMER_DESCRIPTION_TABLE_SIGNATURE,
      EFI_ACPI_6_2_GENERIC_TIMER_DESCRIPTION_TABLE_REVISION,
      CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdGtdt),
      NULL
    },
    // MADT Table
    {
      EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE,
      EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION,
      CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdMadt),
      NULL
    },
    // SPCR Table
    {
      EFI_ACPI_6_2_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE,
      EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_REVISION,
      CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdSpcr),
      NULL
    },
    // DSDT Table
    {
      EFI_ACPI_6_2_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
      0, // Unused
      CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdDsdt),
      (EFI_ACPI_DESCRIPTION_HEADER*)dsdt_aml_code
    },
    // DBG2 Table
    {
      EFI_ACPI_6_2_DEBUG_PORT_2_TABLE_SIGNATURE,
      EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT_REVISION,
      CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdDbg2),
      NULL
    }
  },

  // Boot architecture information
  { EFI_ACPI_6_2_ARM_PSCI_COMPLIANT },              // BootArchFlags

#ifdef HEADLESS_PLATFORM
  // Fixed feature flag information
  { EFI_ACPI_6_2_HEADLESS },                        // Fixed feature flags
#endif

  // Power management profile information
  { EFI_ACPI_6_2_PM_PROFILE_ENTERPRISE_SERVER },    // PowerManagement Profile

  /* GIC CPU Interface information
     GIC_ENTRY (CPUInterfaceNumber, Mpidr, PmuIrq, VGicIrq, EnergyEfficiency)
  */
  {
    GICC_ENTRY (0, GET_MPID (0, 0), 92, 25, 0),
    GICC_ENTRY (1, GET_MPID (0, 1), 93, 25, 0),
    GICC_ENTRY (2, GET_MPID (0, 2), 94, 25, 0),
    GICC_ENTRY (3, GET_MPID (0, 3), 95, 25, 0),

    GICC_ENTRY (4, GET_MPID (1, 0), 96, 25, 0),
    GICC_ENTRY (5, GET_MPID (1, 1), 97, 25, 0),
    GICC_ENTRY (6, GET_MPID (1, 2), 98, 25, 0),
    GICC_ENTRY (7, GET_MPID (1, 3), 99, 25, 0)
  },

  // GIC Distributor Info
  {
    FixedPcdGet64 (PcdGicDistributorBase),  // UINT64  PhysicalBaseAddress
    0,                                      // UINT32  SystemVectorBase
    3                                       // UINT8   GicVersion
  },

  /// GIC Re-Distributor Info
  {
    // UINT64  DiscoveryRangeBaseAddress
    FixedPcdGet64 (PcdGicRedistributorsBase),
    // UINT32  DiscoveryRangeLength
    0x00200000
  },

  // Generic Timer Info
  {
    // The physical base address for the counter control frame
    FVP_SYSTEM_TIMER_BASE_ADDRESS,
    // The physical base address for the counter read frame
    FVP_CNT_READ_BASE_ADDRESS,
    // The secure PL1 timer interrupt
    FixedPcdGet32 (PcdArmArchTimerSecIntrNum),
    // The secure PL1 timer flags
    FVP_GTDT_GTIMER_FLAGS,
    // The non-secure PL1 timer interrupt
    FixedPcdGet32 (PcdArmArchTimerIntrNum),
    // The non-secure PL1 timer flags
    FVP_GTDT_GTIMER_FLAGS,
    // The virtual timer interrupt
    FixedPcdGet32 (PcdArmArchTimerVirtIntrNum),
    // The virtual timer flags
    FVP_GTDT_GTIMER_FLAGS,
    // The non-secure PL2 timer interrupt
    FixedPcdGet32 (PcdArmArchTimerHypIntrNum),
    // The non-secure PL2 timer flags
    FVP_GTDT_GTIMER_FLAGS
  },

  // Generic Timer Block Information
  {
    {
      // The physical base address for the GT Block Timer structure
      FVP_GT_BLOCK_CTL_BASE,
      // The number of timer frames implemented in the GT Block
      FVP_TIMER_FRAMES_COUNT,
      // Reference token for the GT Block timer frame list
      (CM_OBJECT_TOKEN)((UINT8*)&VExpressPlatRepositoryInfo +
        OFFSET_OF (EDKII_PLATFORM_REPOSITORY_INFO, GTBlock0TimerInfo))
    }
  },

  // GT Block Timer Frames
  {
    // Frame 0
    {
      0,                                // UINT8   FrameNumber
      FVP_GT_BLOCK_FRAME0_CTL_BASE,     // UINT64  PhysicalAddressCntBase
      FVP_GT_BLOCK_FRAME0_CTL_EL0_BASE, // UINT64  PhysicalAddressCntEL0Base
      FVP_GT_BLOCK_FRAME0_GSIV,         // UINT32  PhysicalTimerGSIV
      FVP_GTX_TIMER_FLAGS,              // UINT32  PhysicalTimerFlags
      0,                                // UINT32  VirtualTimerGSIV
      0,                                // UINT32  VirtualTimerFlags
      FVP_GTX_COMMON_FLAGS_S            // UINT32  CommonFlags
    },
    // Frame 1
    {
      1,                                // UINT8   FrameNumber
      FVP_GT_BLOCK_FRAME1_CTL_BASE,     // UINT64  PhysicalAddressCntBase
      FVP_GT_BLOCK_FRAME1_CTL_EL0_BASE, // UINT64  PhysicalAddressCntEL0Base
      FVP_GT_BLOCK_FRAME1_GSIV,         // UINT32  PhysicalTimerGSIV
      FVP_GTX_TIMER_FLAGS,              // UINT32  PhysicalTimerFlags
      0,                                // UINT32  VirtualTimerGSIV
      0,                                // UINT32  VirtualTimerFlags
      FVP_GTX_COMMON_FLAGS_NS           // UINT32  CommonFlags
    },
  },

  // Watchdog Info
  {
    // The physical base address of the SBSA Watchdog control frame
    FixedPcdGet64 (PcdGenericWatchdogControlBase),
    // The physical base address of the SBSA Watchdog refresh frame
    FixedPcdGet64 (PcdGenericWatchdogRefreshBase),
    // The watchdog interrupt
    FixedPcdGet32 (PcdGenericWatchdogEl2IntrNum),
    // The watchdog flags
    FVP_SBSA_WATCHDOG_FLAGS
  },

  // SPCR Serial Port
  {
    FixedPcdGet64 (PcdSerialRegisterBase),                    // BaseAddress
    FixedPcdGet32 (PL011UartInterrupt),                       // Interrupt
    FixedPcdGet64 (PcdUartDefaultBaudRate),                   // BaudRate
    FixedPcdGet32 (PL011UartClkInHz),                         // Clock
    EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_ARM_SBSA_GENERIC_UART   // Port subtype
  },
  // Debug Serial Port
  {
    FixedPcdGet64 (PcdSerialDbgRegisterBase),                 // BaseAddress
    38,                                                       // Interrupt
    FixedPcdGet64 (PcdSerialDbgUartBaudRate),                 // BaudRate
    FixedPcdGet32 (PcdSerialDbgUartClkInHz),                  // Clock
    EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_ARM_SBSA_GENERIC_UART   // Port subtype
  },

  // GIC ITS
  {
    // The GIC ITS ID.
    0,
    // The physical address for the Interrupt Translation Service
    0x2f020000
  }
};

/** A helper function for returning the Configuration Manager Objects.

  @param [in]       CmObjectId     The Configuration Manager Object ID.
  @param [in]       Object         Pointer to the Object(s).
  @param [in]       ObjectSize     Total size of the Object(s).
  @param [in]       ObjectCount    Number of Objects.
  @param [in, out]  CmObjectDesc   Pointer to the Configuration Manager Object
                                   descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
**/
STATIC
EFI_STATUS
EFIAPI
HandleCmObject (
  IN  CONST CM_OBJECT_ID                CmObjectId,
  IN        VOID                *       Object,
  IN  CONST UINTN                       ObjectSize,
  IN  CONST UINTN                       ObjectCount,
  IN  OUT   CM_OBJ_DESCRIPTOR   * CONST CmObjectDesc
  )
{
  CmObjectDesc->ObjectId = CmObjectId;
  CmObjectDesc->Size = ObjectSize;
  CmObjectDesc->Data = (VOID*)Object;
  CmObjectDesc->Count = ObjectCount;
  DEBUG ((
    DEBUG_INFO,
    "INFO: CmObjectId = %x, Ptr = 0x%p, Size = %d, Count = %d\n",
    CmObjectId,
    CmObjectDesc->Data,
    CmObjectDesc->Size,
    CmObjectDesc->Count
    ));
  return EFI_SUCCESS;
}

/** A helper function for returning the Configuration Manager Objects that
    match the token.

  @param [in]  This               Pointer to the Configuration Manager Protocol.
  @param [in]  CmObjectId         The Configuration Manager Object ID.
  @param [in]  Object             Pointer to the Object(s).
  @param [in]  ObjectSize         Total size of the Object(s).
  @param [in]  ObjectCount        Number of Objects.
  @param [in]  Token              A token identifying the object.
  @param [in]  HandlerProc        A handler function to search the object
                                  referenced by the token.
  @param [in, out]  CmObjectDesc  Pointer to the Configuration Manager Object
                                  descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
STATIC
EFI_STATUS
EFIAPI
HandleCmObjectRefByToken (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN        VOID                                  *       Object,
  IN  CONST UINTN                                         ObjectSize,
  IN  CONST UINTN                                         ObjectCount,
  IN  CONST CM_OBJECT_TOKEN                               Token,
  IN  CONST CM_OBJECT_HANDLER_PROC                        HandlerProc,
  IN  OUT   CM_OBJ_DESCRIPTOR                     * CONST CmObjectDesc
  )
{
  EFI_STATUS  Status;
  CmObjectDesc->ObjectId = CmObjectId;
  if (Token == CM_NULL_TOKEN) {
    CmObjectDesc->Size = ObjectSize;
    CmObjectDesc->Data = (VOID*)Object;
    CmObjectDesc->Count = ObjectCount;
    Status = EFI_SUCCESS;
  } else {
    Status = HandlerProc (This, CmObjectId, Token, CmObjectDesc);
  }

  DEBUG ((
    DEBUG_INFO,
    "INFO: Token = 0x%p, CmObjectId = %x, Ptr = 0x%p, Size = %d, Count = %d\n",
    (VOID*)Token,
    CmObjectId,
    CmObjectDesc->Data,
    CmObjectDesc->Size,
    CmObjectDesc->Count
    ));
  return Status;
}

/** Initialize the platform configuration repository.

  @param [in]  This        Pointer to the Configuration Manager Protocol.

  @retval
    EFI_SUCCESS   Success
**/
STATIC
EFI_STATUS
EFIAPI
InitializePlatformRepository (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST This
  )
{
  return EFI_SUCCESS;
}

/** Return a GT Block timer frame info list.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       A token for identifying the object
  @param [in, out] CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
EFI_STATUS
EFIAPI
GetGTBlockTimerFrameInfo (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token,
  IN  OUT   CM_OBJ_DESCRIPTOR                     * CONST CmObject
  )
{
  EDKII_PLATFORM_REPOSITORY_INFO  * PlatformRepo;

  if ((This == NULL) || (CmObject == NULL)) {
    ASSERT (This != NULL);
    ASSERT (CmObject != NULL);
    return EFI_INVALID_PARAMETER;
  }

  PlatformRepo = This->PlatRepoInfo;

  if (Token != (CM_OBJECT_TOKEN)&PlatformRepo->GTBlock0TimerInfo) {
    return EFI_NOT_FOUND;
  }

  CmObject->ObjectId = CmObjectId;
  CmObject->Size = sizeof (PlatformRepo->GTBlock0TimerInfo);
  CmObject->Data = (VOID*)&PlatformRepo->GTBlock0TimerInfo;
  CmObject->Count = sizeof (PlatformRepo->GTBlock0TimerInfo) /
                      sizeof (PlatformRepo->GTBlock0TimerInfo[0]);
  return EFI_SUCCESS;
}

/** Return a standard namespace object.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in, out] CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
EFI_STATUS
EFIAPI
GetStandardNameSpaceObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN  OUT   CM_OBJ_DESCRIPTOR                     * CONST CmObject
  )
{
  EFI_STATUS                      Status;
  EDKII_PLATFORM_REPOSITORY_INFO  * PlatformRepo;

  if ((This == NULL) || (CmObject == NULL)) {
    ASSERT (This != NULL);
    ASSERT (CmObject != NULL);
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;
  PlatformRepo = This->PlatRepoInfo;

  switch (GET_CM_OBJECT_ID (CmObjectId)) {
    case EStdObjCfgMgrInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->CmInfo,
                 sizeof (PlatformRepo->CmInfo),
                 1,
                 CmObject
                 );
      break;

    case EStdObjAcpiTableList:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->CmAcpiTableList,
                 sizeof (PlatformRepo->CmAcpiTableList),
                 ARRAY_SIZE (PlatformRepo->CmAcpiTableList),
                 CmObject
                 );
      break;

    default: {
      Status = EFI_NOT_FOUND;
      DEBUG ((
        DEBUG_ERROR,
        "ERROR: Object 0x%x. Status = %r\n",
        CmObjectId,
        Status
        ));
      break;
    }
  }

  return Status;
}

/** Return an ARM namespace object.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in, out] CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
EFI_STATUS
EFIAPI
GetArmNameSpaceObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN  OUT   CM_OBJ_DESCRIPTOR                     * CONST CmObject
  )
{
  EFI_STATUS                        Status;
  EDKII_PLATFORM_REPOSITORY_INFO  * PlatformRepo;

  if ((This == NULL) || (CmObject == NULL)) {
    ASSERT (This != NULL);
    ASSERT (CmObject != NULL);
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;
  PlatformRepo = This->PlatRepoInfo;

  switch (GET_CM_OBJECT_ID (CmObjectId)) {
    case EArmObjBootArchInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->BootArchInfo,
                 sizeof (PlatformRepo->BootArchInfo),
                 1,
                 CmObject
                 );
      break;

#ifdef HEADLESS_PLATFORM
    case EArmObjFixedFeatureFlags:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->FixedFeatureFlags,
                 sizeof (PlatformRepo->FixedFeatureFlags),
                 1,
                 CmObject
                 );
      break;
#endif
    case EArmObjPowerManagementProfileInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->PmProfileInfo,
                 sizeof (PlatformRepo->PmProfileInfo),
                 1,
                 CmObject
                 );
      break;

    case EArmObjGenericTimerInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->GenericTimerInfo,
                 sizeof (PlatformRepo->GenericTimerInfo),
                 1,
                 CmObject
                 );
      break;

    case EArmObjPlatformGenericWatchdogInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->Watchdog,
                 sizeof (PlatformRepo->Watchdog),
                 1,
                 CmObject
                 );
      break;

    case EArmObjPlatformGTBlockInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 PlatformRepo->GTBlockInfo,
                 sizeof (PlatformRepo->GTBlockInfo),
                 ARRAY_SIZE (PlatformRepo->GTBlockInfo),
                 CmObject
                 );
      break;

    case EArmObjGTBlockTimerFrameInfo:
      Status = HandleCmObjectRefByToken (
                 This,
                 CmObjectId,
                 PlatformRepo->GTBlock0TimerInfo,
                 sizeof (PlatformRepo->GTBlock0TimerInfo),
                 ARRAY_SIZE (PlatformRepo->GTBlock0TimerInfo),
                 Token,
                 GetGTBlockTimerFrameInfo,
                 CmObject
                 );
      break;

    case EArmObjGicCInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 PlatformRepo->GicCInfo,
                 sizeof (PlatformRepo->GicCInfo),
                 ARRAY_SIZE (PlatformRepo->GicCInfo),
                 CmObject
                 );
      break;

    case EArmObjGicDInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->GicDInfo,
                 sizeof (PlatformRepo->GicDInfo),
                 1,
                 CmObject
                 );
      break;

    case EArmObjGicRedistributorInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->GicRedistInfo,
                 sizeof (PlatformRepo->GicRedistInfo),
                 1,
                 CmObject
                 );
      break;

    case EArmObjSerialConsolePortInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->SpcrSerialPort,
                 sizeof (PlatformRepo->SpcrSerialPort),
                 1,
                 CmObject
                 );
      break;

    case EArmObjSerialDebugPortInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->DbgSerialPort,
                 sizeof (PlatformRepo->DbgSerialPort),
                 1,
                 CmObject
                 );
      break;

    case EArmObjGicItsInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->GicItsInfo,
                 sizeof (PlatformRepo->GicItsInfo),
                 1,
                 CmObject
                 );
      break;

    default: {
      Status = EFI_NOT_FOUND;
      DEBUG ((
        DEBUG_INFO,
        "INFO: Object 0x%x. Status = %r\n",
        CmObjectId,
        Status
        ));
      break;
    }
  }//switch

  return Status;
}

/** Return an OEM namespace object.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in, out] CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
  @retval EFI_BAD_BUFFER_SIZE   The size returned by the Configuration Manager
                                is less than the Object size for the requested
                                object.
**/
EFI_STATUS
EFIAPI
GetOemNameSpaceObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN  OUT   CM_OBJ_DESCRIPTOR                     * CONST CmObject
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;
  if ((This == NULL) || (CmObject == NULL)) {
    ASSERT (This != NULL);
    ASSERT (CmObject != NULL);
    return EFI_INVALID_PARAMETER;
  }

  switch (GET_CM_OBJECT_ID (CmObjectId)) {
    default: {
      Status = EFI_NOT_FOUND;
      DEBUG ((
        DEBUG_ERROR,
        "ERROR: Object 0x%x. Status = %r\n",
        CmObjectId,
        Status
        ));
      break;
    }
  }

  return Status;
}

/** The GetObject function defines the interface implemented by the
    Configuration Manager Protocol for returning the Configuration
    Manager Objects.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in, out] CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
EFI_STATUS
EFIAPI
ArmVExpressPlatformGetObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN  OUT   CM_OBJ_DESCRIPTOR                     * CONST CmObject
  )
{
  EFI_STATUS  Status;

  if ((This == NULL) || (CmObject == NULL)) {
    ASSERT (This != NULL);
    ASSERT (CmObject != NULL);
    return EFI_INVALID_PARAMETER;
  }

  switch (GET_CM_NAMESPACE_ID (CmObjectId)) {
    case EObjNameSpaceStandard:
      Status = GetStandardNameSpaceObject (This, CmObjectId, Token, CmObject);
      break;
    case EObjNameSpaceArm:
      Status = GetArmNameSpaceObject (This, CmObjectId, Token, CmObject);
      break;
    case EObjNameSpaceOem:
      Status = GetOemNameSpaceObject (This, CmObjectId, Token, CmObject);
      break;
    default: {
      Status = EFI_INVALID_PARAMETER;
      DEBUG ((
        DEBUG_ERROR,
        "ERROR: Unknown Namespace Object = 0x%x. Status = %r\n",
        CmObjectId,
        Status
        ));
      break;
    }
  }

  return Status;
}

/** The SetObject function defines the interface implemented by the
    Configuration Manager Protocol for updating the Configuration
    Manager Objects.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in]      CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the Object.

  @retval EFI_UNSUPPORTED  This operation is not supported.
**/
EFI_STATUS
EFIAPI
ArmVExpressPlatformSetObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN        CM_OBJ_DESCRIPTOR                     * CONST CmObject
  )
{
  return EFI_UNSUPPORTED;
}

/** A structure describing the configuration manager protocol interface.
*/
STATIC
CONST
EDKII_CONFIGURATION_MANAGER_PROTOCOL VExpressPlatformConfigManagerProtocol = {
  CREATE_REVISION(1,0),
  ArmVExpressPlatformGetObject,
  ArmVExpressPlatformSetObject,
  &VExpressPlatRepositoryInfo
};

/**
  Entrypoint of Configuration Manager Dxe.

  @param  ImageHandle
  @param  SystemTable

  @return EFI_SUCCESS
  @return EFI_LOAD_ERROR
  @return EFI_OUT_OF_RESOURCES

**/
EFI_STATUS
EFIAPI
ConfigurationManagerDxeInitialize (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE  * SystemTable
  )
{
  EFI_STATUS  Status;

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEdkiiConfigurationManagerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  (VOID*)&VExpressPlatformConfigManagerProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: Failed to get Install Configuration Manager Protocol." \
      " Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  Status = InitializePlatformRepository (
    &VExpressPlatformConfigManagerProtocol
    );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: Failed to initialize the Platform Configuration Repository." \
      " Status = %r\n",
      Status
      ));
  }

error_handler:
  return Status;
}
