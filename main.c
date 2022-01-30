#include "smbios.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
  InitializeLib(image_handle, system_table);

  EFI_LOADED_IMAGE *loaded_image;
  EFI_STATUS status;

  status = uefi_call_wrapper(BS->OpenProtocol, 6, image_handle, &LoadedImageProtocol, &loaded_image, image_handle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
  if (EFI_ERROR(status)) {
    Print(L"Error! Could not get image handle...");
    uefi_call_wrapper(BS->Stall, 1, 300000000);
    return status;
  }

  EFI_FILE* root = LibOpenRoot(loaded_image->DeviceHandle);
  if (!root) {
    Print(L"Error! Could not open root directory...\n");
    uefi_call_wrapper(BS->Stall, 1, 300000000);
    return status;
  }

  EFI_HANDLE handle;
  status = uefi_call_wrapper(root->Open, 5, root, &handle, L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi", EFI_FILE_MODE_READ, NULL);
  if (EFI_ERROR(status)) {
    Print(L"Error! Windows Not Found...\n");
    uefi_call_wrapper(BS->Stall, 1, 300000000);
    return status;
  }
  // uefi_call_wrapper(handle->Close, 1, handle);


  EFI_DEVICE_PATH* loader_path = FileDevicePath(loaded_image->DeviceHandle, L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi");

  EFI_HANDLE* loader_image;
  status = uefi_call_wrapper(BS->LoadImage, 6, FALSE, image_handle, loader_path, NULL, 0, &loader_image);
  if (EFI_ERROR(status)) {
    Print(L"Error! Could not load image...\n");
    uefi_call_wrapper(BS->Stall, 1, 300000000);
    return status;
  }

  if (!spoof_smbios(system_table)) {
    Print(L"failed to spoof smbios\n");
    return EFI_SUCCESS;
  }
  Print(L"Spoofed smbios\n");

  uefi_call_wrapper(BS->StartImage, 3, loader_image, NULL, NULL);

  return EFI_SUCCESS;
}
