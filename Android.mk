#
# Copyright (C) 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#	  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)

# ==========================================================
# Setup some common variables for the different build
# targets here.
# ==========================================================

main := Main.cpp
sources := \
    	compile/IdAssigner.cpp \
    	compile/InlineXmlFormatParser.cpp \
    	compile/NinePatch.cpp \
    	compile/Png.cpp \
    	compile/PngChunkFilter.cpp \
    	compile/PngCrunch.cpp \
    	compile/PseudolocaleGenerator.cpp \
    	compile/Pseudolocalizer.cpp \
    	compile/XmlIdCollector.cpp \
    	configuration/ConfigurationParser.cpp \
    	filter/AbiFilter.cpp \
    	filter/ConfigFilter.cpp \
    	flatten/Archive.cpp \
    	flatten/TableFlattener.cpp \
    	flatten/XmlFlattener.cpp \
    	io/BigBufferStreams.cpp \
    	io/File.cpp \
    	io/FileInputStream.cpp \
    	io/FileSystem.cpp \
    	io/StringInputStream.cpp \
    	io/Util.cpp \
    	io/ZipArchive.cpp \
    	link/AutoVersioner.cpp \
    	link/ManifestFixer.cpp \
    	link/ProductFilter.cpp \
    	link/PrivateAttributeMover.cpp \
    	link/ReferenceLinker.cpp \
    	link/TableMerger.cpp \
    	link/XmlCompatVersioner.cpp \
    	link/XmlNamespaceRemover.cpp \
    	link/XmlReferenceLinker.cpp \
    	optimize/ResourceDeduper.cpp \
    	optimize/VersionCollapser.cpp \
    	process/SymbolTable.cpp \
    	proto/ProtoHelpers.cpp \
    	proto/TableProtoDeserializer.cpp \
    	proto/TableProtoSerializer.cpp \
    	split/TableSplitter.cpp \
    	text/Unicode.cpp \
    	text/Utf8Iterator.cpp \
    	unflatten/BinaryResourceParser.cpp \
    	unflatten/ResChunkPullParser.cpp \
    	util/BigBuffer.cpp \
    	util/Files.cpp \
    	util/Util.cpp \
    	ConfigDescription.cpp \
    	Debug.cpp \
    	DominatorTree.cpp \
    	Flags.cpp \
    	java/AnnotationProcessor.cpp \
    	java/ClassDefinition.cpp \
    	java/JavaClassGenerator.cpp \
    	java/ManifestClassGenerator.cpp \
    	java/ProguardRules.cpp \
    	LoadedApk.cpp \
    	Locale.cpp \
    	Resource.cpp \
    	ResourceParser.cpp \
    	ResourceTable.cpp \
    	ResourceUtils.cpp \
    	ResourceValues.cpp \
    	SdkConstants.cpp \
    	StringPool.cpp \
    	xml/XmlActionExecutor.cpp \
    	xml/XmlDom.cpp \
    	xml/XmlPullParser.cpp \
    	xml/XmlUtil.cpp \
    	Resources.proto \
    	ResourcesInternal.proto

toolSources := \
	cmd/Compile.cpp \
	cmd/Diff.cpp \
	cmd/Dump.cpp \
	cmd/Link.cpp \
	cmd/Optimize.cpp \
	cmd/Util.cpp

protoIncludes := \
	proto/Format.pb.h

aapt_staticlibs := \
	libandroidfw-static \
	libpng \
	libexpat \
	libutils \
	libcutils \
	libziparchive \
	liblog \
	libbase \
	libm \
	libz \
	libc++_static \
	libprotobuf-cpp-lite_arm

