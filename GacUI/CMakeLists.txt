if (WIN32)
    file(GLOB GacUILite_SOURCES
            "Lib/Release/Import/Skins/DarkSkin/DarkSkin.cpp"
            "Lib/Release/Import/Skins/DarkSkin/DarkSkin.h"
            "Lib/Release/Import/GacUI.cpp"
            "Lib/Release/Import/GacUI.h"
            "Lib/Release/Import/GacUI.Windows.cpp"
            "Lib/Release/Import/GacUI.Windows.h"
            "Lib/Release/Import/Vlpp.cpp"
            "Lib/Release/Import/Vlpp.h"
            "Lib/Release/Import/Vlpp.Windows.cpp"
            "Lib/Release/Import/VlppGlrParser.cpp"
            "Lib/Release/Import/VlppGlrParser.h"
            "Lib/Release/Import/VlppOS.cpp"
            "Lib/Release/Import/VlppOS.h"
            "Lib/Release/Import/VlppOS.Windows.cpp"
            "Lib/Release/Import/VlppParser.cpp"
            "Lib/Release/Import/VlppParser.h"
            "Lib/Release/Import/VlppReflection.cpp"
            "Lib/Release/Import/VlppReflection.h"
            "Lib/Release/Import/VlppRegex.cpp"
            "Lib/Release/Import/VlppRegex.h"
            "Lib/Release/Import/VlppWorkflowLibrary.cpp"
            "Lib/Release/Import/VlppWorkflowLibrary.h"
    )
endif ()

add_library(GacUILite STATIC WinMain.cpp ${GacUILite_SOURCES})
target_include_directories(GacUILite PUBLIC Lib/Release/Import)
target_compile_definitions(GacUILite PUBLIC VCZH_DEBUG_NO_REFLECTION UNICODE _UNICODE)
target_compile_options(GacUILite PUBLIC /bigobj)

file(GLOB UI_SOURCES "UI/Source/*.cpp")
add_executable(GacUIHelloWorld WIN32 main.cpp ${UI_SOURCES})
target_include_directories(GacUIHelloWorld PRIVATE UI/Source)
target_link_libraries(GacUIHelloWorld GacUILite)
target_compile_definitions(GacUIHelloWorld PRIVATE VCZH_DEBUG_NO_REFLECTION UNICODE _UNICODE)
target_compile_options(GacUIHelloWorld PRIVATE /bigobj)
add_custom_command(TARGET GacUIHelloWorld PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/GacUI/HelloWorld.bin $<TARGET_FILE_DIR:GacUIHelloWorld>/)
