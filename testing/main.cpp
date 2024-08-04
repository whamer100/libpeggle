#include "../libpeggle.h"
#include "../macros.h"

int main()
{
    change_logging(Peggle::LogVerbose);

    auto pak = Peggle::Pak("simple.pak");
    // std::printf("Test Pak: IsPak = %s\n", STR_BOOL(pak.IsPak()));
    for (auto& pak_file: pak.GetFileList()) {
        std::printf(" - %s\n", pak_file.c_str());
    }
    pak.Save("simple_out.pak");
    pak.SetXor(0x00);  // easier to read file data in hex editor

    const auto test_file = pak.GetFile("testfile.bin");
    std::printf("test: Size: %d, Data: %s\n", test_file.Size, (const char*)test_file.Data);

    const char* test_replacement = "test data time yippee";
    pak.UpdateFile("testfile.bin", test_replacement, strlen(test_replacement)+1);
    pak.Save("simple_replace_out.pak");

    pak.RemoveFile("testfile.bin");
    const char* test_addition = "NEW data time gaming";
    pak.AddFile("newfile.fart", test_addition, strlen(test_addition)+1);
    pak.Save("simple_fileop_out.pak");

    /* comment this out to enable this

    // if testing, change this path to be a valid path containing a Peggle pak file
    const auto peggle_pak_path = std::filesystem::path(R"(C:\Projects\Generic\Haggle\paks)");

    auto pak_peggle = Peggle::Pak(peggle_pak_path / "Peggle.pak");
    // for (auto& pak_file: pak_peggle.GetFileList()) {
    //     printf(" - %s\n", pak_file.c_str());
    // }
    pak_peggle.Save(peggle_pak_path / "Peggle_out.pak");
    pak_peggle.Export(peggle_pak_path / "export_test");

    auto peggle_folder_pak = Peggle::Pak(peggle_pak_path / "export_test");
    peggle_folder_pak.SetXor(0xF7);
    peggle_folder_pak.Save(peggle_pak_path / "reimport.pak");

    // auto folder_pak = Peggle::Pak("simple");
    // folder_pak.SetXor(0xF7);
    // folder_pak.Save("simple_folder_out.pak");

    // auto peggle_folder = Peggle::Pak(peggle_pak_path / "Peggle");
    // peggle_folder.SetXor(0xF7);
    // peggle_folder.Save("Peggle_folder_out.pak");
    //*/
}