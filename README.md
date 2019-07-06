### La Salle - Universitat Ramon Llull : Master in Advanced Videogame Development
## Final Assignment for Advanced Graphics II Course (MVD) by Andrea Garcia Fàbregas

A small demo to showcase all the main feature of a graphics engine programmed for the duration of my master's degree is presented in this project.
The project shows a small scene with several modifiable objects or features. These are the following:
    - Skybox
    - Terrain
    - Particle system
    - BlendShapes
    - Animation (both basic and skin animations)
    - Tile set
    - AI (navigation AI)
    - Rendering mode (through Volumetric Lights or Deferred Rendering)
    - Light control
   
 A small video is also attached to the project showcasing how the small scene can be edited through a small GUI window activated through ALT+0.
 
----------------------------------------------------------------------------------------------------------------------------------------
 For the small AI for navigation, the instructions are the following:
 The user has a 8x12 grid, made of different floor tiles, in which to draw several paths according to their desires.
 To create a path, the user will have to paint on the tiles of the floor with the different tiles available but always considering a path needs a start point and an end point.

The grid has 4 different types of tiles:
 - Red: marking the end point of a path. This can be selected by pressing the 1 key.
 - Green: marking the start of a path. This can be selected by pressing the 2 key.
 - White: marking a walkable floor tile. This can be selected by pressing the 3 key.
 - Purple: marking a non-walkable floor tile. This can be selected by pressing the 4 key.
 
 There's a 5th type of tile, being a dark green/black tile, this only means the tiles need to be painted by the user.
 
 The player will be active, can move and has gravity, but it does not need to move. Instead it is used as a view point for its camera since it can look around through the mouse freely.  All needed to see the scene, simply move around the scene for the floor tile. It should be located somewhat beneath the player position.
 When the program starts the user has an empty grid in front of it with black tiles, ready for the user to draw on. 
 By pressing the numbers described earlier the user can paint the entire floor. Once it is done, an additional key (being 1 to 4) will need to be pressed in order to start the AI of the program. 
 A teapot will appear on the start point and will move to the end point, if possible. Once it is done it will stay there until the R key is pressed and the whole process is restarted. 
 The user can repaint the floor again if desired.
 
Finally, if the user wishes to restart painting the grid at any time, it can be done by pressing the R key.
----------------------------------------------------------------------------------------------------------------------------------------

Some important notes:
    -There is an issue with the face texture, these is why the element does not carry any texture on and is pitch black. It still functions properly though, as showed in the video. This issue can sometimes make the program crash, if so, restart it and it should work fine.
    -The skinned animation can't go higher than 41.666 ms, without activating (Start) both of their elements. If done the animation will stop when both Stop buttons are pressed.
    - Lights CANNOT be erased during runtime. They will disappear when the project is closed and restarted.
