#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS status;
    EFI_LOADED_IMAGE *LoadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
    EFI_FILE_PROTOCOL *Root, *Kernel;

    status = uefi_call_wrapper(BS->HandleProtocol, 3, ImageHandle, &LoadedImageProtocol, (void**)&LoadedImage);
    if (EFI_ERROR(status)) return status;

    status = uefi_call_wrapper(BS->HandleProtocol, 3, LoadedImage->DeviceHandle, &FileSystemProtocol, (void**)&FileSystem);
    if (EFI_ERROR(status)) return status;

    status = uefi_call_wrapper(FileSystem->OpenVolume, 2, FileSystem, &Root);
    if (EFI_ERROR(status)) return status;

    status = uefi_call_wrapper(Root->Open, 5, Root, &Kernel, L"kernel.bin", EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        Print(L"Could not open kernel.bin\n");
        return status;
    }

    EFI_FILE_INFO *FileInfo;
    UINTN info_size = SIZE_OF_EFI_FILE_INFO + 200;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, info_size, (void**)&FileInfo);
    if (EFI_ERROR(status)) return status;
    status = uefi_call_wrapper(Kernel->GetInfo, 4, Kernel, &GenericFileInfo, &info_size, FileInfo);
    if (EFI_ERROR(status)) return status;
    UINTN file_size = FileInfo->FileSize;
    uefi_call_wrapper(BS->FreePool, 1, FileInfo);

    EFI_PHYSICAL_ADDRESS kernel_addr = 0x100000;
    UINTN pages = (file_size + 0xFFF) / 0x1000;
    status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderData, pages, &kernel_addr);
    if (EFI_ERROR(status)) {
        status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, pages, &kernel_addr);
        if (EFI_ERROR(status)) return status;
    }

    UINTN bytes = file_size;
    status = uefi_call_wrapper(Kernel->Read, 3, Kernel, &bytes, (void*)kernel_addr);
    if (EFI_ERROR(status)) return status;
    uefi_call_wrapper(Kernel->Close, 1, Kernel);

    UINTN map_size = 0, map_key, desc_size; UINT32 desc_ver; EFI_MEMORY_DESCRIPTOR *mem_map = NULL;
    uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, mem_map, &map_key, &desc_size, &desc_ver);
    map_size += desc_size * 8;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, map_size, (void**)&mem_map);
    if (EFI_ERROR(status)) return status;
    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, mem_map, &map_key, &desc_size, &desc_ver);
    if (EFI_ERROR(status)) return status;
    status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, map_key);
    if (EFI_ERROR(status)) return status;

    void (*kernel_entry)(void) = (void (*)(void))kernel_addr;
    kernel_entry();

    for (;;);
    return EFI_SUCCESS;
}