CFLAGS := \
	-DHAVE_SYS_UIO_H \
	-DHAVE_PTHREADS \
	-DHAVE_SCHED_H \
	-DHAVE_SYS_UIO_H \
	-DHAVE_IOCTL \
	-DHAVE_TM_GMTOFF \
	-DANDROID_SMP=1  \
	-DHAVE_ENDIAN_H \
	-DHAVE_POSIX_FILEMAP \
	-DHAVE_OFF64_T \
	-DHAVE_ENDIAN_H \
	-DHAVE_SCHED_H \
	-DHAVE_LITTLE_ENDIAN_H \
	-D__ANDROID__ \
	-DHAVE_ANDROID_OS=1 \
	-D_ANDROID_CONFIG_H \
	-D_BYPASS_DSO_ERROR \
	-DHAVE_ERRNO_H='1' \
	-DSTATIC_ANDROIDFW_FOR_TOOLS \
	-DLIBLOG_LOG_TAG=1005 \
        -DSNET_EVENT_LOG_TAG=1397638484 \
        -Werror -fvisibility=hidden \
        -D__ANDROID_USE_LIBLOG_EVENT_INTERFACE \
        -D__ANDROID_USE_LIBLOG_EVENT_INTERFACE \
        -DLIBLOG_LOG_TAG=1005 \
        -DTARGET_USES_LOGD=true

aapt_cflags += -D'AAPT_VERSION="android-$(PLATFORM_VERSION)-$(TARGET_BUILD_VARIANT)"'
aapt_cflags += -Wno-format-y2k
aapt_cflags += $(CFLAGS)

aapt_ldlibs := -lm -ldl -lz

aapt_cflags += \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-maybe-uninitialized \
  -Wno-non-virtual-dtor \
	-Wno-error=non-virtual-dtor

aapt_cxxflags := \
	-std=gnu++1y \
	-Wno-missing-field-initializers \
	-fno-exceptions \
	-fno-rtti \
	-Wno-missing-field-initializers \
        -Wno-non-virtual-dtor \
	-Wno-error=non-virtual-dtor

aapt_includes := \
	$(LOCAL_PATH)/include \
	system/core/base/include \
	system/core/libutils \
	system/core/liblog \
	system/core/libcutils \
	external/libpng \
	external/expat \
        external/expat/lib \
	external/zlib \
	external/libcxx/include \
	system/core/libziparchive \
	frameworks/base/libs/androidfw \
	frameworks/base/libs/androidfw/include \
	external/protobuf/src \
	external/protobuf/android \
        system/core/libziparchive \
        system/core/libziparchive/include

cFlags := \
	-Wall \
	-Werror \
	-Wno-unused-parameter \
	-D'AAPT_VERSION="android-$(PLATFORM_VERSION)-$(TARGET_BUILD_VARIANT)"' \
	-Wno-format-y2k \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-maybe-uninitialized


cFlags_darwin := -D_DARWIN_UNLIMITED_STREAMS
cFlags_windows := -Wno-maybe-uninitialized # Incorrectly marking use of Maybe.value() as error.
cppFlags := -Wno-missing-field-initializers -fno-exceptions -fno-rtti

# Statically link libz for MinGW (Win SDK under Linux),
# and dynamically link for all others.
hostStaticLibs_windows := libz
hostLdLibs_linux := -lz
hostLdLibs_darwin := -lz
host_protoIncludes := $(call generated-sources-dir-for,STATIC_LIBRARIES,libaapt2,HOST)

FIND_HOSTOS := $(shell uname -s)
HOST_NAME := $(shell echo $(FIND_HOSTOS) |sed -e s/L/l/ |sed -e s/D/d/ |sed s/W/w/ )

include $(CLEAR_VARS)
ANDROIDFW_PATH := ../../libs/androidfw
LOCAL_C_INCLUDES := \
         $(LOCAL_PATH)/include \
         $(aapt_includes)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
