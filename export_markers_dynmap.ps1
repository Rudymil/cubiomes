cd build
cmake ..
if ($?) {
    cmake --build .
    if ($?) {
        .\export_markers_dynmap.exe
    }
}
cd ..