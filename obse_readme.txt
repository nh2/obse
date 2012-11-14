Oblivion Script Extender v0020
by Ian Patterson, Stephen Abel, and Paul Connelly
(ianpatt, behippo, and scruggsywuggsy the ferret)

Additional contributions from Timeslip, The J, DragoonWraith, SkyRanger-1, badhair, JRoush, shademe, and kyoma.

The Oblivion Script Extender, or OBSE for short, is a modder's resource that expands the scripting capabilities of Oblivion. It doesn't make any modifications to oblivion.exe, TESConstructionSet.exe, or any other files in your oblivion install, so don't worry about permanent side effects. It is compatible with the 1.2.0.416 Oblivion patch, as well as the 1.2 version of the Construction Set. Versions of Oblivion downloaded via Steam are supported as well.

This release adds:
-Ability to use OBSE data types and expressions as arguments to *all* commands through optional compiler override
-Path grid commands
-Versions of existing commands which do not save their changes to the savegame
-Commands to allow increased control over OBSE's text input functionality
-Torch- and light-related commands
-Numerous new cell-related commands
-Several misc. new commands, plus bug-fixes and tweaks
-Updates to plugin API

[ Installation ]

The instructions for installing and running OBSE differ based on whether you are using a retail or Steam version of the game.

IF YOU PURCHASED A RETAIL (NON-STEAM) VERSION OF OBLIVION:

1. Copy obse_1_2_416.dll, obse_editor_1_2.dll, and obse_loader.exe to your Oblivion directory. This is usually in your Program Files folder, and should contain files called "Oblivion.exe" and "OblivionLauncher.exe".
2. Run oblivion by running obse_loader.exe from the Oblivion directory.

If you use a desktop shortcut to launch Oblivion normally, just update the shortcut to point to obse_loader.exe instead of oblivion.exe.

IF YOU ARE USING THE STEAM VERSION OF OBLIVION:

1. Copy obse_1_2_416.dll, obse_editor_1_2.dll, and obse_steam_loader.dll to your Oblivion directory. This is usually "C:\Program Files\Valve\Steam\SteamApps\common\oblivion".
2. Ensure you have enabled the Steam community in-game, or OBSE will fail to load. Go to Steam > Settings > In-Game and check the box marked "Enable Steam Community In-Game".
3. Launch Oblivion via Steam or by running Oblivion.exe. OBSE will automatically be run along with Oblivion when launched. To disable this, rename or move obse_steam_loader.dll. You do not need to use obse_loader.exe unless you are running the editor.

RUNNING TES:CONSTRUCTION SET WITH OBSE:

Scripts written with these new commands must be written via the TESConstructionSet launched with obse_loader. Open a command prompt window, navigate to your oblivion install folder, and type "obse_loader -editor". The normal editor can open plugins with these extended scripts fine, it just can't recompile them and will give errors if you try.

[ Suggestions for Modders ]

If your mod requires OBSE, please provide a link to the main OBSE website, http://obse.silverlock.org/ instead of packaging it with your mod install. Future versions of OBSE will be backwards compatible, so including a potentially old version can cause confusion and/or break other mods that need newer versions. If you're making a large mod with an installer, inclusion of a specific version of OBSE is OK, but please check the file versions of the OBSE files before replacing them, and only replace earlier versions.

When your mod loads, use the command GetOBSEVersion to make sure a compatible version of OBSE is installed. In general, make sure you're testing for any version later than the minimum version you support, as each update to OBSE will have a higher version number. Something like:

if GetOBSEVersion < 5 then
  MessageBox "This mod requires a newer version of OBSE."
endif

Another alternative that is even more user-friendly is releasing your mod as an OMOD, and adding a script to check the version when installing the mod:

IfNot ScriptExtenderNewerThan #.#.#.#
    Message "Requires Oblivion Script Extender v#### or greater"
    FatalError
EndIf

[ Troubleshooting / FAQ ]

Oblivion doesn't launch after running obse_loader.exe:

- Make sure you've copied the OBSE files to your oblivion folder. That folder should also contain oblivion.exe.
- Check the file obse_loader.log in your oblivion folder for errors.

