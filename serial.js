import { Matrix4, Quaternion,Vector3 } from "./node_modules/three/build/three.module.js";
import { globalModel , tempMesh, tempJoint, tempAnimation} from "./globalmodel.js";

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
                tempMesh.vertices = instr["Vertices"];
                tempMesh.indices = instr["Indices"];
                tempMesh.uvs = instr["UVs"];
                tempMesh.normals = instr["Normals"];
                tempMesh.joint_index = instr["Bone"];

                globalModel.meshes.push({
                    vertices: [...tempMesh.vertices],
                    indices: [...tempMesh.indices],
                    uvs: [...tempMesh.uvs],
                    normals: [...tempMesh.normals],
                    joint_index: tempMesh.joint_index
                });                
                
                tempMesh.vertices = [];
                tempMesh.indices = [];
                tempMesh.uvs = [];
                tempMesh.normals = [];
                tempMesh.joint_index = 0;

                await sendMessage();
                await readLoop();
            }

            else if(instr["Type"]=="Bone"){

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

                tempJoint.inter_T= new Vector3(0.0,0.0,0.0);
                tempJoint.inter_R = new Quaternion(0.0,1.0,0.0,0.0);
                tempJoint.inter_S = new Vector3(1.0,1.0,1.0);

                tempJoint.children = instr["Children"];


                globalModel.joints.push({
                   root : tempJoint.root,
                   translation:tempJoint.translation,
                   rotation:tempJoint.rotation,
                   scale:tempJoint.scale,
                   matrix:tempJoint.matrix,
                   children: [...tempJoint.children],
                   animations:[],
                   inter_T: tempJoint.inter_T,
                   inter_R: tempJoint.inter_R,
                   inter_S: tempJoint.inter_S,
                });                
                
                tempJoint.children=[];

                await sendMessage();
                await readLoop();

            }

            else if (instr["Type"]=="Animation"){
                tempAnimation.translations = instr["Translations"];
                tempAnimation.rotations = instr["Rotations"];
                tempAnimation.scales = instr["Scales"];

                tempAnimation.trans_times = instr["T_Times"];
                tempAnimation.rot_times = instr ["R_Times"];
                tempAnimation.scal_times = instr["S_Times"];

                tempAnimation.joint_index = instr["Bone"];


                globalModel.joints[tempAnimation.joint_index].animations.push({
                    joint_index:tempAnimation.joint_index,
                    translations:[...tempAnimation.translations],
                    rotations:[...tempAnimation.rotations],
                    scales:[...tempAnimation.scales],

                    trans_times:[...tempAnimation.trans_times],
                    rot_times:[...tempAnimation.rot_times],
                    scal_times:[...tempAnimation.scal_times],

                });

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