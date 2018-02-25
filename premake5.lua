-- use one of these commands to generate solution files
--      premake5 vs2017
--      premake5 gmake2

local binaries_folder = "Binaries/"
local source_folder = "Source/"
local ide_project_folder = "Temporary/"
local binary_target_dir = (binaries_folder .. "%{cfg.platform}")

function LinkOptionsForPDBandIlkCleanup(projectName) 
    return {
        ("-PDB:" .. projectName .. ".pdb"), -- This is necessary to move the .pdb file away from .exe output folder...
        ("-IMPLIB:" .. projectName .. ".ilk") -- This command somehow makes .exp and .ilk stay in the temporary folder... A duplicate still appears with the .exe though.
    }
end


defines { }
workspace "SectraAssignmentWorkspace"
    location(ide_project_folder)
    language "C++"
    cppdialect "C++17" -- flag needed for gcc/clang, visual studio 2017 does not need it
                       -- C++14 confirmed to work with both Mingw32 / gcc and Visual Studio

    configurations { "Debug", "Release" }
    platforms { "Win64" }

    debugdir(binary_target_dir)
    
    filter { "platforms:Win64" }
        defines { "WINDOWS" }
        system "Windows"
        architecture "x64"
    
    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"
    
    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"

    filter{}



-- EXECUTABLES
local projectName = "ImageStorageEstimator"
project(projectName)
    kind "ConsoleApp"
    location(ide_project_folder)
    targetname(projectName)
    targetdir(binary_target_dir)

    linkoptions (LinkOptionsForPDBandIlkCleanup(projectName))

    files { source_folder .. "**.h", source_folder .. "**.cpp" }