# sand

Is a little sand engine, written in C++ compiled to web assembly.

Left click to add sand </br>
Right click to add stone </br>
Arrow keys to move </br>

* Uses dirty rectangles to only updates changing atoms
* Uses chunks to split up the world, allowing for infinite terrain
* An implementation that uses multi-threading to increase performance, however it does not actually improve performance :P 
