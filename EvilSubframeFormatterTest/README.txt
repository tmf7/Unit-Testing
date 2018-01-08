This is testing out a new tool for the Editor-of-Evil that reads a Unity .meta file
for multi-Sprite/UI sub-frame data and exports it to and Engine-of-Evil consumable format.

Of special note is that Unity addresses Textures from the bottom-left, while
Engine of Evil (EoE) addresses them from the top-left, and Unity meta files do not contain texture dimensions,
so image heights must manually be added to each .meta file to convert to EoE format

One un-addressed output of this is that Unity also has its sub-frame rectangle data arranged
in rows from the bottom of the image up, while EoE arranges them from the top down. Which has a simple
solution of manually swapping the rows in the final EoE file.

This could be addressed in a future test by re-parsing the output file to automatically swap
the order of rows.

