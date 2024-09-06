import { Matrix4, Quaternion,Vector3} from './node_modules/three/build/three.module.js';

const INTERP_TRESHOLD = 0.9;

function transToMat(T , R, S){
    let outM = new Matrix4().identity();

    let outT = new Matrix4().identity();
    let outR = new Matrix4().identity();
    let outS = new Matrix4().identity();

    outT.makeTranslation(T.x, T.y, T.z);
    outR.makeRotationFromQuaternion(R);
    outS.makeScale(S.x, S.y, S.z);

    outM.multiply(outT);
    outM.multiply(outR);
    outM.multiply(outS);

    return outM;
}

function getInterpolationValue(current, begin, end) {
    if (begin > end) {
        end += begin;
    }

    const range = end - begin;
    const value = ( (current - begin) / range ) ;

    

    return Math.min(Math.max(value, 0.0), 1.0);
}

class AnimationBuffer{

    translations;
    rotations;
    scales;
    trans_times;
    rot_times;
    scal_times;
    joint_index;

    T_index;
    R_index;
    S_index;

    constructor(){
        this.T_index=0;
        this.R_index=0;
        this.S_index=0;

        this.translations =[];
        this.rotations =[];
        this.scales =[];

        this.trans_times=[];
        this.rot_times=[];
        this.scal_times=[];
        this.joint_index=0;

    }
}

class JointBuffer{

    translation;
    rotation;
    scale;   
    children;
    root;

    inter_T;
    inter_R;
    inter_S;

    matrix;
    reset_matrix;

    animations;

    constructor(){
        this.translation= new Vector3(0.0,0.0,0.0);
        this.rotation = new Quaternion(0.0,1.0,0.0,0.0);
        this.scale = new Vector3(1.0,1.0,1.0);
        this.matrix = new Matrix4();
        this.matrix.identity();
        this.reset_matrix = new Matrix4();
        this.reset_matrix.identity();
        this.animations= [];
        this.children=[];
        this.root=false;
        this.inter_T= new Vector3(0.0,0.0,0.0);
        this.inter_R = new Quaternion(0.0,1.0,0.0,0.0);
        this.inter_S = new Vector3(1.0,1.0,1.0);
    }

   

}

class MeshBuffer{

    vertices;
    indices;
    uvs;
    normals;
    joint_index;

    constructor(){
        this.vertices=[];
        this.indices=[];
        this.uvs=[];
        this.normals=[];
        this.joint_index=0;
    }

}

class ImageBuffer{

    data;
    width;
    height;

    constructor(){
        this.data=[];
        this.width=0;
        this.height=0;
    }

}

class SoundBuffer{

    data;
    sample_count;
    sample_rate;

    constructor(){
        this.data=[];
        this.sample_count=0;
        this.sample_rate=0;
    }
    
}

class Model{    

    joints;
    meshes;
   
    image;
    sound;

    root;

    current_anim;

    constructor(){
        this.meshes = [];
        this.joints = [];
        
        this.image=new ImageBuffer();
        this.sound=new SoundBuffer();
        this.current_anim = 0;
        this.root = 0;
    }


    reset_pose(){

        this.joints.forEach(joint => {          

            joint.matrix = new Matrix4().copy(joint.reset_matrix);

           
        });
    }
    
    add_parent_matrix(parentMatrix, joint) {
        

        
        let newMatrix = parentMatrix.clone().multiply(this.joints[joint].matrix);
        
        

        this.joints[joint].matrix = newMatrix;      
        
        for (let i = 0; i < this.joints[joint].children.length; i++) {
            this.add_parent_matrix(newMatrix, this.joints[joint].children[i]);
        }
    }

    interpolate_matrix(in_val) {
      
        this.joints.forEach(joint => {
          
           
           

            let T_end = ((joint.animations[this.current_anim].T_index + 1) < joint.animations[this.current_anim].trans_times.length) ? joint.animations[this.current_anim].T_index + 1 : 0;
           
            let T_val = getInterpolationValue(in_val, joint.animations[this.current_anim].trans_times[joint.animations[this.current_anim].T_index], joint.animations[this.current_anim].trans_times[T_end]);

            

            let T_current = new Vector3().copy(joint.animations[this.current_anim].translations[joint.animations[this.current_anim].T_index]).lerp(joint.animations[this.current_anim].translations[T_end], T_val);

           


            if (T_val > INTERP_TRESHOLD) {
                joint.animations[this.current_anim].T_index++;
                if (joint.animations[this.current_anim].T_index >= joint.animations[this.current_anim].trans_times.length) {
                    joint.animations[this.current_anim].T_index = 0;
                }
            }

            let R_end = ((joint.animations[this.current_anim].R_index + 1) < joint.animations[this.current_anim].rot_times.length) ? joint.animations[this.current_anim].R_index + 1 : 0;
         
          
            let R_val = getInterpolationValue(in_val, joint.animations[this.current_anim].rot_times[joint.animations[this.current_anim].R_index], joint.animations[this.current_anim].rot_times[R_end]);

            let R_current = new Quaternion().copy(joint.animations[this.current_anim].rotations[joint.animations[this.current_anim].R_index]).slerp(joint.animations[this.current_anim].rotations[R_end], R_val);

            if (R_val > INTERP_TRESHOLD) {
                joint.animations[this.current_anim].R_index++;
                if (joint.animations[this.current_anim].R_index >= joint.animations[this.current_anim].rot_times.length) {
                    joint.animations[this.current_anim].R_index = 0;
                }
            }

            let S_end = ((joint.animations[this.current_anim].S_index + 1) < joint.animations[this.current_anim].scal_times.length) ? joint.animations[this.current_anim].S_index + 1 : 0;

            let S_val = getInterpolationValue(in_val, joint.animations[this.current_anim].scal_times[joint.animations[this.current_anim].S_index], joint.animations[this.current_anim].scal_times[S_end]);

            let S_current = new Vector3().copy(joint.animations[this.current_anim].scales[joint.animations[this.current_anim].S_index]).lerp(joint.animations[this.current_anim].scales[S_end], S_val);

            if (S_val > INTERP_TRESHOLD) {
                joint.animations[this.current_anim].S_index++;
                if (joint.animations[this.current_anim].S_index >= joint.animations[this.current_anim].scal_times.length) {
                    joint.animations[this.current_anim].S_index = 0;
                }
            }    
            
            
            let newMatrix = new Matrix4().copy(transToMat(T_current,R_current,S_current));

            
           

            joint.matrix = newMatrix;

           
            
        });

    }

    
}

class Report{

    ready;
    anim;

    constructor(){
        this.ready=false;
        this.anim=0;
    }

}

export {Model,AnimationBuffer,JointBuffer,MeshBuffer,ImageBuffer,SoundBuffer,Report} 