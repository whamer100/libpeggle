#include "../libpeggle.h"
#include "../macros.h"

int main()
{
    change_logging(Peggle::LogVerbose);
    const auto pak = Peggle::Pak("simple.pak");
    std::printf("Test Pak: IsPak = %s\n", STR_BOOL(pak.IsPak()));
}