set(buzzsaw_source
    source/utility/utility.h
    source/utility/logger.h
    source/utility/logger.cpp
    source/utility/recycle.h

    source/math/const.h

    source/audio/pitch.h
    source/audio/generator.h
    source/audio/envelope.h
    source/audio/samples.h

    source/buzzsawController.h
    source/buzzsawController.cpp
    source/buzzsawProcessor.h
    source/buzzsawProcessor.cpp
    source/buzzsawFactory.cpp

    source/subprocessors/engineSubprocessor.h
    source/subprocessors/engineSubprocessor.cpp
)

set(target buzzsaw)

smtg_add_vst3plugin(${target} ${buzzsaw_source})
set_target_properties(${target} PROPERTIES ${SDK_IDE_MYPLUGINS_FOLDER})
target_link_libraries(${target} PRIVATE base sdk)

if(SMTG_MAC)
    smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/buzzsaw.plist" PREPROCESS)
elseif(SMTG_WIN)
    target_sources(${target} PRIVATE resource/buzzsaw.rc)
endif()
