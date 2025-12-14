#include <array>
#include "sob.hpp"

struct SobCxxContext
{
    static constexpr std::string_view cxx{"g++"};
    static constexpr sopho::StaticString obj_postfix{".o"};
    static constexpr sopho::StaticString build_prefix{"build/"};
};

struct SobSource
{
    using Dependent = std::tuple<>;
    static constexpr sopho::StaticString source{"sob.cpp"};
};
struct Sob
{
    using Dependent = std::tuple<SobSource>;
    static constexpr sopho::StaticString target{"sob"};
};

struct TigrCxxContext
{
    static constexpr std::string_view cxx{"g++"};
    static constexpr sopho::StaticString obj_postfix{".o"};
    static constexpr sopho::StaticString build_prefix{"build/"};
    static constexpr std::array<std::string_view, 3> cxxflags{"-Ithirdparty/tigr/include"};
    static constexpr std::array<std::string_view, 3> ldflags{"-lGLU", "-lGL", "-lX11"};
};

struct TigrSource
{
    using Dependent = std::tuple<>;
    static constexpr sopho::StaticString source{"thirdparty/tigr/src/tigr.c"};
};

struct MainSource
{
    using Dependent = std::tuple<>;
    static constexpr sopho::StaticString source{"src/main.cpp"};
};

struct Main
{
    using Dependent = std::tuple<TigrSource, MainSource>;
    static constexpr sopho::StaticString target{"main"};
};

int main()
{
    sopho::CxxToolchain<SobCxxContext>::CxxBuilder<Sob>::build();
    sopho::CxxToolchain<TigrCxxContext>::CxxBuilder<Main>::build();
    return 0;
}
