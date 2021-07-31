set_xmakever("2.5.0")
set_project("UplayEmulators")

-- default configurations
add_rules("mode.debug", "mode.release")

set_languages("c++17", "c98")

-- TODO: packages
-- "fmt", "date", "string_theory", "cereal"

set_targetdir("bin/$(arch)")

add_requires("fmt", "minhook")

target("OrbitR2")
    set_kind("shared")
    set_arch("x86")
    set_basename("ubiorbitapi_r2_loader")
    --set_runtimes("MD")
    add_files("orbit_src/**.cpp")
    add_headerfiles("orbit_src/**.hpp")
    add_includedirs("dependency")
    add_packages("fmt", "minhook")
    add_syslinks("User32")
    add_defines("NOMINMAX")
    --if is_os("windows") then
    --    add_syslinks("User32", "DbgHelp", "Advapi32")
    --end

target("UplayR1")
    set_kind("shared")
    set_arch("x86")
    set_basename("uplay_r1_loader")
    --set_runtimes("MD")
    add_files("uplay_src/**.cpp")
    add_headerfiles("uplay_src/**.hpp")
    add_includedirs("dependency")
    add_packages("fmt", "minhook")
    add_syslinks("User32")
    --if is_os("windows") then
    --    add_syslinks("User32", "DbgHelp", "Advapi32")
    --end