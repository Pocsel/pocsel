macro(AddResources src resources)

    set(headers "")
    foreach (res ${resources})
        
        string(REPLACE "${CMAKE_SOURCE_DIR}/" "" res ${res})
        list(APPEND headers "${CMAKE_CURRENT_BINARY_DIR}/${res}.hpp")


        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${res}.hpp" "\n")
        ADD_CUSTOM_COMMAND(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${res}.hpp"
            COMMAND respacker 
            ARGS "${CMAKE_SOURCE_DIR}/${res}" "${CMAKE_CURRENT_BINARY_DIR}/${res}.hpp"
            MAIN_DEPENDENCY ${res})

    endforeach()

    CreateGroups(${resources})
    add_custom_target(resources ALL DEPENDS ${headers})

endmacro()