#include <fstream>

#include "../libpeggle.h"
#include "../macros.h"
#define FORE_FAIL        "\033[91m"
#define FORE_PASS        "\033[92m"

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

    // auto test_stages = Peggle::Config::LoadStageConfig(pak, "levels\\stages.cfg");
    // auto test_trophies = Peggle::Config::LoadTrophyConfig(pak, "levels\\trophy.cfg");
    // auto test_characters = Peggle::Config::LoadCharacterConfig(pak, "characters\\characters.cfg");
    //
    // auto test_stages_str = Peggle::Config::BuildConfig(test_stages);
    // auto test_trophies_str = Peggle::Config::BuildConfig(test_trophies);
    // auto test_characters_str = Peggle::Config::BuildConfig(test_characters);

    auto lvl_level1 = Peggle::Level::LoadLevel(pak, "levels\\level1.dat");

    std::vector<Peggle::LevelTypes::Element> new_elements;

    for (auto& element : lvl_level1.Elements) {
        if (element.eType == Peggle::LevelTypes::Circle) {
            auto circle = Peggle::Level::AccessCircle(*element.entry);
            circle->mPos.x += 10.;
            circle->mPos.y -= 10.;

            auto new_element = Peggle::Level::CloneElement(element);
            auto new_circle = Peggle::Level::AccessCircle(*new_element.entry);
            new_circle->mPos.x += 50.;
            new_circle->mPos.y -= 50.;
            new_elements.emplace_back(new_element);

            break;
        }
    }

    for (auto& element : new_elements) {
        lvl_level1.Elements.emplace_back(element);
    }

    // auto test_level = Peggle::Level::LoadLevel(pak, "levels\\theamoeban.dat");
    // const auto test_level_build = Peggle::Level::BuildLevel(test_level);
    // // pak.UpdateFile("levels\\tubing.dat", test_level_build.Data, test_level_build.Size);
    //
    // if (std::ofstream test_lvl_file(R"(C:\Projects\Generic\Haggle\test_theamoeban.dat)", std::ios::out | std::ios::binary); test_lvl_file.is_open()) {
    //     test_lvl_file.write(static_cast<const char *>(test_level_build.Data), test_level_build.Size);
    //     test_lvl_file.close();
    // }


    // pak.UpdateFile("levels\\stages.cfg", test_stages_str.c_str(), test_stages_str.size());
    // pak.UpdateFile("levels\\trophy.cfg", test_trophies_str.c_str(), test_trophies_str.size());
    // pak.UpdateFile("characters\\characters.cfg", test_characters_str.c_str(), test_characters_str.size());
    // pak.SetXor(0xF7);
    // pak.Save("Peggle_out.pak");

    /*
    auto start = std::chrono::steady_clock::now();

    const auto peggle_pak_path = std::filesystem::path(R"(C:\Projects\Generic\Haggle\paks\Peggle.pak)");
    auto pak = Peggle::Pak(peggle_pak_path);

    auto test_stages = Peggle::Config::LoadStageConfig(pak, "levels\\stages.cfg");

    int total = 0;
    int pass = 0;
    int stage_id = 1;
    for (const auto& stage : test_stages.Stages) {
        int level_id = 1;
        for (const auto& level : stage.Levels) {
            const auto level_path = std::format("levels\\{}.dat", level.Id);
            printf("[Level %d-%d (%s)] ", stage_id, level_id, level.Name.c_str());
            ++level_id;
            ++total;
            try {
                const auto level_data = pak.GetFile(level_path);
                const auto level_obj = Peggle::Level::LoadLevel(level_data);
                const auto level_built = Peggle::Level::BuildLevel(level_obj);
                std::string out_name;
                std::string out_name_orig;
                if (level_data.Size != level_built.Size) {
                    printf(FORE_FAIL "fail [size mismatch]\n" FORE_RESET);
                    goto dump_data;
                }
                if (std::memcmp(level_data.Data, level_built.Data, level_data.Size) != 0) {
                    printf(FORE_FAIL "fail [data mismatch]\n" FORE_RESET);
                    goto dump_data;
                }
                goto check_passed;
            dump_data:
                out_name = std::format(R"(C:\Projects\Generic\Haggle\test_{}_fail.dat)", level.Id);
                if (std::ofstream test_lvl_file(out_name, std::ios::out | std::ios::binary); test_lvl_file.is_open()) {
                    test_lvl_file.write(static_cast<const char *>(level_built.Data), level_built.Size);
                    test_lvl_file.close();
                }
                out_name_orig = std::format(R"(C:\Projects\Generic\Haggle\test_{}_orig.dat)", level.Id);
                if (std::ofstream test_lvl_file(out_name_orig, std::ios::out | std::ios::binary); test_lvl_file.is_open()) {
                    test_lvl_file.write(static_cast<const char *>(level_data.Data), level_data.Size);
                    test_lvl_file.close();
                }
                continue;
            check_passed:
                printf(FORE_PASS "pass\n" FORE_RESET);
                ++pass;
            } catch (...) {
                printf(FORE_FAIL "fail [exception]\n" FORE_RESET);
            }
        }
        ++stage_id;
        printf("\n" FORE_RESET);
    }
    printf("[results] Total: %d (pass: %d, fail: %d)\n", total, pass, total - pass);

    auto finish = std::chrono::steady_clock::now();
    double elapsed_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(finish - start).count();
    std::printf("time taken: %f ms\n", elapsed_ms);
    //*/

    // auto test_level = Peggle::Level::LoadLevel(R"(C:\Projects\Generic\Haggle\test_all_types.dat)");
    // const auto test_level_build = Peggle::Level::BuildLevel(test_level);
    // if (std::ofstream test_lvl_file(R"(C:\Projects\Generic\Haggle\test_all_types_out.dat)", std::ios::out | std::ios::binary); test_lvl_file.is_open()) {
    //     test_lvl_file.write(static_cast<const char *>(test_level_build.Data), test_level_build.Size);
    //     test_lvl_file.close();
    // }

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