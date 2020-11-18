# ofxTCPSyphonServer

Sending image data over network to multiple (low powered) devices like Raspberry PI, Orange Pi, Banana pi, etc.<br><br>
Attempting to recreate the functionality of TCPSClient from https://techlife.sg but completely open source, based on openframeworks.
The Client component can be found here: https://github.com/Andymann/ofxTcpSyphonClient.
<br>
<br>
Dependencies:<br>
ofxNDI https://github.com/nariakiiwatani/ofxNDI <br>
ofxTurboJpeg https://github.com/Andymann/ofxTurboJpeg <br>


The name is actually misleading in mutlitple ways:
It doesn't use Syphon but NDI and the only thing it has in common with the original version from techlife.sg is the structure of the header data that is added to the network payload. Those are derived from techlife's idea. See this repo for more information: https://github.com/Andymann/tcpSyphonServer_Java.

This software is NOT compatible with the TCPSyphonServer /-Client from techlife.sg. For the original version have a look over here: https://github.com/z37soft
