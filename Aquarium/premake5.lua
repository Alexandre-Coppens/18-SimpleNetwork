-- Visual Studio 2022 x64 :
--   vendor\premake\premake5.exe vs2022
-- puis ouvrir vs2022\tps.sln

local function vendorRoot()
  if os.isdir("../Dependencies/raylib") then
    return path.getabsolute("../Dependencies")
  end
  error("Dossier dependencies introuvable.")
end

local vendor = vendorRoot()
local sdl2DLL = vendor .. "/SDL2/bin/SDL2.dll"
local sdl2imageDLL = vendor .. "/SDL2/bin/SDL2_image.dll"
local sdl2netDLL = vendor .. "/SDL2/bin/SDL2_net.dll"
local sdl2ttfDLL = vendor .. "/SDL2/bin/SDL2_ttf.dll"

workspace "Aquarium"
  architecture "x64"
  configurations { "Debug", "Release" }
  location "build"
  cppdialect "C++17"
  multiprocessorcompile "On"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
  filter "configurations:Release"
    runtime "Release"
    optimize "On"
  filter "system:windows"
    defines { "NOMINMAX", "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS" }
    systemversion "latest"
  filter {}

local function cppProject(name, dir)
  project(name)
    kind "ConsoleApp"
    language "C++"
    targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
    objdir("%{wks.location}/obj/" .. name)
    files { dir .. "/src/**.cpp", dir .. "/src/**.hpp" }
    includedirs { dir .. "/src" }
    debugdir(dir)
end

local function addDependencies(dir)
  includedirs { 
    vendor .. "/raylib/source",
    vendor .. "/raylib/external",
    vendor .. "/SDL2/SDL2-2.0.20/include",
    vendor .. "/SDL2/SDL2_ttf-2.0.18/include",
    vendor .. "/SDL2/SDL2_net-2.0.1/include",
    vendor .. "/SDL2/SDL2_image-2.0.5/include"
  }
  libdirs { 
    vendor .. "/raylib/build/bin/",
    vendor .. "/SDL2/SDL2-2.0.20/lib/x64",
    vendor .. "/SDL2/SDL2_ttf-2.0.18/lib/x64",
    vendor .. "/SDL2/SDL2_net-2.0.1/lib/x64",
    vendor .. "/SDL2/SDL2_image-2.0.5/lib/x64"
  }
  links { 
    "SDL2_image", "SDL2_net", "SDL2_ttf", "SDL2", "SDL2main", "SDL2test",
    "raylib", "opengl32", "kernel32", "user32", "gdi32", "winmm", "winspool", "comdlg32", "advapi32", "shell32", "ole32", "oleaut32", "uuid", "odbc32", "odbccp32"
  }

  filter "system:windows"
    local cmds = { '{COPYFILE} "' .. sdl2DLL .. '" "%{cfg.targetdir}"' }
    table.insert(cmds, '{COPYFILE} "' .. sdl2imageDLL .. '" "%{cfg.targetdir}"')
    table.insert(cmds, '{COPYFILE} "' .. sdl2netDLL .. '" "%{cfg.targetdir}"')
    table.insert(cmds, '{COPYFILE} "' .. sdl2ttfDLL .. '" "%{cfg.targetdir}"')

    postbuildcommands(cmds)
  filter {}
end

local function add(name, dir)
  if not os.isdir(dir) then
    return
  end

  cppProject(name, dir)
  addDependencies(dir)
end

add("AquariumClient", "CLIENT")
add("AquariumServer", "SERVER")