obse_loader.log tells me it couldn't find a checksum:

- You may have a version of Oblivion that isn't supported. I have the english official patch v1.2.0.416.  Localized versions with different executables or different patches may not work, but many have been successful. If there's enough legitimate demand for it, I can add support for other versions in the future.
- Your Oblivion install may be corrupt. Hacks or no-cd patches may also change the checksum of the game, making it impossible to detect the installed version.

The OBSE loader tells me I need to use the autopatcher:

- Go to to OBSE website (http://obse.silverlock.org) and download autopatcher, which will walk you through the update process. You will need the latest patch from Bethesda, as well as your original Oblivion DVD.

OBSE doesn't launch with the Direct2Drive version:

- The Direct2Drive version of the Oblivion executable has additional DRM applied, which would be illegal for us to bypass. We cannot support this version. If you own a retail version of Oblivion, please use the autopatcher to extract a usable executable.
- Note that if you are interested in buying a digitally distributed version of Oblivion, the Steam version is supported.

Crashes or other strange behavior:

- Let me know how you made it crash, and I'll see about fixing it.

Xbox 360 or PS3 version?

- Impossible.

Running OBSE and Oldblivion at the same time:

- Copy your oldblivion support files in to the Oblivion folder (oldblivion.dll, oldblivion.cfg, shaders.sdp), then run the loader with the -old command argument.

I'm using Windows 2000 and some mods (specifically mods using IsKeyPressed) don't work correctly. What should I do?

- Open your Oblivion.ini (found in My Documents/My Games/Oblivion) and change the "bBackground Keyboard=0" line to "bBackground Keyboard=1".

How do I change the script editor font?

- Hold F12 or F12 while opening the script editor. F12 will default to Lucida Console 9pt, and F11 will show a font picker dialog box.

Can I modify and release my own version of OBSE based on the released source code?

- This is highly discouraged. Each command must be assigned a unique and constant opcode from 0x1000-0x7FFF. Bethesda started adding commands at 0x1000, and OBSE started adding at 0x1400. If you add new commands yourself, they may conflict with later releases of OBSE. The suggested method for extending OBSE is to write a plugin. If this does not meet your needs, please email the contact addresses listed below.

How do I write a plugin for OBSE?

- Start with the obse_plugin_example project in the OBSE source distribution. Currently the documentation for the plugin API can be found in the source distribution under obse/obse/PluginAPI.h. Note that due to the opcode allocation issues discussed above, you will need to request an opcode range for your plugin by emailing the contact addresses at the bottom of the readme. Also note that plugins must have their source code available. The OBSE team has spent a very long time developing and documenting the interface to Oblivion's internals, so we request that plugin developers also share their findings with the public. This will also help us make sure that we keep Oblivion as stable as possible even when using third-party plugins.

What is the 'src' folder for?

- The 'src' folder contains the source code for OBSE. Feel free to delete or ignore it if you are not a programmer interested in contributing to the project.

How do I use OBSE with 3D Analyze?

- Run normal Oblivion with 3DA once, then quit. Set up any configuration options you need at this point. This should create a config_DX.ini file in the Oblivion folder. Then, copy dat3.000 from the 3DA folder in to the Oblivion folder as well, and rename it to d3d9.dll. This makes Oblivion use 3DA all the time, so now just use obse_loader.exe like normal. If you buy an actual video card and want to disable 3DA, delete the new d3d9.dll from the Oblivion folder.

I'm using the Steam version of Oblivion and OBSE doesn't seem to be working:

- Go to your Steam Settings page, pick the "In-game" tab, and make sure the "Enable Steam Community In-Game" box is checked.

[ Contact the OBSE Team ]

Before contacting us, make sure that your game launches properly without OBSE first.

Ian (ianpatt) - Designer and lead programmer
Send email to ianpatt+obse [at] gmail [dot] com. (yes, keep the + symbol, it helps me sort email)

Stephen (behippo)
Send email to gamer [at] silverlock [dot] org

Paul (scruggsy)
Send email to scruggsyW [at] comcast [dot] net
