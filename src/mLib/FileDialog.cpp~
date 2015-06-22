
#include "main.h"

#ifdef WIN32
#include <Windows.h>

string FileDialog::showOpen()
{
    OPENFILENAME data;

    char buffer[1024];

    ZeroMemory(&data, sizeof(data));

    data.lStructSize = sizeof(data);
    data.hwndOwner = NULL;
    data.lpstrFile = buffer;
    data.lpstrFile[0] = '\0';
    data.nMaxFile = sizeof(buffer);
    data.lpstrFilter = "Charge puzzle (*.pzl)\0*.pzl\0All (*.*)\0*.*\0";
    data.nFilterIndex = 1;
    data.lpstrFileTitle = NULL;
    data.nMaxFileTitle = 0;
    data.lpstrInitialDir = NULL;
    data.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    GetOpenFileName(&data);

    return string(buffer);
}

string FileDialog::showSave()
{
    OPENFILENAME data;

    char buffer[1024];

    ZeroMemory(&data, sizeof(data));

    data.lStructSize = sizeof(data);
    data.hwndOwner = NULL;
    data.lpstrFile = buffer;
    data.lpstrFile[0] = '\0';
    data.nMaxFile = sizeof(buffer);
    data.lpstrFilter = "Charge puzzle (*.pzl)\0*.pzl\0All (*.*)\0*.*\0";
    data.nFilterIndex = 1;
    data.lpstrFileTitle = NULL;
    data.nMaxFileTitle = 0;
    data.lpstrInitialDir = NULL;
    data.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    GetSaveFileName(&data);

    return string(buffer);
}

#endif