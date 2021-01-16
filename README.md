# GomuGomu Engine
Engine being developed during UPC's Masters Advanced Programming for AAA video games.

# About the engine
This is the first implementation of the engine. Right now it can load models, textures and move through the scene with a camera. It also has an editor to play with settings of different modules. The functionalities correspond to the ones asked for the first assignment and it will be updated in the next weeks/months.

It's named after the Gomu Gomu no Mi devil fruit from my favourite manga (One Piece) to represent the ability of this engine to adapt to any feature needed.

# Authors
Guillem Burgués Miró - https://github.com/GBMiro/
David Sierra González - https://github.com/devildrake

# Source Code
You can find the code in the following link: https://github.com/GBMiro/GomuGomuEngine

# Engine Controls

- **Alt left + Mouse left click :** Orbital camera around the model's center
- **Right click + W/S/A/D/Q/E:** Free look with the camera and you can move forward/backwards, left/right and up/down with the previous keywords
- **Mouse wheel:** Camera zoom in and zoom out
- **F:** Focus the camera on the current model's center keeping your camera direction
- **Left shift:** Duplicates the speed when using W/S/A/D/Q/E to move the camera position
- **ESC:** Closes the engine.
- **Drag and drop:** You can load any model in fbx format with drag and drop. Upon dropping an picture in .png, .jpg and .tif, said texture will be loaded in the engine so it may be selected on the Mesh Renderer Component display, under material, on the Properties window.
- **Alt left + Mouse right click :** Move camera forward/backwards	
- **Left click on a GameObject inside the scene: Select said GameObject, there's several options as to how this is done. More on the Configuration Section.
- **Transform Gizmos: ** If a Gameobject is selected, a transform gizmo will be shown, said gizmo can be toggled with W/E/R for Translation/Rotation/Scale, Locald/Global gizmo modes can be switched with L/G
- **Play/Stop Buttons: ** Pressing Play button will make an aux copy of the scene, pressing Stop button will reload said aux copy of the scene, rendering all changes made in "play mode" ignored.


**NOTES:** By default, the provided fbx models load the png texture but you can drag and drop the dds ones.

## Engine Editor

The editor has a main bar with 3 tabs:

- **File:** Right now shows the option to quit the engine
- **Windows:** You can show/hide editor windows through this tab. By default all windows are close
- **About:** Contains a link to the source code and you can read a short description of the engine.

### Editor Windows

- **Configuration:** You can toggle various options from the camera (zNear/zFar, FOV, camera speed...), textures (Min/Mag filter, MipMap, WRAP...), window (width, height...),input(mouse sensitivity), rendererer (tonemapping/Gamma correction), Scene (Ambient light, Frustum Culling), Editor (How the Mouse picking is done, wether via QuadTree accel or directly, Frame rate capping, Vsync toggling).
- **Monitor:** Shows information about the hardware the engine is running on, libraries that are being used and some performance data
- **GameObject Hierarchy:** Current scene GameObjects will be displayed here in a hierarchichal way, users can drag/drop GameObjects on top of others to modify the hierarchy. Right clicking on hierarchy will show an option to Create a GameObject, if a GameObject was selected, the Destroy button will also be shown. This can also be done by perssing Supr with a selected GameObject
- **Properties:** If users have a slected GameObject on the hierarchy window, this window will display all components contained withing the GameObject, each Component with its different exposed variables.
- **Console:** Shows all LOGs made by the code, can be cleared by right clicking on it and pressing button named clear




# Comments
- If you download the source code and open it with visual studio, before executing you need to set the project working directory to $(ProjectDir)/Game so it can find all the .dll files
- If you can't close the engine through the "X" in the upper bar, you may need to close all window editors that are not docked.
- If you can't load a texture, take a look into the log window to get more details. The editor will load a black texture in case it could not find the one dropped
- Sometimes the combo box for Min filters may not let you select a filter. Try resizing the configuration editor window