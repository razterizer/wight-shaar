# Wight-Shaar

This game is from a final homework assignment and game competition in the course "Advanced Computer Graphics" tought by Ulf Assarson at Chalmers University of Technology in Gothenburg 2006.
I didn't win unfortunately, both because of good competition but mainly - I think - due to a too physically accurate simulation, rendering it extremely hard to play well.

This is a simple `C++` / `OpenGL` /  `GLUT` game that I know runs on Windows. It might work on Linux/BSD/Unix or whatnot with some minor modifications (if at all) I guess.

The last compiled exe from that time is included and it still works (tested today at the moment of writing; 2024-11-05).

Here's the original README:

Wight-Shaar 1.3.5
=================

1. LICENSE AGREEMENT
--------------------
All sourcecode in this folder that makes up the framework for the game and the
game itself is created by the author, B. Rasmus Anthin and as such is
proprietary goods of the author.

Altering or redistributing the code without the owner's permission is strictly
prohibited. The code does not come under a license per se, but is copyrighted
by the owner/author of the code.

All artwork
(except for the procedural stars/nebula texture "Star Field"
created by Nate Ryan for the modelling/rendering application Art of Illusion:
http://www.housepixels.com/aoitiki/tiki-index.php?page=Texture%20Repository&pagenum=5)
is created by the author, B. Rasmus Anthin. The same rules for the source code
also applies for the artwork.

Copyright (c) 2006-05-18, B. Rasmus Anthin.
Gothenburg, Sweden.


2. ABOUT THE FRAMEWORK
----------------------
The game is built on a framework consisting of a ODE solver, collision
detector, scene-graph, a group of geometry classes, transformation classes and
a texture repository/handler.
The scene-graph is not a scene-graph in its classical meaning, i.e. a graph
that contains lights, cameras and objects, but rather just contains internal
nodes and leaf nodes that contain pointers to the atomic geometry data
(groups of faces with material/texture data).

The framework is moderately easy to use but may need some tweaking to work in
a canonical manner. Some solutions are less elegant than others. For example,
since geometry data is shared during tree-copying deleting the tree must be
performed according to a specific routine. First the sub-tree containing one
instance of the shared geometry must be freed using the method:
"free_upon_delete", then that subtree is to be deleted using the "remove"
method, then the superior tree containing the rest of the references to the 
shared geometry must be told not to free that data upon deletion of the tree
using "free_upon_delete" once more and then finally, the tree can be safely
deleted. This must be performed for all geometry that is shared in other parts
of the tree.
Geometry sharing can be bypassed by repeatedly importing or creating geometry
anew for each instance of a subtree/object, but this is not recommended since
the redundant geometry will take up unnessecary memory.

2.1 ODE

The ODE solver is a abstract class from which two classes Euler and RungeKutta
are derived. The solver takes a Vessle class (which is a GBase node containing
rigid body data) and performs state changes on it each time the method "step"
is invoked. The solver is initialized with a function pointer to one of two
predefined models: "diff_rigid" and "diff_spring". Additional models can be
added.

2.2 CD

The collision detector uses the AABBs stored in the scenegraph (the scengraph
is also a AABB-BVH with OBBs in the leaf nodes) and tests intersections between
two boxes in the tree. Rather, the CD takes two branches from the scene graph
as arguments and tests collisions between those. Upon collision, the CD calls
a function from a function pointer (which is initialized via the constructor)
in which collision actions are taken. The function pointer takes a void
funciton with two arguments both being pointers to a GBase node (those that
collided).

2.3 Texture

The Texture class is a texture manager that both stores the textures and binds
them using "glBindTexture". Textures are then retrieved by name using the
"render(const string&)" method which is called prior to rendering a object that
already has its face- or vertex-normals set (preferrably a display list).
A static Texture object is available for all GBase nodes which manages the
textures in the scene. It is also possible to set individual loading
priorities.

2.4 Scene Graph

The scene graph is built on the base class GBase. The more specialized classes
GObject, GFunc and Vessel all inherits from GBase directly. GObject is a
container for one atomic chunk of geometry that has exactly one diffusive
texture and one set of material properties. GObject uses this geometry to
derive the extents of its OBB from which an AABB is derived. GFunc is a very
lightweight class that more or less just contains a function pointer to a void
function with no arguments that contains the geometry (a display list is
compiled from this function). GFunc also contains some administrative methods.
Vessel perhaps has more in common with GBase than the previously mentioned sub-
classes, in that it does not have to be a leaf node. GObject and GFunc must
both be leaves or else an exception is raised. Vessel contains rigid body data
such as inertia tensor, mass and state-variables. It also contains a Thuster-
pointer from which an array of thrusters can be built. Vessel also contains
some thruster managing routines, such as binding thruster orientations to a
specific sub-node under a Vessel-node instance. Ship inherits from Vessel and
contains a pointer to a Torpedo object. The class Torpedo is also a subclass
of Vessel. The Ship class contains a method for setting the reference to a
torpedo node (could and should be a sibling node) and a method for activating
that torpedo that is referenced to by this pointer.

A subtree of the scengraph should invoke its "set_ancestor" function only for
such subtrees which contains leaves that are to be tested for collision.
This makes it possible to identify which node that owns a set of leaves
(collision detection terminates at leaf nodes). This is especially useful
for Ship type objects (nodes) and Torpedo objects.


3. ABOUT THE GAME
-----------------

Wight-Shaar is an attempt to get the look and feel of the old Lucasarts-
classics such as TIE-fighter and X-wing alliance and make such types of games
available for platforms other than Windows, such as Linux, BSD and UNIX.
Joystick control is not yet supported. Depending on how much more time I have
available to spend on developing this game, I just might consider adding
support for joystick.

The physics model for the rigid body motion is perhaps a bit too realistic. It
is simply too hard for most people to steer the ship and to quickly get a good
aim to shoot. Perhaps using Hollywood physics is justified after all!

Condsidering it took about one month to accomplish the backbone of the game as
it is now, it would take perhaps 4 months at the same rate - 1 month give or
take - to make it a "fully" working game. However, I won't have that much time
at my disposal, so expect it to take triple or quadruple that amount of time.


4. INSTALLATION
---------------

To be able to run this game on your computer, make sure you have STL libraries
installed. You will also need the makefile system and of course you need a c++
compiler. Gnu's c++ compiler g++ will suffice.
There will be a precompiled executable for Debian "wight-shaar"
and one for Windows "wight-shaar.exe". Try these first.

4.1 Windows

Click on the file "wight-shaar.exe" to start the game. Or build from source:

Download Minimalist Gnu for Windows (MinGW) from
   http://www.mingw.org/
and make sure you have installed MSYS. Go to the game directory and type:
"make windows".
Finally type wight-shaar (optionally with the flag "-help") on the DOS/MSYS
prompt or simply click on the exe-file in a file browser.

4.2 Linux/BSD/UNIX

In Linux, it is pretty much the same way. First try to execute "./wight-shaar"
and if that fails, compile from source:
Go to the wight-shaar-x.x.x directory in a terminal window and type "make".
Run using ./wight-shaar (optionally with the flag "-help").

4.3 Other

Unpack the contents to some directory, go to the "src" directory then
compile using "make -f Makefile.os" where "Makefile.os" is the makefile
modified to work for your OS.

Enjoy!


5. TROUBLESHOOTING
------------------
Email to *******@gmail.com if you experience any problems regarding the
installation or the game itself. Use the flag "-help" for a list of possible
command line options.


/B. Rasmus Anthin
2006-05-31 00:58 (GMT+1)


EOF
