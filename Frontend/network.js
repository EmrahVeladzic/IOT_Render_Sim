import { globalModel, globalReport } from "./globalmodel.js";
import { Matrix4, Quaternion,Vector3 } from "./node_modules/three/build/three.module.js";
import {AnimationBuffer, JointBuffer, MeshBuffer} from './types.js'

const base_addr = "http://192.168.1.96:";
const port = "5166";
const model_addr = "/api/Model/";
const M_req = "Model";
const R_req = "Status";


function delay(milliseconds) {
    return new Promise(resolve => {
        setTimeout(resolve, milliseconds);
    });
}

async function getReportUpdate(){

    await fetch(base_addr+port+model_addr+R_req).then($response=>$response.json()).then($data=>{

        
        globalReport.anim = $data.anim % globalModel.joints[0].animations.length;
    
        globalModel.current_anim=globalReport.anim;
       
    
    
    });
    
   

}





export async function waitForServerReady(){

  while(!globalReport.ready){
    await delay(5000);

    

    await fetch(base_addr+port+model_addr+R_req).then($response=>$response.json()).then($data=>{

        globalReport.ready = $data.ready;
        globalReport.anim = $data.anim;

       


    });



    
    


  }


  await fetch(base_addr+port+model_addr+M_req).then($response=>$response.json()).then($data=>{




    for(var i =0; i < $data.meshes.length;i++){
        var ms = new MeshBuffer();

        ms.vertices = $data.meshes[i].vertices;
        ms.indices = $data.meshes[i].indices;
        ms.normals = $data.meshes[i].normals;
        ms.uvs = $data.meshes[i].uVs;
        ms.joint_index = $data.meshes[i].joint_Index;

        globalModel.meshes.push(ms);

     

    }




   globalModel.current_anim=$data.current_Anim;

   globalModel.image.data=$data.texture.data;
   globalModel.image.width=$data.texture.width;
   globalModel.image.height=$data.texture.height;

 
   

    globalModel.root = $data.root;

   globalModel.sound.sample_count = $data.sound.sample_Count;
   globalModel.sound.sample_rate = $data.sound.sample_Rate;
   globalModel.sound.data = $data.sound.samples;

    for(var i =0; i < $data.bones.length;i++){
        var jb = new JointBuffer();

        jb.root = $data.bones[i].root;
        jb.children = $data.bones[i].children;
        jb.translation = new Vector3($data.bones[i].translation[0],$data.bones[i].translation[1],$data.bones[i].translation[2]);
        jb.rotation = new Quaternion($data.bones[i].rotation[0],$data.bones[i].rotation[1],$data.bones[i].rotation[2],$data.bones[i].rotation[3])
        jb.scale = new Vector3($data.bones[i].scale[0],$data.bones[i].scale[1],$data.bones[i].scale[2]);

        
        jb.matrix.identity();
        const translationMatrix = new Matrix4();
        translationMatrix.makeTranslation(jb.translation.x,jb.translation.y,jb.translation.z);

      
      
        jb.matrix.multiply(translationMatrix);
        const rotationMatrix = new Matrix4();
        rotationMatrix.makeRotationFromQuaternion(jb.rotation);
        jb.matrix.multiply(rotationMatrix);
        const scaleMatrix = new Matrix4();
        scaleMatrix.makeScale(jb.scale.x,jb.scale.y,jb.scale.z);
        jb.matrix.multiply(scaleMatrix);

        

        jb.reset_matrix = new Matrix4().copy(jb.matrix);

        jb.inter_T= new Vector3(0.0,0.0,0.0);
        jb.inter_R = new Quaternion(0.0,1.0,0.0,0.0);
        jb.inter_S = new Vector3(1.0,1.0,1.0);        

        for(var j = 0; j<$data.bones[i].animations.length;j++){

            var an = new AnimationBuffer();

            an.joint_index=$data.bones[i].animations[j].joint_index;
            an.T_index=0;
            an.R_index=0;
            an.S_index=0;
           

            an.joint_index = $data.bones[i].animations[j].joint_Index;

            an.trans_times=$data.bones[i].animations[j].trans_Times;
            an.rot_times=$data.bones[i].animations[j].rot_Times;
            an.scal_times=$data.bones[i].animations[j].scal_Times;

            for(var k = 0; k < $data.bones[i].animations[j].translations.length;k+=3){

                an.translations.push(new Vector3( $data.bones[i].animations[j].translations[k], $data.bones[i].animations[j].translations[k+1], $data.bones[i].animations[j].translations[k+2]));

            }


            for(var k = 0; k < $data.bones[i].animations[j].rotations.length;k+=4){

                an.rotations.push(new Quaternion( $data.bones[i].animations[j].rotations[k], $data.bones[i].animations[j].rotations[k+1], $data.bones[i].animations[j].rotations[k+2], $data.bones[i].animations[j].rotations[k+3]));

            }

            for(var k = 0; k < $data.bones[i].animations[j].scales.length;k+=3){

                an.scales.push(new Vector3( $data.bones[i].animations[j].scales[k], $data.bones[i].animations[j].scales[k+1], $data.bones[i].animations[j].scales[k+2]));

            }
            
           

            

            jb.animations.push(an);

        }
        
        
        globalModel.joints.push(jb);
       
      
       
       
    }
  
    
    setInterval(() => {getReportUpdate();
    
    }, 5000);


});



    
}