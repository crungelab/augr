include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Fulkan.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Shell.cmake)

function(USES_EXAMPLES THIS)
    USES_AUGR(${THIS})
    USES_SHELL(${THIS})

    target_include_directories(${THIS} PRIVATE
        ${AUGR_ROOT}
    )

    target_link_libraries(${THIS} PRIVATE FlkExamples)

    target_link_directories(${THIS} PRIVATE ${VULKAN_LIB})
    
    message(VULKAN_LIB)
    message(${VULKAN_LIB})

    set_target_properties(
        ${THIS} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${AUGR_ROOT}/examples/${THIS}"
    )

    set_target_properties(
        ${THIS} PROPERTIES
        FOLDER "Examples"
    )

    if(${AUGR_PLATFORM_WINDOWS})
    add_custom_command(TARGET ${THIS} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${ENGINE_OUT}/flutter_engine.dll"
                "${ENGINE_OUT}/flutter_engine.dll.pdb"
                $<TARGET_FILE_DIR:${THIS}>)

    add_custom_command(TARGET ${THIS} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${THIS}> $<TARGET_FILE_DIR:${THIS}>
        COMMAND_EXPAND_LISTS
    )
    endif()
        
endfunction()
