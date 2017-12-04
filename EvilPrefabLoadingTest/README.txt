- This test uses Game to parse a "map" file, but in EngineOfEvil it will be its eMap.

- Users can derive from ePrefabFactoryBase and implement and override the CreatePrefab method,
which can construct the prototype for eEntity types to be instanced

- eMap will have a std::unique_ptr<ePrefabFactoryBase> so that users can configure it with their own PrefabFactoryXYZ (abstract factory/strategy)

- Dictionary uses string-string key-value pairs to simulate "polymorphic" POD-type behavior without inheritance,
but a proper template-inheritance blackboard could implement non-POD types (eg: composite classes)

- The global Dictionary globalSpawnArgs gets populated as eMap reads the .map file and sees prefabs that need instancing/initializing (Spawning),
at which point eMap calls prefabFactory->CreatePrefabInstance (which polymorphically calls Spawn on the eEntity instance in PrefabFactoryBase::prefabEntities)

- All eEntity classes that override eEntity::Spawn will have to make a copy of the prefab instance (eEntity::this) and
initialize that new instance throughout Spawn, then **move** that new instance to eGame::entities via eGame::AddEntity.
(otherwise, the new instance gets destroyed by its std::unique_ptr going out of function scope,
OR--if directly initializing and **moving the prefab** into eGame::entities--the prefab in PrefabFactoryBase::prefabEntities becomes undefined)

- Within overridden eEntity::Spawn functions users can add and initialize custom elements to their eEntity-derived class,
for example Behaviors and Behavior-derived classes, which is what is used in this test

- Again, with overridden eEntity::Spawn functions users can make a copy of globalSpawanArgs if needed, or just read from it (before eMap clears it)
to get user-defined initialization values (eg: xposition: 32 behavior: behavior2, etc as seen in the .map file)

- This also tests copying derived component (eg: Behavior) types in the eEntity copy ctor/assignemnt by calling the virtual GetCopy method of the component class (eg: DerivedBehavior2),
without type-splitting or casting