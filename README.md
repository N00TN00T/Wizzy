# Wizzy
Wizzy Engine is a cross-platform 3D Game Engine.

Mac not yet supported

To generate project with premake use the premake5 executable in vendor/bin/premake and run 'vendor/bin/premake/YOUR_PLATFORM/premake5 PROJECT_TYPE' in repo directory.

Project types that can be generated by premake 5 are:
- 'codelite'  - Genereate CodeLite project files
- 'gmake'     - GenereateGNU makefiles for POSIX, MinGW, and Cygwin
- 'gmake2'    - GenereateGNU makefiles for POSIX, MinGW, and Cygwin
- 'vs2005'    - Generate Visual Studio 2005 project files
- 'vs2008'    - Generate Visual Studio 2008 project files
- 'vs2010'    - Generate Visual Studio 2010 project files
- 'vs2012'    - Generate Visual Studio 2012 project files
- 'vs2013'    - Generate Visual Studio 2013 project files
- 'vs2015'    - Generate Visual Studio 2015 project files
- 'vs2017'    - Generate Visual Studio 2017 project files
- 'xcode4'    - Generate Apple Xcode 4 project files

On linux you will need to install the packages 'xorg' and 'openbox' to be able to compile Wizzy.
