#include <fstream>

#include "../libpeggle.h"
#include "../macros.h"

int main()
{
    change_logging(Peggle::LogVerbose);
    // change_logging(Peggle::LogDisable);

    // const auto peggle_pak_path = std::filesystem::path(R"(C:\Projects\Generic\Haggle\paks\Peggle)");
    // auto test_characters = Peggle::Config::LoadCharacterConfig(peggle_pak_path / "characters" / "characters.cfg");
    // auto test_trophies = Peggle::Config::LoadTrophyConfig(peggle_pak_path / "levels" / "trophy.cfg");
    // auto test_stages = Peggle::Config::LoadStageConfig(peggle_pak_path / "levels" / "stages.cfg");

    const auto peggle_pak_path = std::filesystem::path(R"(C:\Projects\Generic\Haggle\paks\Peggle.pak)");
    auto pak = Peggle::Pak(peggle_pak_path);

    auto test_stages = Peggle::Config::LoadStageConfig(pak, "levels\\stages.cfg");
    auto test_trophies = Peggle::Config::LoadTrophyConfig(pak, "levels\\trophy.cfg");
    auto test_characters = Peggle::Config::LoadCharacterConfig(pak, "characters\\characters.cfg");

    auto test_stages_str = Peggle::Config::BuildConfig(test_stages);
    auto test_trophies_str = Peggle::Config::BuildConfig(test_trophies);
    auto test_characters_str = Peggle::Config::BuildConfig(test_characters);


    auto test_level = Peggle::Level::LoadLevel(pak, "levels\\vortex.dat");
    const auto test_level_build = Peggle::Level::BuildLevel(test_level);
    pak.UpdateFile("levels\\vortex.dat", test_level_build.Data, test_level_build.Size);

    if (std::ofstream test_lvl_file(R"(C:\Projects\Generic\Haggle\test_vortex.dat)", std::ios::out | std::ios::binary); test_lvl_file.is_open()) {
        test_lvl_file.write(static_cast<const char *>(test_level_build.Data), test_level_build.Size);
        test_lvl_file.close();
    }


    pak.UpdateFile("levels\\stages.cfg", test_stages_str.c_str(), test_stages_str.size());
    pak.UpdateFile("levels\\trophy.cfg", test_trophies_str.c_str(), test_trophies_str.size());
    pak.UpdateFile("characters\\characters.cfg", test_characters_str.c_str(), test_characters_str.size());
    pak.SetXor(0xF7);
    pak.Save("Peggle_out.pak");

    // std::printf("%s\n", test_stages_str.c_str());

    // if (std::ofstream test_cfg_file(R"(C:\Projects\Generic\Haggle\test_stages.cfg)", std::ios::out); test_cfg_file.is_open()) {
    //     test_cfg_file.write(test_stages_str.c_str(), test_stages_str.size());
    //     test_cfg_file.close();
    // }
    //
    // if (std::ofstream test_cfg_file(R"(C:\Projects\Generic\Haggle\test_trophies.cfg)", std::ios::out); test_cfg_file.is_open()) {
    //     test_cfg_file.write(test_trophies_str.c_str(), test_trophies_str.size());
    //     test_cfg_file.close();
    // }
    //
    // if (std::ofstream test_cfg_file(R"(C:\Projects\Generic\Haggle\test_characters.cfg)", std::ios::out); test_cfg_file.is_open()) {
    //     test_cfg_file.write(test_characters_str.c_str(), test_characters_str.size());
    //     test_cfg_file.close();
    // }

    // auto pak = Peggle::Pak(peggle_pak_path);

    /* comment this out to enable this
    auto pak = Peggle::Pak("simple.pak");
    std::printf("Test Pak: IsPak = %s\n", STR_BOOL(pak.IsPak()));
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

    //*/

    /* comment this out to enable this

    // if testing, change this path to be a valid path containing a Peggle pak file
    const auto peggle_pak_path = std::filesystem::path(R"(C:\Projects\Generic\Haggle\paks)");

    // auto total_size = 0;
    // auto number_of_files = 0;
    // auto start = std::chrono::steady_clock::now();

    auto pak_peggle = Peggle::Pak(peggle_pak_path / "Peggle.pak");

    pak_peggle.Save(peggle_pak_path / "newtestone.pak");
    // for (auto& pak_file: pak_peggle.GetFileList()) {
    //     const auto file_data = pak_peggle.GetFile(pak_file);
    //     total_size += file_data.Size;
    //     ++number_of_files;
    //     // printf(" - %s\n", pak_file.c_str());
    // }
    // auto finish = std::chrono::steady_clock::now();
    // double elapsed_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(finish - start).count();
    // std::printf("%f\n", elapsed_ms);
    // std::printf("%d, %d\n", total_size, number_of_files);
    /*
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