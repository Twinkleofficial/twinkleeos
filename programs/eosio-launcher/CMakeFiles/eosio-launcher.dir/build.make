# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/secondtonone1/workspace/twinkleeos

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/secondtonone1/workspace/twinkleeos

# Include any dependencies generated for this target.
include programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/depend.make

# Include the progress variables for this target.
include programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/progress.make

# Include the compile flags for this target's objects.
include programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/flags.make

programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o: programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/flags.make
programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o: programs/eosio-launcher/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/secondtonone1/workspace/twinkleeos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o"
	cd /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher && /usr/bin/clang++-4.0  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/eosio-launcher.dir/main.cpp.o -c /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher/main.cpp

programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/eosio-launcher.dir/main.cpp.i"
	cd /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher && /usr/bin/clang++-4.0 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher/main.cpp > CMakeFiles/eosio-launcher.dir/main.cpp.i

programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/eosio-launcher.dir/main.cpp.s"
	cd /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher && /usr/bin/clang++-4.0 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher/main.cpp -o CMakeFiles/eosio-launcher.dir/main.cpp.s

programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o.requires:

.PHONY : programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o.requires

programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o.provides: programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o.requires
	$(MAKE) -f programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/build.make programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o.provides.build
.PHONY : programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o.provides

programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o.provides.build: programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o


# Object files for target eosio-launcher
eosio__launcher_OBJECTS = \
"CMakeFiles/eosio-launcher.dir/main.cpp.o"

# External object files for target eosio-launcher
eosio__launcher_EXTERNAL_OBJECTS =

programs/eosio-launcher/eosio-launcher: programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o
programs/eosio-launcher/eosio-launcher: programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/build.make
programs/eosio-launcher/eosio-launcher: libraries/chain/libeosio_chain.a
programs/eosio-launcher/eosio-launcher: libraries/fc/libfc_debug.a
programs/eosio-launcher/eosio-launcher: libraries/utilities/libeos_utilities.a
programs/eosio-launcher/eosio-launcher: libraries/fc/libfc_debug.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_program_options.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_signals.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_serialization.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_context.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_iostreams.a
programs/eosio-launcher/eosio-launcher: /usr/lib/x86_64-linux-gnu/libssl.a
programs/eosio-launcher/eosio-launcher: /usr/lib/x86_64-linux-gnu/libcrypto.a
programs/eosio-launcher/eosio-launcher: /usr/lib/x86_64-linux-gnu/libz.so
programs/eosio-launcher/eosio-launcher: /usr/local/lib/libsecp256k1.a
programs/eosio-launcher/eosio-launcher: /usr/lib/x86_64-linux-gnu/libgmp.a
programs/eosio-launcher/eosio-launcher: libraries/chainbase/libchainbase.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_thread.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_date_time.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_filesystem.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_system.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_chrono.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_unit_test_framework.a
programs/eosio-launcher/eosio-launcher: /home/secondtonone1/opt/boost/lib/libboost_locale.a
programs/eosio-launcher/eosio-launcher: /usr/lib/x86_64-linux-gnu/libpthread.so
programs/eosio-launcher/eosio-launcher: libraries/wasm-jit/Source/WAST/libWAST.a
programs/eosio-launcher/eosio-launcher: libraries/wasm-jit/Source/WASM/libWASM.a
programs/eosio-launcher/eosio-launcher: libraries/wasm-jit/Source/Runtime/libRuntime.a
programs/eosio-launcher/eosio-launcher: libraries/wasm-jit/Source/IR/libIR.a
programs/eosio-launcher/eosio-launcher: libraries/wasm-jit/Source/Logging/libLogging.a
programs/eosio-launcher/eosio-launcher: libraries/wasm-jit/Source/Platform/libPlatform.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMPasses.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMipo.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMInstrumentation.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMVectorize.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMIRReader.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMAsmParser.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMLinker.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMMCJIT.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMExecutionEngine.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMRuntimeDyld.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMDebugInfoDWARF.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMX86CodeGen.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMAsmPrinter.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMDebugInfoCodeView.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMDebugInfoMSF.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMGlobalISel.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMSelectionDAG.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMCodeGen.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMScalarOpts.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMInstCombine.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMBitWriter.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMTransformUtils.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMTarget.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMAnalysis.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMProfileData.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMX86AsmParser.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMX86Desc.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMX86AsmPrinter.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMX86Utils.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMObject.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMMCParser.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMBitReader.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMCore.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMX86Disassembler.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMX86Info.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMMCDisassembler.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMMC.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMSupport.a
programs/eosio-launcher/eosio-launcher: /usr/lib/llvm-4.0/lib/libLLVMDemangle.a
programs/eosio-launcher/eosio-launcher: externals/binaryen/lib/libwasm.a
programs/eosio-launcher/eosio-launcher: externals/binaryen/lib/libasmjs.a
programs/eosio-launcher/eosio-launcher: externals/binaryen/lib/libpasses.a
programs/eosio-launcher/eosio-launcher: externals/binaryen/lib/libcfg.a
programs/eosio-launcher/eosio-launcher: externals/binaryen/lib/libast.a
programs/eosio-launcher/eosio-launcher: externals/binaryen/lib/libemscripten-optimizer.a
programs/eosio-launcher/eosio-launcher: externals/binaryen/lib/libsupport.a
programs/eosio-launcher/eosio-launcher: libraries/softfloat/libsoftfloat.a
programs/eosio-launcher/eosio-launcher: libraries/builtins/libbuiltins.a
programs/eosio-launcher/eosio-launcher: programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/secondtonone1/workspace/twinkleeos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable eosio-launcher"
	cd /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/eosio-launcher.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/build: programs/eosio-launcher/eosio-launcher

.PHONY : programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/build

programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/requires: programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/main.cpp.o.requires

.PHONY : programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/requires

programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/clean:
	cd /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher && $(CMAKE_COMMAND) -P CMakeFiles/eosio-launcher.dir/cmake_clean.cmake
.PHONY : programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/clean

programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/depend:
	cd /home/secondtonone1/workspace/twinkleeos && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/secondtonone1/workspace/twinkleeos /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher /home/secondtonone1/workspace/twinkleeos /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher /home/secondtonone1/workspace/twinkleeos/programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : programs/eosio-launcher/CMakeFiles/eosio-launcher.dir/depend

