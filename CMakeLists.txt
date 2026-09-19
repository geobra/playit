cmake_minimum_required(VERSION 3.20)
project(playit VERSION 1.0)


find_package(Qt6 REQUIRED COMPONENTS
    Core
    Quick
    Test
    Gui
    QuickControls2
    Widgets
    Xml
    Multimedia
    DBus
)

#qt_standard_project_setup()

# --- Enable automatic Qt processing ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# --- ECM ---
find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
#include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMFindQmlModule)
include(ECMQmlModule)

#find_package(KF6Kirigami2 REQUIRED) # Kirigami


# --- KF6 ---
find_package(KF6 REQUIRED COMPONENTS
    Kirigami
    I18n
    IconThemes
)

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)

# --- Application target ---
add_executable(playit
    src/MediaManager.h src/MediaManager.cpp)

# Main sources
target_sources(playit PRIVATE
    src/main.cpp
    src/model.cpp
    src/model.h
    src/MediaInfo.h
    src/Media.h
    src/Media.cpp
    src/MediaManager.h
    src/MediaManager.cpp
    src/Settings.cpp
    src/Settings.h
    src/MprisPlayer.cpp
    src/MprisPlayer.h
)

# --- QML module ---
ecm_add_qml_module(playit
    URI org.kde.playit
)

ecm_target_qml_sources(playit
    SOURCES
    src/Main.qml
    #src/FolderPickerDialog.qml
    src/SeekBar.qml
)

# --- Link libraries ---
target_link_libraries(playit PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Xml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::DBus
    KF6::Kirigami
    KF6::I18n
    KF6::IconThemes
)

# --- Installation ---
install(TARGETS playit ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.tutorial.desktop DESTINATION ${KDE_INSTALL_APPDIR})

# --- Feature summary ---
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
