# mini-host plugin loader

What is Chibi?
---------------

Chibi is a mini-host audio plugin loader, meant to load one plugin at a time as if it was a standalone application.  

These are the goals for the project:
 - Provide a quick way to start audio plugins
 - Integrate with relevant Session Managers
 - Serve as test case for Carla's plugin embeddable UIs

Chibi is basically Carla's little sister.  
It runs Carla's JACK backend behind the scenes and some of its frontend code directly.  
Building on top of what Carla has already achieved, it adds only the missing pieces for a "mini-host" setup.
