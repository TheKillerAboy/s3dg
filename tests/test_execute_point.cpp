#include <catch2/catch_session.hpp>
#include <spdlog/cfg/env.h>

int main(int argc, char* argv[]) {
    spdlog::cfg::load_env_levels();
    int result = Catch::Session().run(argc, argv);
    return result;
}
