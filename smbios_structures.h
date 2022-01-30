#include <efi.h>
#include <efilib.h>

/// ENTRY POINTS

typedef struct {
  char anchor[5]; // _SM3_
  UINT8 eps_checksum;
  UINT8 entry_length;
  UINT8 major_version;
  UINT8 minor_version;
  UINT8 doc_version;
  UINT8 entry_revision;
  UINT8 reserved;
  UINT32 structure_table_length;
  UINT64 structure_table_address;
} smbios3_entry;

typedef struct {
  UINT32 anchor; // _SM_
  UINT8 eps_checksum;
  UINT8 entry_length;
  UINT8 major_version;
  UINT8 minor_version;
  UINT16 max_structure_size;
  UINT8 entry_revision;
  char formatted[5];
  char second_anchor[5];
  UINT8 second_checksum;
  UINT16 structure_table_length;
  UINT32 structure_table_address;
  UINT16 structure_count;
  UINT8 bcd_revision;
} smbios_entry;

/// STRUCTURES

typedef struct {
  UINT16 time : 12;
  UINT8 v1 : 1;
  UINT8 v2 : 1;
  UINT8 v3 : 1;
  UINT8 v4 : 1;
} uuid_version;

// header that is at the start of each structure
typedef struct {
 	UINT8 type;
 	UINT8 length;
 	UINT16 handle;
} smbios_header;

typedef struct {
 	UINT8 type;
 	UINT8 length;
 	UINT16 handle;
  UINT8 vendor_string;
  UINT8 bios_version_string;
  UINT16 bios_start_address;
  UINT8 bios_release_date_string;
  UINT8 bios_rom_size;
  UINT64 bios_characteristics;
  // ext bytes?
  UINT8 sys_bios_major_release;
  UINT8 sys_bios_minor_release;
  UINT8 controller_firmware_major_release;
  UINT8 controller_firmware_minor_release;
  UINT16 extended_bios_rom_size;
} smbios_structure_0;

// type 1 - system information
typedef struct {
 	UINT8 type;
 	UINT8 length;
 	UINT16 handle;
  UINT8 manafacturer_str;
  UINT8 product_name_str;
  UINT8 version_str;
  UINT8 serial_str;
  UINT8 uuid[16];
  UINT8 wake_up_type_str;
  UINT8 sku_str;
  UINT8 family_str;
} smbios_structure_1;

// type 2 - baseboard information
typedef struct {
  smbios_header header;
  UINT8 manafacturer_str;
  UINT8 product_str;
  UINT8 version_str;
  UINT8 serial_str;
  UINT8 asset_tag_str;
  UINT8 feature_flags;
  UINT8 chassis_location_str;
  UINT16 chassis_handle;
  UINT8 board_type;
  UINT8 num_object_handles;
  UINT16 object_handles; // sizeof(UINT16 * num_object_handles)
} smbios_structure_2;

typedef struct {
  smbios_header header;
  UINT8 manafacturer_str;
  UINT8 type;
  UINT8 version_str;
  UINT8 serial_str;
  UINT8 asset_tag_str;
  UINT8 boot_state;
  UINT8 power_state;
  UINT8 thermal_state;
  UINT8 security_state;
  UINT32 oem_reserved;
  UINT8 height;
  UINT8 power_cord_count;
  UINT8 element_count;
  UINT8 element_length;
  UINT8 elements;
  UINT8 sku_str;
} smbios_structure_3;

/// GUIDS

EFI_GUID smbios_guid = { 0xEB9D2D31, 0x2D88, 0x11D3, { 0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D }};
EFI_GUID smbios3_guid = { 0xF2FD1544, 0x9794, 0x4A2C, { 0x99, 0x2E, 0xE5, 0xBB, 0xCF, 0x20, 0xE3, 0x94 }};
