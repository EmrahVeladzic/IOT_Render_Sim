import { Matrix4, Quaternion,Vector3 } from "./node_modules/three/build/three.module.js";
import {AnimationBuffer, JointBuffer , MeshBuffer} from './types.js'
import { globalModel} from "./globalmodel.js";

let ready = false;

let port;
let writer;
let reader;
let isOpen = false;document.addEventListener('click', async () => {    if(!isOpen){
    try {
        port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });        const encoder = new TextEncoderStream();
        const outputDone = encoder.readable.pipeTo(port.writable);
        writer = encoder.writable.getWriter();        const decoder = new TextDecoderStream();
        const inputDone = port.readable.pipeTo(decoder.writable);
        
        reader = decoder.readable.getReader();
        isOpen = true;
        
        

         await sendMessage();       
          
         await readLoop();


    } catch (error) {
        console.error('Error connecting to the serial port:', error);
    }
}});



async function sendMessage() {
    const message = 'R_REQ\n';
    await writer.write(message);
}



async function readLoop() {
    let currentLine = ''; 

    while (true) {
        const { value, done } = await reader.read();
        if (done) {
            reader.releaseLock();
            break;
        }

        currentLine += value; 

       
        const newlineIndex = currentLine.indexOf('\n');
        if (newlineIndex !== -1) {
           
            const line = currentLine.substring(0, newlineIndex);                    

            var instr = JSON.parse(line);

            if(instr["Type"]=="Texture"){
               
                globalModel.image.width=instr["Width"];
                globalModel.image.height=instr["Height"];
                globalModel.image.data=instr["Data"];

                

                await sendMessage();
                await readLoop();
            }
            else if(instr["Type"]=="SFX"){
                globalModel.sound.sample_count=instr["SampleCount"];
                globalModel.sound.sample_rate=instr["SampleRate"];
                globalModel.sound.data=instr["Data"];

                
                await sendMessage();
                await readLoop();
            }
            else if(instr["Type"]=="Mesh"){
                let tempMesh = new MeshBuffer();

                tempMesh.vertices = instr["Vertices"];
                tempMesh.indices = instr["Indices"];
                tempMesh.uvs = instr["UVs"];
                tempMesh.normals = instr["Normals"];
                tempMesh.joint_index = instr["Bone"];

                globalModel.meshes.push(tempMesh);            
                
                await sendMessage();
                await readLoop();
            }

            else if(instr["Type"]=="Bone"){
                let tempJoint = new JointBuffer();

                tempJoint.root = instr["Root"];

                tempJoint.translation = new Vector3(instr["T_Init"][0],instr["T_Init"][1],instr["T_Init"][2]);
                tempJoint.rotation = new Quaternion(instr["R_Init"][0],instr["R_Init"][1],instr["R_Init"][2],instr["R_Init"][3]);
                tempJoint.scale = new Vector3(instr["S_Init"][0],instr["S_Init"][1],instr["S_Init"][2]);
                tempJoint.matrix.identity();
                const translationMatrix = new Matrix4();
                translationMatrix.makeTranslation(tempJoint.translation.x,tempJoint.translation.y,tempJoint.translation.z);
                tempJoint.matrix.multiply(translationMatrix);
                const rotationMatrix = new Matrix4();
                rotationMatrix.makeRotationFromQuaternion(tempJoint.rotation);
                tempJoint.matrix.multiply(rotationMatrix);
                const scaleMatrix = new Matrix4();
                scaleMatrix.makeScale(tempJoint.scale.x,tempJoint.scale.y,tempJoint.scale.z);
                tempJoint.matrix.multiply(scaleMatrix);

                tempJoint.reset_matrix = new Matrix4().copy(tempJoint.matrix);

                tempJoint.inter_T= new Vector3(0.0,0.0,0.0);
                tempJoint.inter_R = new Quaternion(0.0,1.0,0.0,0.0);
                tempJoint.inter_S = new Vector3(1.0,1.0,1.0);

                tempJoint.children = instr["Children"];

                globalModel.joints.push(tempJoint);
            
              
                if(tempJoint.root){
                    globalModel.root=globalModel.joints.length-1;
                }

               

                await sendMessage();
                await readLoop();

            }

            else if (instr["Type"]=="Animation"){
                let tempAnimation = new AnimationBuffer();
               
                for (let i = 0; i < instr["Translations"].length; i += 3) {
                    let vector = new Vector3(instr["Translations"][i], instr["Translations"][i + 1], instr["Translations"][i + 2]);
                    tempAnimation.translations.push(vector);
                }               
                
                for (let i = 0; i < instr["Rotations"].length; i += 4) {
                    let quaternion = new Quaternion(instr["Rotations"][i], instr["Rotations"][i + 1], instr["Rotations"][i + 2], instr["Rotations"][i+3]);
                    tempAnimation.rotations.push(quaternion);
                }

                for (let i = 0; i < instr["Scales"].length; i += 3) {
                    let vector = new Vector3(instr["Scales"][i], instr["Scales"][i + 1], instr["Scales"][i + 2]);
                    tempAnimation.scales.push(vector);

                }
               

                tempAnimation.trans_times = instr["T_Times"];
                tempAnimation.rot_times = instr ["R_Times"];
                tempAnimation.scal_times = instr["S_Times"];

                tempAnimation.joint_index = instr["Bone"];


                globalModel.joints[tempAnimation.joint_index].animations.push(tempAnimation);

                

                await sendMessage();
                await readLoop();
            }

            else{
                if(instr["Value"]=="R_READY"){               
                ready=true;
                }
                else{
                    globalModel.current_anim = globalModel.current_anim === 0 ? 1 : 0;                    
                }

                
                await readLoop();
            }
        }
    }
 
    
}

export{ready}