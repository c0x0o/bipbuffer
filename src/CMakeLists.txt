set(bipbuffer_source_dir ${PROJECT_SOURCE_DIR}/src)
set(bipbuffer_source ${bipbuffer_source_dir}/bipbuffer.c)

include_directories(${bipbuffer_source_dir})

add_library(bipbuffer STATIC ${bipbuffer_source})
