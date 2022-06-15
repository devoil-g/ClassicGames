CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)
PROJECT(ClassicGames)

# Set game sources
SET(GAME_DOOM_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Automap.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Automap.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Camera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Camera.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Doom.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Doom.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Statusbar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Statusbar.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Wad.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Wad.hpp
)
SET(GAME_DOOM_FLAT_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Flat/AbstractFlat.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Flat/AbstractFlat.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Flat/AnimatedFlat.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Flat/AnimatedFlat.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Flat/StaticFlat.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Flat/StaticFlat.hpp
)
SET(GAME_DOOM_LINEDEF_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/AbstractLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/AbstractLinedef.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/AbstractTriggerableLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/AbstractTriggerableLinedef.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/NullLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/NullLinedef.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/ScrollerLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/ScrollerLinedef.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/ActionTriggerableLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/ActionTriggerableLinedef.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/DonutTriggerableLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/DonutTriggerableLinedef.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/ExitLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/ExitLinedef.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/LightTriggerableLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/LightTriggerableLinedef.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/StairTriggerableLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/StairTriggerableLinedef.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/StopTriggerableLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/StopTriggerableLinedef.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/TeleporterTriggerableLinedef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Linedef/TeleporterTriggerableLinedef.hpp
)
SET(GAME_DOOM_ACTION_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/AbstractAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/AbstractAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/AbstractStoppableAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/AbstractStoppableAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/AbstractTypeAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/AbstractTypeAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/AbstractLevelingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/AbstractLevelingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/BlinkLightingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/BlinkLightingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/CeilingLevelingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/CeilingLevelingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/CrusherLevelingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/CrusherLevelingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/DoorLevelingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/DoorLevelingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/FlickerLightingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/FlickerLightingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/FloorLevelingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/FloorLevelingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/LiftLevelingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/LiftLevelingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/OpenLevelingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/OpenLevelingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/OscillateLightingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/OscillateLightingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/PlatformLevelingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/PlatformLevelingAction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/RandomLightingAction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Action/RandomLightingAction.hpp
)
SET(GAME_DOOM_THING_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Thing/AbstractThing.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Thing/AbstractThing.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Thing/PlayerThing.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Thing/PlayerThing.hpp
)
SET(GAME_DOOM_STATES_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/DoomScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/DoomScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/GameDoomScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/GameDoomScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/IntermissionDoomScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/IntermissionDoomScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/MenuDoomScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/MenuDoomScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/SplashDoomScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/SplashDoomScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/StartDoomScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/StartDoomScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/TextDoomScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/TextDoomScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/TransitionDoomScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Doom/Scenes/TransitionDoomScene.hpp
)
SET(GAME_GBC_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/CentralProcessingUnit.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/CentralProcessingUnit.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/EmulationScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/EmulationScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/GameBoyColor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/GameBoyColor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/MemoryBankController.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/MemoryBankController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/MemoryBankController1.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/MemoryBankController1.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/MemoryBankController2.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/MemoryBankController2.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/MemoryBankController3.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/MemoryBankController3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/MemoryBankController5.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/MemoryBankController5.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/PixelProcessingUnit.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/PixelProcessingUnit.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/SelectionScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/GameBoyColor/SelectionScene.cpp
)
SET(GAME_MATH_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Math/Math.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Math/Math.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Math/Vector.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Math/Vector.hpp
)
SET(GAME_STATES_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/SceneMachine.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/SceneMachine.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/AbstractScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/AbstractScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/LoadingScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/LoadingScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/MessageScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/MessageScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/MidiScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/MidiScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/SplashScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/SplashScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/StartScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/StartScene.hpp
)
SET(GAME_STATES_MENU_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/Menu/AbstractMenuScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/Menu/AbstractMenuScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/Menu/MainMenuScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/Menu/MainMenuScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/Menu/OptionsMenuScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/Scenes/Menu/OptionsMenuScene.hpp
)
SET(GAME_SYSTEM_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Config.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Config.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Utilities.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Utilities.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Window.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Window.hpp
)
SET(GAME_SYSTEM_AUDIO_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Audio/Midi.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Audio/Midi.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Audio/Sound.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Audio/Sound.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Audio/Soundfont.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Audio/Soundfont.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Audio/Synthesizer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Audio/Synthesizer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Audio/Pitch.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Audio/Pitch.hpp
)
SET(GAME_SYSTEM_LIBRARY_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Library/AbstractLibrary.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Library/AbstractLibrary.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Library/FontLibrary.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Library/FontLibrary.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Library/TextureLibrary.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/System/Library/TextureLibrary.hpp
)
SET(GAME_SRCS
	${GAME_DOOM_SRCS}
	${GAME_DOOM_FLAT_SRCS}
	${GAME_DOOM_LINEDEF_SRCS}
	${GAME_DOOM_ACTION_SRCS}
	${GAME_DOOM_THING_SRCS}
	${GAME_DOOM_STATES_SRCS}
	${GAME_GBC_SRCS}
	${GAME_MATH_SRCS}
	${GAME_STATES_SRCS}
	${GAME_STATES_MENU_SRCS}
	${GAME_SYSTEM_SRCS}
	${GAME_SYSTEM_AUDIO_SRCS}
	${GAME_SYSTEM_LIBRARY_SRCS}
	${CMAKE_CURRENT_SOURCE_DIR}/sources/main.cpp
)

