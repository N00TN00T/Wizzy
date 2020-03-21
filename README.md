# Wizzy Engine
Wizzy engine aims to be a cross-platform interactive application engine for creating any form of interactive applications from a 3D modelling application or a 2D graphics application to a game engine.

## Building

All files linked for this example are from my website under https://charlie-malmqvist.com/Wizzy/Examples/MarioApp/.

Wizzy Engine is currently not as smooth as it could be to use as a dependency for your project, but it's definitely possible. In its current state, it's recommended that you simply create a client project to be part of the Wizzy workspace. This will let you easily link to all dependencies and Wizzy while also being able to pull updates and rebuild them without issues. It would look like something like this:

- `git clone https://github.com/N00TN00T/Wizzy`

- `cd Wizzy`

- `git submodule update --init`

- `cd ..`

- `mkdir MyProject`

Then, in MyProject directory, create a pch.h, pch.cpp and MyProject.cpp. Make sure that pch.cpp includes `#include "pch.h"` and that pch.h includes `#include <wzpch.h>`. 
[Here](https://charlie-malmqvist.com/WIzzy/Examples/MarioApp/MarioApp.cpp) is some example code for MyProject.cpp that simply renders an image of mario and shows how to set things up.

Now, to generate projects and link everything you will need a premake script. Create a file called premake5.lua in the root directory, above Wizzy and MyProject. The example script can be found [here](https://charlie-malmqvist.com/Wizzy/Examples/MarioApp/premake5.lua).


To invoke the script with premake you can use the premake binary in the repo:
`Wizzy\vendor\bin\premake\windows\premake5.lua XXXX`
Replace 'XXXX' with whatever target you want to generate project files. Supported targets are:
-codelite          Generate CodeLite project files
-gmake             Generate GNU makefiles for POSIX, MinGW, and Cygwin
-gmake2            Generate GNU makefiles for POSIX, MinGW, and Cygwin
-vs2005            Generate Visual Studio 2005 project files
-vs2008            Generate Visual Studio 2008 project files
-vs2010            Generate Visual Studio 2010 project files
-vs2012            Generate Visual Studio 2012 project files
-vs2013            Generate Visual Studio 2013 project files
-vs2015            Generate Visual Studio 2015 project files
-vs2017            Generate Visual Studio 2017 project files
-vs2019            Generate Visual Studio 2019 project files

Now, in the root directory, create a directory called 'res'. You will need two resources for this app:

1. This Image:<br>![Mario](https://charlie-malmqvist.com/Wizzy/Examples/MarioApp/mario.png)
2. [A basic 2D shader](https://charlie-malmqvist.com/Wizzy/Examples/MarioApp/texture2d.shader)

As you build the project (from the Wizzy workspace) and run, you should get this result:

![Wizzy Mario](https://github.com/N00TN00T/Wizzy/repo-assets/wizzy-mario.png)
