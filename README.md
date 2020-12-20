in_ncsf - NCSF Winamp Plugin By Naram Qashat (CyberBotX)
[cyberbotx@cyberbotx.com]

(If you need to email me, please include NCSF in the subject line.)

Special thanks to fincs for FeOS Sound System, as well as the DeSmuME team for
their open-source Nintendo DS emulator.

This is a Winamp plugin to play NCSF files. NCSF is a PSF-style music format
that uses SDAT files from Nintendo DS ROMs as it's "program".

Contains:

-     in_ncsf.dll - The NCSF Winamp plugin
- zlib DLL v1.2.8 - Required by the plugin

To allow Winamp to use the plugin, place in_ncsf.dll into your Winamp plugins
directory and zlib1.dll into your Winamp directory (NOTE: NOT the plugins
directory). Once this has been done, start Winamp and you should be able to play
NCSF files.

If you wish to download from GitHub, you may do so from:

http://github.com/CyberBotX/in_xsf

This contains not only the above NCSF plugin, but also plugins for SNSF, GSF,
and 2SF. While the latter 3 are similar to existing plugins for those formats,
my versions utilize the same xSF framework that the NCSF plugin uses. In
addition, all plugins are much more modified than the versions they originate
from.

You may need the Microsoft Visual C++ 2010 Runtime, if you do not already have
it installed. You can obtain it here:

http://www.microsoft.com/en-us/download/details.aspx?id=5555

(NOTE: You will usually not require this runtime if you have Visual Studio
installed, or if you plan on installing Visual Studio. But you will need it if
you do not have Visual Studio installed.)

If you wish to compile the plugin yourself, you can do so with Microsoft Visual
Studio. You must use version 2010 or later. If you do not have Microsoft Visual
Studio, you can obtain the Express version of Microsoft Visual C++ from
Microsoft, at:

http://www.microsoft.com/visualstudio/eng/downloads

You may use any Express version that is 2010 or later. Once you have Visual
Studio installed, you need to open the SDATStuff.sln file. If using a version of
Visual Studio later than 2010, you may be asked to upgrade the solution.

Of note, the utilities will not compile out of the box immediately. First, you
need the Winamp SDK. This is not included with the plugin's source code. You can
obtain the SDK from here:

http://download.nullsoft.com/winamp/plugin-dev/WA5.55_SDK.exe

The zlib library is also required. zlib can be obtained from here:

http://www.zlib.net/

You need to download the "compiled DLL" version.

Once you have this, you need to modify the values of zlibRootDir and
WinampSDKDir in src\in_xsf_framework\common.props. Just replace ZLIBFIXME with
the directory to which you extracted zlib to, and replace WINAMPSDKFIXME with
the directory to which you installed the SDK to. Once this has been done, you
can compile the solution.
