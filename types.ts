import * as THREE from 'three';




class AnimationBuffer{

    translations:THREE.Vector3[];
    rotations:THREE.Quaternion[];
    scales:THREE.Vector3[];
    trans_times:Float32Array;
    rot_times:Float32Array;
    scal_times:Float32Array;
    joint_index:Number;
}

class JointBuffer{

    translation:THREE.Vector3;
    rotation:THREE.Quaternion;
    scale:THREE.Vector3;
    transformMat:THREE.Matrix4;    
    children:Uint8Array;

}

class MeshBuffer{

    vertices:Float32Array;
    indices:Uint16Array;
    uvs:Float32Array;
    normals:Float32Array;
    joint_index:Number;

}

class ImageBuffer{

    data:Uint8Array;
    width:Number;
    height:Number;

}

class SoundBuffer{

    data:Int16Array;
    sample_count:Number;
    sample_rate:Number;
    
}

class Model{    

    joints:JointBuffer[];
    meshes:MeshBuffer[];
    animations:AnimationBuffer[];

    root:Number;

    image:ImageBuffer;
    sound:SoundBuffer;

}


