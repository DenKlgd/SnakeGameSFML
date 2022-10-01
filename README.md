# SnakeGameSFML
A simple snake game based on SFML

![image](https://user-images.githubusercontent.com/114682454/193425233-f2dfcd21-b8f4-4c29-a178-523e7bb86f14.png)

![image](https://user-images.githubusercontent.com/114682454/193425245-23f24f89-ca22-437a-9801-1b250d4add13.png)


# How to build

If you build from my solution, skip to the step 2.

1)In the project's properties, add:

*The path to the SFML headers (<sfml-install-path>/include) to C/C++ » General » Additional Include Directories;

*The path to the SFML libraries (<sfml-install-path>/lib) to Linker » General » Additional Library Directories.

![image](https://user-images.githubusercontent.com/114682454/193425702-35d46d65-03b0-49d5-b681-8d08ffb5db27.png)

These paths are the same in both Debug and Release configuration, so you can set them globally for your project ("All configurations").

The next step is to link to the SFML libraries ("sfml-xxx-d.lib" for Debug, and "sfml-xxx.lib" for Release):

-sfml-graphics.lib;

-sfml-window.lib;

-sfml-audio.lib;

-sfml-system.lib.

![image](https://user-images.githubusercontent.com/114682454/193426014-e51beb13-365f-4028-a008-03130cf2c311.png)


2)After compilation, you need to copy "images" and "sounds" folders and wiguru-13 font to the directory of .exe file.

Also you need to copy the following DLLs from SFML-2.5.1/bin to the directory of .exe ("sfml-xxx-d-2.dll" for Debug or "sfml-xxx-2.dll" for Release): 
  
  -openal32;
  
  -sfml-audio-2;
  
  -sfml-graphics-2;
  
  -sfml-system-2;
  
  -sfml-window-2.
