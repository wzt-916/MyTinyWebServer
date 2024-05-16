#include "config.h"

int main(int argc, char *argv[])
{
    Config config;
    config.parse_arg(argc, argv);

    return 0;
}