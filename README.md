# EngineForAnimationCourse
Graphic Engine based on Libigl

For compiling:
1. Clone or download the project
2. Download Cmake from the link in the site
3. Run Cmake gui. choose the project folder and destination folder for the cpp project files. choose after pressing configure choose compiler (VS2019 for example). After finish configuration successfully, press configure again and after it finishes press generate. 
4. If everything pass successfully got to the destination folder and launch the project. 
5. Copy configuration.txt from tutorial/sandBox to build/tutorial/sandBox sandBox as a startup project and compile the project (it could take few mineutes);   


Submitters:
    - Guy Madmon 318465671
    - Ido Shimshi 314800582

Git Link: 

Keys:
    Escape - Close program
    Space - Play/Pause animation
    Arrows - Rotate camera
    W|A|S|D - Move picked objects / Move camera - on X/Y axis
    E|R - Move Picked objects on z axis
    C - Next cube map
    M - Change texture mode for picked objects
    F - Focus Camera on picked objects
    G - Set Camera to default location
    LeftControl - Control left screen
    RightControl - Control right screen

What we did:
    - Moving objects when animate - Moving the objects according to Bezier curve
    - Adding cameras, Switching between cameras for both screens
    - Moving the camera left, right, up, down, forward and backward
    - Multi picking and highlight picked objects (when design): Zoom on object, move object
      when chosen with keys, rotate object when chosen with left mouse button
    - Cubemap – more than one background, switching with a key (specified above)
    - Highlight picked objects
    - Menu which includes:
        - Load object option, the new object will appear at (0,0,0)
        - Add layer option. Every object is attached to a layer. Layer can be hide or unhide
        - Set layer to picked objects
        - Split screen around y axis or x axis.
        - Choose main camera for each screen
        - Add camera option: Adding camera and switch between cameras at each screen
        - Time slider (define how much time after play button hits the animation will run).
        - Play checkbox for starts and stops the animation
        - Add/Change Material to picked object (each object can have only one material)
        - Zoom in to a chosen area and zoom out to a default state.
        - Set Transparent objects
        - Set far coeff
        - Set blur objects
        - Add bezier movement :
            1) Add points
            2) Choose time
            3) Select objects
            4) Add bezier movement
        - Add childs to parent :
            1) Select object to be childs
            2) Press choose childs
            3) Select object to be parent (only 1)
            4) Press Set parent
    - Transparent objects

Specials:
    - Blur far objects & blur from menu
    - Change the texture mode of picked objects

Difficulties
    - Understanding how renderer works, drawinfos, viewports.
    - Managing bezier movements
    - Gaussian blur, pick the right constants
    - Picking - set viewports correct and call the right functions


