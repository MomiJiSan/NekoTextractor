function(add_textractor_extension target output_name)
	add_library(${target} MODULE ${ARGN})
	set_target_properties(${target} PROPERTIES OUTPUT_NAME "${output_name}")
	target_precompile_headers(${target} REUSE_FROM pch)
endfunction()

add_textractor_extension(Bing_Translate "Bing Translate" bingtranslate.cpp translatewrapper.cpp network.cpp extensionimpl.cpp)
add_textractor_extension(Copy_to_Clipboard "Copy to Clipboard" copyclipboard.cpp extensionimpl.cpp)
add_textractor_extension(DeepL_Translate "DeepL Translate" deepltranslate.cpp translatewrapper.cpp network.cpp extensionimpl.cpp)
add_textractor_extension(DevTools_DeepL_Translate "DevTools DeepL Translate" devtoolsdeepltranslate.cpp devtools.cpp translatewrapper.cpp network.cpp extensionimpl.cpp)
add_textractor_extension(DevTools_Papago_Translate "DevTools Papago Translate" devtoolspapagotranslate.cpp devtools.cpp translatewrapper.cpp network.cpp extensionimpl.cpp)
add_textractor_extension(DevTools_Systran_Translate "DevTools Systran Translate" devtoolssystrantranslate.cpp devtools.cpp translatewrapper.cpp network.cpp extensionimpl.cpp)
add_textractor_extension(Extra_Newlines "Extra Newlines" extranewlines.cpp extensionimpl.cpp)
add_textractor_extension(Extra_Window "Extra Window" extrawindow.cpp extensionimpl.cpp)
add_textractor_extension(Google_Translate "Google Translate" googletranslate.cpp translatewrapper.cpp network.cpp extensionimpl.cpp)
add_textractor_extension(Lua Lua lua.cpp extensionimpl.cpp)
add_textractor_extension(Regex_Filter "Regex Filter" regexfilter.cpp extensionimpl.cpp)
add_textractor_extension(Regex_Replacer "Regex Replacer" regexreplacer.cpp extensionimpl.cpp)
add_textractor_extension(Remove_Repeated_Characters "Remove Repeated Characters" removerepeatchar.cpp extensionimpl.cpp)
add_textractor_extension(Remove_Repeated_Phrases "Remove Repeated Phrases" removerepeatphrase.cpp extensionimpl.cpp)
add_textractor_extension(Remove_Repeated_Phrases_2 "Remove Repeated Phrases 2" removerepeatphrase2.cpp extensionimpl.cpp)
add_textractor_extension(Remove_30_Repeated_Sentences "Remove 30 Repeated Sentences" removerepeatsentence.cpp extensionimpl.cpp)
add_textractor_extension(Replacer Replacer replacer.cpp extensionimpl.cpp)
add_textractor_extension(Styler Styler styler.cpp extensionimpl.cpp)
add_textractor_extension(Thread_Linker "Thread Linker" threadlinker.cpp extensionimpl.cpp)

target_link_libraries(Bing_Translate winhttp Qt5::Widgets)
target_link_libraries(DeepL_Translate winhttp Qt5::Widgets)
target_link_libraries(DevTools_DeepL_Translate shell32 winhttp Qt5::Widgets Qt5::WebSockets)
target_link_libraries(DevTools_Papago_Translate shell32 winhttp Qt5::Widgets Qt5::WebSockets)
target_link_libraries(DevTools_Systran_Translate shell32 winhttp Qt5::Widgets Qt5::WebSockets)
target_link_libraries(Extra_Window Qt5::Widgets)
target_link_libraries(Google_Translate winhttp Qt5::Widgets)
target_link_libraries(Lua lua53 Qt5::Widgets)
target_link_libraries(Regex_Filter Qt5::Widgets)
target_link_libraries(Styler Qt5::Widgets)
target_link_libraries(Thread_Linker Qt5::Widgets)

add_custom_target(Cleaner ALL COMMAND del *.xdll WORKING_DIRECTORY ${CMAKE_FINAL_OUTPUT_DIRECTORY})

if (NOT EXISTS ${CMAKE_FINAL_OUTPUT_DIRECTORY}/Qt5WebSockets.dll AND NOT EXISTS ${CMAKE_FINAL_OUTPUT_DIRECTORY}/Qt5WebSocketsd.dll)
	add_custom_command(TARGET DevTools_DeepL_Translate
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
		COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
		COMMAND Qt5::windeployqt --dir ${CMAKE_FINAL_OUTPUT_DIRECTORY} "${CMAKE_FINAL_OUTPUT_DIRECTORY}/DevTools DeepL Translate.dll"
	)
endif()
