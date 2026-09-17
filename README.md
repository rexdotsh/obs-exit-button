# OBS Exit Button

Restores the **Exit** button that was removed from the OBS Studio Controls dock in OBS 31.1.

This plugin follows the behavior removed by [obsproject/obs-studio#11887](https://github.com/obsproject/obs-studio/pull/11887): it adds a native Qt button to the Controls dock and routes clicks through OBS's normal window-close flow. Active-output warnings and shutdown cleanup therefore continue to work as they do for **File → Exit** or the window close button.

## Compatibility

- Windows x64
- OBS Studio 32.2.1

The plugin is built against the exact OBS and Qt dependency versions used by OBS 32.2.1. Other OBS 32.x versions may work but are not claimed as tested.

## Install

1. Download the Windows ZIP from [Releases](https://github.com/rexdotsh/obs-exit-button/releases).
2. Extract the `obs-exit-button` folder into `%PROGRAMDATA%\obs-studio\plugins`.
3. Start OBS. The **Exit** button appears below **Settings** in the Controls dock.

To uninstall, close OBS and delete `%PROGRAMDATA%\obs-studio\plugins\obs-exit-button`.

## Why this implementation

The implementation is intentionally small, but it does not assume every lookup succeeds. It:

- waits for the OBS frontend to finish loading;
- scopes UI discovery to the Controls dock;
- inserts the button before the dock's expanding spacer, matching the original placement;
- prevents duplicates;
- uses OBS's localized `Exit` label and normal close path;
- removes the button and frontend callback if the plugin is unloaded;
- logs a warning instead of dereferencing missing UI objects.

## Build

Requires CMake 3.28+ and Visual Studio 2022:

```powershell
cmake --preset windows-x64
cmake --build --preset windows-x64 --config Release
cmake --install build_x64 --prefix release --config Release
```

The first configure downloads verified OBS 32.2.1 sources and matching OBS dependencies.

## License

GPL-2.0-or-later
