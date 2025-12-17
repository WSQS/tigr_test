#include <array>
#include "sob.hpp"

struct SobClContext
{
    static constexpr std::string_view cxx{"cl"};
    static constexpr sopho::StaticString obj_prefix{" /Fo:"};
    static constexpr sopho::StaticString obj_postfix{".obj"};
    static constexpr sopho::StaticString bin_prefix{" /Fe:"};
    static constexpr sopho::StaticString build_prefix{"build/"};
    static constexpr std::array<std::string_view, 1> cxxflags{"/std:c++17"};
};

struct SobGxxContext
{
    static constexpr std::string_view cxx{"g++"};
    static constexpr sopho::StaticString obj_prefix{" -o "};
    static constexpr sopho::StaticString obj_postfix{".o"};
    static constexpr sopho::StaticString bin_prefix{" -o "};
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

struct TigrClContext
{
    static constexpr std::string_view cxx{"cl"};
    static constexpr sopho::StaticString obj_prefix{" /Fo:"};
    static constexpr sopho::StaticString obj_postfix{".obj"};
    static constexpr sopho::StaticString bin_prefix{" /Fe:"};
    static constexpr sopho::StaticString build_prefix{"build/"};
    static constexpr std::array<std::string_view, 3> cxxflags{"-Ithirdparty/tigr/include","/utf-8"};
    static constexpr std::array<std::string_view, 4> ldflags{"opengl32.lib", "gdi32.lib", "/link","/SUBSYSTEM:WINDOWS"};
};

struct TigrGxxContext
{
    static constexpr std::string_view cxx{"g++"};
    static constexpr sopho::StaticString obj_prefix{" -o "};
    static constexpr sopho::StaticString obj_postfix{".o"};
    static constexpr sopho::StaticString bin_prefix{" -o "};
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

// 测试构建配置
struct TestClContext
{
    static constexpr std::string_view cxx{"cl"};
    static constexpr sopho::StaticString obj_prefix{" /Fo:"};
    static constexpr sopho::StaticString obj_postfix{".obj"};
    static constexpr sopho::StaticString bin_prefix{" /Fe:"};
    static constexpr sopho::StaticString build_prefix{"build/tests/"};
    static constexpr std::array<std::string_view, 3> cxxflags{"/std:c++17", "/Itests", "/utf-8"};
};

struct TestGxxContext
{
    static constexpr std::string_view cxx{"g++"};
    static constexpr sopho::StaticString obj_prefix{" -o "};
    static constexpr sopho::StaticString obj_postfix{".o"};
    static constexpr sopho::StaticString bin_prefix{" -o "};
    static constexpr sopho::StaticString build_prefix{"build/tests/"};
    static constexpr std::array<std::string_view, 2> cxxflags{"-std=c++17", "-Itests"};
};

struct SimpleTestSource
{
    using Dependent = std::tuple<>;
    static constexpr sopho::StaticString source{"tests/simple_test.cpp"};
};

struct BulletTestSource
{
    using Dependent = std::tuple<>;
    static constexpr sopho::StaticString source{"tests/bullet_test.cpp"};
};

struct AllTestsSource
{
    using Dependent = std::tuple<>;
    static constexpr sopho::StaticString source{"tests/all_tests.cpp"};
};

struct AllTests
{
    using Dependent = std::tuple<SimpleTestSource, BulletTestSource, AllTestsSource>;
    static constexpr sopho::StaticString target{"all_tests"};
};

#if defined(_MSC_VER)
    using SobCxxContext = SobClContext;
    using TigrCxxContext = TigrClContext;
    using TestCxxContext = TestClContext;
#elif defined(__GNUC__)
    using SobCxxContext = SobGxxContext;
    using TigrCxxContext = TigrGxxContext;
    using TestCxxContext = TestGxxContext;
#else
#endif

int main()
{
    sopho::CxxToolchain<SobCxxContext>::CxxBuilder<Sob>::build();
    sopho::CxxToolchain<TigrCxxContext>::CxxBuilder<Main>::build();
    sopho::CxxToolchain<TestCxxContext>::CxxBuilder<AllTests>::build();
    return 0;
}
