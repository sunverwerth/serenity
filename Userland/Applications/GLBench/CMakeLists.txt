serenity_component(
    GLBench
    RECOMMENDED
    TARGETS GLBench
)

set(SOURCES
    main.cpp
)

serenity_app(GLBench ICON app-3d-file-viewer)
target_link_libraries(GLBench LibGUI LibGL LibMain)