# Set game resources
SET(GAME_RSRC
	${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/Icons.rc
	${CMAKE_CURRENT_SOURCE_DIR}/assets/levels/dmsp1666.txt
	${CMAKE_CURRENT_SOURCE_DIR}/assets/levels/MUSform.txt
)

# Set game source groups
SOURCE_GROUP("\\" FILES ${CMAKE_CURRENT_SOURCE_DIR}/sources/main.cpp)
SOURCE_GROUP("\\Doom\\" FILES ${GAME_DOOM_SRCS})
SOURCE_GROUP("\\Doom\\Flat\\" FILES ${GAME_DOOM_FLAT_SRCS})
SOURCE_GROUP("\\Doom\\Linedef\\" FILES ${GAME_DOOM_LINEDEF_SRCS})
SOURCE_GROUP("\\Doom\\Action\\" FILES ${GAME_DOOM_ACTION_SRCS})
SOURCE_GROUP("\\Doom\\Thing\\" FILES ${GAME_DOOM_THING_SRCS})
SOURCE_GROUP("\\Doom\\Scenes\\" FILES ${GAME_DOOM_STATES_SRCS})
SOURCE_GROUP("\\GameBoyColor\\" FILES ${GAME_GBC_SRCS})
SOURCE_GROUP("\\Math\\" FILES ${GAME_MATH_SRCS})
SOURCE_GROUP("\\Scenes\\" FILES ${GAME_STATES_SRCS})
SOURCE_GROUP("\\Scenes\\Menu\\" FILES ${GAME_STATES_MENU_SRCS})
SOURCE_GROUP("\\System\\" FILES ${GAME_SYSTEM_SRCS})
SOURCE_GROUP("\\System\\Audio\\" FILES ${GAME_SYSTEM_AUDIO_SRCS})
SOURCE_GROUP("\\System\\Library\\" FILES ${GAME_SYSTEM_LIBRARY_SRCS})
SOURCE_GROUP("\\Resources\\" FILES ${GAME_RSRC})

# Import libraries
FIND_PACKAGE(SFML 2.5 COMPONENTS system window network graphics audio REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

# Set includes directories
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/sources/)

# Generate executables
ADD_EXECUTABLE(Game ${GAME_SRCS} ${GAME_RSRC})
TARGET_LINK_LIBRARIES(Game Threads::Threads sfml-system sfml-window sfml-network sfml-graphics sfml-audio)

# Force C++20
SET_PROPERTY(TARGET Game PROPERTY CXX_STANDARD 20)

# Copy assets to build directory
ADD_CUSTOM_COMMAND(TARGET Game POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:Game>/assets/)
