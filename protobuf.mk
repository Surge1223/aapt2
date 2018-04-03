LOCAL_PATH:= $(call my-dir)

FIND_HOSTOS := $(shell uname -s)
HOST_NAME := $(shell echo $(FIND_HOSTOS) |sed -e s/L/l/ |sed -e s/D/d/ |sed s/W/w/ )


common_includes := \
        $(LOCAL_PATH)/include \
        system/core/base/include \
        system/core/libutils \
        system/core/liblog \
        system/core/libcutils \
        external/libpng \
        external/expat \
        external/zlib \
        external/libcxx/include \
        system/core/libziparchive \
        frameworks/base/libs/androidfw \
        frameworks/base/include/androidfw \
        $(DIR)/android \
        $(DIR)/src \
        $(DIR)


common_cxxflags := \
       -std=gnu++1y \
       -Wno-missing-field-initializers \
       -fno-exceptions \
       -fno-rtti \
       -Wno-missing-field-initializers

common_staticlibs := \
       libpng \
       liblog \
       libexpat_static \
       libutils \
       libcutils \
       libziparchive \
       libbase \
       libm \
       libc \
       libz \
       libc++_static \
       libprotobuf-cpp-lite_arm



include $(CLEAR_VARS)

LOCAL_MODULE := libprotobuf-cpp-lite_arm
IGNORED_WARNINGS := -Wno-sign-compare -Wno-unused-parameter -Wno-sign-promo -Wno-error=return-type
LOCAL_CPP_EXTENSION := .cc
DIR := external/protobuf

CC_LITE_SRC_FILES := \
	$(DIR)/src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc \
	$(DIR)/src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc \
	$(DIR)/src/google/protobuf/stubs/common.cc \
	$(DIR)/src/google/protobuf/stubs/once.cc \
	$(DIR)/src/google/protobuf/stubs/stringprintf.cc \
	$(DIR)/src/google/protobuf/extension_set.cc \
	$(DIR)/src/google/protobuf/generated_message_util.cc \
	$(DIR)/src/google/protobuf/message_lite.cc \
	$(DIR)/src/google/protobuf/repeated_field.cc \
	$(DIR)/src/google/protobuf/wire_format_lite.cc \
	$(DIR)/src/google/protobuf/io/coded_stream.cc \
	$(DIR)/src/google/protobuf/io/zero_copy_stream.cc \
	$(DIR)/src/google/protobuf/io/zero_copy_stream_impl_lite.cc


LOCAL_C_INCLUDES := $(common_includes) 
LOCAL_CPPFLAGS := $(common_cxxflags)    

LOCAL_CFLAGS := -DGOOGLE_PROTOBUF_NO_RTTI $(IGNORED_WARNINGS)
LOCAL_SRC_FILES := $(CC_LITE_SRC_FILES)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(common_includes)
    
include $(BUILD_STATIC_LIBRARY)


# Android Protocol buffer compiler, protoc (target executable)
# used by the build systems as $(PROTOC) defined in
# build/core/config.mk
#
# =======================================================

include $(CLEAR_VARS)

LOCAL_MODULE := protoc
LOCAL_MODULE_TAGS := optional
LOCAL_CXX_STL := libc++_static
LOCAL_CPP_EXTENSION := .cc
DIR := external/protobuf
IGNORED_WARNINGS := -Wno-sign-compare -Wno-unused-parameter -Wno-sign-promo -Wno-error=return-type