androidfw_srcs := \
    $(ANDROIDFW_PATH)/Asset.cpp \
    $(ANDROIDFW_PATH)/AssetDir.cpp \
    $(ANDROIDFW_PATH)/AssetManager.cpp \
    $(ANDROIDFW_PATH)/AssetManager2.cpp \
    $(ANDROIDFW_PATH)/AttributeResolution.cpp \
    $(ANDROIDFW_PATH)/ChunkIterator.cpp \
    $(ANDROIDFW_PATH)/LoadedArsc.cpp \
    $(ANDROIDFW_PATH)/LocaleData.cpp \
    $(ANDROIDFW_PATH)/misc.cpp \
    $(ANDROIDFW_PATH)/ObbFile.cpp \
    $(ANDROIDFW_PATH)/ResourceTypes.cpp \
    $(ANDROIDFW_PATH)/ResourceUtils.cpp \
    $(ANDROIDFW_PATH)/StreamingZipInflater.cpp \
    $(ANDROIDFW_PATH)/TypeWrappers.cpp \
    $(ANDROIDFW_PATH)/Util.cpp \
    $(ANDROIDFW_PATH)/ZipFileRO.cpp \
    $(ANDROIDFW_PATH)/ZipUtils.cpp \
    $(ANDROIDFW_PATH)/BackupData.cpp \
    $(ANDROIDFW_PATH)/BackupHelpers.cpp \
    $(ANDROIDFW_PATH)/CursorWindow.cpp \
    $(ANDROIDFW_PATH)/DisplayEventDispatcher.cpp

LOCAL_MODULE:= libandroidfw-static
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS :=  $(aoptCflags)
LOCAL_CPPFLAGS := $(aoptCppFlags)
LOCAL_STATIC_LIBRARIES := libziparchive libbase
LOCAL_SRC_FILES := $(androidfw_srcs)
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libprotobuf-cpp-lite_arm
IGNORED_WARNINGS := -Wno-sign-compare -Wno-unused-parameter -Wno-sign-promo -Wno-error=return-type
LOCAL_CPP_EXTENSION := .cc
DIR := ../../../../external/protobuf
CC_LITE_SRC_FILES := \
        $(DIR)/src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc \
        $(DIR)/src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc \
        $(DIR)/src/google/protobuf/stubs/bytestream.cc \
        $(DIR)/src/google/protobuf/stubs/common.cc \
        $(DIR)/src/google/protobuf/stubs/int128.cc \
        $(DIR)/src/google/protobuf/stubs/once.cc \
        $(DIR)/src/google/protobuf/stubs/status.cc \
        $(DIR)/src/google/protobuf/stubs/statusor.cc \
        $(DIR)/src/google/protobuf/stubs/stringpiece.cc \
        $(DIR)/src/google/protobuf/stubs/stringprintf.cc \
        $(DIR)/src/google/protobuf/stubs/structurally_valid.cc \
        $(DIR)/src/google/protobuf/stubs/strutil.cc \
        $(DIR)/src/google/protobuf/stubs/time.cc \
        $(DIR)/src/google/protobuf/arena.cc \
        $(DIR)/src/google/protobuf/arenastring.cc \
        $(DIR)/src/google/protobuf/extension_set.cc \
        $(DIR)/src/google/protobuf/generated_message_util.cc \
        $(DIR)/src/google/protobuf/message_lite.cc \
        $(DIR)/src/google/protobuf/repeated_field.cc \
        $(DIR)/src/google/protobuf/wire_format_lite.cc \
        $(DIR)/src/google/protobuf/io/coded_stream.cc \
        $(DIR)/src/google/protobuf/io/zero_copy_stream.cc \
        $(DIR)/src/google/protobuf/io/zero_copy_stream_impl_lite.cc

LOCAL_C_INCLUDES := \
	$(DIR)/android \
	$(DIR)/src \
	$(aapt_includes) \
	$(DIR)

LOCAL_CPPFLAGS := $(aapt_cxxflags)
LOCAL_CFLAGS := -DGOOGLE_PROTOBUF_NO_RTTI $(IGNORED_WARNINGS) $(aapt_cxxflags)
LOCAL_SRC_FILES := $(CC_LITE_SRC_FILES)
LOCAL_EXPORT_C_INCLUDE_DIRS := \
	$(DIR)/android \
	$(DIR)/src \
	$(DIR)

include $(BUILD_STATIC_LIBRARY)
# Android Protocol buffer compiler, aprotoc (target executable)
# used by the build systems as $(PROTOC) defined in
# build/core/config.mk
#
# Note: this isnt needed anymore since making the temp files
# 	I included it for historical reasons
# =======================================================
include $(CLEAR_VARS)

