import { Quaternion,Vector3} from './node_modules/three/build/three.module.js';

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

    animations;

    constructor(){
        this.translation= new Vector3(0.0,0.0,0.0);
        this.rotation = new Quaternion(0.0,1.0,0.0,0.0);
        this.scale = new Vector3(1.0,1.0,1.0);
        this.animations= [];
        this.children=[];
        this.root=false;
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
}

export {Model,AnimationBuffer,JointBuffer,MeshBuffer,ImageBuffer,SoundBuffer} 