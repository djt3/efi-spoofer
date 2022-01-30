#include "smbios_structures.h"

// finds the address of the smbios entry point using the system config table
EFI_CONFIGURATION_TABLE* find_smbios_config_table(EFI_SYSTEM_TABLE* system_table) {
  EFI_CONFIGURATION_TABLE* config_table = system_table->ConfigurationTable;

  while (CompareMem(&config_table->VendorGuid, &smbios_guid, sizeof(smbios_guid)))
    config_table++;

  return config_table;
}

smbios_header* get_next_table(smbios_header* header) {
  char* address = (char*)((UINT64)header + header->length + 1);

  int i = 0;
  while (address[0] != '\0' || address[1] != '\0') {
    address = (UINT64)address + 1;

    i++;
  }

  address = (UINT64)address + 2;

  return address;
}

char* find_smbios_string(smbios_header* header, UINT8 string_idx) {
  char* address = (char*)((UINT64)header + header->length);

  int i = 1;
  while (i != string_idx) {
    // move to the address of the next string
    do {
      address++;
    } while (*(address - 1) != '\0');

    i++;
    if (*address == '\0')
     break;
  }

  return address;
}

void print_smbios_string(char* str) {
  while (*str != '\0') {
    Print(L"%c", *str);
    str++;
  }
}

UINT64 ticks_read() {
  UINT64 a, d;
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  return (d << 32) | a;
}

int random_seed = 0;

UINT64 rand() {
  random_seed++;
  return random_seed * 153 * ticks_read();
}

void randomize_smbios_string(smbios_header* structure, UINT8 str_index) {
  char* str = find_smbios_string(structure, str_index);
  print_smbios_string(str);
  Print(L" -> ");

  while (str[0] != '\0') {
    if (str[0] >= '0' && str[0] <= '9')
      str[0] = (rand() % 10) + '0';
    else if (str[0] >= 'a' && str[0] <= 'z')
      str[0] = (rand() % ('z' - 'a')) + 'a';
    else if (str[0] >= 'A' && str[0] <= 'Z')
      str[0] = (rand() % ('Z' - 'A')) + 'A';

    str++;
  }

  print_smbios_string(str);
  Print("\n");
}

void randomize_smbios_uuid(UINT8* uuid) {
  for (int i = 0; i < 16; i++)
    uuid[i] = rand() % 255;

  uuid_version* version = uuid + 0x6;

  version->v1 = 0;
  version->v2 = 1;
  version->v3 = 0;
  version->v4 = 0;
}

void spoof_tables(void* table_address) {
  table_address = get_next_table(table_address);

  smbios_structure_1* structure = (smbios_structure_1*)table_address;
  randomize_smbios_string(structure, structure->serial_str);
  randomize_smbios_uuid(structure->uuid);

  table_address = get_next_table(table_address);

  smbios_structure_2* structure2 = (smbios_structure_2*)table_address;
  randomize_smbios_string(structure2, structure2->serial_str);

  table_address = get_next_table(table_address);

  smbios_structure_3* structure3 = (smbios_structure_3*)table_address;
  randomize_smbios_string(structure3, structure3->serial_str);
}

BOOLEAN spoof_smbios(EFI_SYSTEM_TABLE* system_table) {
  Print(L"searching for smbios...\n");
  EFI_CONFIGURATION_TABLE* smbios_table = find_smbios_config_table(system_table);
  if (smbios_table == 0) {
    Print(L"error finding smbios table\n");
    return FALSE;
  }
  Print(L"found smbios entry at %x\n", smbios_table->VendorTable);


  smbios_entry* old_entry = (smbios_entry*)smbios_table->VendorTable;

  EFI_PHYSICAL_ADDRESS buffer_ptr;
  EFI_STATUS status = uefi_call_wrapper(BS->AllocatePages, 4, 0, EfiACPIMemoryNVS, EFI_SIZE_TO_PAGES(sizeof(smbios_entry)), &buffer_ptr);

  if (EFI_ERROR(status)) {
    Print(L"error allocing mem\n");
    return FALSE;
  }

  smbios_entry* new_entry = (smbios_entry*)(UINTN)buffer_ptr;

  CopyMem(new_entry, old_entry, sizeof(smbios_entry));
  Print(L"copied smbios entry to %x\n", new_entry);

  smbios_table->VendorTable = new_entry;
  Print(L"replaced smbios entry pointer with new entry\n");

  status = uefi_call_wrapper(BS->AllocatePages, 4, 0, EfiACPIMemoryNVS, EFI_SIZE_TO_PAGES(new_entry->structure_table_length), &buffer_ptr);

  if (EFI_ERROR(status)) {
   Print(L"error allocing mem\n");
   return FALSE;
  }

  void* new_table = (void*)(UINTN)buffer_ptr;
  CopyMem(new_table, (void*)old_entry->structure_table_address, new_entry->structure_table_length);
  Print(L"copied smbios table to %x\n", new_table);

  new_entry->structure_table_address = (UINT32)new_table;
  Print(L"replaced table pointer in new entry\n");

  spoof_tables(new_table);

  return TRUE;
}
