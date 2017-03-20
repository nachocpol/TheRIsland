--premake4.lua
--GraphicsDemoSolution Project Generator

--Solution
solution "GraphicsDemoSolution"
	configurations {"Debug", "Release"}

--GraphicsDemo
project "GraphicsDemo"
	kind "ConsoleApp"
	language "C++"
	location "vsproject"
	targetdir "build"
	libdirs "../SeriousEngineWork/lib"
	includedirs {   "include",
					"../SeriousEngineWork/include",
					"depen/GLFW/include", 
					"depen/GLM", 
					"depen/GLEW/include",
					"depen/STB/include",
					"depen/IMGUI",
					"depen/STB/src",
					"depen/LUA/src",
					"depen/LUABRIDGE/",
					"depen/OPENAL/include",
					"depen/STB/include",
					"depen/STB/src",
					"depen/SIMPLEXNOISE"}

	files {	"include/Demo/**.h", 
           	"src/Demo/**.cc",
           	"depen/SIMPLEXNOISE/**.cc"}

    links {	"SeriousEngine",
				"opengl32",
				"OpenAL32",
				"GLEW",
				"GLFW"}

  	configuration "Debug"
	    defines "_ITERATOR_DEBUG_LEVEL"
	    flags "Symbols"

  	configuration "Release"
	    defines "_ITERATOR_DEBUG_LEVEL"
	    flags "Optimize"


