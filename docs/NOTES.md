git submodule add -b docking https://github.com/ocornut/imgui libs/imgui

cmake -S tests -B build/tests



cd tools/vcpkg
./vcpkg install glfw3:x64-windows
./vcpkg install rtaudio:x64-windows
./vcpkg install rtmidi:x64-windows
./vcpkg install rapidjson:x64-windows

flutter create myapp

cd myapp

flutter run -d windows

cd build/windows
cmake ../../windows
or
cmake -S windows -B build/windows

#validate
../../bin/Debug/validator.exe panner_vst3.vst3

#if this
'FlutterEngineInitialize' returned 'kInvalidArguments'. Not running in AOT mode but could not resolve the kernel binary
#run this
flutter build bundle


    find_package(RtAudio CONFIG REQUIRED)
    target_link_libraries(main PRIVATE RtAudio::rtaudio)

    find_package(RtMidi CONFIG REQUIRED)
    target_link_libraries(main PRIVATE RtMidi::rtmidi)
    
    find_package(RapidJSON CONFIG REQUIRED)
    target_link_libraries(main PRIVATE rapidjson)

flutter create --org farm.tune --template=plugin --platforms=windows,linux demo_plg

dart error: can't load kernel binary: invalid sdk hash
rm flutter/bin/cache
flutter doctor
flutter upgrade