LOCAL_MODULE := aprotoc
LOCAL_MODULE_TAGS := optional
LOCAL_CXX_STL := libc++_static
LOCAL_CPP_EXTENSION := .cc
IGNORED_WARNINGS := -Wno-sign-compare -Wno-unused-parameter -Wno-sign-promo -Wno-error=return-type
COMPILER_SRC_FILES :=  \
        $(DIR)/src/google/protobuf/any.cc \
        $(DIR)/src/google/protobuf/arena.cc \
        $(DIR)/src/google/protobuf/arenastring.cc \
        $(DIR)/src/google/protobuf/descriptor.cc \
        $(DIR)/src/google/protobuf/descriptor.pb.cc \
        $(DIR)/src/google/protobuf/descriptor_database.cc \
        $(DIR)/src/google/protobuf/dynamic_message.cc \
        $(DIR)/src/google/protobuf/extension_set.cc \
        $(DIR)/src/google/protobuf/extension_set_heavy.cc \
        $(DIR)/src/google/protobuf/generated_message_reflection.cc \
        $(DIR)/src/google/protobuf/generated_message_util.cc \
        $(DIR)/src/google/protobuf/map_field.cc \
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
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_enum.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_enum_field.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_extension.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_field.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_file.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_generator.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_helpers.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_map_field.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_message.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_message_field.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_primitive_field.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_service.cc \
        $(DIR)/src/google/protobuf/compiler/cpp/cpp_string_field.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_doc_comment.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_enum.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_enum_field.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_field_base.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_generator.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_helpers.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_map_field.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_message.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_message_field.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_primitive_field.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_reflection_class.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_repeated_enum_field.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_repeated_message_field.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_repeated_primitive_field.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_source_generator_base.cc \
        $(DIR)/src/google/protobuf/compiler/csharp/csharp_wrapper_field.cc \
        $(DIR)/src/google/protobuf/compiler/importer.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_context.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_doc_comment.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_enum.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_enum_field.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_enum_field_lite.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_enum_lite.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_extension.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_extension_lite.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_field.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_file.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_generator.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_generator_factory.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_helpers.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_lazy_message_field.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_lazy_message_field_lite.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_map_field.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_map_field_lite.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_message_builder.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_message_builder_lite.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_message.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_message_field.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_message_field_lite.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_message_lite.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_name_resolver.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_primitive_field.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_primitive_field_lite.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_service.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_shared_code_generator.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_string_field.cc \
        $(DIR)/src/google/protobuf/compiler/java/java_string_field_lite.cc \
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
        $(DIR)/src/google/protobuf/compiler/javanano/javanano_map_field.cc \
        $(DIR)/src/google/protobuf/compiler/javanano/javanano_message.cc \
        $(DIR)/src/google/protobuf/compiler/javanano/javanano_message_field.cc \
        $(DIR)/src/google/protobuf/compiler/javanano/javanano_primitive_field.cc \
        $(DIR)/src/google/protobuf/compiler/js/js_generator.cc \
        $(DIR)/src/google/protobuf/compiler/main.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_enum.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_enum_field.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_extension.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_field.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_file.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_generator.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_helpers.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_map_field.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_message.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_message_field.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_oneof.cc \
        $(DIR)/src/google/protobuf/compiler/objectivec/objectivec_primitive_field.cc \
        $(DIR)/src/google/protobuf/compiler/parser.cc \
        $(DIR)/src/google/protobuf/compiler/plugin.cc \
        $(DIR)/src/google/protobuf/compiler/plugin.pb.cc \
        $(DIR)/src/google/protobuf/compiler/python/python_generator.cc \
        $(DIR)/src/google/protobuf/compiler/ruby/ruby_generator.cc \
        $(DIR)/src/google/protobuf/compiler/subprocess.cc \
        $(DIR)/src/google/protobuf/compiler/zip_writer.cc \
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
        $(DIR)/src/google/protobuf/stubs/int128.cc \
        $(DIR)/src/google/protobuf/stubs/once.cc \
        $(DIR)/src/google/protobuf/stubs/status.cc \
        $(DIR)/src/google/protobuf/stubs/stringpiece.cc \
        $(DIR)/src/google/protobuf/stubs/structurally_valid.cc \
        $(DIR)/src/google/protobuf/stubs/strutil.cc \
        $(DIR)/src/google/protobuf/stubs/substitute.cc \
        $(DIR)/src/google/protobuf/stubs/stringprintf.cc

