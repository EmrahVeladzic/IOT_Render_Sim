import { Matrix4, Quaternion,Vector3} from './node_modules/three/build/three.module.js';

const INTERP_TRESHOLD = 0.99;



function getInterpolationValue(current, begin, end) {
    if (begin > end) {
        end += begin;
    }

    let out = (current - begin) / (end - begin);

    if (out > 1.0) {
        out = 1.0;
    }
    if (out < 0.0) {
        out = 0.0;
    }

    return out;
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

    animations;

    constructor(){
        this.translation= new Vector3(0.0,0.0,0.0);
        this.rotation = new Quaternion(0.0,1.0,0.0,0.0);
        this.scale = new Vector3(1.0,1.0,1.0);
        this.matrix = new Matrix4();
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

    current_anim;

    constructor(){
        this.meshes = [];
        this.joints = [];
        
        this.image=new ImageBuffer();
        this.sound=new SoundBuffer();
        this.current_anim = 0;
       
    }

    
    add_parent_matrix(in_matrix, joint){

        let tempMat = this.joints[joint].matrix;


        //this.joints[joint].matrix = in_matrix.multiply(tempMat);

        /*

        for (let index = 0; index < this.joints[joint].children.length; index++) {
           this.add_parent_matrix( this.joints[joint].children[index].matrix,  this.joints[joint].children[index].children[index]);    
        }

        */

    }

    interpolate_matrix(in_val) {
        this.joints.forEach(joint => {
            joint.matrix.identity();

            
            let animation = joint.animations[this.current_anim];

            let T_next = animation.T_index == (animation.trans_times.length) ? 0 : animation.T_index + 1;

            let T_Begin = animation.trans_times[animation.T_index];
            let T_End = animation.trans_times[T_next];

            let T_out = getInterpolationValue(in_val, T_Begin, T_End);

            //let T_current = new Vector3().copy(animation.translations[animation.T_index]).lerp(animation.translations[T_next], T_out);

            let T_current = new Vector3();

            let T_Mat = new Matrix4();
            T_Mat.makeTranslation(T_current);
            joint.matrix.multiply(T_Mat);

            if (T_out > INTERP_TRESHOLD) {
                animation.T_index++;
                if (animation.T_index >= animation.trans_times.length) {
                    animation.T_index = 0;
                }
            }

            let R_next = animation.R_index == (animation.rot_times.length) ? 0 : animation.R_index + 1;

            let R_Begin = animation.rot_times[animation.R_index];
            let R_End = animation.rot_times[R_next];

            let R_out = getInterpolationValue(in_val, R_Begin, R_End);

           // let R_current = new Quaternion().copy(animation.rotations[animation.R_index]).slerp(animation.rotations[R_next], R_out);

            let R_current = new Quaternion();


            let R_Mat = new Matrix4();
            R_Mat.makeRotationFromQuaternion(R_current);
            joint.matrix.multiply(R_Mat);

            if (R_out > INTERP_TRESHOLD) {
                animation.R_index++;
                if (animation.R_index >= animation.rot_times.length) {
                    animation.R_index = 0;
                }
            }

            let S_next = animation.S_index == (animation.scal_times.length) ? 0 : animation.S_index + 1;

            let S_Begin = animation.scal_times[animation.S_index];
            let S_End = animation.scal_times[S_next];

            let S_out = getInterpolationValue(in_val, S_Begin, S_End);

            //let S_current = new Vector3().copy(animation.scales[animation.S_index]).lerp(animation.scales[S_next], S_out);

            let S_current = new Vector3();

            let S_Mat = new Matrix4();
            S_Mat.makeScale(S_current);
            joint.matrix.multiply(S_Mat);

            if (S_out > INTERP_TRESHOLD) {
                animation.S_index++;
                if (animation.S_index >= animation.scal_times.length) {
                    animation.S_index = 0;
                }
            }
        });
    }
 
}

export {Model,AnimationBuffer,JointBuffer,MeshBuffer,ImageBuffer,SoundBuffer} 