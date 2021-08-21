set(SOURCES
    DepthBuffer.cpp
)

serenity_lib(LibSoftGL softgl)
target_link_libraries(LibGL LibM LibCore LibGfx)
