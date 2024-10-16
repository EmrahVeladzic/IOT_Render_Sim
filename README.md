# IOT_Render_Sim
An ESP32 + Asp.Net + Three.js project meant to showcase a rendering and audio pipeline.
The ESP32 will convert files using custom formats (which can be created via the LearnGL project converters found on https://github.com/EmrahVeladzic/LearnGL) into data buffers which can then easily be used to render a simple scene using Three.js.

How to use:

1. Wire up the device. You can refer to ESP32/diagram.json for the correct wiring.

2. Using the aformentioned converters, create your own files. Place the generated files (.vt,.ind,.uv,.nrm,.fkr,.anm,.wl,.rpf) onto a FAT32 microSD card and insert it into the card reader of the device. Note: the ESP currently only supports mono audio and 4bpp textures. Ensure you are converting mono wav audio, and that the DEPTH in RPF.h file is set to no more than 16. To create the skeletal mesh files, set the useAST in SystemConfig.h to false. 

3. Edit the launchSettings.json file on the backend and the network.js file on the frontend. Ensure it points to the correct ip/domain you want to host the project on. Alternatively, use localhost.

4. Run the api/Renderer/Initialize and api/Renderer/Select controller actions. You may also edit the frontend if you wish to be able to do it from the frontend. 

5. Click anywhere on the frontend page body to trigger it. It should recieve the assets once they are loaded.

6. Assuming that your model uses multiple animations, you can press the button to trigger an animation change.

