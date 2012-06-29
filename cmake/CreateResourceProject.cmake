macro(CreateResourceProject name src resources)

    set(headers "")
    foreach (fullres ${resources})

        string(REPLACE "${CMAKE_SOURCE_DIR}/" "" res ${fullres})
        list(APPEND headers "${CMAKE_CURRENT_BINARY_DIR}/${res}.hpp")

        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${res}.hpp" "\n")
        file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/${res}.hpp")
        
        ADD_CUSTOM_COMMAND(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${res}.hpp"
            COMMAND respacker 
            ARGS "${CMAKE_SOURCE_DIR}/${res}" "${CMAKE_CURRENT_BINARY_DIR}/${res}.hpp"
            MAIN_DEPENDENCY ${fullres})

    endforeach()

    CreateGroups("${resources}")
    add_custom_target(${name} ALL DEPENDS ${headers})

endmacro()