LOCAL_MODULE_STEM_32 := aprotoc
LOCAL_MODULE_STEM_64 := aprotoc64
LOCAL_MULTILIB := both
LOCAL_SRC_FILES := $(COMPILER_SRC_FILES)

LOCAL_C_INCLUDES := \
	$(DIR)/android \
	$(DIR)/src \
	$(aapt_includes) \
	$(DIR)

LOCAL_CPPFLAGS := -std=c++14 -frtti
LOCAL_CFLAGS := $(IGNORED_WARNINGS)
LOCAL_EXPORT_C_INCLUDE_DIRS := \
	$(DIR)/android \
	$(DIR)/src \
	$(aapt_includes) \
	$(DIR)

LOCAL_STATIC_LIBRARIES :=  $(aapt_staticlibs)
LOCAL_CFLAGS := $(IGNORED_WARNINGS)
LOCAL_LDFLAGS += -static
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_PACK_MODULE_RELOCATIONS := false
include $(BUILD_EXECUTABLE)

# =======================================================

include $(CLEAR_VARS)
LOCAL_MODULE := libaapt2
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_CPPFLAGS := $(aapt_cxxflags)
LOCAL_CFLAGS := -D'AAPT_VERSION="android-$(PLATFORM_VERSION)-$(TARGET_BUILD_VARIANT)"'
LOCAL_C_INCLUDES := \
	$(aapt_includes) \
	$(protoIncludes)

LOCAL_SRC_FILES := $(sources)
include $(BUILD_STATIC_LIBRARY)

# ==========================================================
# Build the target executable: aapt2
# ==========================================================
include $(CLEAR_VARS)
LOCAL_MODULE := aapt2

LOCAL_CPPFLAGS := $(aapt_cxxflags) -D__ANDROID__
LOCAL_C_INCLUDES := \
	$(aapt_includes) \
	$(protoIncludes)

LOCAL_MODULE_TAGS := optional
LOCAL_STATIC_LIBRARIES := libaapt2 $(aapt_staticlibs)
LOCAL_LDLIBS := $(aapt_ldlibs)
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_UNSTRIPPED_PATH := $(PRODUCT_OUT)/symbols/utilities
LOCAL_CFLAGS := -D'AAPT_VERSION="android-$(PLATFORM_VERSION)-$(TARGET_BUILD_VARIANT)"'
LOCAL_LDFLAGS := -static
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_PACK_MODULE_RELOCATIONS := false
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk

LOCAL_SRC_FILES := $(main) $(toolSources)
LOCAL_MODULE_STEM_32 := aapt2
LOCAL_MODULE_STEM_64 := aapt2-64
LOCAL_MODULE_PATH_32 := $(ANDROID_PRODUCT_OUT)/system/bin
LOCAL_MODULE_PATH_64 := $(ANDROID_PRODUCT_OUT)/system/bin
LOCAL_MULTILIB := both

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := libaapt2_jni
LOCAL_C_INCLUDES := $(aapt_includes)
LOCAL_SRC_FILES :=  $(toolSources) $(main) jni/aapt2_main.cpp jni/aapt2_jni.cpp
LOCAL_STATIC_LIBRARIES := libaapt2 $(aapt_staticlibs)
LOCAL_CFLAGS := -D'AAPT_VERSION="android-$(PLATFORM_VERSION)-$(TARGET_BUILD_VARIANT)"'
LOCAL_CPPFLAGS := $(aapt_cxxflags) -D__ANDROID__
include $(BUILD_SHARED_LIBRARY)
