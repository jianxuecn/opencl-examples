cmake_minimum_required(VERSION 2.8)
if(COMMAND CMAKE_POLICY)
  # Works around warnings libraries linked against that don't
  # have absolute paths (e.g. -lOgreMain)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0020 NEW)
  cmake_policy(SET CMP0048 NEW)
  cmake_policy(SET CMP0053 NEW)
endif(COMMAND CMAKE_POLICY)

project(ocl_example VERSION 1.0.0)

set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/dist" CACHE PATH "IUltra install prefix" FORCE)
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(PrepareQt)
set(OCL_EXAMPLE_USE_QT5 ${USE_QT5})

if(USE_QT5)
  PREPARE_QT5(Core Gui Widgets)
else(USE_QT5)
  set(QT_USE_QTXML 1)
  set(QT_USE_QTOPENGL 1)
  PREPARE_QT4(4.8.0 Core Gui OpenGL Xml Sql)
endif(USE_QT5)

find_package(OpenCL REQUIRED)

set(OCL_EXAMPLE_MOC_FILES
  include/ImageProcessingDialog.h
  include/ToolWindow.h
  include/ImageViewWidget.h
  include/LogWidget.h
)

set(OCL_EXAMPLE_HEADER_FILES
  ${OCL_EXAMPLE_MOC_FILES}
  include/Timer.h
  include/OCLUtils.h
  include/Logger.h
  include/FileLogger.h
  include/ExampleLogger.h
  include/LogManager.h
  include/LogUtils.h
)

set(OCL_EXAMPLE_SOURCE_FILES
  src/Timer.cpp
  src/ImageProcessingDialog.cpp
  src/ToolWindow.cpp
  src/ImageViewWidget.cpp
  src/LogWidget.cpp
  src/OCLUtils.cpp
  src/Logger.cpp
  src/FileLogger.cpp
  src/ExampleLogger.cpp
  src/LogManager.cpp
  src/main.cpp
)

set(OCL_EXAMPLE_UI_FILES
  ui/ImageProcessingDialog.ui
  ui/ImageViewWidget.ui
  ui/LogWidget.ui
)

set(TARGETNAME OCLExample)

QT_WRAP_CPP_NEW(OCL_EXAMPLE_MOC_SRCS ${OCL_EXAMPLE_MOC_FILES})
QT_WRAP_UI_NEW(OCL_EXAMPLE_UI_SRCS ${OCL_EXAMPLE_UI_FILES})

#SOURCE_GROUP("Header Files" FILES ${VISGMD_HEADER_FILES})
source_group("Moc Files" FILES ${OCL_EXAMPLE_MOC_SRCS})
source_group("Ui Files" FILES ${OCL_EXAMPLE_UI_FILES})

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${OpenCL_INCLUDE_DIRS}
)

set(OCL_EXAMPLE_SRCS ${OCL_EXAMPLE_HEADER_FILES} ${OCL_EXAMPLE_SOURCE_FILES} ${OCL_EXAMPLE_MOC_SRCS} ${OCL_EXAMPLE_UI_FILES})
set(OCL_PROGRAM_FILE ${PROJECT_SOURCE_DIR}/filters/image_processing.cl)

if(WIN32)
  include(InstallDependencies)
  install_qt_dll(${QT_VER} ${QT_USED_DLL_DIR} ${QT_USED_DLL_NAMES})
  #install icu dlls required by Qt5
  if(USE_QT5)
    install_qt_plugins(${QT_VER} ${QT_USED_DLL_DIR}/../plugins platforms minimal offscreen windows)
    install_qt_plugins(${QT_VER} ${QT_USED_DLL_DIR}/../plugins styles windowsvistastyle)
    install_qt_plugins(${QT_VER} ${QT_USED_DLL_DIR}/../plugins imageformats gif ico jpeg svg tga tiff wbmp webp)
    install_qt_plugins(${QT_VER} ${QT_USED_DLL_DIR}/../plugins iconengines svgicon)
    #install_icu_dll(${QT_USED_DLL_DIR} icudt53 icuin53 icuuc53)
  endif(USE_QT5)
  add_executable(${TARGETNAME} WIN32 ${OCL_EXAMPLE_SRCS})
