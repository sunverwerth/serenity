set(SOURCES
    Buffer.cpp
    Device.cpp
    ShaderVM.cpp
    Sampler.cpp
)

serenity_lib(LibSoftGPU softgpu)
target_link_libraries(LibSoftGPU LibCore)
