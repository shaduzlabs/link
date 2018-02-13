add_library(AsioStandalone::AsioStandalone IMPORTED INTERFACE)


if(NOT ASIO_STANDALONE_INCLUDE_DIR)
  find_path(ASIO_STANDALONE_INCLUDE_DIR asio.hpp)
endif()

if(ASIO_STANDALONE_INCLUDE_DIR)
  set_property(TARGET AsioStandalone::AsioStandalone APPEND PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES
    ${ASIO_STANDALONE_INCLUDE_DIR}
  )
else()
  set_property(TARGET AsioStandalone::AsioStandalone APPEND PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_LIST_DIR}/../modules/asio-standalone/asio/include
  )
endif()

set_property(TARGET AsioStandalone::AsioStandalone APPEND PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_LIST_DIR}/../modules/asio-standalone/asio/include
)

if (WIN32)
  set_property(TARGET AsioStandalone::AsioStandalone APPEND PROPERTY
    INTERFACE_COMPILE_DEFINITIONS
    _WIN32_WINNT=0x0501
    _SCL_SECURE_NO_WARNINGS
  )
endif()
