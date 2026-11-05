# Arm Nongmuang LA-2A — GitHub Ready

JUCE 8 / CMake starter plugin project for Windows VST3 and Standalone.

## Features in this starter
- Mono/stereo audio processing
- Peak Reduction and Gain parameters
- Compress/Limit selection and Bypass
- Optical-style envelope compression, gain reduction display and soft saturation
- Parameter state save/restore
- GitHub Actions workflow builds Windows artifacts

## Build on GitHub
1. Create a GitHub repository and upload the contents of this folder to its root.
2. Push to `main` (or `master`).
3. Open **Actions → Build Windows VST3**.
4. Download the `Arm-Nongmuang-LA2A-Windows` artifact after a successful run.

## Local build
Requires Visual Studio 2022 C++ workload and CMake 3.22+:
```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
```

## NAM compatibility
NAM `.nam` model loading is **not implemented in this starter**. A real NAM feature requires
integrating a compatible Neural Amp Modeler inference runtime and validating the target
model architecture/version. The plugin must not claim NAM or “NAM 2A” support until that
integration and testing are complete.

## PNG assets
`Assets/` contains any PNG files included in the supplied separated-assets archive.
They are artwork resources only at this stage; the editor currently draws its interface
with JUCE and does not yet use PNGs for the skin or hit-area mapping.
