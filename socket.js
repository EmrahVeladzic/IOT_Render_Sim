import {globalModel} from './globalmodel'

globalModel.root=21;

const socket = new WebSocket();

socket.onmessage = function(event) {
 
  const jsonData = JSON.parse(event.data);
  
  console.log('Received JSON data:', jsonData);
};