COMPILER_SRC_FILES :=  \
    $(DIR)/src/google/protobuf/descriptor.cc \
    $(DIR)/src/google/protobuf/descriptor.pb.cc \
    $(DIR)/src/google/protobuf/descriptor_database.cc \
    $(DIR)/src/google/protobuf/dynamic_message.cc \
    $(DIR)/src/google/protobuf/extension_set.cc \
    $(DIR)/src/google/protobuf/extension_set_heavy.cc \
    $(DIR)/src/google/protobuf/generated_message_reflection.cc \
    $(DIR)/src/google/protobuf/generated_message_util.cc \
    $(DIR)/src/google/protobuf/message.cc \
    $(DIR)/src/google/protobuf/message_lite.cc \
    $(DIR)/src/google/protobuf/reflection_ops.cc \
    $(DIR)/src/google/protobuf/repeated_field.cc \
    $(DIR)/src/google/protobuf/service.cc \
    $(DIR)/src/google/protobuf/text_format.cc \
    $(DIR)/src/google/protobuf/unknown_field_set.cc \
    $(DIR)/src/google/protobuf/wire_format.cc \
    $(DIR)/src/google/protobuf/wire_format_lite.cc \
    $(DIR)/src/google/protobuf/compiler/code_generator.cc \
    $(DIR)/src/google/protobuf/compiler/command_line_interface.cc \
    $(DIR)/src/google/protobuf/compiler/importer.cc \
    $(DIR)/src/google/protobuf/compiler/main.cc \
    $(DIR)/src/google/protobuf/compiler/parser.cc \
    $(DIR)/src/google/protobuf/compiler/plugin.cc \
    $(DIR)/src/google/protobuf/compiler/plugin.pb.cc \
    $(DIR)/src/google/protobuf/compiler/subprocess.cc \
    $(DIR)/src/google/protobuf/compiler/zip_writer.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_enum.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_enum_field.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_extension.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_field.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_file.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_generator.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_helpers.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_message.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_message_field.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_primitive_field.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_service.cc \
    $(DIR)/src/google/protobuf/compiler/cpp/cpp_string_field.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_context.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_enum.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_enum_field.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_extension.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_field.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_file.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_generator.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_generator_factory.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_helpers.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_lazy_message_field.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_message.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_message_field.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_name_resolver.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_primitive_field.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_shared_code_generator.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_service.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_string_field.cc \
    $(DIR)/src/google/protobuf/compiler/java/java_doc_comment.cc \
    $(DIR)/src/google/protobuf/compiler/javamicro/javamicro_enum.cc \
    $(DIR)/src/google/protobuf/compiler/javamicro/javamicro_enum_field.cc \
    $(DIR)/src/google/protobuf/compiler/javamicro/javamicro_field.cc \
    $(DIR)/src/google/protobuf/compiler/javamicro/javamicro_file.cc \
    $(DIR)/src/google/protobuf/compiler/javamicro/javamicro_generator.cc \
    $(DIR)/src/google/protobuf/compiler/javamicro/javamicro_helpers.cc \
    $(DIR)/src/google/protobuf/compiler/javamicro/javamicro_message.cc \
    $(DIR)/src/google/protobuf/compiler/javamicro/javamicro_message_field.cc \
    $(DIR)/src/google/protobuf/compiler/javamicro/javamicro_primitive_field.cc \
    $(DIR)/src/google/protobuf/compiler/javanano/javanano_enum.cc \
    $(DIR)/src/google/protobuf/compiler/javanano/javanano_enum_field.cc \
    $(DIR)/src/google/protobuf/compiler/javanano/javanano_extension.cc \
    $(DIR)/src/google/protobuf/compiler/javanano/javanano_field.cc \
    $(DIR)/src/google/protobuf/compiler/javanano/javanano_file.cc \
    $(DIR)/src/google/protobuf/compiler/javanano/javanano_generator.cc \
    $(DIR)/src/google/protobuf/compiler/javanano/javanano_helpers.cc \
    $(DIR)/src/google/protobuf/compiler/javanano/javanano_message.cc \
    $(DIR)/src/google/protobuf/compiler/javanano/javanano_message_field.cc \
    $(DIR)/src/google/protobuf/compiler/javanano/javanano_primitive_field.cc \
    $(DIR)/src/google/protobuf/compiler/python/python_generator.cc \
    $(DIR)/src/google/protobuf/io/coded_stream.cc \
    $(DIR)/src/google/protobuf/io/gzip_stream.cc \
    $(DIR)/src/google/protobuf/io/printer.cc \
    $(DIR)/src/google/protobuf/io/strtod.cc \
    $(DIR)/src/google/protobuf/io/tokenizer.cc \
    $(DIR)/src/google/protobuf/io/zero_copy_stream.cc \
    $(DIR)/src/google/protobuf/io/zero_copy_stream_impl.cc \
    $(DIR)/src/google/protobuf/io/zero_copy_stream_impl_lite.cc \
    $(DIR)/src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc \
    $(DIR)/src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc \
    $(DIR)/src/google/protobuf/stubs/common.cc \
    $(DIR)/src/google/protobuf/stubs/once.cc \
    $(DIR)/src/google/protobuf/stubs/structurally_valid.cc \
    $(DIR)/src/google/protobuf/stubs/strutil.cc \
    $(DIR)/src/google/protobuf/stubs/substitute.cc \
    $(DIR)/src/google/protobuf/stubs/stringprintf.cc
    
LOCAL_MODULE_STEM_32 := protoc        
LOCAL_MODULE_STEM_64 := protoc64
LOCAL_MULTILIB := both
LOCAL_SRC_FILES := $(COMPILER_SRC_FILES)
LOCAL_C_INCLUDES := $(common_includes)
LOCAL_CPPFLAGS := $(common_cxxflags)
LOCAL_CFLAGS := $(IGNORED_WARNINGS) 
LOCAL_EXPORT_C_INCLUDE_DIRS := $(common_includes) 
    
LOCAL_STATIC_LIBRARIES :=  $(common_staticlibs)
LOCAL_CFLAGS := $(IGNORED_WARNINGS)
LOCAL_LDFLAGS += -static
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_PACK_MODULE_RELOCATIONS := false
include $(BUILD_EXECUTABLE)
