This tests compiling Engine of Evil (EoE) to a dynamically linked library to be linked against
by users. 

The "PureGame" source was compiled to LibrarySpawn.dll. It also tests using the pImpl design pattern to obscure class definitions from users.

The "UserOfGameEngine" source successfully calls a function from within the generated LibrarySpawn.dll.