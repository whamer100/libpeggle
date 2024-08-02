#include "../libpeggle.h"
#include "../macros.h"

int main()
{
    change_logging(Peggle::LogVerbose);
    const auto pak = Peggle::Pak("simple.pak");
    std::printf("Test Pak: IsPak = %s\n", STR_BOOL(pak.IsPak()));
    // pak.Save("simple_out.pak");
    // const auto pak_peggle = Peggle::Pak("Peggle.pak");
    // pak_peggle.Save("Peggle_out.pak");
}