elseif(APPLE)
  # Set the OS X Bundle specific CMake variables which will be used to populate the plist for
  # the application bundle
  set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}")
  #set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.rogue-research.SimpleCocoaVTK")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} Version ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
  set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
  set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2014. All Rights Reserved.")

  # These variables are specific to our plist and are NOT standard CMake variables
  #set(MACOSX_BUNDLE_NSMAIN_NIB_FILE "MainMenu")
  #set(MACOSX_BUNDLE_NSPRINCIPAL_CLASS "NSApplication")
  #set(MACOSX_BUNDLE_ICON_FILE OCLExample.icns)
  #set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/res/OCLExample.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  #set(OCLEXAMPLE_RES ${CMAKE_CURRENT_SOURCE_DIR}/res/OCLExample.icns)
  set_source_files_properties(${OCL_PROGRAM_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/filters)

  add_executable(${TARGETNAME} MACOSX_BUNDLE ${OCL_EXAMPLE_SRCS} ${OCL_PROGRAM_FILE})

  # Set a custom plist file for the app bundle
  set_target_properties(${TARGETNAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/Info.plist.in)

else(WIN32)
  add_executable(${TARGETNAME} ${OCL_EXAMPLE_SRCS})
endif(WIN32)

set_target_properties(${TARGETNAME} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
  LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
  DEBUG_POSTFIX d
)
target_link_libraries(${TARGETNAME} ${OpenCL_LIBRARIES} ${QT_USED_LIBRARIES})

set(plugin_dest_dir plugins)
set(APPS "\${CMAKE_INSTALL_PREFIX}/${TARGETNAME}")
if(APPLE)
  set(plugin_dest_dir ${TARGETNAME}.app/Contents/PlugIns)
  #set(qtconf_dest_dir QtTest.app/Contents/Resources)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/${TARGETNAME}.app")
endif(APPLE)
if(WIN32)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/${TARGETNAME}.exe")
endif(WIN32)
  
install(TARGETS ${TARGETNAME}
  BUNDLE DESTINATION . COMPONENT Runtime 
  RUNTIME DESTINATION . CONFIGURATIONS Release None ""
)

if(APPLE)
  #install(FILES ${OCL_PROGRAM_FILE} DESTINATION . COMPONENT Runtime)
else(APPLE)
  install(FILES ${OCL_PROGRAM_FILE} DESTINATION filters CONFIGURATIONS Release None "")
endif(APPLE)

if(WIN32)
  install(TARGETS ${TARGETNAME} RUNTIME DESTINATION Debug CONFIGURATIONS Debug)
  install(TARGETS ${TARGETNAME} RUNTIME DESTINATION RelWithDebInfo CONFIGURATIONS RelWithDebInfo)
  install(TARGETS ${TARGETNAME} RUNTIME DESTINATION MinSizeRel CONFIGURATIONS MinSizeRel)
endif(WIN32)

# deal with dependencies
if(APPLE)
  install(DIRECTORY "${QT_PLUGINS_DIR}/platforms" DESTINATION ${plugin_dest_dir} COMPONENT Runtime)
  install(DIRECTORY "${QT_PLUGINS_DIR}/styles" DESTINATION ${plugin_dest_dir} COMPONENT Runtime)
  install(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir} COMPONENT Runtime)
  #install(FILES "${QT_PROPERTYBROWSER_LIB_REL}" DESTINATION ${plugin_dest_dir} COMPONENT Runtime)
  set(DEP_DIRS ${QT_LIBRARY_DIRS})
  install(CODE "
      file(GLOB_RECURSE QTPLUGINS
        \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
      include(BundleUtilities)
      fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DEP_DIRS}\")
      " COMPONENT Runtime
)
endif(APPLE)

if(APPLE)
else(APPLE)
  add_custom_command(TARGET ${TARGETNAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${TARGETNAME}>/filters)
  add_custom_command(TARGET ${TARGETNAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OCL_PROGRAM_FILE} $<TARGET_FILE_DIR:${TARGETNAME}>/filters)
endif(APPLE)
# setup CPack
set(CPACK_INSTALL_CMAKE_PROJECTS "${PROJECT_BINARY_DIR}" "${TARGETNAME}" "ALL" "/")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${TARGETNAME} - A visualization software for animation of the block system.")
set(CPACK_PACKAGE_VENDOR "UCAS")
set(CPACK_PACKAGE_VERSION_MAJOR "${OCL_EXAMPLE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${OCL_EXAMPLE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${OCL_EXAMPLE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${TARGETNAME} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_EXECUTABLES "${TARGETNAME}" "${TARGETNAME}")
if(WIN32)
  #string(REGEX REPLACE "/" "\\\\\\\\" CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/Release/VTKInstall.bmp")
  #set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\IUltra.exe")
  #set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}\\\\res\\\\IUltraServer.ico")
  #set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}\\\\res\\\\IUltraServer.ico")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  #set(CPACK_NSIS_HELP_LINK "http://www.viskit.org")
  #set(CPACK_NSIS_URL_INFO_ABOUT "http://www.viskit.org")
  #set(CPACK_NSIS_CONTACT "viskit@viskit.org")
  if(CMAKE_CL_64)
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} (Win64)")
  else(CMAKE_CL_64)
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  endif(CMAKE_CL_64)
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
elseif(APPLE)
  set(CPACK_BINARY_DRAGNDROP ON)
  set(CPACK_GENERATOR "DragNDrop")
  #set(CPACK_PACKAGE_ICON "${CMAKE_INSTALL_PREFIX}/${TARGETNAME}.app/Contents/Resources/VisKBT.icns")
  #set(CPACK_BUNDLE_NAME "VisKBT")
  #set(CPACK_BUNDLE_PLIST "${CMAKE_INSTALL_PREFIX}/${TARGETNAME}.app/Contents/Info.plist")
  #set(CPACK_BUNDLE_ICON "${CMAKE_INSTALL_PREFIX}/${TARGETNAME}.app/Contents/Resources/VisKBT.icns")
else(WIN32)
  set(CPACK_GENERATOR "RPM")
endif(WIN32)

include(CPack)
