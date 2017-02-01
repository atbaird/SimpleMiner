Alexander Baird's SimpleMiner
----------------------------------

Controls:
	Shift: Move 8 times as fast as normal
	SPACE & not Z: fly up
	SPACE (walking mode) 
		- while on the ground, jumps upwards
		- while off the ground, allows for operation of the jetpack, which will propel the player upwards so long as there is still fuel in it.
	Z & not SPACE: fly down (unless in WALKING mode, then does nothing).
	E & not D: move forward along XY plain (MOMENTUM IS MAINTAINED, with gradual de-acelleration)
	D & not E: move backward along XY plain (MOMENTUM IS MAINTAINED, with gradual de-acelleration)
	S & not F: straif left (no momentum)
	F & not S: straif right (no momentum)
	F5: Cycle through first person, third person and fixed-angle views
	B: toggle show collision model of Player
	W: select left adjacent block type
	R: select right adjacent block type
	LEFT CLICK: turn block at center of chunk that's not air, into air.
	RIGHT CLICK: place block of presently selected type at center of chunk
	0-9: select from 10 blocks in order to 

Features:
	Camera:
		Look up, down, left and right via the mouse
		move forward, and back via E and D
		move left and right via S and F
		move up and down via SPACE and Z
	Blocks:
		3D cubes drawn via the program.
		Take a bit to destroy, based on type.
		4 block types
			Air - no textures
			Water - 1 texture used for all sides, texture is semi-transparent. collision treated same as Air block type.
			Grass - 3 textures
			Dirt - 1 texture used for all plains
			Cobblestone Block - 1 texture used for all plains
			Stone - 1 texture used for all plains
			Sand - 1 texture used for all plains.
			Sandstone block - 1 texture used for all plains
			Brick block - 1 texture used for all plains
			Gold brick block - 1 texture used for all sides
			Glow stone block - 1 texture used for all sides, projects light from it that's dimmer than skylight.
			Obsidian block - 1 texture used for all sides.
	Chunks:
		A bunch of blocks generated together, via perlin noise
		Height levels between where the air, grass, dirt, sand, and stone blocks start varies slightly; done via rand().
		Randomly generated via Perlin Noise, provided a save file for them does not already exist.
		Saves and Loads the chunks as you visit them or when you quit the game and reload the game.
	Lighting:
		Dirty's block as player clicks to add or remove blocks from the world.
		Dirty blocks get updated per frame, and will, if under cover, darken to simulate light dimming as it gets away from the light source (aka glowstone or the sky).
		
	Game Modes:
		NO_CLIP (fly around the world; ignore collision)
		FLYING (fly around the world; do not ignore collision)
	Debug Text:
		BitmapText class works, demonstrated by automatically printing "Hello World" to the screen.
	Placing and removing blocks:
		left click deletes blocks
		right click places block selected.
		W: wraps about a selection of four blocks in the left direction.
		R: wraps about a selection of four blocks in the right direction.
	Saving and Loading
		Game saves and loads as you close it or leave chunks behind, have to have a save01 folder in the Data folder for this to work.
		

Additional Features:
	JetPack
		- when in walking mode, the player can use a jetpack to fly around.
		- the jet pack has limited fuel, the percentage of which is indicated by a green bar up in the top left hand corner of the screen.
		- holding down space will, after a short delay of 0.5 seconds, cause the player to be propelled up in the air by the rocket, provided that there is still fuel in the tank.
		- after being on the ground for two seconds, fuel will recover at half the rate that it is expended; this fuel recovery only occurs while standing on the ground.
		- a static sound effect plays while the player is using the rocket.
	Randomly Generated Lava
		-For now considered the same as a solid block.
		- Glows in the dark
		- Generated via perlin noise between z = 0 and up to around 3/4ths the way up the stone layer.
	Randomly Generated Caves
		- Generated on chunk creation via Perlin Worms.
		- can go up and down and all around.

Building the Project:
	Download everything out of the perforce folder locations:
		//depot/C25/Students/atbaird/Projects/SD2/SimpleMiner/
	and
		//depot/C25/Students/atbaird/Projects/Engine/

	I forget exactly what issues perforce would cause to projects.  As far as external files to the game's project folder is concerned,
it pulls from my customly build engine; all the code should be in the Engine folder in Perforce; Classes such as ExtendedMath and Vector2.
Other than that, it is pretty self reliant.
	If you open up the Asteroids solution properties, please be sure that the following is true:
		Configuration Properties
			-> General
				Output Directory: $(SolutionDir)Temporary\$(ProjectName)_$(Platform)_$(Configuration)\
				Intermediate Directory: $(SolutionDir)Temporary\$(ProjectName)_$(Platform)_$(Configuration)\
			->C/C++
				->General
					Additional Include Directories: $(SolutionDir)../../Engine/Code/;$(SolutionDir)Code/

	Other than that, I think it should build appropriatly, provided you have both the Asteroids project and my Engine project as it exists in Perforce.

Running the game:
	Additional files required:
	Data/
		Images/
			Test_StbiAndDirectX.png
			SimpleMinerAtlas.png
		Fonts/
			SquirrelFixedFont.png

Testing the game:
	Please test to see that all the controls and features list up above the "Building the Project" section, work as advertised.

Deep Learning:
	As the final submission for this project is drawing near, I've found that my major issues I have had to tackle have not been with my additional features I'd chosen,
but instead with bugs in some of the bigger and nastier things, such as Physics and Lighting.  At this point I am convinced that I have managed to destroy all of the
lighting bugs, but the physics bugs still escape me.
	If anything I'd learned form this project, its that physics is a huge pain to get working correctly, especially in the 3D space, thus perhaps I should not be so
harsh when I find hilarious bugs in triple A games. I would of liked more time to hammer out the physics and also would of liked more experience with the physics
as well. I have presently implemented Corrective method. I could not get the preventative fully working.
	My time management skill has helped me quite a lot between this assignment and balancing all my other assignments. Without it, I have a feeling I would of drowned
under the workload of TGP1 and the like.  This assignment, over all, was pretty darn tough, but I loved every second of it.