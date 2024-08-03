#include "../libpeggle.h"
#include "../macros.h"

int main()
{
    change_logging(Peggle::LogVerbose);

    const auto pak = Peggle::Pak("simple.pak");
    // std::printf("Test Pak: IsPak = %s\n", STR_BOOL(pak.IsPak()));
    pak.Save("simple_out.pak");

    // if testing, change this path to be a valid path containing a Peggle pak file
    const auto peggle_pak_path = std::filesystem::path(R"(C:\Projects\Generic\Haggle\paks)");

    const auto pak_peggle = Peggle::Pak(peggle_pak_path / "Peggle.pak");
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
}