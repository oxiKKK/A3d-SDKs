# A3D API versions (`a3dapi.dll`)

35 distinct builds of the A3D runtime DLL, ranging from 2.0.1.0 (October 1998) to 3.3.678.0 (July 2000). Every binary here is unique by SHA-256.

## Layout

`<version>_<export-date-UTC>/<filename>` plus a `.sha256` checksum next to each DLL. Distinct binaries that share a version and timestamp are named `a3dapi_<SHA-256-prefix>.dll`. Each checksum file contains the full SHA-256 and the adjacent DLL filename.

Versions come from `VS_FIXEDFILEINFO`. Dates come from `IMAGE_EXPORT_DIRECTORY.TimeDateStamp`, interpreted as UTC — these are linker-recorded timestamps, not independently verified compilation dates.

## manifest.json

Records, for every build: version, export timestamp (raw and UTC), size, full SHA-256, path, and checksum path.
