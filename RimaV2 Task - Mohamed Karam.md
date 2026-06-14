RimaV2 Task - Mohamed Karam



Project changes:



\*Game feature folder:

&#x20; Plugins/RimaTask\_MohamedKaram



\*Applied changes to default game tags

\*Applied a change to AbilitySet\_ShooterHero of shootercore (Added grapple ability)



\*grapple hook input action is in All/Game/Input/Actions - **it's also wired to J not E**, cuz E does sth else and I didn't want to break things :(



\*GC\_GameplayCue\_RimaRifle\_Damage and GC\_GameplayCue\_RimaRifle\_Heal (game play cues for primary rifle) are in Plugins/ShooterCore/GameplayCues

&#x20;- although not showing for some reason, even though the print string debug message shows





CPP classes:

\- RimaRifle --> not used, was just a starting point that didn't go anywhere

\- RimaRifleExecution --> the actual ally/enemy calculation. also applies the chain link effect based on a tag called from the GA\_RimaRifle blueprint (found in Plugins/RimaTask\_MohamedKaram/Content/Equipment)

\- RimaGameplayAbility\_Grapple --> grapple hook ability logic





Main Blueprints:

\- GA\_RimaRifle (Plugins/RimaTask\_MohamedKaram/Content/Equipment) --> in the eventgraph I do the sphere cast to apply the chain link heal/dmg spec. effect itself is calculated in RimaRifleExecution (mentioned above)

\- pretty much everything else is duplicated/reused/mimicked to Lyra's base stuff (including weapon/equipment hierarchy, basic experience definitions, basic ability sets, native and granted abilities...etc)





How and where to test:

\- my map of testing --> L\_ShooterGym\_RimaTask (Plugins/RimaTask\_MohamedKaram/Content/Maps)

\- for ease of testing I have a teamsetup blueprint in Plugins/RimaTask\_MohamedKaram/Content/Experience, basically if I want to test peacefully with no enemies firing I delete the second team spawning

&#x20; if I want to test enemy stuff (dmg) I re-add it.

\- character gets the weapon from the light purple weapon spawner to the right, it's surrounded with dmg plates cuz AI for some reason rushes to it, so I figured it would be a good idea to test healing there



Multiplayer:

* sadly I couldn't test multiplayer, maybe it works tho :)



Having values being tunable:

* I have the right idea about this but couldn't get it in time:

  * I'd make a data asset hold params for each ability
  * adjust my classes/blueprints to use data from the DA instead of raw data in the code/blueprint



Builds:

* sadly I didn't have time to build as well :( 

















